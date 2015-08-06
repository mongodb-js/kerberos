#include "kerberos_context.h"

Persistent<FunctionTemplate> KerberosContext::constructor_template;

KerberosContext::KerberosContext() : ObjectWrap() {
}

KerberosContext::~KerberosContext() {
}

KerberosContext* KerberosContext::New() {
  NanScope();  
  Local<Object> obj = NanNew(constructor_template)->GetFunction()->NewInstance();
  KerberosContext *kerberos_context = ObjectWrap::Unwrap<KerberosContext>(obj);  
  return kerberos_context;
}

NAN_METHOD(KerberosContext::New) {
  NanScope();
  // Create code object
  KerberosContext *kerberos_context = new KerberosContext();
  // Wrap it
  kerberos_context->Wrap(args.This());
  // Return the object
  NanReturnValue(args.This());
}

void KerberosContext::Initialize(v8::Handle<v8::Object> target) {
  // Grab the scope of the call from Node
  NanScope();

  // Define a new function template
  // Local<FunctionTemplate> t = NanNew<FunctionTemplate>(New);
  Local<FunctionTemplate> t = NanNew<v8::FunctionTemplate>(static_cast<NAN_METHOD((*))>(New));
  t->InstanceTemplate()->SetInternalFieldCount(1);
  t->SetClassName(NanNew<String>("KerberosContext"));

  // Get prototype
  Local<ObjectTemplate> proto = t->PrototypeTemplate();

  // Getter for the response
  proto->SetAccessor(NanNew<String>("response"), KerberosContext::ResponseGetter);

  // Getter for the username
  proto->SetAccessor(NanNew<String>("username"), KerberosContext::UsernameGetter);

  // Getter for the targetname - server side only
  proto->SetAccessor(NanNew<String>("targetname"), KerberosContext::TargetnameGetter);

  // Set persistent
  NanAssignPersistent(constructor_template, t);

  // Set the symbol
  target->ForceSet(NanNew<String>("KerberosContext"), t->GetFunction());
}


// Response Setter / Getter
NAN_GETTER(KerberosContext::ResponseGetter) {
  NanScope();
  gss_client_state *client_state;
  gss_server_state *server_state;

  // Unpack the object
  KerberosContext *context = ObjectWrap::Unwrap<KerberosContext>(args.This());

  // Response could come from client or server state...
  client_state = context->state;
  server_state = context->server_state;

  // If client state is in use, take response from there, otherwise from server
  char *response = client_state != NULL ? client_state->response :
	  server_state != NULL ? server_state->response : NULL;

  if(response == NULL) {
    NanReturnValue(NanNull());
  } else {
    // Return the response
    NanReturnValue(NanNew<String>(response));
  }
}

// username Getter
NAN_GETTER(KerberosContext::UsernameGetter) {
  NanScope();

  // Unpack the object
  KerberosContext *context = ObjectWrap::Unwrap<KerberosContext>(args.This());

  gss_client_state *client_state = context->state;
  gss_server_state *server_state = context->server_state;

  // If client state is in use, take response from there, otherwise from server
  char *username = client_state != NULL ? client_state->username :
	  server_state != NULL ? server_state->username : NULL;

  if(username == NULL) {
    NanReturnValue(NanNull());
  } else {
    NanReturnValue(NanNew<String>(username));
  }
}

// targetname Getter - server side only
NAN_GETTER(KerberosContext::TargetnameGetter) {
  NanScope();

  // Unpack the object
  KerberosContext *context = ObjectWrap::Unwrap<KerberosContext>(args.This());

  gss_server_state *server_state = context->server_state;

  char *targetname = server_state != NULL ? server_state->targetname : NULL;

  if(targetname == NULL) {
    NanReturnValue(NanNull());
  } else {
    NanReturnValue(NanNew<String>(targetname));
  }
}
