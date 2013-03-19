#include "kerberos.h"
#include <stdlib.h>
#include "worker.h"
#include "kerberos_context.h"

#ifndef ARRAY_SIZE
# define ARRAY_SIZE(a) (sizeof((a)) / sizeof((a)[0]))
#endif

Persistent<FunctionTemplate> Kerberos::constructor_template;

// Call structs
typedef struct AuthGSSClientCall {
  uint32_t  flags;
  char *uri;
} AuthGSSClientCall;

typedef struct AuthGSSClientStepCall {
  KerberosContext *context;
  char *challenge;
} AuthGSSClientStepCall;

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

  // Set up method for the Kerberos instance
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "authGSSClientInit", AuthGSSClientInit);  
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "authGSSClientStep", AuthGSSClientStep);  
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "authGSSClientResponse", AuthGSSClientResponse);

  // Set the symbol
  target->ForceSet(String::NewSymbol("Kerberos"), constructor_template->GetFunction());
}

Handle<Value> Kerberos::New(const Arguments &args) {
  // Create a Kerberos instance
  Kerberos *kerberos = new Kerberos();
  // Return the kerberos object
  kerberos->Wrap(args.This());
  return args.This();
}

//
// authGSSClientInit
//
static void _authGSSClientInit(Worker *worker) {
  gss_client_state *state;
  gss_client_response *response;

  // Allocate state
  state = (gss_client_state *)malloc(sizeof(gss_client_state));
  
  // Unpack the parameter data struct
  AuthGSSClientCall *call = (AuthGSSClientCall *)worker->parameters;
  // Start the kerberos client
  response = authenticate_gss_client_init(call->uri, call->flags, state);

  // Release the parameter struct memory
  free(call->uri);
  free(call);

  // If we have an error mark worker as having had an error
  if(response->return_code == AUTH_GSS_ERROR) {
    worker->error = TRUE;
    worker->error_code = response->return_code;
    worker->error_message = response->message;
  } else {
    worker->return_value = state;
  }

  // Free structure
  free(response);
}

static Handle<Value> _map_authGSSClientInit(Worker *worker) {
  HandleScope scope;

  KerberosContext *context = KerberosContext::New();
  context->state = (gss_client_state *)worker->return_value;
  // Persistent<Value> _context = Persistent<Value>::New(context->handle_);
  return scope.Close(context->handle_);
}

// Initialize method
Handle<Value> Kerberos::AuthGSSClientInit(const Arguments &args) {
  HandleScope scope;

  // Ensure valid call
  if(args.Length() != 3) return VException("Requires a service string uri, integer flags and a callback function");
  if(args.Length() == 3 && !args[0]->IsString() && !args[1]->IsInt32() && !args[2]->IsFunction()) 
      return VException("Requires a service string uri, integer flags and a callback function");    

  Local<String> service = args[0]->ToString();
  // Convert uri string to c-string
  char *service_str = (char *)calloc(service->Utf8Length() + 1, sizeof(char));
  // Write v8 string to c-string
  service->WriteUtf8(service_str);

  // Allocate a structure
  AuthGSSClientCall *call = (AuthGSSClientCall *)calloc(1, sizeof(AuthGSSClientCall));
  call->flags =args[1]->ToInt32()->Uint32Value();
  call->uri = service_str;

  // Unpack the callback
  Local<Function> callback = Local<Function>::Cast(args[2]);

  // Let's allocate some space
  Worker *worker = new Worker();
  worker->error = false;
  worker->request.data = worker;
  worker->callback = Persistent<Function>::New(callback);
  worker->parameters = call;
  worker->execute = _authGSSClientInit;
  worker->mapper = _map_authGSSClientInit;

  // Schedule the worker with lib_uv
  uv_queue_work(uv_default_loop(), &worker->request, Kerberos::Process, Kerberos::After);
  // Return no value as it's callback based
  return scope.Close(Undefined());
}

