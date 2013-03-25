#include "kerberos.h"
#include <stdlib.h>

#ifndef ARRAY_SIZE
# define ARRAY_SIZE(a) (sizeof((a)) / sizeof((a)[0]))
#endif

#define SECURITY_WIN32  1

extern "C" {
  #include "kerberos_sspi.h"
}

Persistent<FunctionTemplate> Kerberos::constructor_template;

// VException object (causes throw in calling code)
static Handle<Value> VException(const char *msg) {
  HandleScope scope;
  return ThrowException(Exception::Error(String::New(msg)));
}

Kerberos::Kerberos() : ObjectWrap() {
}

void Kerberos::Initialize(v8::Handle<v8::Object> target) {
  // Grab the scope of the call from Node
  HandleScope scope;
  // Define a new function template
  Local<FunctionTemplate> t = FunctionTemplate::New(Kerberos::New);
  constructor_template = Persistent<FunctionTemplate>::New(t);
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("Kerberos"));

  // // Set up method for the Kerberos instance
  // NODE_SET_PROTOTYPE_METHOD(constructor_template, "authGSSClientInit", AuthGSSClientInit);  
  // NODE_SET_PROTOTYPE_METHOD(constructor_template, "authGSSClientStep", AuthGSSClientStep);  
  // NODE_SET_PROTOTYPE_METHOD(constructor_template, "authGSSClientUnwrap", AuthGSSClientUnwrap);
  // NODE_SET_PROTOTYPE_METHOD(constructor_template, "authGSSClientWrap", AuthGSSClientWrap);
  // NODE_SET_PROTOTYPE_METHOD(constructor_template, "authGSSClientClean", AuthGSSClientClean);

  // Set the symbol
  target->ForceSet(String::NewSymbol("Kerberos"), constructor_template->GetFunction());
}

Handle<Value> Kerberos::New(const Arguments &args) {
  load_library();

  // Create a Kerberos instance
  Kerberos *kerberos = new Kerberos();
  // Return the kerberos object
  kerberos->Wrap(args.This());
  return args.This();
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// UV Lib callbacks
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void Kerberos::Process(uv_work_t* work_req) {
  // // Grab the worker
  // Worker *worker = static_cast<Worker*>(work_req->data);
  // // Execute the worker code
  // worker->execute(worker);
}

void Kerberos::After(uv_work_t* work_req) {
  // Grab the scope of the call from Node
  v8::HandleScope scope;

  // // Get the worker reference
  // Worker *worker = static_cast<Worker*>(work_req->data);

  // // If we have an error
  // if(worker->error) {
  //   v8::Local<v8::Value> err = v8::Exception::Error(v8::String::New(worker->error_message));
  //   Local<Object> obj = err->ToObject();
  //   obj->Set(NODE_PSYMBOL("code"), Int32::New(worker->error_code));
  //   v8::Local<v8::Value> args[2] = { err, v8::Local<v8::Value>::New(v8::Null()) };
  //   // Execute the error
  //   v8::TryCatch try_catch;
  //   // Call the callback
  //   worker->callback->Call(v8::Context::GetCurrent()->Global(), ARRAY_SIZE(args), args);
  //   // If we have an exception handle it as a fatalexception
  //   if (try_catch.HasCaught()) {
  //     node::FatalException(try_catch);
  //   }
  // } else {
  //   // // Map the data
  //   v8::Handle<v8::Value> result = worker->mapper(worker);
  //   // Set up the callback with a null first
  //   v8::Handle<v8::Value> args[2] = { v8::Local<v8::Value>::New(v8::Null()), result};
  //   // Wrap the callback function call in a TryCatch so that we can call
  //   // node's FatalException afterwards. This makes it possible to catch
  //   // the exception from JavaScript land using the
  //   // process.on('uncaughtException') event.
  //   v8::TryCatch try_catch;
  //   // Call the callback
  //   worker->callback->Call(v8::Context::GetCurrent()->Global(), ARRAY_SIZE(args), args);
  //   // If we have an exception handle it as a fatalexception
  //   if (try_catch.HasCaught()) {
  //     node::FatalException(try_catch);
  //   }
  // }

  // // Clean up the memory
  // worker->callback.Dispose();
  // delete worker;
}


// // Exporting function
// extern "C" void init(Handle<Object> target) {
//   HandleScope scope;
//   Kerberos::Initialize(target);
// }

NODE_MODULE(kerberos, Kerberos::Initialize);