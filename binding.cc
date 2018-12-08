#include <napi-macros.h>
#include <node_api.h>

#include <leveldb/db.h>
#include <leveldb/write_batch.h>
#include <leveldb/cache.h>
#include <leveldb/filter_policy.h>

#include <map>
#include <vector>

/**
 * Forward declarations.
 */
struct Database;
struct Iterator;
struct EndWorker;

/**
 * Macros.
 */

#define NAPI_DB_CONTEXT() \
  Database* database = NULL; \
  NAPI_STATUS_THROWS(napi_get_value_external(env, argv[0], (void**)&database));

#define NAPI_ITERATOR_CONTEXT() \
  Iterator* iterator = NULL; \
  NAPI_STATUS_THROWS(napi_get_value_external(env, argv[0], (void**)&iterator));

#define NAPI_BATCH_CONTEXT() \
  Batch* batch = NULL; \
  NAPI_STATUS_THROWS(napi_get_value_external(env, argv[0], (void**)&batch));

#define NAPI_RETURN_UNDEFINED() \
  return 0;

#define LD_STRING_OR_BUFFER_TO_COPY(env, from, to)                      \
  char* to##Ch_ = 0;                                                    \
  size_t to##Sz_ = 0;                                                   \
  if (IsString(env, from)) {                                            \
    napi_get_value_string_utf8(env, from, NULL, 0, &to##Sz_);           \
    to##Ch_ = new char[to##Sz_ + 1];                                    \
    napi_get_value_string_utf8(env, from, to##Ch_, to##Sz_ + 1, &to##Sz_); \
    to##Ch_[to##Sz_] = '\0';                                            \
    printf("-- LD_STRING_OR_BUFFER_TO_COPY STRING length: %zu content: %s\n", to##Sz_, to##Ch_); \
  } else if (IsBuffer(env, from)) {                                     \
    char* buf = 0;                                                      \
    napi_get_buffer_info(env, from, (void **)&buf, &to##Sz_);           \
    to##Ch_ = new char[to##Sz_];                                        \
    memcpy(to##Ch_, buf, to##Sz_);                                      \
    printf("-- LD_STRING_OR_BUFFER_TO_COPY BUFFER length: %zu content: %s\n", to##Sz_, to##Ch_); \
  }

/*********************************************************************
 * Helpers.
 ********************************************************************/

/**
 * Returns true if 'value' is a string.
 */
static bool IsString (napi_env env, napi_value value) {
  napi_valuetype type;
  napi_typeof(env, value, &type);
  return type == napi_string;
}

/**
 * Returns true if 'value' is a buffer.
 */
static bool IsBuffer (napi_env env, napi_value value) {
  bool isBuffer;
  napi_is_buffer(env, value, &isBuffer);
  return isBuffer;
}

/**
 * Returns true if 'value' is an object.
 */
static bool IsObject (napi_env env, napi_value value) {
  napi_valuetype type;
  napi_typeof(env, value, &type);
  return type == napi_object;
}

/**
 * Returns true if 'obj' has a property 'key'.
 */
static bool HasProperty (napi_env env, napi_value obj, const char* key) {
  // TODO switch to use napi_has_named_property() (no need to make a napi_value)
  bool has = false;
  napi_value _key;
  napi_create_string_utf8(env, key, strlen(key), &_key);
  napi_has_property(env, obj, _key, &has);
  return has;
}

/**
 * Returns a property in napi_value form.
 */
static napi_value GetProperty (napi_env env, napi_value obj, const char* key) {
  // TODO switch to use napi_get_named_property() (no need to make a napi_value)
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
static bool BooleanProperty (napi_env env, napi_value obj, const char* key,
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
 * Returns a uint32 property 'key' from 'obj'.
 * Returns 'DEFAULT' if the property doesn't exist.
 */
static uint32_t Uint32Property (napi_env env, napi_value obj, const char* key,
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
 * Returns a uint32 property 'key' from 'obj'.
 * Returns 'DEFAULT' if the property doesn't exist.
 */
static int Int32Property (napi_env env, napi_value obj, const char* key,
                          int DEFAULT) {
  if (HasProperty(env, obj, key)) {
    napi_value value = GetProperty(env, obj, key);
    int result;
    napi_get_value_int32(env, value, &result);
    return result;
  }

  return DEFAULT;
}

/**
 * Returns a string property 'key' from 'obj'.
 * Returns empty string if the property doesn't exist.
 */
static std::string StringProperty (napi_env env, napi_value obj, const char* key) {
  if (HasProperty(env, obj, key)) {
    napi_value value = GetProperty(env, obj, key);
    if (IsString(env, value)) {
      size_t size = 0;
      napi_get_value_string_utf8(env, value, NULL, 0, &size);

      char* buf = new char[size + 1];
      napi_get_value_string_utf8(env, value, buf, size + 1, &size);
      buf[size] = '\0';

      std::string result = buf;
      delete [] buf;
      return result;
    }
  }

  return "";
}

/**
 * Convert a napi_value to a leveldb::Slice.
 */
static leveldb::Slice ToSlice (napi_env env, napi_value from) {
  LD_STRING_OR_BUFFER_TO_COPY(env, from, to);
  return leveldb::Slice(toCh_, toSz_);
}

/**
 * Returns length of string or buffer
 */
static size_t StringOrBufferLength (napi_env env, napi_value value) {
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
 * Base worker class. Handles the async work.
 */
struct BaseWorker {
  BaseWorker (napi_env env,
              Database* database,
              napi_value callback,
              const char* resourceName)
    : env_(env), database_(database), errMsg_(NULL) {
    NAPI_STATUS_THROWS(napi_create_reference(env_, callback, 1, &callbackRef_));
    napi_value asyncResourceName;
    NAPI_STATUS_THROWS(napi_create_string_utf8(env_, resourceName,
                                               NAPI_AUTO_LENGTH,
                                               &asyncResourceName));
    NAPI_STATUS_THROWS(napi_create_async_work(env_, callback,
                                              asyncResourceName,
                                              BaseWorker::Execute,
                                              BaseWorker::Complete,
                                              this, &asyncWork_));
  }

  virtual ~BaseWorker () {
    delete [] errMsg_;
    napi_delete_reference(env_, callbackRef_);
    napi_delete_async_work(env_, asyncWork_);
  }

  static void Execute (napi_env env, void* data) {
    BaseWorker* self = (BaseWorker*)data;
    self->DoExecute();
  }

  void SetStatus (leveldb::Status status) {
    status_ = status;
    if (!status.ok()) {
      SetErrorMessage(status.ToString().c_str());
    }
  }

  void SetErrorMessage(const char *msg) {
    delete [] errMsg_;
    size_t size = strlen(msg) + 1;
    errMsg_ = new char[size];
    memcpy(errMsg_, msg, size);
  }

  virtual void DoExecute () = 0;

  static void Complete (napi_env env, napi_status status, void* data) {
    BaseWorker* self = (BaseWorker*)data;
    self->DoComplete();
    delete self;
  }

  void DoComplete () {
    if (status_.ok()) {
      return HandleOKCallback();
    }

    // TODO the global, callback, and calling the function code
    // could be refactored with HandleOKCallback()

    napi_value global;
    napi_get_global(env_, &global);
    napi_value callback;
    napi_get_reference_value(env_, callbackRef_, &callback);

    napi_value msg;
    napi_create_string_utf8(env_, errMsg_, strlen(errMsg_), &msg);

    const int argc = 1;
    napi_value argv[argc];
    napi_create_error(env_, NULL, msg, &argv[0]);

    napi_call_function(env_, global, callback, argc, argv, NULL);
  }

  virtual void HandleOKCallback () {
    napi_value global;
    napi_get_global(env_, &global);
    napi_value callback;
    napi_get_reference_value(env_, callbackRef_, &callback);

    const int argc = 1;
    napi_value argv[argc];
    napi_get_null(env_, &argv[0]);

    napi_call_function(env_, global, callback, argc, argv, NULL);
  }

  void Queue () {
    napi_queue_async_work(env_, asyncWork_);
  }

  napi_env env_;
  napi_ref callbackRef_;
  napi_async_work asyncWork_;
  Database* database_;

private:
  leveldb::Status status_;
  char *errMsg_;
};

/**
 * Owns the LevelDB storage, cache, filter policy and iterators.
 */
struct Database {
  Database (napi_env env)
    : env_(env),
      db_(NULL),
      blockCache_(NULL),
      filterPolicy_(NULL),
      currentIteratorId_(0),
      pendingCloseWorker_(NULL) {}

  ~Database () {
    if (db_ != NULL) {
      delete db_;
      db_ = NULL;
    }
  }

  leveldb::Status Open (const leveldb::Options& options,
                        const char* location) {
    return leveldb::DB::Open(options, location, &db_);
  }

  void CloseDatabase () {
    delete db_;
    db_ = NULL;
    if (blockCache_) {
      delete blockCache_;
      blockCache_ = NULL;
    }
    if (filterPolicy_) {
      delete filterPolicy_;
      filterPolicy_ = NULL;
    }
  }

  leveldb::Status Put (const leveldb::WriteOptions& options,
                       leveldb::Slice key,
                       leveldb::Slice value) {
    return db_->Put(options, key, value);
  }

  leveldb::Status Get (const leveldb::ReadOptions& options,
                       leveldb::Slice key,
                       std::string& value) {
    return db_->Get(options, key, &value);
  }

  leveldb::Status Del (const leveldb::WriteOptions& options,
                       leveldb::Slice key) {
    return db_->Delete(options, key);
  }

  leveldb::Status WriteBatch (const leveldb::WriteOptions& options,
                              leveldb::WriteBatch* batch) {
    return db_->Write(options, batch);
  }

  const leveldb::Snapshot* NewSnapshot () {
    return db_->GetSnapshot();
  }

  leveldb::Iterator* NewIterator (leveldb::ReadOptions* options) {
    return db_->NewIterator(*options);
  }

  void ReleaseSnapshot (const leveldb::Snapshot* snapshot) {
    return db_->ReleaseSnapshot(snapshot);
  }

  void ReleaseIterator (uint32_t id) {
    // called each time an Iterator is End()ed, in the main thread
    // we have to remove our reference to it and if it's the last
    // iterator we have to invoke a pending CloseWorker if there
    // is one if there is a pending CloseWorker it means that
    // we're waiting for iterators to end before we can close them
    iterators_.erase(id);
    if (iterators_.empty() && pendingCloseWorker_ != NULL) {
      // TODO Test this!
      pendingCloseWorker_->Queue();
      pendingCloseWorker_ = NULL;
    }
  }

  napi_env env_;
  leveldb::DB* db_;
  leveldb::Cache* blockCache_;
  // TODO figure out if we can use filterPolicy_ across
  // several Open()/Close(), i.e. can we create it _one_
  // time in the constructor?
  const leveldb::FilterPolicy* filterPolicy_;
  uint32_t currentIteratorId_;
  // TODO this worker should have a proper type, now
  // it's some form of function pointer bastard, it should
  // just be a CloseWorker object
  //void (*pendingCloseWorker_);
  BaseWorker *pendingCloseWorker_;
  std::map< uint32_t, Iterator * > iterators_;
};

/**
 * Runs when a Database is garbage collected.
 */
static void FinalizeDatabase (napi_env env, void* data, void* hint) {
  if (data) {
    delete (Database*)data;
  }
}

/**
 * Returns a context object for a database.
 */
NAPI_METHOD(db_init) {
  Database* database = new Database(env);

  napi_value result;
  NAPI_STATUS_THROWS(napi_create_external(env, database,
                                          FinalizeDatabase,
                                          NULL, &result));
  return result;
}

/**
 * Worker class for opening a database.
 */
struct OpenWorker : public BaseWorker {
  OpenWorker (napi_env env,
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
    : BaseWorker(env, database, callback, "leveldown.db.open"),
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

  virtual ~OpenWorker () {
    free(location_);
  }

  virtual void DoExecute () {
    SetStatus(database_->Open(options_, location_));
  }

  leveldb::Options options_;
  char* location_;
};

/**
 * Open a database.
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
 * Worker class for closing a database
 */
struct CloseWorker : public BaseWorker {
  CloseWorker (napi_env env,
               Database* database,
               napi_value callback)
    : BaseWorker(env, database, callback, "leveldown.db.close") {}

  virtual ~CloseWorker () {}

  virtual void DoExecute () {
    database_->CloseDatabase();
  }
};

/**
 * Close a database.
 */
NAPI_METHOD(db_close) {
  NAPI_ARGV(2);
  NAPI_DB_CONTEXT();

  napi_value callback = argv[1];
  CloseWorker* worker = new CloseWorker(env, database, callback);

  if (database->iterators_.empty()) {
    worker->Queue();
    NAPI_RETURN_UNDEFINED();
  }

  // TODO fix me!

  /*
  // yikes, we still have iterators open! naughty naughty.
  // we have to queue up a CloseWorker and manually close each of them.
  // the CloseWorker will be invoked once they are all cleaned up
  database->pendingCloseWorker = worker;

  for (
       std::map< uint32_t, leveldown::Iterator * >::iterator it
         = database->iterators.begin()
         ; it != database->iterators.end()
         ; ++it) {

    // for each iterator still open, first check if it's already in
    // the process of ending (ended==true means an async End() is
    // in progress), if not, then we call End() with an empty callback
    // function and wait for it to hit ReleaseIterator() where our
    // CloseWorker will be invoked

    leveldown::Iterator *iterator = it->second;

    if (!iterator->ended) {
      v8::Local<v8::Function> end =
        v8::Local<v8::Function>::Cast(iterator->handle()->Get(
                  Nan::New<v8::String>("end").ToLocalChecked()));
      v8::Local<v8::Value> argv[] = {
        Nan::New<v8::FunctionTemplate>(EmptyMethod)->GetFunction() // empty callback
      };
      Nan::AsyncResource ar("leveldown:iterator.end");
      ar.runInAsyncScope(iterator->handle(), end, 1, argv);
    }
  }

  */

  NAPI_RETURN_UNDEFINED();
}

/**
 * Worker class for putting key/value to the database
 */
struct PutWorker : public BaseWorker {
  PutWorker (napi_env env,
             Database* database,
             napi_value callback,
             napi_value key,
             napi_value value,
             bool sync)
    : BaseWorker(env, database, callback, "leveldown.db.put"),
      key_(ToSlice(env, key)), value_(ToSlice(env, value)) {
    options_.sync = sync;
  }

  virtual ~PutWorker () {
    // TODO clean up key_ and value_ if they aren't empty?
    // See DisposeStringOrBufferFromSlice()
  }

  virtual void DoExecute () {
    SetStatus(database_->Put(options_, key_, value_));
  }

  leveldb::WriteOptions options_;
  leveldb::Slice key_;
  leveldb::Slice value_;
};

/**
 * Puts a key and a value to a database.
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
 * Worker class for getting a value from a database.
 */
struct GetWorker : public BaseWorker {
  GetWorker (napi_env env,
             Database* database,
             napi_value callback,
             napi_value key,
             bool asBuffer,
             bool fillCache)
    : BaseWorker(env, database, callback, "leveldown.db.get"),
      key_(ToSlice(env, key)),
      asBuffer_(asBuffer) {
    options_.fill_cache = fillCache;
  }

  virtual ~GetWorker () {
    // TODO clean up key_ if not empty?
    // See DisposeStringOrBufferFromSlice()
  }

  virtual void DoExecute () {
    SetStatus(database_->Get(options_, key_, value_));
  }

  virtual void HandleOKCallback() {
    const int argc = 2;
    napi_value argv[argc];
    napi_get_null(env_, &argv[0]);

    if (asBuffer_) {
      napi_create_buffer_copy(env_, value_.size(), value_.data(), NULL, &argv[1]);
    } else {
      napi_create_string_utf8(env_, value_.data(), value_.size(), &argv[1]);
    }

    // TODO move to base class
    napi_value global;
    napi_get_global(env_, &global);
    napi_value callback;
    napi_get_reference_value(env_, callbackRef_, &callback);

    napi_call_function(env_, global, callback, argc, argv, NULL);
  }

  leveldb::ReadOptions options_;
  leveldb::Slice key_;
  std::string value_;
  bool asBuffer_;
};

/**
 * Gets a value from a database.
 */
NAPI_METHOD(db_get) {
  NAPI_ARGV(4);
  NAPI_DB_CONTEXT();

  napi_value key = argv[1];
  napi_value options = argv[2];
  bool asBuffer = BooleanProperty(env, options, "asBuffer", true);
  bool fillCache = BooleanProperty(env, options, "fillCache", true);
  napi_value callback = argv[3];

  GetWorker* worker = new GetWorker(env,
                                    database,
                                    callback,
                                    key,
                                    asBuffer,
                                    fillCache);
  worker->Queue();

  NAPI_RETURN_UNDEFINED();
}

/**
 * Worker class for getting a value from a database.
 */
struct DelWorker : public BaseWorker {
  DelWorker (napi_env env,
             Database* database,
             napi_value callback,
             napi_value key,
             bool sync)
    : BaseWorker(env, database, callback, "leveldown.db.get"),
      key_(ToSlice(env, key)) {
    options_.sync = sync;
  }

  virtual ~DelWorker () {
    // TODO clean up key_ if not empty?
    // See DisposeStringOrBufferFromSlice()
  }

  virtual void DoExecute () {
    SetStatus(database_->Del(options_, key_));
  }

  leveldb::WriteOptions options_;
  leveldb::Slice key_;
};

/**
 * Gets a value from a database.
 */
NAPI_METHOD(db_del) {
  NAPI_ARGV(4);
  NAPI_DB_CONTEXT();

  napi_value key = argv[1];
  bool sync = BooleanProperty(env, argv[2], "sync", false);
  napi_value callback = argv[3];

  DelWorker* worker = new DelWorker(env,
                                    database,
                                    callback,
                                    key,
                                    sync);
  worker->Queue();

  NAPI_RETURN_UNDEFINED();
}

/**
 * Owns a leveldb iterator.
 */
struct Iterator {
  Iterator (Database* database,
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
      ended_(false),
      endWorker_(NULL) {
    options_ = new leveldb::ReadOptions();
    options_->fill_cache = fillCache;
    options_->snapshot = database->NewSnapshot();
  }

  ~Iterator () {
    ReleaseTarget();
    if (start_ != NULL) {
      // Special case for `start` option: it won't be
      // freed up by any of the delete calls below.
      if (!((lt_ != NULL && reverse_)
            || (lte_ != NULL && reverse_)
            || (gt_ != NULL && !reverse_)
            || (gte_ != NULL && !reverse_))) {
        delete [] start_->data();
      }
      delete start_;
    }
    if (end_ != NULL) {
      delete end_;
    }
    if (lt_ != NULL) {
      delete lt_;
    }
    if (gt_ != NULL) {
      delete gt_;
    }
    if (lte_ != NULL) {
      delete lte_;
    }
    if (gte_ != NULL) {
      delete gte_;
    }
  }

  void ReleaseTarget () {
    if (target_ != NULL) {
      if (!target_->empty()) {
        delete [] target_->data();
      }
      delete target_;
      target_ = NULL;
    }
  }

  void Release () {
    database_->ReleaseIterator(id_);
  }

  leveldb::Status IteratorStatus () {
    return dbIterator_->status();
  }

  void IteratorEnd () {
    delete dbIterator_;
    dbIterator_ = NULL;
    database_->ReleaseSnapshot(options_->snapshot);
  }

  bool GetIterator () {
    if (dbIterator_ == NULL) {
      dbIterator_ = database_->NewIterator(options_);

      if (start_ != NULL) {
        dbIterator_->Seek(*start_);

        if (reverse_) {
          if (!dbIterator_->Valid()) {
            // if it's past the last key, step back
            dbIterator_->SeekToLast();
          } else {
            std::string keyStr = dbIterator_->key().ToString();

            if (lt_ != NULL) {
              // TODO make a compount if statement
              if (lt_->compare(keyStr) <= 0) {
                dbIterator_->Prev();
              }
            } else if (lte_ != NULL) {
              // TODO make a compount if statement
              if (lte_->compare(keyStr) < 0) {
                dbIterator_->Prev();
              }
            } else if (start_ != NULL) {
              // TODO make a compount if statement
              if (start_->compare(keyStr)) {
                dbIterator_->Prev();
              }
            }
          }

          if (dbIterator_->Valid() && lt_ != NULL) {
            if (lt_->compare(dbIterator_->key().ToString()) <= 0) {
              dbIterator_->Prev();
            }
          }
        } else {
          // TODO this could be an else if
          if (dbIterator_->Valid() && gt_ != NULL
              && gt_->compare(dbIterator_->key().ToString()) == 0) {
            dbIterator_->Next();
          }
        }
      } else if (reverse_) {
        dbIterator_->SeekToLast();
      } else {
        dbIterator_->SeekToFirst();
      }

      return true;
    }

    return false;
  }

  bool Read (std::string& key, std::string& value) {
    // if it's not the first call, move to next item.
    if (!GetIterator() && !seeking_) {
      if (reverse_) {
        dbIterator_->Prev();
      }
      else {
        dbIterator_->Next();
      }
    }

    seeking_ = false;

    // now check if this is the end or not, if not then return the key & value
    if (dbIterator_->Valid()) {
      std::string keyStr = dbIterator_->key().ToString();
      const int isEnd = end_ == NULL ? 1 : end_->compare(keyStr);

      if ((limit_ < 0 || ++count_ <= limit_)
          && (end_ == NULL
              || (reverse_ && (isEnd <= 0))
              || (!reverse_ && (isEnd >= 0)))
          && ( lt_  != NULL ? (lt_->compare(keyStr) > 0)
               : lte_ != NULL ? (lte_->compare(keyStr) >= 0)
               : true )
          && ( gt_  != NULL ? (gt_->compare(keyStr) < 0)
               : gte_ != NULL ? (gte_->compare(keyStr) <= 0)
               : true )
          ) {
        if (keys_) {
          key.assign(dbIterator_->key().data(), dbIterator_->key().size());
        }
        if (values_) {
          value.assign(dbIterator_->value().data(), dbIterator_->value().size());
        }
        return true;
      }
    }

    return false;
  }

  bool OutOfRange (leveldb::Slice* target) {
    if (lt_ != NULL) {
      if (target->compare(*lt_) >= 0)
        return true;
    } else if (lte_ != NULL) {
      if (target->compare(*lte_) > 0)
        return true;
    } else if (start_ != NULL && reverse_) {
      if (target->compare(*start_) > 0)
        return true;
    }

    if (end_ != NULL) {
      int d = target->compare(*end_);
      if (reverse_ ? d < 0 : d > 0)
        return true;
    }

    if (gt_ != NULL) {
      if (target->compare(*gt_) <= 0)
        return true;
    } else if (gte_ != NULL) {
      if (target->compare(*gte_) < 0)
        return true;
    } else if (start_ != NULL && !reverse_) {
      if (target->compare(*start_) < 0)
        return true;
    }

    return false;
  }

  bool IteratorNext (std::vector<std::pair<std::string, std::string> >& result) {
    size_t size = 0;
    while (true) {
      std::string key, value;
      bool ok = Read(key, value);

      if (ok) {
        result.push_back(std::make_pair(key, value));

        if (!landed_) {
          landed_ = true;
          return true;
        }

        size = size + key.size() + value.size();
        if (size > highWaterMark_) {
          return true;
        }

      } else {
        return false;
      }
    }
  }

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

  leveldb::ReadOptions* options_;
  EndWorker* endWorker_;
};

/**
 * Runs when an Iterator is garbage collected.
 */
static void FinalizeIterator (napi_env env, void* data, void* hint) {
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
 * Create an iterator.
 */
NAPI_METHOD(iterator_init) {
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
  NAPI_ITERATOR_CONTEXT();

  iterator->ReleaseTarget();
  iterator->target_ = new leveldb::Slice(ToSlice(env, argv[1]));
  iterator->GetIterator();

  leveldb::Iterator* dbIterator = iterator->dbIterator_;
  dbIterator->Seek(*iterator->target_);

  iterator->seeking_ = true;
  iterator->landed_ = false;

  if (iterator->OutOfRange(iterator->target_)) {
    if (iterator->reverse_) {
      dbIterator->SeekToFirst();
      dbIterator->Prev();
    } else {
      dbIterator->SeekToLast();
      dbIterator->Next();
    }
  }
  else {
    if (dbIterator->Valid()) {
      int cmp = dbIterator->key().compare(*iterator->target_);
      if (cmp > 0 && iterator->reverse_) {
        dbIterator->Prev();
      } else if (cmp < 0 && !iterator->reverse_) {
        dbIterator->Next();
      }
    } else {
      if (iterator->reverse_) {
        dbIterator->SeekToLast();
      } else {
        dbIterator->SeekToFirst();
      }
      if (dbIterator->Valid()) {
        int cmp = dbIterator->key().compare(*iterator->target_);
        if (cmp > 0 && iterator->reverse_) {
          dbIterator->SeekToFirst();
          dbIterator->Prev();
        } else if (cmp < 0 && !iterator->reverse_) {
          dbIterator->SeekToLast();
          dbIterator->Next();
        }
      }
    }
  }

  NAPI_RETURN_UNDEFINED();
}

/**
 * Worker class for ending an iterator
 */
struct EndWorker : public BaseWorker {
  EndWorker (napi_env env,
             Iterator* iterator,
             napi_value callback)
    : BaseWorker(env, iterator->database_, callback,
                 "leveldown.iterator.end"),
      iterator_(iterator) {}

  virtual ~EndWorker () {}

  virtual void DoExecute () {
    iterator_->IteratorEnd();
  }

  virtual void HandleOKCallback () {
    iterator_->Release();
    BaseWorker::HandleOKCallback();
  }

  Iterator* iterator_;
};

/**
 * Ends an iterator.
 */
NAPI_METHOD(iterator_end) {
  NAPI_ARGV(2);
  NAPI_ITERATOR_CONTEXT();

  napi_value callback = argv[1];

  if (!iterator->ended_) {
    EndWorker* worker = new EndWorker(env, iterator, callback);
    iterator->ended_ = true;

    if (iterator->nexting_) {
      // waiting for a next() to return, queue the end
      iterator->endWorker_ = worker;
    } else {
      worker->Queue();
    }
  }

  NAPI_RETURN_UNDEFINED();
}

/**
 * TODO Move this to Iterator. There isn't any reason
 * for this function being a separate function pointer.
 */
void CheckEndCallback (Iterator* iterator) {
  iterator->ReleaseTarget();
  iterator->nexting_ = false;
  if (iterator->endWorker_ != NULL) {
    iterator->endWorker_->Queue();
    iterator->endWorker_ = NULL;
  }
}

/**
 * Worker class for nexting an iterator.
 */
struct NextWorker : public BaseWorker {
  NextWorker (napi_env env,
              Iterator* iterator,
              napi_value callback,
              void (*localCallback)(Iterator*))
    : BaseWorker(env, iterator->database_, callback,
                 "leveldown.iterator.next"),
      iterator_(iterator),
      localCallback_(localCallback) {}

  virtual ~NextWorker () {}

  virtual void DoExecute () {
    ok_ = iterator_->IteratorNext(result_);
    if (!ok_) {
      SetStatus(iterator_->IteratorStatus());
    }
  }

  virtual void HandleOKCallback () {
    size_t arraySize = result_.size() * 2;
    napi_value jsArray;
    napi_create_array_with_length(env_, arraySize, &jsArray);

    for (size_t idx = 0; idx < result_.size(); ++idx) {
      std::pair<std::string, std::string> row = result_[idx];
      std::string key = row.first;
      std::string value = row.second;

      napi_value returnKey;
      if (iterator_->keyAsBuffer_) {
        napi_create_buffer_copy(env_, key.size(), key.data(), NULL, &returnKey);
      } else {
        napi_create_string_utf8(env_, key.data(), key.size(), &returnKey);
      }

      napi_value returnValue;
      if (iterator_->valueAsBuffer_) {
        napi_create_buffer_copy(env_, value.size(), value.data(), NULL, &returnValue);
      } else {
        napi_create_string_utf8(env_, value.data(), value.size(), &returnValue);
      }

      // put the key & value in a descending order, so that they can be .pop:ed in javascript-land
      napi_set_element(env_, jsArray, static_cast<int>(arraySize - idx * 2 - 1), returnKey);
      napi_set_element(env_, jsArray, static_cast<int>(arraySize - idx * 2 - 2), returnValue);
    }

    // clean up & handle the next/end state
    // TODO this should just do iterator_->CheckEndCallback();
    localCallback_(iterator_);

    const int argc = 3;
    napi_value argv[argc];

    napi_get_null(env_, &argv[0]);
    argv[1] = jsArray;
    napi_get_boolean(env_, !ok_, &argv[2]);

    // TODO move to base class
    napi_value global;
    napi_get_global(env_, &global);
    napi_value callback;
    napi_get_reference_value(env_, callbackRef_, &callback);

    napi_call_function(env_, global, callback, argc, argv, NULL);
  }

  Iterator* iterator_;
  // TODO why do we need a function pointer for this?
  void (*localCallback_)(Iterator*);
  std::vector<std::pair<std::string, std::string> > result_;
  bool ok_;
};

/**
 * Moves an iterator to next element.
 */
NAPI_METHOD(iterator_next) {
  NAPI_ARGV(2);
  NAPI_ITERATOR_CONTEXT();

  napi_value callback = argv[1];

  if (iterator->ended_) {
    // TODO call back with error "iterator has ended"
    //v8::Local<v8::Value> argv[] = { Nan::Error("iterator has ended") };
    //LD_RUN_CALLBACK("leveldown:iterator.next", callback, 1, argv);
    NAPI_RETURN_UNDEFINED();
  }

  NextWorker* worker = new NextWorker(env,
                                      iterator,
                                      callback,
                                      CheckEndCallback);
  iterator->nexting_ = true;
  worker->Queue();

  NAPI_RETURN_UNDEFINED();
}

/**
 * Worker class for batch write operation.
 */
struct BatchWorker : public BaseWorker {
  BatchWorker (napi_env env,
               Database* database,
               napi_value callback,
               leveldb::WriteBatch* batch,
               bool sync)
    : BaseWorker(env, database, callback, "leveldown.batch.do"),
      batch_(batch) {
    options_.sync = sync;
  }

  virtual ~BatchWorker () {
    delete batch_;
  }

  virtual void DoExecute () {
    SetStatus(database_->WriteBatch(options_, batch_));
  }

  leveldb::WriteOptions options_;
  leveldb::WriteBatch* batch_;
};

/**
 * Does a batch write operation on a database.
 */
NAPI_METHOD(batch_do) {
  NAPI_ARGV(4);
  NAPI_DB_CONTEXT();

  napi_value array = argv[1];
  bool sync = BooleanProperty(env, argv[2], "sync", false);
  napi_value callback = argv[3];

  uint32_t length;
  napi_get_array_length(env, array, &length);

  leveldb::WriteBatch* batch = new leveldb::WriteBatch();
  bool hasData = false;

  for (uint32_t i = 0; i < length; i++) {
    napi_value element;
    napi_get_element(env, array, i, &element);

    if (!IsObject(env, element)) continue;

    std::string type = StringProperty(env, element, "type");
    printf("-- Batch element type: %s\n", type.c_str());

    if (type == "del") {
      if (!HasProperty(env, element, "key")) continue;
      leveldb::Slice key = ToSlice(env, GetProperty(env, element, "key"));

      batch->Delete(key);
      if (!hasData) hasData = true;

      // TODO clean up slices
      //DisposeStringOrBufferFromSlice(keyBuffer, key);
    } else if (type == "put") {
      if (!HasProperty(env, element, "key")) continue;
      if (!HasProperty(env, element, "value")) continue;

      leveldb::Slice key = ToSlice(env, GetProperty(env, element, "key"));
      leveldb::Slice value = ToSlice(env, GetProperty(env, element, "value"));

      batch->Put(key, value);
      if (!hasData) hasData = true;

      // TODO clean up slices
      //DisposeStringOrBufferFromSlice(keyBuffer, key);
      //DisposeStringOrBufferFromSlice(valueBuffer, value);
    }
  }

  if (hasData) {
    BatchWorker* worker = new BatchWorker(env, database, callback, batch, sync);
    worker->Queue();
  } else {
    delete batch;
    // TODO refactor with other callback code
    napi_value global;
    napi_get_global(env, &global);

    const int argc = 1;
    napi_value argv[argc];
    napi_get_null(env, &argv[0]);

    napi_call_function(env, global, callback, argc, argv, NULL);
  }

  NAPI_RETURN_UNDEFINED();
}

/**
 * Owns a WriteBatch.
 */
struct Batch {
  Batch (Database* database)
    : database_(database),
      batch_(new leveldb::WriteBatch()),
      hasData_(false) {}

  ~Batch () {
    delete batch_;
  }

  void Put (leveldb::Slice key, leveldb::Slice value) {
    batch_->Put(key, value);
    hasData_ = true;
  }

  void Del (leveldb::Slice key) {
    batch_->Delete(key);
    hasData_ = true;
  }

  void Clear () {
    batch_->Clear();
    hasData_ = false;
  }

  leveldb::Status Write (bool sync) {
    leveldb::WriteOptions options;
    options.sync = sync;
    return database_->WriteBatch(options, batch_);
  }

  Database* database_;
  leveldb::WriteBatch* batch_;
  bool hasData_;
};

/**
 * Runs when a Batch is garbage collected.
 */
static void FinalizeBatch (napi_env env, void* data, void* hint) {
  if (data) {
    delete (Batch*)data;
  }
}

/**
 * Return a batch object.
 */
NAPI_METHOD(batch_init) {
  NAPI_ARGV(1);
  NAPI_DB_CONTEXT();

  Batch* batch = new Batch(database);

  napi_value result;
  NAPI_STATUS_THROWS(napi_create_external(env, batch,
                                          FinalizeBatch,
                                          NULL, &result));
  return result;
}

/**
 * Adds a put instruction to a batch object.
 */
NAPI_METHOD(batch_put) {
  NAPI_ARGV(3);
  NAPI_BATCH_CONTEXT();

  leveldb::Slice key = ToSlice(env, argv[1]);
  leveldb::Slice value = ToSlice(env, argv[2]);

  batch->Put(key, value);

  // TODO clean up slices
  //DisposeStringOrBufferFromSlice(keyBuffer, key);
  //DisposeStringOrBufferFromSlice(valueBuffer, value);

  NAPI_RETURN_UNDEFINED();
}

/**
 * Adds a delete instruction to a batch object.
 */
NAPI_METHOD(batch_del) {
  NAPI_ARGV(2);
  NAPI_BATCH_CONTEXT();

  leveldb::Slice key = ToSlice(env, argv[1]);

  batch->Del(key);

  // TODO clean up slice
  //DisposeStringOrBufferFromSlice(keyBuffer, key);

  NAPI_RETURN_UNDEFINED();
}

/**
 * Clears a batch object.
 */
NAPI_METHOD(batch_clear) {
  NAPI_ARGV(1);
  NAPI_BATCH_CONTEXT();

  batch->Clear();

  NAPI_RETURN_UNDEFINED();
}

/**
 * Worker class for batch write operation.
 */
struct BatchWriteWorker : public BaseWorker {
  BatchWriteWorker (napi_env env,
                    Batch* batch,
                    napi_value callback,
                    bool sync)
    : BaseWorker(env, batch->database_, callback, "leveldown.batch.write"),
      batch_(batch),
      sync_(sync) {}

  virtual ~BatchWriteWorker () {}

  virtual void DoExecute () {
    SetStatus(batch_->Write(sync_));
  }

  Batch* batch_;
  bool sync_;
};

/**
 * Writes a batch object.
 */
NAPI_METHOD(batch_write) {
  NAPI_ARGV(3);
  NAPI_BATCH_CONTEXT();

  napi_value options = argv[1];
  bool sync = BooleanProperty(env, options, "sync", false);
  napi_value callback = argv[2];

  BatchWriteWorker* worker  = new BatchWriteWorker(env, batch, callback, sync);
  worker->Queue();

  NAPI_RETURN_UNDEFINED();
}

/**
 * All exported functions.
 */
NAPI_INIT() {
  /**
   * Database related functions.
   */
  NAPI_EXPORT_FUNCTION(db_init);
  NAPI_EXPORT_FUNCTION(db_open);
  NAPI_EXPORT_FUNCTION(db_close);
  NAPI_EXPORT_FUNCTION(db_put);
  NAPI_EXPORT_FUNCTION(db_get);
  NAPI_EXPORT_FUNCTION(db_del);

  /**
   * Iterator related functions.
   */
  NAPI_EXPORT_FUNCTION(iterator_init);
  NAPI_EXPORT_FUNCTION(iterator_seek);
  NAPI_EXPORT_FUNCTION(iterator_end);
  NAPI_EXPORT_FUNCTION(iterator_next);

  /**
   * Batch related functions.
   */
  NAPI_EXPORT_FUNCTION(batch_do);
  NAPI_EXPORT_FUNCTION(batch_init);
  NAPI_EXPORT_FUNCTION(batch_put);
  NAPI_EXPORT_FUNCTION(batch_del);
  NAPI_EXPORT_FUNCTION(batch_clear);
  NAPI_EXPORT_FUNCTION(batch_write);
}