//
// authGSSClientStep
//
static void _authGSSClientStep(Worker *worker) {
  gss_client_state *state;
  gss_client_response *response;
  char *challenge;

  // Unpack the parameter data struct
  AuthGSSClientStepCall *call = (AuthGSSClientStepCall *)worker->parameters;
  // Get the state
  state = call->context->state;

  // Check what kind of challenge we have
  if(call->challenge == NULL) {
    challenge = (char *)"";
  }

  // Perform authentication step
  response = authenticate_gss_client_step(state, challenge);

  // If we have an error mark worker as having had an error
  if(response->return_code == AUTH_GSS_ERROR) {
    worker->error = TRUE;
    worker->error_code = response->return_code;
    worker->error_message = response->message;
  } else {
    worker->return_code = response->return_code;
  }

  // Free up structure
  if(call->challenge != NULL) free(call->challenge);
  free(call);
  free(response);
}

static Handle<Value> _map_authGSSClientStep(Worker *worker) {
  HandleScope scope;
  // Return the return code
  return scope.Close(Int32::New(worker->return_code));
}

// Initialize method
Handle<Value> Kerberos::AuthGSSClientStep(const Arguments &args) {
  HandleScope scope;

  // Ensure valid call
  if(args.Length() != 2 && args.Length() != 3) return VException("Requires a GSS context, optional challenge string and callback function");
  if(args.Length() == 2 && !KerberosContext::HasInstance(args[0])) return VException("Requires a GSS context, optional challenge string and callback function");
  if(args.Length() == 3 && !KerberosContext::HasInstance(args[0]) && !args[1]->IsString()) return VException("Requires a GSS context, optional challenge string and callback function");

  // Challenge string
  char *challenge_str = NULL;
  // Let's unpack the parameters
  Local<Object> object = args[0]->ToObject();
  KerberosContext *kerberos_context = KerberosContext::Unwrap<KerberosContext>(object);

  // If we have a challenge string
  if(args.Length() == 2) {
    // The possible callback
    Local<String> challenge = args[0]->ToString();
    // Convert uri string to c-string
    challenge_str = (char *)calloc(challenge->Utf8Length() + 1, sizeof(char));
    // Write v8 string to c-string
    challenge->WriteUtf8(challenge_str);    
  }

  // Allocate a structure
  AuthGSSClientStepCall *call = (AuthGSSClientStepCall *)calloc(1, sizeof(AuthGSSClientCall));
  call->context = kerberos_context;
  call->challenge = challenge_str;

  // Unpack the callback
  Local<Function> callback = Local<Function>::Cast(args[2]);

  // Let's allocate some space
  Worker *worker = new Worker();
  worker->error = false;
  worker->request.data = worker;
  worker->callback = Persistent<Function>::New(callback);
  worker->parameters = call;
  worker->execute = _authGSSClientStep;
  worker->mapper = _map_authGSSClientStep;

  // Schedule the worker with lib_uv
  uv_queue_work(uv_default_loop(), &worker->request, Kerberos::Process, Kerberos::After);

  // Return no value as it's callback based
  return scope.Close(Undefined());
}

//
// authGSSClientStep
//
static void _authGSSClientResponse(Worker *worker) {
  // gss_client_state *state;
  // char *challenge;
  // int result = 0;

  // // Unpack the parameter data struct
  // AuthGSSClientStepCall *call = (AuthGSSClientStepCall *)worker->parameters;
  // // Get the state
  // state = call->context->state;

  // // Check what kind of challenge we have
  // if(call->challenge == NULL) {
  //   challenge = (char *)"";
  // }

  // // Perform authentication step
  // result = authenticate_gss_client_step(state, challenge);

  // // If we have an error mark worker as having had an error
  // if(result == AUTH_GSS_ERROR) {
  //   worker->error = TRUE;
  //   worker->error_code = result;
  //   worker->error_message = (char *)"GSS Authentication step failed";
  // } else {
  //   worker->return_code = result;
  // }

  // // Free up structure
  // if(call->challenge != NULL) free(call->challenge);
  // free(call);
}

static Handle<Value> _map_authGSSClientResponse(Worker *worker) {
  HandleScope scope;
  // // Return the return code
  return scope.Close(Int32::New(0));
}

