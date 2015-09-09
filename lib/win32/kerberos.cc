#include "kerberos.h"
#include <stdlib.h>
#include <tchar.h>
#include "base64.h"
#include "wrappers/security_buffer.h"
#include "wrappers/security_buffer_descriptor.h"
#include "wrappers/security_context.h"
#include "wrappers/security_credentials.h"

Nan::Persistent<FunctionTemplate> Kerberos::constructor_template;

Kerberos::Kerberos() : Nan::ObjectWrap() {
}

void Kerberos::Initialize(v8::Handle<v8::Object> target) {
  // Grab the scope of the call from Node
  Nan::HandleScope scope;

  // Define a new function template
  Local<FunctionTemplate> t = Nan::New<FunctionTemplate>(New);
  t->InstanceTemplate()->SetInternalFieldCount(1);
  t->SetClassName(Nan::New<String>("Kerberos").ToLocalChecked());

  // Set persistent
  constructor_template.Reset(t);

  // Set the symbol
  target->ForceSet(Nan::New<String>("Kerberos").ToLocalChecked(), t->GetFunction());
}

NAN_METHOD(Kerberos::New) {
  Nan::HandleScope scope;
  // Load the security.dll library
  load_library();
  // Create a Kerberos instance
  Kerberos *kerberos = new Kerberos();
  // Return the kerberos object
  kerberos->Wrap(info.This());
  // Return the object
  info.GetReturnValue().Set(info.This());
}

// Exporting function
extern "C" void init(Handle<Object> target) {
  Nan::HandleScope scope;
  Kerberos::Initialize(target);
  SecurityContext::Initialize(target);
  SecurityBuffer::Initialize(target);
  SecurityBufferDescriptor::Initialize(target);
  SecurityCredentials::Initialize(target);
}

NODE_MODULE(kerberos, init);
