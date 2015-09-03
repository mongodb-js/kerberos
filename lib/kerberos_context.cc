#include "kerberos_context.h"

Nan::Persistent<FunctionTemplate> KerberosContext::constructor_template;

KerberosContext::KerberosContext() : Nan::ObjectWrap() {
}

KerberosContext::~KerberosContext() {
}

KerberosContext* KerberosContext::New() {
  Nan::HandleScope scope; 
  Local<Object> obj = Nan::New(constructor_template)->GetFunction()->NewInstance();
  KerberosContext *kerberos_context = Nan::ObjectWrap::Unwrap<KerberosContext>(obj);  
  return kerberos_context;
}

NAN_METHOD(KerberosContext::New) {
  Nan::HandleScope scope;
  // Create code object
  KerberosContext *kerberos_context = new KerberosContext();
  // Wrap it
  kerberos_context->Wrap(info.This());
  // Return the object
  info.GetReturnValue().Set(info.This());
}

void KerberosContext::Initialize(v8::Handle<v8::Object> target) {
  // Grab the scope of the call from Node
  Nan::HandleScope scope;

  // Define a new function template
  // Local<FunctionTemplate> t = NanNew<FunctionTemplate>(New);
  Local<FunctionTemplate> t = Nan::New<v8::FunctionTemplate>(static_cast<NAN_METHOD((*))>(New));
  t->InstanceTemplate()->SetInternalFieldCount(1);
  t->SetClassName(Nan::New<String>("KerberosContext").ToLocalChecked());

  // Get prototype
  Local<ObjectTemplate> proto = t->PrototypeTemplate();

  // Getter for the response
  Nan::SetAccessor(proto, Nan::New<String>("response").ToLocalChecked(), KerberosContext::ResponseGetter);

  // Set persistent
  constructor_template.Reset(t);
 // NanAssignPersistent(constructor_template, t);

  // Set the symbol
  target->ForceSet(Nan::New<String>("KerberosContext").ToLocalChecked(), t->GetFunction());
}

//
// Response Setter / Getter
NAN_GETTER(KerberosContext::ResponseGetter) {
  Nan::HandleScope scope;
  gss_client_state *state;

  // Unpack the object
  KerberosContext *context = Nan::ObjectWrap::Unwrap<KerberosContext>(info.This());
  // Let's grab the response
  state = context->state;
  // No state no response
  if(state == NULL || state->response == NULL) {
    info.GetReturnValue().Set(Nan::Null());
  } else {
    // Return the response
    info.GetReturnValue().Set(Nan::New<String>(state->response).ToLocalChecked());
  }
}









