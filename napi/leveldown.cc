#include "napi.h"
#include <leveldb/db.h>
#include <leveldb/cache.h>
#include <leveldb/filter_policy.h>

/**
 * Owns the LevelDB storage together with cache and filter
 * policy.
 */
struct DbContext {
  DbContext (napi_env env)
    : env_(env), db_(NULL), blockCache_(NULL), filterPolicy_(NULL) {}

  ~DbContext () {
    if (db_ != NULL) {
      delete db_;
      db_ = NULL;
    }
  }

  napi_env env_;
  leveldb::DB* db_;
  leveldb::Cache* blockCache_;
  const leveldb::FilterPolicy* filterPolicy_;
};

/**
 * Runs when a DbContext is garbage collected.
 */
static void FinalizeDbContext(napi_env env, void* data, void* hint) {
  if (data) {
    delete (DbContext*)data;
  }
}

/**
 * Returns a context object for a database.
 * E.g. `var dbContext = leveldown()`
 */
NAPI_METHOD(leveldown) {
  DbContext* dbContext = new DbContext(env);

  napi_value result;
  NAPI_STATUS_THROWS(napi_create_external(env, dbContext,
                                          FinalizeDbContext,
                                          NULL, &result));
  return result;
}

// TODO BooleanProperty() and Uint32Property() can be refactored

/**
 * Returns a boolean property 'key' from 'obj'.
 * Returns 'DEFAULT' if the property doesn't exist.
 */
static bool BooleanProperty(napi_env env,
                            napi_value obj,
                            const char* key,
                            bool DEFAULT) {
  bool exists = false;
  napi_value _key;
  napi_create_string_utf8(env, key, strlen(key), &_key);
  napi_has_property(env, obj, _key, &exists);

  if (exists) {
    napi_value value;
    napi_get_property(env, obj, _key, &value);
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
  bool exists = false;
  napi_value _key;
  napi_create_string_utf8(env, key, strlen(key), &_key);
  napi_has_property(env, obj, _key, &exists);

  if (exists) {
    napi_value value;
    napi_get_property(env, obj, _key, &value);
    uint32_t result;
    napi_get_value_uint32(env, value, &result);
    return result;
  }

  return DEFAULT;
}

/**
 * Base worker class.
 */
struct BaseWorker {
  BaseWorker(napi_env env,
             DbContext* dbContext,
             napi_value callback,
             const char* resourceName)
    : env_(env), dbContext_(dbContext) {
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
  DbContext* dbContext_;
  leveldb::Status status_;
};

/**
 * Worker class for opening the database
 */
struct OpenWorker : public BaseWorker {
  OpenWorker(napi_env env,
             DbContext* dbContext,
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
    : BaseWorker(env, dbContext, callback, "leveldown::open"),
      location_(location) {
    options_.block_cache = dbContext->blockCache_;
    options_.filter_policy = dbContext->filterPolicy_;
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
    status_ = leveldb::DB::Open(options_, location_, &dbContext_->db_);
  }

  leveldb::Options options_;
  char* location_;
};

/**
 * Opens a database.
 */
NAPI_METHOD(open) {
  NAPI_ARGV(4);
  NAPI_DB_CONTEXT();
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
  dbContext->blockCache_ = leveldb::NewLRUCache(cacheSize);
  dbContext->filterPolicy_ = leveldb::NewBloomFilterPolicy(10);

  napi_value callback = argv[3];
  OpenWorker* worker = new OpenWorker(env,
                                      dbContext,
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
 * Returns true if 'value' is a buffer otherwise false.
 */
static bool isBuffer(napi_env env, napi_value value) {
  bool isBuffer;
  napi_is_buffer(env, value, &isBuffer);
  return isBuffer;
}

/**
 * Convert a napi_value to a leveldb::Slice.
 */
static leveldb::Slice ToSlice(napi_env env, napi_value from) {
  // size_t to ## Sz_;
  // char* to ## Ch_;
  // if (from->IsNull() || from->IsUndefined()) {
  //   to ## Sz_ = 0;
  //   to ## Ch_ = 0;
  // } else if (!from->ToObject().IsEmpty()
  //     && node::Buffer::HasInstance(from->ToObject())) {
  //   to ## Sz_ = node::Buffer::Length(from->ToObject());
  //   to ## Ch_ = node::Buffer::Data(from->ToObject());
  // } else {
  //   v8::Local<v8::String> to ## Str = from->ToString();
  //   to ## Sz_ = to ## Str->Utf8Length();
  //   to ## Ch_ = new char[to ## Sz_];
  //   to ## Str->WriteUtf8(to ## Ch_, -1, NULL, v8::String::NO_NULL_TERMINATION);
  // }
  // leveldb::Slice to(to ## Ch_, to ## Sz_);
  size_t toLength = 0;
  char* toChar = 0;

  napi_valuetype type;
  napi_typeof(env, from, &type);

  if (type == napi_null) {
    printf("FROM is null\n");
  } else if (type == napi_undefined) {
    printf("FROM is undefined\n");
  }

  if (type != napi_null && type != napi_undefined) {
    if (type == napi_string) {
      printf("FROM is a string\n");
      size_t size = 0;
      napi_get_value_string_utf8(env, from, NULL, 0, &size);
      if (size > 0) {
        toChar = (char*)malloc((size + 1) * sizeof(char));
        napi_get_value_string_utf8(env, from, toChar, size + 1, &toLength);
        toChar[toLength] = '\0';
      }
    } else if (isBuffer(env, from)) {
      printf("FROM is a buffer\n");
      // TODO what to do with buffers? we could either always
      // copy them, or use an out parameter to store the fact
      // that we did allocate and only clean in that case
    }
  }

  return leveldb::Slice(toChar, toLength);
}

/**
 * Worker class for putting key/value to the database
 */
struct PutWorker : public BaseWorker {
  PutWorker(napi_env env,
            DbContext* dbContext,
            napi_value callback,
            napi_value key,
            napi_value value,
            bool sync)
    : BaseWorker(env, dbContext, callback, "leveldown::put"),
      key_(ToSlice(env, key)), value_(ToSlice(env, value)) {
    options_.sync = sync;
  }

  ~PutWorker() {
    // TODO clean up key_ and value_ if they aren't empty?
    // See DisposeStringOrBufferFromSlice()
  }

  virtual void DoExecute() {
    status_ = dbContext_->db_->Put(options_, key_, value_);
  }

  leveldb::WriteOptions options_;
  leveldb::Slice key_;
  leveldb::Slice value_;
};

/**
 * Puts a key and a value to the database.
 */
NAPI_METHOD(put) {
  NAPI_ARGV(5);
  NAPI_DB_CONTEXT();

  napi_value key = argv[1];
  napi_value value = argv[2];
  bool sync = BooleanProperty(env, argv[3], "sync", false);
  napi_value callback = argv[4];
  PutWorker* worker = new PutWorker(env,
                                    dbContext,
                                    callback,
                                    key,
                                    value,
                                    sync);
  worker->Queue();
  NAPI_RETURN_UNDEFINED();
}

NAPI_INIT() {
  NAPI_EXPORT_FUNCTION(leveldown);
  NAPI_EXPORT_FUNCTION(open);
  NAPI_EXPORT_FUNCTION(put);
}
