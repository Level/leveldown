/* Copyright (c) 2012-2013 LevelDOWN contributors
 * See list at <https://github.com/rvagg/node-leveldown#contributing>
 * MIT +no-false-attribs License <https://github.com/rvagg/node-leveldown/blob/master/LICENSE>
 */

#include <node.h>
#include "leveldown.h"
#include "database.h"
#include "iterator.h"

namespace leveldown {

v8::Handle<v8::Value> RegisterPlugin (const v8::Arguments& args) {
  v8::HandleScope scope;

  char symbol[1024], *base, *pos;
  uv_lib_t lib;
  int r;

  if (args.Length() == 0) {
    LD_THROW_RETURN("leveldown._registerPlugin() requires a module location argument")
  }

  v8::String::Utf8Value filename(args[0]); // Cast

  if (uv_dlopen(*filename, &lib)) {
    v8::Local<v8::String> errmsg = v8::String::New(uv_dlerror(&lib));
#ifdef _WIN32
    // Windows needs to add the filename into the error message
    errmsg = v8::String::Concat(errmsg, args[0]->ToString());
#endif
    return v8::ThrowException(v8::Exception::Error(errmsg));
  }

  v8::String::Utf8Value path(args[0]);
  base = *path;

  /* Find the shared library filename within the full path. */
#ifdef __POSIX__
  pos = strrchr(base, '/');
  if (pos != NULL) {
    base = pos + 1;
  }
#else // Windows
  for (;;) {
    pos = strpbrk(base, "\\/:");
    if (pos == NULL) {
      break;
    }
    base = pos + 1;
  }
#endif

  /* Strip the .node extension. */
  pos = strrchr(base, '.');
  if (pos != NULL) {
    *pos = '\0';
  }

  r = snprintf(symbol, sizeof symbol, "%s_plugin", base);
  if (r <= 0 || static_cast<size_t>(r) >= sizeof symbol) {
    v8::Local<v8::Value> exception =
        v8::Exception::Error(v8::String::New("Out of memory."));
    return v8::ThrowException(exception);
  }

  CreatePlugin *create;
  uv_dlsym(&lib, symbol, reinterpret_cast<void**>(&create));

  Plugin* mod = create();

  plugins.push_back(mod);

  return v8::Undefined();
}

void Init (v8::Handle<v8::Object> target) {
  Database::Init();
  leveldown::Iterator::Init();

  v8::Local<v8::Function> leveldown =
      v8::FunctionTemplate::New(LevelDOWN)->GetFunction();

  leveldown->Set(v8::String::NewSymbol("_registerPlugin")
    , v8::FunctionTemplate::New(RegisterPlugin)->GetFunction());

  target->Set(v8::String::NewSymbol("leveldown"), leveldown);
}

NODE_MODULE(leveldown, Init)

} // namespace leveldown
