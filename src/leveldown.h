/* Copyright (c) 2012-2013 LevelDOWN contributors
 * See list at <https://github.com/rvagg/node-leveldown#contributing>
 * MIT +no-false-attribs License <https://github.com/rvagg/node-leveldown/blob/master/LICENSE>
 */
#ifndef LD_LEVELDOWN_H
#define LD_LEVELDOWN_H

#include <node.h>
#include <node_buffer.h>
#include <leveldb/slice.h>

#include "nan.h"

static inline size_t StringOrBufferLength(v8::Local<v8::Value> obj) {
  Nan::HandleScope scope;

  return (!obj->ToObject().IsEmpty()
    && node::Buffer::HasInstance(obj->ToObject()))
    ? node::Buffer::Length(obj->ToObject())
    : obj->ToString()->Utf8Length();
}

// NOTE: this MUST be called on objects created by
// LD_STRING_OR_BUFFER_TO_SLICE
static inline void DisposeStringOrBufferFromSlice(
        Nan::Persistent<v8::Object> &handle
      , leveldb::Slice slice) {
  Nan::HandleScope scope;

  if (!slice.empty()) {
    v8::Local<v8::Value> obj = Nan::New<v8::Object>(handle)->Get(Nan::New<v8::String>("obj").ToLocalChecked());
    if (!node::Buffer::HasInstance(obj))
      delete[] slice.data();
  }

  handle.Reset();
}

static inline void DisposeStringOrBufferFromSlice(
        v8::Local<v8::Value> handle
      , leveldb::Slice slice) {

  if (!slice.empty() && !node::Buffer::HasInstance(handle))
    delete[] slice.data();
}

#define LD_CB_ERR_IF_NULL_OR_UNDEFINED(thing, name)                            \
  if (thing->IsNull() || thing->IsUndefined()) {                               \
    LD_RETURN_CALLBACK_OR_ERROR(callback, #name " cannot be `null` or `undefined`") \
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
      Nan::Error(msg)                                                          \
    };                                                                         \
    LD_RUN_CALLBACK(callback, 1, argv)                                         \
    info.GetReturnValue().SetUndefined();                                      \
    return;                                                                    \
  }                                                                            \
  return Nan::ThrowError(msg);

#define LD_RUN_CALLBACK(callback, argc, argv)                                  \
  Nan::MakeCallback(                                                           \
      Nan::GetCurrentContext()->Global(), callback, argc, argv);

/* LD_METHOD_SETUP_COMMON setup the following objects:
 *  - Database* database
 *  - v8::Local<v8::Object> optionsObj (may be empty)
 *  - Nan::Persistent<v8::Function> callback (won't be empty)
 * Will throw/return if there isn't a callback in arg 0 or 1
 */
#define LD_METHOD_SETUP_COMMON(name, optionPos, callbackPos)                   \
  if (info.Length() == 0)                                                      \
    return Nan::ThrowError(#name "() requires a callback argument");           \
  leveldown::Database* database =                                              \
    Nan::ObjectWrap::Unwrap<leveldown::Database>(info.This());                 \
  v8::Local<v8::Object> optionsObj;                                            \
  v8::Local<v8::Function> callback;                                            \
  if (optionPos == -1 && info[callbackPos]->IsFunction()) {                    \
    callback = info[callbackPos].As<v8::Function>();                           \
  } else if (optionPos != -1 && info[callbackPos - 1]->IsFunction()) {         \
    callback = info[callbackPos - 1].As<v8::Function>();                       \
  } else if (optionPos != -1                                                   \
        && info[optionPos]->IsObject()                                         \
        && info[callbackPos]->IsFunction()) {                                  \
    optionsObj = info[optionPos].As<v8::Object>();                             \
    callback = info[callbackPos].As<v8::Function>();                           \
  } else {                                                                     \
    return Nan::ThrowError(#name "() requires a callback argument");           \
  }

#define LD_METHOD_SETUP_COMMON_ONEARG(name) LD_METHOD_SETUP_COMMON(name, -1, 0)

#endif
