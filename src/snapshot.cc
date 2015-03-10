/* Copyright (c) 2012-2015 LevelDOWN contributors
 * See list at <https://github.com/rvagg/node-leveldown#contributing>
 * MIT License <https://github.com/rvagg/node-leveldown/blob/master/LICENSE.md>
 */

#include <node.h>
#include <node_buffer.h>

#include "database.h"
#include "snapshot.h"

namespace leveldown {


static v8::Persistent<v8::FunctionTemplate> snapshot_constructor;

Snapshot::Snapshot (
    Database* database
) : database(database)
  , dbSnapshot(database->NewSnapshot())
{
  NanScope();

  v8::Local<v8::Object> obj = NanNew<v8::Object>();
  NanAssignPersistent(persistentHandle, obj);
}

Snapshot::~Snapshot () {
  database->ReleaseSnapshot(dbSnapshot); // TODO: is this safe?
  if (!persistentHandle.IsEmpty())
    NanDisposePersistent(persistentHandle);
}

void Snapshot::Init () {
  v8::Local<v8::FunctionTemplate> tpl =
      NanNew<v8::FunctionTemplate>(Snapshot::New);
  NanAssignPersistent(snapshot_constructor, tpl);
  tpl->SetClassName(NanNew("Snapshot"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
}

v8::Local<v8::Object> Snapshot::NewInstance (
        v8::Local<v8::Object> database
      , v8::Local<v8::Object> optionsObj
    ) {
  NanEscapableScope();

  v8::Local<v8::Object> instance;
  v8::Local<v8::FunctionTemplate> constructorHandle =
      NanNew<v8::FunctionTemplate>(snapshot_constructor);

  if (optionsObj.IsEmpty()) {
    v8::Handle<v8::Value> argv[1] = { database };
    instance = constructorHandle->GetFunction()->NewInstance(1, argv);
  } else {
    v8::Handle<v8::Value> argv[2] = { database, optionsObj };
    instance = constructorHandle->GetFunction()->NewInstance(2, argv);
  }

  return NanEscapeScope(instance);
}

bool Snapshot::HasInstance (
        v8::Handle<v8::Value> value
    ) {
  return NanHasInstance(snapshot_constructor, value);
}

NAN_METHOD(Snapshot::New) {
  NanScope();

  Database* database = node::ObjectWrap::Unwrap<Database>(args[0]->ToObject());

  Snapshot* snapshot = new Snapshot(
      database
  );
  snapshot->Wrap(args.This());

  NanReturnValue(args.This());
}


} // namespace leveldown
