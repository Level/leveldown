/* Copyright (c) 2012-2013 LevelDOWN contributors
 * See list at <https://github.com/rvagg/node-leveldown#contributing>
 * MIT +no-false-attribs License <https://github.com/rvagg/node-leveldown/blob/master/LICENSE>
 */
#ifndef LD_LEVELDOWN_H
#define LD_LEVELDOWN_H

#define LD_SYMBOL(var, key) \
  static const v8::Persistent<v8::String> var = \
    v8::Persistent<v8::String>::New(v8::String::NewSymbol(#key));

#define LD_V8_METHOD(name) \
  static v8::Handle<v8::Value> name (const v8::Arguments& args);

#define LD_CB_ERR_IF_NULL_OR_UNDEFINED(thing, name) \
  if (thing->IsNull() || thing->IsUndefined()) { \
    v8::Local<v8::Value> argv[] = { \
      v8::Local<v8::Value>::New(v8::Exception::Error( \
        v8::String::New(#name " cannot be `null` or `undefined`")) \
      ) \
    }; \
    LD_RUN_CALLBACK(callback, argv, 1); \
    return v8::Undefined(); \
  }

#define LD_FROM_V8_STRING(to, from) \
  size_t to ## Sz_; \
  char* to; \
  v8::Local<v8::String> to ## Str = from->ToString(); \
  to ## Sz_ = to ## Str->Utf8Length(); \
  to = new char[to ## Sz_ + 1]; \
  to ## Str->WriteUtf8(to, -1, NULL, v8::String::NO_OPTIONS);

#define LD_STRING_OR_BUFFER_TO_SLICE(to, from, name) \
  size_t to ## Sz_; \
  char* to ## Ch_; \
  if (node::Buffer::HasInstance(from->ToObject())) { \
    to ## Sz_ = node::Buffer::Length(from->ToObject()); \
    if (to ## Sz_ == 0) { \
      LD_RETURN_CALLBACK_OR_ERROR(callback, #name " argument cannot be an empty Buffer") \
    } \
    to ## Ch_ = node::Buffer::Data(from->ToObject()); \
  } else { \
    v8::Local<v8::String> to ## Str = from->ToString(); \
    to ## Sz_ = to ## Str->Utf8Length(); \
    if (to ## Sz_ == 0) { \
      LD_RETURN_CALLBACK_OR_ERROR(callback, #name " argument cannot be an empty String") \
    } \
    to ## Ch_ = new char[to ## Sz_]; \
    to ## Str->WriteUtf8( \
        to ## Ch_ \
      , -1 \
      , NULL, v8::String::NO_NULL_TERMINATION \
    ); \
  } \
  leveldb::Slice to(to ## Ch_, to ## Sz_);

#define LD_BOOLEAN_OPTION_VALUE(optionsObj, opt) \
  bool opt = !optionsObj.IsEmpty() \
    && optionsObj->Has(option_ ## opt) \
    && optionsObj->Get(option_ ## opt)->BooleanValue();

#define LD_BOOLEAN_OPTION_VALUE_DEFTRUE(optionsObj, opt) \
  bool opt = optionsObj.IsEmpty() \
    || !optionsObj->Has(option_ ## opt) \
    || optionsObj->Get(option_ ## opt)->BooleanValue();

#define LD_UINT32_OPTION_VALUE(optionsObj, opt, default) \
  uint32_t opt = !optionsObj.IsEmpty() \
    && optionsObj->Has(option_ ## opt) \
    && optionsObj->Get(option_ ## opt)->IsUint32() \
      ? optionsObj->Get(option_ ## opt)->Uint32Value() \
      : default;

#define LD_RETURN_CALLBACK_OR_ERROR(callback, msg) \
  if (callback->IsFunction()) { \
    v8::Local<v8::Value> argv[] = { \
      v8::Local<v8::Value>::New(v8::Exception::Error( \
        v8::String::New(msg)) \
      ) \
    }; \
    LD_RUN_CALLBACK(callback, argv, 1) \
    return v8::Undefined(); \
  } \
  LD_THROW_RETURN(msg)

#define LD_RUN_CALLBACK(callback, argv, length) \
  v8::TryCatch try_catch; \
  callback->Call(v8::Context::GetCurrent()->Global(), length, argv); \
  if (try_catch.HasCaught()) { \
    node::FatalException(try_catch); \
  }

#define LD_THROW_RETURN(msg) \
  v8::ThrowException(v8::Exception::Error(v8::String::New(#msg))); \
  return v8::Undefined();

/* LD_METHOD_SETUP_COMMON setup the following objects:
 *  - Database* database
 *  - v8::Local<v8::Object> optionsObj (may be empty)
 *  - v8::Persistent<v8::Function> callback (won't be empty)
 * Will LD_THROW_RETURN if there isn't a callback in arg 0 or 1
 */
#define LD_METHOD_SETUP_COMMON(name, optionPos, callbackPos) \
  if (args.Length() == 0) { \
    LD_THROW_RETURN(name() requires a callback argument) \
  } \
  Database* database = ObjectWrap::Unwrap<Database>(args.This()); \
  v8::Local<v8::Object> optionsObj; \
  v8::Persistent<v8::Function> callback; \
  if (optionPos == -1 && args[callbackPos]->IsFunction()) { \
    callback = v8::Persistent<v8::Function>::New( \
      v8::Local<v8::Function>::Cast(args[callbackPos]) \
    ); \
  } else if (optionPos != -1 && args[callbackPos - 1]->IsFunction()) { \
    callback = v8::Persistent<v8::Function>::New( \
      v8::Local<v8::Function>::Cast(args[callbackPos - 1]) \
    ); \
  } else if (optionPos != -1 \
        && args[optionPos]->IsObject() \
        && args[callbackPos]->IsFunction()) { \
    optionsObj = v8::Local<v8::Object>::Cast(args[optionPos]); \
    callback = v8::Persistent<v8::Function>::New( \
      v8::Local<v8::Function>::Cast(args[callbackPos]) \
    ); \
  } else { \
    LD_THROW_RETURN(name() requires a callback argument) \
  }

#define LD_METHOD_SETUP_COMMON_ONEARG(name) LD_METHOD_SETUP_COMMON(name, -1, 0)

#endif
