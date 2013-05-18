/* Copyright (c) 2012-2013 LevelDOWN contributors
 * See list at <https://github.com/rvagg/node-leveldown#contributing>
 * MIT +no-false-attribs License <https://github.com/rvagg/node-leveldown/blob/master/LICENSE>
 */
#ifndef LD_LEVELDOWN_H
#define LD_LEVELDOWN_H

#include <node.h>
#include <node_buffer.h>
#include <leveldb/slice.h>

static inline char* FromV8String(v8::Local<v8::Value> from) {
  size_t sz_;
  char* to;
  v8::Local<v8::String> toStr = from->ToString();
  sz_ = toStr->Utf8Length();
  to = new char[sz_ + 1];
  toStr->WriteUtf8(to, -1, NULL, v8::String::NO_OPTIONS);
  return to;
}

static inline size_t StringOrBufferLength(v8::Local<v8::Value> obj) {
  return node::Buffer::HasInstance(obj->ToObject())
    ? node::Buffer::Length(obj->ToObject())
    : obj->ToString()->Utf8Length();
}

static inline bool BooleanOptionValue(
      v8::Local<v8::Object> optionsObj
    , v8::Handle<v8::String> opt) {

  return !optionsObj.IsEmpty()
    && optionsObj->Has(opt)
    && optionsObj->Get(opt)->BooleanValue();
}

static inline bool BooleanOptionValueDefTrue(
      v8::Local<v8::Object> optionsObj
    , v8::Handle<v8::String> opt) {

  return optionsObj.IsEmpty()
    || !optionsObj->Has(opt)
    || optionsObj->Get(opt)->BooleanValue();
}

static inline uint32_t UInt32OptionValue(
      v8::Local<v8::Object> optionsObj
    , v8::Handle<v8::String> opt
    , uint32_t def) {

  return !optionsObj.IsEmpty()
    && optionsObj->Has(opt)
    && optionsObj->Get(opt)->IsUint32()
      ? optionsObj->Get(opt)->Uint32Value()
      : def;
}

// node_isolate stuff introduced with V8 upgrade, see https://github.com/joyent/node/pull/5077
#if NODE_MODULE_VERSION > 0x000B
#  define LD_NODE_ISOLATE node::node_isolate
#  define LD_NODE_ISOLATE_PRE node::node_isolate, 
#  define LD_NODE_ISOLATE_POST , node::node_isolate 
#else
#  define LD_NODE_ISOLATE
#  define LD_NODE_ISOLATE_PRE
#  define LD_NODE_ISOLATE_POST
#endif

