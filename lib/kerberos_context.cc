#include "kerberos_context.h"

Persistent<FunctionTemplate> KerberosContext::constructor_template;

KerberosContext::KerberosContext() : ObjectWrap() {
}

KerberosContext::~KerberosContext() {
}

KerberosContext* KerberosContext::New() {
  HandleScope scope;
  
  Local<Object> obj = constructor_template->GetFunction()->NewInstance();
  KerberosContext *kerberos_context = ObjectWrap::Unwrap<KerberosContext>(obj);  
  
  return kerberos_context;
}

Handle<Value> KerberosContext::New(const Arguments &args) {
  HandleScope scope;    
  // Create code object
  KerberosContext *kerberos_context = new KerberosContext();
  // Wrap it
  kerberos_context->Wrap(args.This());
  // Return the object
  return args.This();    
}

void KerberosContext::Initialize(Handle<Object> target) {
  // Grab the scope of the call from Node
  HandleScope scope;
  // Define a new function template
  Local<FunctionTemplate> t = FunctionTemplate::New(New);
  constructor_template = Persistent<FunctionTemplate>::New(t);
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("KerberosContext"));
  
  // // Instance methods
  // NODE_SET_PROTOTYPE_METHOD(constructor_template, "toString", ToString);
  // NODE_SET_PROTOTYPE_METHOD(constructor_template, "inspect", Inspect);  

  target->Set(String::NewSymbol("KerberosContext"), constructor_template->GetFunction());
}

// Handle<Value> MinKey::Inspect(const Arguments &args) {
//   return MinKey::ToString(args);
// }

// Handle<Value> MinKey::ToString(const Arguments &args) {
//   HandleScope scope;
//   // Return the raw data  
//   return Object::New()->ToString();
// }









