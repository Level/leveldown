/* Copyright (c) 2012-2017 LevelDOWN contributors
 * See list at <https://github.com/level/leveldown#contributing>
 * MIT License <https://github.com/level/leveldown/blob/master/LICENSE.md>
 */

#ifndef LD_COMMON_H
#define LD_COMMON_H

#include <nan.h>

namespace leveldown {

NAN_INLINE bool BooleanOptionValue(v8::Local<v8::Object> options,
                                   const char* _key,
                                   bool def = false) {
  Nan::HandleScope scope;
  v8::Local<v8::String> key = Nan::New(_key).ToLocalChecked();
  return !options.IsEmpty()
    && options->Has(key)
    ? options->Get(key)->BooleanValue()
    : def;
}

NAN_INLINE uint32_t UInt32OptionValue(v8::Local<v8::Object> options,
                                      const char* _key,
                                      uint32_t def) {
  Nan::HandleScope scope;
  v8::Local<v8::String> key = Nan::New(_key).ToLocalChecked();
  return !options.IsEmpty()
    && options->Has(key)
    && options->Get(key)->IsNumber()
    ? options->Get(key)->Uint32Value()
    : def;
}

} // namespace leveldown

#endif