// Initialize method
Handle<Value> Kerberos::AuthGSSClientResponse(const Arguments &args) {
  HandleScope scope;

  // // Ensure valid call
  // if(args.Length() != 2 && args.Length() != 3) return VException("Requires a GSS context, optional challenge string and callback function");
  // if(args.Length() == 2 && !KerberosContext::HasInstance(args[0])) return VException("Requires a GSS context, optional challenge string and callback function");
  // if(args.Length() == 3 && !KerberosContext::HasInstance(args[0]) && !args[1]->IsString()) return VException("Requires a GSS context, optional challenge string and callback function");

  // // Challenge string
  // char *challenge_str = NULL;
  // // Let's unpack the parameters
  // Local<Object> object = args[0]->ToObject();
  // KerberosContext *kerberos_context = KerberosContext::Unwrap<KerberosContext>(object);

  // // If we have a challenge string
  // if(args.Length() == 2) {
  //   // The possible callback
  //   Local<String> challenge = args[0]->ToString();
  //   // Convert uri string to c-string
  //   challenge_str = (char *)calloc(challenge->Utf8Length() + 1, sizeof(char));
  //   // Write v8 string to c-string
  //   challenge->WriteUtf8(challenge_str);    
  // }

  // // Allocate a structure
  // AuthGSSClientStepCall *call = (AuthGSSClientStepCall *)calloc(1, sizeof(AuthGSSClientCall));
  // call->context = kerberos_context;
  // call->challenge = challenge_str;

  // // Unpack the callback
  // Local<Function> callback = Local<Function>::Cast(args[2]);

  // // Let's allocate some space
  // Worker *worker = new Worker();
  // worker->error = false;
  // worker->request.data = worker;
  // worker->callback = Persistent<Function>::New(callback);
  // worker->parameters = call;
  // worker->execute = _authGSSClientStep;
  // worker->mapper = _map_authGSSClientStep;

  // // Schedule the worker with lib_uv
  // uv_queue_work(uv_default_loop(), &worker->request, Kerberos::Process, Kerberos::After);

  // Return no value as it's callback based
  return scope.Close(Undefined());
}

//
// UV Lib callbacks
//
void Kerberos::Process(uv_work_t* work_req) {
  // Grab the worker
  Worker *worker = static_cast<Worker*>(work_req->data);
  // Execute the worker code
  worker->execute(worker);
}

void Kerberos::After(uv_work_t* work_req) {
  // Grab the scope of the call from Node
  v8::HandleScope scope;

  // Get the worker reference
  Worker *worker = static_cast<Worker*>(work_req->data);

  // If we have an error
  if(worker->error) {
    v8::Local<v8::Value> err = v8::Exception::Error(v8::String::New(worker->error_message));
    Local<Object> obj = err->ToObject();
    obj->Set(NODE_PSYMBOL("code"), Int32::New(worker->error_code));
    v8::Local<v8::Value> args[2] = { err, v8::Local<v8::Value>::New(v8::Null()) };
    // Execute the error
    v8::TryCatch try_catch;
    // Call the callback
    worker->callback->Call(v8::Context::GetCurrent()->Global(), ARRAY_SIZE(args), args);
    // If we have an exception handle it as a fatalexception
    if (try_catch.HasCaught()) {
      node::FatalException(try_catch);
    }
  } else {
    // // Map the data
    v8::Handle<v8::Value> result = worker->mapper(worker);
    // Set up the callback with a null first
    v8::Handle<v8::Value> args[2] = { v8::Local<v8::Value>::New(v8::Null()), result};
    // Wrap the callback function call in a TryCatch so that we can call
    // node's FatalException afterwards. This makes it possible to catch
    // the exception from JavaScript land using the
    // process.on('uncaughtException') event.
    v8::TryCatch try_catch;
    // Call the callback
    worker->callback->Call(v8::Context::GetCurrent()->Global(), ARRAY_SIZE(args), args);
    // If we have an exception handle it as a fatalexception
    if (try_catch.HasCaught()) {
      node::FatalException(try_catch);
    }
  }

  // Clean up the memory
  worker->callback.Dispose();
  delete worker;
}


// Exporting function
extern "C" void init(Handle<Object> target) {
  HandleScope scope;
  Kerberos::Initialize(target);
  KerberosContext::Initialize(target);
}