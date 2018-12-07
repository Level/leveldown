#include "napi.h"
#include <leveldb/db.h>
#include <leveldb/cache.h>
#include <leveldb/filter_policy.h>
#include <map>

/**
 * Forward declarations.
 */
struct Iterator;

/**
 * Owns the LevelDB storage together with cache and filter policy.
 */
struct Database {
  Database (napi_env env)
    : env_(env),
      db_(NULL),
      blockCache_(NULL),
      filterPolicy_(NULL),
      currentIteratorId_(0) {}

  ~Database () {
    if (db_ != NULL) {
      delete db_;
      db_ = NULL;
    }
  }

  leveldb::Status Open(const leveldb::Options& options,
                       const char* location) {
    return leveldb::DB::Open(options, location, &db_);
  }

  leveldb::Status Put(const leveldb::WriteOptions& options,
                      leveldb::Slice key,
                      leveldb::Slice value) {
    return db_->Put(options, key, value);
  }

  const leveldb::Snapshot* NewSnapshot() {
    return db_->GetSnapshot();
  }

  napi_env env_;
  leveldb::DB* db_;
  leveldb::Cache* blockCache_;
  // TODO figure out if we can use filterPolicy_ across
  // several Open()/Close(), i.e. can we create it _one_
  // time in the constructor?
  const leveldb::FilterPolicy* filterPolicy_;
  uint32_t currentIteratorId_;
  std::map< uint32_t, Iterator * > iterators_;
};

/**
 * Runs when a Database is garbage collected.
 */
static void FinalizeDatabase(napi_env env, void* data, void* hint) {
  if (data) {
    delete (Database*)data;
  }
}

/**
 * Returns a context object for a database.
 */
NAPI_METHOD(db) {
  Database* database = new Database(env);

  napi_value result;
  NAPI_STATUS_THROWS(napi_create_external(env, database,
                                          FinalizeDatabase,
                                          NULL, &result));
  return result;
}

// TODO BooleanProperty() and Uint32Property() can be refactored

/**
 * Returns true if 'obj' has a property 'key'
 */
static bool HasProperty(napi_env env, napi_value obj, const char* key) {
  bool has = false;
  napi_value _key;
  napi_create_string_utf8(env, key, strlen(key), &_key);
  napi_has_property(env, obj, _key, &has);
  return has;
}

/**
 * Returns a property in napi_value form
 */
static napi_value GetProperty(napi_env env,
                              napi_value obj,
                              const char* key) {
  napi_value _key;
  napi_create_string_utf8(env, key, strlen(key), &_key);

  napi_value value;
  napi_get_property(env, obj, _key, &value);

  return value;
}

/**
 * Returns a boolean property 'key' from 'obj'.
 * Returns 'DEFAULT' if the property doesn't exist.
 */
static bool BooleanProperty(napi_env env,
                            napi_value obj,
                            const char* key,
                            bool DEFAULT) {
  if (HasProperty(env, obj, key)) {
    napi_value value = GetProperty(env, obj, key);
    bool result;
    napi_get_value_bool(env, value, &result);
    return result;
  }

  return DEFAULT;
}

/**
 * Returns a uint32 property 'key' from 'obj'
 * Returns 'DEFAULT' if the property doesn't exist.
 */
static uint32_t Uint32Property(napi_env env,
                               napi_value obj,
                               const char* key,
                               uint32_t DEFAULT) {
  if (HasProperty(env, obj, key)) {
    napi_value value = GetProperty(env, obj, key);
    uint32_t result;
    napi_get_value_uint32(env, value, &result);
    return result;
  }

  return DEFAULT;
}

/**
 * Returns a uint32 property 'key' from 'obj'
 * Returns 'DEFAULT' if the property doesn't exist.
 */
static int Int32Property(napi_env env,
                         napi_value obj,
                         const char* key,
                         int DEFAULT) {
  bool exists = false;
  napi_value _key;
  napi_create_string_utf8(env, key, strlen(key), &_key);
  napi_has_property(env, obj, _key, &exists);

  if (exists) {
    napi_value value;
    napi_get_property(env, obj, _key, &value);
    int result;
    napi_get_value_int32(env, value, &result);
    return result;
  }

  return DEFAULT;
}

/**
 * Base worker class.
 */