#define LD_SYMBOL(var, key)                                                    \
  static const v8::Persistent<v8::String> var =                                \
    v8::Persistent<v8::String>::New(                                           \
      LD_NODE_ISOLATE_PRE v8::String::NewSymbol(#key)                          \
    );

#define LD_V8_METHOD(name)                                                     \
  static v8::Handle<v8::Value> name (const v8::Arguments& args);

#define LD_CB_ERR_IF_NULL_OR_UNDEFINED(thing, name)                            \
  if (thing->IsNull() || thing->IsUndefined()) {                               \
    LD_RETURN_CALLBACK_OR_ERROR(callback, #name " cannot be `null` or `undefined`") \
  }

// NOTE: this MUST be called on objects created by
// LD_STRING_OR_BUFFER_TO_SLICE
static inline void DisposeStringOrBufferFromSlice(v8::Persistent<v8::Value> ptr
      , leveldb::Slice slice) {

  if (!node::Buffer::HasInstance(ptr))
    delete[] slice.data();
  ptr.Dispose(LD_NODE_ISOLATE);
}

// NOTE: must call DisposeStringOrBufferFromSlice() on objects created here
#define LD_STRING_OR_BUFFER_TO_SLICE(to, from, name)                           \
  size_t to ## Sz_;                                                            \
  char* to ## Ch_;                                                             \
  if (node::Buffer::HasInstance(from->ToObject())) {                           \
    to ## Sz_ = node::Buffer::Length(from->ToObject());                        \
    if (to ## Sz_ == 0) {                                                      \
      LD_RETURN_CALLBACK_OR_ERROR(callback, #name " cannot be an empty Buffer") \
    }                                                                          \
    to ## Ch_ = node::Buffer::Data(from->ToObject());                          \
  } else {                                                                     \
    v8::Local<v8::String> to ## Str = from->ToString();                        \
    to ## Sz_ = to ## Str->Utf8Length();                                       \
    if (to ## Sz_ == 0) {                                                      \
      LD_RETURN_CALLBACK_OR_ERROR(callback, #name " cannot be an empty String") \
    }                                                                          \
    to ## Ch_ = new char[to ## Sz_];                                           \
    to ## Str->WriteUtf8(                                                      \
        to ## Ch_                                                              \
      , -1                                                                     \
      , NULL, v8::String::NO_NULL_TERMINATION                                  \
    );                                                                         \
  }                                                                            \
  leveldb::Slice to(to ## Ch_, to ## Sz_);

#define LD_RETURN_CALLBACK_OR_ERROR(callback, msg)                             \
  if (!callback.IsEmpty() && callback->IsFunction()) {                         \
    v8::Local<v8::Value> argv[] = {                                            \
      v8::Local<v8::Value>::New(v8::Exception::Error(                          \
        v8::String::New(msg))                                                  \
      )                                                                        \
    };                                                                         \
    LD_RUN_CALLBACK(callback, argv, 1)                                         \
    return v8::Undefined();                                                    \
  }                                                                            \
  v8::ThrowException(v8::Exception::Error(v8::String::New(msg)));              \
  return v8::Undefined();

#define LD_RUN_CALLBACK(callback, argv, length)                                \
  v8::TryCatch try_catch;                                                      \
  callback->Call(v8::Context::GetCurrent()->Global(), length, argv);           \
  if (try_catch.HasCaught()) {                                                 \
    node::FatalException(try_catch);                                           \
  }

#define LD_THROW_RETURN(...)                                                   \
  v8::ThrowException(v8::Exception::Error(v8::String::New(#__VA_ARGS__)));     \
  return v8::Undefined();

/* LD_METHOD_SETUP_COMMON setup the following objects:
 *  - Database* database
 *  - v8::Local<v8::Object> optionsObj (may be empty)
 *  - v8::Persistent<v8::Function> callback (won't be empty)
 * Will LD_THROW_RETURN if there isn't a callback in arg 0 or 1
 */
#define LD_METHOD_SETUP_COMMON(name, optionPos, callbackPos)                   \
  if (args.Length() == 0) {                                                    \
    LD_THROW_RETURN(name() requires a callback argument)                       \
  }                                                                            \
  leveldown::Database* database =                                              \
    node::ObjectWrap::Unwrap<leveldown::Database>(args.This());                \
  v8::Local<v8::Object> optionsObj;                                            \
  v8::Persistent<v8::Function> callback;                                       \
  if (optionPos == -1 && args[callbackPos]->IsFunction()) {                    \
    callback = v8::Persistent<v8::Function>::New(                              \
      LD_NODE_ISOLATE_PRE                                                      \
      v8::Local<v8::Function>::Cast(args[callbackPos])                         \
    );                                                                         \
  } else if (optionPos != -1 && args[callbackPos - 1]->IsFunction()) {         \
    callback = v8::Persistent<v8::Function>::New(                              \
      LD_NODE_ISOLATE_PRE                                                      \
      v8::Local<v8::Function>::Cast(args[callbackPos - 1])                     \
    );                                                                         \
  } else if (optionPos != -1                                                   \
        && args[optionPos]->IsObject()                                         \
        && args[callbackPos]->IsFunction()) {                                  \
    optionsObj = v8::Local<v8::Object>::Cast(args[optionPos]);                 \
    callback = v8::Persistent<v8::Function>::New(                              \
      LD_NODE_ISOLATE_PRE                                                      \
      v8::Local<v8::Function>::Cast(args[callbackPos])                         \
    );                                                                         \
  } else {                                                                     \
    LD_THROW_RETURN(name() requires a callback argument)                       \
  }

#define LD_METHOD_SETUP_COMMON_ONEARG(name) LD_METHOD_SETUP_COMMON(name, -1, 0)

#endif
