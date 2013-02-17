/* Copyright (c) 2012-2013 LevelDOWN contributors
 * See list at <https://github.com/rvagg/node-leveldown#contributing>
 * MIT +no-false-attribs License <https://github.com/rvagg/node-leveldown/blob/master/LICENSE>
 */

#ifndef LU_LEVELDOWN_H
#define LU_LEVELDOWN_H

#define LU_STR(key) \
  static v8::Persistent<v8::String> str_ ## key = \
    v8::Persistent<v8::String>::New(v8::String::New(#key));

#define LU_OPTION(key) \
  static v8::Persistent<v8::String> option_ ## key = \
    v8::Persistent<v8::String>::New(v8::String::New(#key));

#define LU_V8_METHOD(name) \
  static v8::Handle<v8::Value> name (const v8::Arguments& args);

#define CB_ERR_IF_NULL_OR_UNDEFINED(index, name) \
  if (args[index]->IsNull() || args[index]->IsUndefined()) { \
    v8::Local<v8::Value> argv[] = { \
      v8::Local<v8::Value>::New(v8::Exception::Error( \
        v8::String::New(#name " argument cannot be `null` or `undefined`")) \
      ) \
    }; \
    RUN_CALLBACK(callback, argv, 1); \
    return v8::Undefined(); \
  }

#define FROM_V8_STRING(to, from) \
  size_t to ## Sz_; \
  char* to; \
  v8::Local<v8::String> to ## Str = from->ToString(); \
  to ## Sz_ = to ## Str->Utf8Length(); \
  to = new char[to ## Sz_ + 1]; \
  to ## Str->WriteUtf8(to, -1, NULL, v8::String::NO_OPTIONS);

#define STRING_OR_BUFFER_TO_SLICE(to, from, name) \
  size_t to ## Sz_; \
  char* to ## Ch_; \
  if (node::Buffer::HasInstance(from->ToObject())) { \
    to ## Sz_ = node::Buffer::Length(from->ToObject()); \
    if (to ## Sz_ == 0) { \
      v8::Local<v8::Value> argv[] = { \
        v8::Local<v8::Value>::New(v8::Exception::Error( \
          v8::String::New(#name " argument cannot be an empty Buffer")) \
        ) \
      }; \
      RUN_CALLBACK(callback, argv, 1); \
      return v8::Undefined(); \
    } \
    to ## Ch_ = node::Buffer::Data(from->ToObject()); \
  } else { \
    v8::Local<v8::String> to ## Str = from->ToString(); \
    to ## Sz_ = to ## Str->Utf8Length(); \
    if (to ## Sz_ == 0) { \
      v8::Local<v8::Value> argv[] = { \
        v8::Local<v8::Value>::New(v8::Exception::Error( \
          v8::String::New(#name " argument cannot be an empty String")) \
        ) \
      }; \
      RUN_CALLBACK(callback, argv, 1); \
      return v8::Undefined(); \
    } \
    to ## Ch_ = new char[to ## Sz_]; \
    to ## Str->WriteUtf8(to ## Ch_, -1, NULL, v8::String::NO_NULL_TERMINATION); \
  } \
  leveldb::Slice to(to ## Ch_, to ## Sz_);

#define BOOLEAN_OPTION_VALUE(optionsObj, opt) \
  bool opt = !optionsObj.IsEmpty() \
    && optionsObj->Has(option_ ## opt) \
    && optionsObj->Get(option_ ## opt)->BooleanValue();

#define BOOLEAN_OPTION_VALUE_DEFTRUE(optionsObj, opt) \
  bool opt = optionsObj.IsEmpty() \
    || !optionsObj->Has(option_ ## opt) \
    || optionsObj->Get(option_ ## opt)->BooleanValue();

#define UINT32_OPTION_VALUE(optionsObj, opt, default) \
  uint32_t opt = !optionsObj.IsEmpty() \
    && optionsObj->Has(option_ ## opt) \
    && optionsObj->Get(option_ ## opt)->IsUint32() \
      ? optionsObj->Get(option_ ## opt)->Uint32Value() \
      : default;

#define RUN_CALLBACK(callback, argv, length) \
  v8::TryCatch try_catch; \
  callback->Call(v8::Context::GetCurrent()->Global(), length, argv); \
  if (try_catch.HasCaught()) { \
    node::FatalException(try_catch); \
  }

#define THROW_RETURN(msg) \
  v8::ThrowException(v8::Exception::Error(v8::String::New(#msg))); \
  return v8::Undefined();

/* METHOD_SETUP_COMMON setup the following objects:
 *  - Database* database
 *  - v8::Local<v8::Object> optionsObj (may be empty)
 *  - v8::Persistent<v8::Function> callback (won't be empty)
 * Will THROW_RETURN if there isn't a callback in arg 0 or 1
 */
#define METHOD_SETUP_COMMON(name, optionPos, callbackPos) \
  if (args.Length() == 0) { \
    THROW_RETURN(name() requires a callback argument) \
  } \
  Database* database = ObjectWrap::Unwrap<Database>(args.This()); \
  v8::Local<v8::Object> optionsObj; \
  v8::Persistent<v8::Function> callback; \
  if (optionPos == -1) { \
    callback = v8::Persistent<v8::Function>::New( \
      v8::Local<v8::Function>::Cast(args[callbackPos]) \
    ); \
  } else if (args[callbackPos - 1]->IsFunction()) { \
    callback = v8::Persistent<v8::Function>::New( \
      v8::Local<v8::Function>::Cast(args[callbackPos - 1]) \
    ); \
  } else if (args[optionPos]->IsObject() && args[callbackPos]->IsFunction()) { \
    optionsObj = v8::Local<v8::Object>::Cast(args[optionPos]); \
    callback = v8::Persistent<v8::Function>::New( \
      v8::Local<v8::Function>::Cast(args[callbackPos]) \
    ); \
  } else { \
    THROW_RETURN(name() requires a callback argument) \
  }

#define METHOD_SETUP_COMMON_ONEARG(name) \
  if (!args[0]->IsFunction()) { \
    THROW_RETURN(name() requires a callback argument) \
  } \
  METHOD_SETUP_COMMON(name, -1, 0)

#endif
