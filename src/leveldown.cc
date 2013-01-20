/* Copyright (c) 2012-2013 LevelDOWN contributors
 * See list at <https://github.com/rvagg/node-leveldown#contributing>
 * MIT +no-false-attribs License <https://github.com/rvagg/node-leveldown/blob/master/LICENSE>
 */

#include <node.h>

#include "leveldown.h"
#include "database.h"
#include "iterator.h"

using namespace v8;
using namespace node;
using namespace leveldown;

void Init (Handle<Object> target) {
  Database::Init();
  leveldown::Iterator::Init();

  target->Set(String::NewSymbol("createDatabase"), FunctionTemplate::New(CreateDatabase)->GetFunction());
  target->Set(String::NewSymbol("createIterator"), FunctionTemplate::New(CreateIterator)->GetFunction());
}

NODE_MODULE(leveldown, Init)

// util

void RunCallback (Persistent<Function> callback, Local<Value> argv[], int length) {
  TryCatch try_catch;
 
  callback->Call(Context::GetCurrent()->Global(), length, argv);
  if (try_catch.HasCaught()) {
    FatalException(try_catch);
  }
}
