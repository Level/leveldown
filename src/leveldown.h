/* Copyright (c) 2012-2013 LevelDOWN contributors
 * See list at <https://github.com/rvagg/node-leveldown#contributing>
 * MIT +no-false-attribs License <https://github.com/rvagg/node-leveldown/blob/master/LICENSE>
 */

#ifndef LU_LEVELDOWN_H
#define LU_LEVELDOWN_H

#define LU_STR(key) \
  static Persistent<String> str_ ## key = Persistent<String>::New(String::New(#key));

#define LU_OPTION(key) \
  static Persistent<String> option_ ## key = Persistent<String>::New(String::New(#key));

#define LU_V8_METHOD(name) \
  static v8::Handle<v8::Value> name (const v8::Arguments& args);

#define CB_ERR_IF_NULL_OR_UNDEFINED(index, name) \
  if (args[index]->IsNull() || args[index]->IsUndefined()) { \
    Local<Value> argv[] = { \
      Local<Value>::New(Exception::Error(String::New("#name cannot be `null` or `undefined`"))) \
    }; \
    RunCallback(callback, argv, 1); \
    return Undefined(); \
  }

#define FROM_V8_STRING(to, from) \
  size_t to ## Sz_; \
  char* to; \
  Local<String> to ## Str = from->ToString(); \
  to ## Sz_ = to ## Str->Utf8Length(); \
  to = new char[to ## Sz_ + 1]; \
  to ## Str->WriteUtf8(to, -1, NULL, String::NO_OPTIONS);

#define STRING_OR_BUFFER_TO_SLICE(to, from) \
  size_t to ## Sz_; \
  char* to ## Ch_; \
  if (Buffer::HasInstance(from->ToObject())) { \
    to ## Sz_ = Buffer::Length(from->ToObject()); \
    to ## Ch_ = Buffer::Data(from->ToObject()); \
  } else { \
    Local<String> to ## Str = from->ToString(); \
    to ## Sz_ = to ## Str->Utf8Length(); \
    to ## Ch_ = new char[to ## Sz_]; \
    to ## Str->WriteUtf8(to ## Ch_, -1, NULL, String::NO_NULL_TERMINATION); \
  } \
  Slice to(to ## Ch_, to ## Sz_);

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

#define RUN_CALLBACK(callback, length, argv) \
  TryCatch try_catch; \
  callback->Call(Context::GetCurrent()->Global(), length, argv); \
  if (try_catch.HasCaught()) { \
    FatalException(try_catch); \
  }

#define THROW_RETURN(msg) \
  ThrowException(Exception::Error(String::New(#msg))); \
  return Undefined();

/* METHOD_SETUP_COMMON setup the following objects:
 *  - Database* database
 *  - Local<Object> optionsObj (may be empty)
 *  - Persistent<Function> callback (won't be empty)
 * Will THROW_RETURN if there isn't a callback in arg 0 or 1
 */
#define METHOD_SETUP_COMMON(name, optionPos, callbackPos) \
  if (args.Length() == 0) { \
    THROW_RETURN(name() requires a callback argument 2) \
  } \
  Database* database = ObjectWrap::Unwrap<Database>(args.This()); \
  Local<Object> optionsObj; \
  Persistent<Function> callback; \
  if (optionPos == -1) { \
    callback = Persistent<Function>::New(Local<Function>::Cast(args[callbackPos])); \
  } else if (args[callbackPos - 1]->IsFunction()) { \
    callback = Persistent<Function>::New(Local<Function>::Cast(args[callbackPos - 1])); \
  } else if (args[optionPos]->IsObject()) { \
    optionsObj = Local<Object>::Cast(args[optionPos]); \
    callback = Persistent<Function>::New(Local<Function>::Cast(args[callbackPos])); \
  } else { \
    THROW_RETURN(name() requires a callback argument 3) \
  }

#define METHOD_SETUP_COMMON_ONEARG(name) \
  if (!args[0]->IsFunction()) { \
    THROW_RETURN(name() requires a callback argument 1) \
  } \
  METHOD_SETUP_COMMON(name, -1, 0)

void RunCallback (v8::Persistent<v8::Function> callback, v8::Local<v8::Value> argv[], int length);

#endif