struct BaseWorker {
  BaseWorker(napi_env env,
             Database* database,
             napi_value callback,
             const char* resourceName)
    : env_(env), database_(database) {
    napi_create_reference(env_, callback, 1, &callbackRef_);
    napi_value asyncResourceName;
    napi_create_string_utf8(env_, resourceName,
                            NAPI_AUTO_LENGTH,
                            &asyncResourceName);
    napi_create_async_work(env_, callback,
                           asyncResourceName,
                           BaseWorker::Execute,
                           BaseWorker::Complete,
                           this,
                           &asyncWork_);
  }

  virtual ~BaseWorker() {
    napi_delete_reference(env_, callbackRef_);
    napi_delete_async_work(env_, asyncWork_);
  }

  /**
   * Calls virtual DoExecute().
   */
  static void Execute(napi_env env, void* data) {
    BaseWorker* self = (BaseWorker*)data;
    self->DoExecute();
  }

  /**
   * MUST be overriden.
   */
  virtual void DoExecute() = 0;

  /**
   * Calls DoComplete() and kills the worker.
   */
  static void Complete(napi_env env, napi_status status, void* data) {
    BaseWorker* self = (BaseWorker*)data;
    self->DoComplete();
    delete self;
  }

  /**
   * Default handling when work is complete.
   * - Calls back with NULL if no error
   * - Calls back with error if status is an error
   */
  virtual void DoComplete() {
    const int argc = 1;
    napi_value argv[argc];

    napi_value global;
    napi_get_global(env_, &global);
    napi_value callback;
    napi_get_reference_value(env_, callbackRef_, &callback);

    if (status_.ok()) {
      napi_get_null(env_, &argv[0]);
    } else {
      const char* str = status_.ToString().c_str();
      napi_value msg;
      napi_create_string_utf8(env_, str, strlen(str), &msg);
      napi_create_error(env_, NULL, msg, &argv[0]);
    }

    napi_call_function(env_, global, callback, argc, argv, NULL);
  }

  void Queue() {
    napi_queue_async_work(env_, asyncWork_);
  }

  napi_env env_;
  napi_ref callbackRef_;
  napi_async_work asyncWork_;
  Database* database_;
  leveldb::Status status_;
};

/**
 * Worker class for opening the database
 */
struct OpenWorker : public BaseWorker {
  OpenWorker(napi_env env,
             Database* database,
             napi_value callback,
             char* location,
             bool createIfMissing,
             bool errorIfExists,
             bool compression,
             uint32_t writeBufferSize,
             uint32_t blockSize,
             uint32_t maxOpenFiles,
             uint32_t blockRestartInterval,
             uint32_t maxFileSize)
    : BaseWorker(env, database, callback, "leveldown::open"),
      location_(location) {
    options_.block_cache = database->blockCache_;
    options_.filter_policy = database->filterPolicy_;
    options_.create_if_missing = createIfMissing;
    options_.error_if_exists = errorIfExists;
    options_.compression = compression
      ? leveldb::kSnappyCompression
      : leveldb::kNoCompression;
    options_.write_buffer_size = writeBufferSize;
    options_.block_size = blockSize;
    options_.max_open_files = maxOpenFiles;
    options_.block_restart_interval = blockRestartInterval;
    options_.max_file_size = maxFileSize;
  }

  virtual ~OpenWorker() {
    free(location_);
  }

  virtual void DoExecute() {
    status_ = database_->Open(options_, location_);
  }

  leveldb::Options options_;
  char* location_;
};

/**
 * Opens a database.
 */
NAPI_METHOD(db_open) {
  NAPI_ARGV(4);
  NAPI_DB_CONTEXT();
  // TODO create a NAPI_ARGV_UTF8_NEW() macro that uses new instead of malloc
  // so we have similar allocation/deallocation mechanisms everywhere
  NAPI_ARGV_UTF8_MALLOC(location, 1);

  // Options object and properties.
  napi_value options = argv[2];
  bool createIfMissing = BooleanProperty(env, options, "createIfMissing", true);
  bool errorIfExists = BooleanProperty(env, options, "errorIfExists", false);
  bool compression = BooleanProperty(env, options, "compression", true);

  uint32_t cacheSize = Uint32Property(env, options, "cacheSize", 8 << 20);
  uint32_t writeBufferSize = Uint32Property(env, options , "writeBufferSize" , 4 << 20);
  uint32_t blockSize = Uint32Property(env, options, "blockSize", 4096);
  uint32_t maxOpenFiles = Uint32Property(env, options, "maxOpenFiles", 1000);
  uint32_t blockRestartInterval = Uint32Property(env, options,
                                                 "blockRestartInterval", 16);
  uint32_t maxFileSize = Uint32Property(env, options, "maxFileSize", 2 << 20);

  // TODO clean these up in close()
  database->blockCache_ = leveldb::NewLRUCache(cacheSize);
  database->filterPolicy_ = leveldb::NewBloomFilterPolicy(10);

  napi_value callback = argv[3];
  OpenWorker* worker = new OpenWorker(env,
                                      database,
                                      callback,
                                      location,
                                      createIfMissing,
                                      errorIfExists,
                                      compression,
                                      writeBufferSize,
                                      blockSize,
                                      maxOpenFiles,
                                      blockRestartInterval,
                                      maxFileSize);
  worker->Queue();
  NAPI_RETURN_UNDEFINED();
}

