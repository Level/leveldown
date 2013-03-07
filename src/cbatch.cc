#include "cbatch.h"

using namespace v8;
using namespace node;

Persistent<FunctionTemplate> CBatch::constructor;

void CBatch::Initialize(v8::Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  constructor = Persistent<FunctionTemplate>::New(tpl);
  constructor->InstanceTemplate()->SetInternalFieldCount(1);
  constructor->SetClassName(String::NewSymbol("CBatch"));

  SetPrototypeMethod(constructor, "put", Put);
  SetPrototypeMethod(constructor, "del", Del);
  SetPrototypeMethod(constructor, "clear", Clear);

  target->Set(String::NewSymbol("CBatch"), constructor->GetFunction());
}

Handle<Value> CBatch::New(const Arguments& args) {
  HandleScope scope;

  CBatch *wrapper = new CBatch();
  wrapper->Wrap(args.Holder());
  return scope.Close(args.Holder());
}

/**
 * Put
 *
 * @param {string} key
 * @param {string} val
 * @returns {object} CBatch
 */

Handle<Value> CBatch::Put(const Arguments& args) {
  HandleScope scope;
  CBatch* self = ObjectWrap::Unwrap<CBatch>(args.Holder());

  if (args.Length() < 2) {
    ThrowException(Exception::Error(String::New("Key and Value required")));
    return scope.Close(Undefined());
  }
  if (!args[1]->IsString()) {
    ThrowException(Exception::Error(String::New("Only strings as value")));
    return scope.Close(Undefined());
  }

  String::Utf8Value key(args[0]->ToString());
  String::Utf8Value val(args[1]->ToString());

  self->batch.Put(*key, *val);

  return scope.Close(args.Holder());
}

/**
 * Delete
 *
 * @param {string} key
 * @returns {object} CBatch
 */

Handle<Value> CBatch::Del(const Arguments& args) {
  HandleScope scope;
  CBatch* self = ObjectWrap::Unwrap<CBatch>(args.Holder());

  if (args.Length() == 0) {
    ThrowException(Exception::Error(String::New("key required")));
    return scope.Close(Undefined());
  }

  String::Utf8Value key(args[0]->ToString());
  self->batch.Delete(*key);

  return scope.Close(args.Holder());
}

/**
 * Clear
 *
 * @returns {object} CBatch
 */

Handle<Value> CBatch::Clear(const Arguments& args) {
  HandleScope scope;
  CBatch* self = ObjectWrap::Unwrap<CBatch>(args.Holder());

  self->batch.Clear();

  return scope.Close(args.Holder());
}

/**
 * CBatch constructor
 */

CBatch::CBatch() {}

CBatch::~CBatch() {}