/**
 * Returns true if 'value' is a string otherwise false.
 */
static bool IsString(napi_env env, napi_value value) {
  napi_valuetype type;
  napi_typeof(env, value, &type);
  return type == napi_string;
}

/**
 * Returns true if 'value' is a buffer otherwise false.
 */
static bool IsBuffer(napi_env env, napi_value value) {
  bool isBuffer;
  napi_is_buffer(env, value, &isBuffer);
  return isBuffer;
}

/**
 * Macro to copy memory from a buffer or string.
 */
#define LD_STRING_OR_BUFFER_TO_COPY(env, from, to)                      \
  char* to##Ch_ = 0;                                                    \
  size_t to##Sz_ = 0;                                                   \
  if (IsString(env, from)) {                                            \
    napi_get_value_string_utf8(env, from, NULL, 0, &to##Sz_);           \
    to##Ch_ = new char[to##Sz_ + 1];                                    \
    napi_get_value_string_utf8(env, from, to##Ch_, to##Sz_ + 1, &to##Sz_); \
    to##Ch_[to##Sz_] = '\0';                                            \
    printf("LD_STRING_OR_BUFFER_TO_COPY STRING length: %d content: %s\n", to##Sz_, to##Ch_); \
  } else if (IsBuffer(env, from)) {                                     \
    char* buf = 0;                                                      \
    napi_get_buffer_info(env, from, (void **)&buf, &to##Sz_);           \
    to##Ch_ = new char[to##Sz_];                                        \
    memcpy(to##Ch_, buf, to##Sz_);                                      \
    printf("LD_STRING_OR_BUFFER_TO_COPY BUFFER length: %d content: %s\n", to##Sz_, to##Ch_); \
  }

/**
 * Convert a napi_value to a leveldb::Slice.
 */
static leveldb::Slice ToSlice(napi_env env, napi_value from) {
  LD_STRING_OR_BUFFER_TO_COPY(env, from, to);
  return leveldb::Slice(toCh_, toSz_);
}

/**
 * Worker class for putting key/value to the database
 */
struct PutWorker : public BaseWorker {
  PutWorker(napi_env env,
            Database* database,
            napi_value callback,
            napi_value key,
            napi_value value,
            bool sync)
    : BaseWorker(env, database, callback, "leveldown::put"),
      key_(ToSlice(env, key)), value_(ToSlice(env, value)) {
    options_.sync = sync;
  }

  ~PutWorker() {
    // TODO clean up key_ and value_ if they aren't empty?
    // See DisposeStringOrBufferFromSlice()
  }

  virtual void DoExecute() {
    status_ = database_->Put(options_, key_, value_);
  }

  leveldb::WriteOptions options_;
  leveldb::Slice key_;
  leveldb::Slice value_;
};

/**
 * Puts a key and a value to the database.
 */
NAPI_METHOD(db_put) {
  NAPI_ARGV(5);
  NAPI_DB_CONTEXT();

  napi_value key = argv[1];
  napi_value value = argv[2];
  bool sync = BooleanProperty(env, argv[3], "sync", false);
  napi_value callback = argv[4];
  PutWorker* worker = new PutWorker(env,
                                    database,
                                    callback,
                                    key,
                                    value,
                                    sync);
  worker->Queue();
  NAPI_RETURN_UNDEFINED();
}

/**
 * Owns a leveldb iterator.
 */
struct Iterator {
  Iterator(Database* database,
           uint32_t id,
           leveldb::Slice* start,
           std::string* end,
           bool reverse,
           bool keys,
           bool values,
           int limit,
           std::string* lt,
           std::string* lte,
           std::string* gt,
           std::string* gte,
           bool fillCache,
           bool keyAsBuffer,
           bool valueAsBuffer,
           uint32_t highWaterMark)
    : database_(database),
      id_(id),
      start_(start),
      end_(end),
      reverse_(reverse),
      keys_(keys),
      values_(values),
      limit_(limit),
      lt_(lt),
      lte_(lte),
      gt_(gt),
      gte_(gte),
      keyAsBuffer_(keyAsBuffer),
      valueAsBuffer_(valueAsBuffer),
      highWaterMark_(highWaterMark),
      dbIterator_(NULL),
      count_(0),
      target_(NULL),
      seeking_(false),
      landed_(false),
      nexting_(false),
      ended_(false) {
    // TODO need something different for endWorker_
    // endWorker_ = NULL;
    // options    = new leveldb::ReadOptions();
    options_.fill_cache = fillCache;
    options_.snapshot = database->NewSnapshot();
  }

  ~Iterator() {

  }

  // TODO pull in all methods from src/iterator.cc and go
  // through EACH of them and make sure that they are used
  // and WHERE they are used

  // TODO can we get rid of pointers below? Generally it shouldn't
  // be needed since this is a class and can handle the memory
  // automatically (with member constructors/destructors)
  // Keeping it the same for now.

  Database* database_;
  uint32_t id_;
  leveldb::Slice* start_;
  std::string* end_;
  bool reverse_;
  bool keys_;
  bool values_;
  int limit_;
  std::string* lt_;
  std::string* lte_;
  std::string* gt_;
  std::string* gte_;
  bool keyAsBuffer_;
  bool valueAsBuffer_;
  uint32_t highWaterMark_;
  leveldb::Iterator* dbIterator_;
  int count_;
  leveldb::Slice* target_;
  bool seeking_;
  bool landed_;
  bool nexting_;
  bool ended_;

  leveldb::ReadOptions options_;
  // TODO what is this used for and how can we do it otherwise?
  // AsyncWorker* endWorker_;
};

/**
 * Runs when an Iterator is garbage collected.
 */
static void FinalizeIterator(napi_env env, void* data, void* hint) {
  if (data) {
    // TODO this might be incorrect, at the moment mimicing the behavior
    // of Database. We might need to review the life cycle of Iterator
    // and if it's garbage collected it might be enough to unhook itself
    // from the Database (it has a pointer to it and could do this from
    // its destructor).
    delete (Iterator*)data;
  }
}

/**
 * Returns length of string or buffer
 */
static size_t StringOrBufferLength(napi_env env, napi_value value) {
  size_t size = 0;

  if (IsString(env, value)) {
    napi_get_value_string_utf8(env, value, NULL, 0, &size);
  } else if (IsBuffer(env, value)) {
    char* buf;
    napi_get_buffer_info(env, value, (void **)&buf, &size);
  }

  return size;
}

/**
 * Creates an iterator.
 */
NAPI_METHOD(iterator) {
  NAPI_ARGV(2);
  NAPI_DB_CONTEXT();

  napi_value options = argv[1];
  bool reverse = BooleanProperty(env, options, "reverse", false);
  bool keys = BooleanProperty(env, options, "keys", true);
  bool values = BooleanProperty(env, options, "values", true);
  bool fillCache = BooleanProperty(env, options, "fillCache", false);
  bool keyAsBuffer = BooleanProperty(env, options, "keyAsBuffer", true);
  bool valueAsBuffer = BooleanProperty(env, options, "valueAsBuffer", true);
  int limit = Int32Property(env, options, "limit", -1);
  uint32_t highWaterMark = Uint32Property(env, options, "highWaterMark",
                                          16 * 1024);

  // TODO simplify and refactor the hideous code below

  leveldb::Slice* start = NULL;
  char *startStr = NULL;
  if (HasProperty(env, options, "start")) {
    napi_value value = GetProperty(env, options, "start");
    if (IsString(env, value) || IsBuffer(env, value)) {
      if (StringOrBufferLength(env, value) > 0) {
        LD_STRING_OR_BUFFER_TO_COPY(env, value, _start);
        start = new leveldb::Slice(_startCh_, _startSz_);
        startStr = _startCh_;
      }
    }
  }

  std::string* end = NULL;
  if (HasProperty(env, options, "end")) {
    napi_value value = GetProperty(env, options, "end");
    if (IsString(env, value) || IsBuffer(env, value)) {
      if (StringOrBufferLength(env, value) > 0) {
        LD_STRING_OR_BUFFER_TO_COPY(env, value, _end);
        end = new std::string(_endCh_, _endSz_);
        delete [] _endCh_;
      }
    }
  }

  std::string* lt = NULL;
  if (HasProperty(env, options, "lt")) {
    napi_value value = GetProperty(env, options, "lt");
    if (IsString(env, value) || IsBuffer(env, value)) {
      if (StringOrBufferLength(env, value) > 0) {
        LD_STRING_OR_BUFFER_TO_COPY(env, value, _lt);
        lt = new std::string(_ltCh_, _ltSz_);
        delete [] _ltCh_;
        if (reverse) {
          if (startStr != NULL) {
            delete [] startStr;
            startStr = NULL;
          }
          if (start != NULL) {
            delete start;
          }
          start = new leveldb::Slice(lt->data(), lt->size());
        }
      }
    }
  }

  std::string* lte = NULL;
  if (HasProperty(env, options, "lte")) {
    napi_value value = GetProperty(env, options, "lte");
    if (IsString(env, value) || IsBuffer(env, value)) {
      if (StringOrBufferLength(env, value) > 0) {
        LD_STRING_OR_BUFFER_TO_COPY(env, value, _lte);
        lte = new std::string(_lteCh_, _lteSz_);
        delete [] _lteCh_;
        if (reverse) {
          if (startStr != NULL) {
            delete [] startStr;
            startStr = NULL;
          }
          if (start != NULL) {
            delete start;
          }
          start = new leveldb::Slice(lte->data(), lte->size());
        }
      }
    }
  }

  std::string* gt = NULL;
  if (HasProperty(env, options, "gt")) {
    napi_value value = GetProperty(env, options, "gt");
    if (IsString(env, value) || IsBuffer(env, value)) {
      if (StringOrBufferLength(env, value) > 0) {
        LD_STRING_OR_BUFFER_TO_COPY(env, value, _gt);
        gt = new std::string(_gtCh_, _gtSz_);
        delete [] _gtCh_;
        if (!reverse) {
          if (startStr != NULL) {
            delete [] startStr;
            startStr = NULL;
          }
          if (start != NULL) {
            delete start;
          }
          start = new leveldb::Slice(gt->data(), gt->size());
        }
      }
    }
  }

  std::string* gte = NULL;
  if (HasProperty(env, options, "gte")) {
    napi_value value = GetProperty(env, options, "gte");
    if (IsString(env, value) || IsBuffer(env, value)) {
      if (StringOrBufferLength(env, value) > 0) {
        LD_STRING_OR_BUFFER_TO_COPY(env, value, _gte);
        gte = new std::string(_gteCh_, _gteSz_);
        delete [] _gteCh_;
        if (!reverse) {
          if (startStr != NULL) {
            delete [] startStr;
            startStr = NULL;
          }
          if (start != NULL) {
            delete start;
          }
          start = new leveldb::Slice(gte->data(), gte->size());
        }
      }
    }
  }

  uint32_t id = database->currentIteratorId_++;
  Iterator* iterator = new Iterator(database,
                                    id,
                                    start,
                                    end,
                                    reverse,
                                    keys,
                                    values,
                                    limit,
                                    lt,
                                    lte,
                                    gt,
                                    gte,
                                    fillCache,
                                    keyAsBuffer,
                                    valueAsBuffer,
                                    highWaterMark);
  database->iterators_[id] = iterator;

  napi_value result;
  NAPI_STATUS_THROWS(napi_create_external(env, iterator,
                                          FinalizeIterator,
                                          NULL, &result));
  return result;
}

/**
 * Seeks an iterator.
 */
NAPI_METHOD(iterator_seek) {
  NAPI_ARGV(2);
  // NAPI_ITERATOR_CONTEXT();

  NAPI_RETURN_UNDEFINED();
}

/**
 * Moves an iterator to next element.
 */
NAPI_METHOD(iterator_next) {
  NAPI_ARGV(2);
  // NAPI_ITERATOR_CONTEXT();

  NAPI_RETURN_UNDEFINED();
}

/**
 * Ends an iterator.
 */
NAPI_METHOD(iterator_end) {
  NAPI_ARGV(2);
  // NAPI_ITERATOR_CONTEXT();

  NAPI_RETURN_UNDEFINED();
}

NAPI_INIT() {
  NAPI_EXPORT_FUNCTION(db);
  NAPI_EXPORT_FUNCTION(db_open);
  NAPI_EXPORT_FUNCTION(db_put);

  NAPI_EXPORT_FUNCTION(iterator);
  NAPI_EXPORT_FUNCTION(iterator_seek);
  NAPI_EXPORT_FUNCTION(iterator_next);
  NAPI_EXPORT_FUNCTION(iterator_end);
}
