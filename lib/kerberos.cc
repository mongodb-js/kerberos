#include "kerberos.h"

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
  Local<FunctionTemplate> t = FunctionTemplate::New(New);
  constructor_template = Persistent<FunctionTemplate>::New(t);
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("Kerberos"));

  // Set up method for the Kerberos instance
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "authGSSClientInit", AuthGSSClientInit);  

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

// Initialize method
Handle<Value> Kerberos::AuthGSSClientInit(const Arguments &args) {
  HandleScope scope;
  // Ensure we have a valid object
  if(args.Length() == 1 && !args[0]->IsObject()) return VException("One argument required - [object]");
  if(args.Length() == 2 && !args[0]->IsObject() && !args[1]->IsBoolean())  return VException("Two arguments required - [object, boolean]");
  if(args.Length() > 3) return VException("One or two arguments required - [object] or [object, boolean]");

  // Unpack the BSON parser instance
  BSON *bson = ObjectWrap::Unwrap<BSON>(args.This());
  bool serializeFunctions = (args.Length() >= 2) && args[1]->BooleanValue();
  BSONSerializer<CountStream> countSerializer(bson, false, serializeFunctions);
  countSerializer.SerializeDocument(args[0]);

  // Return the object size
  return scope.Close(Uint32::New((uint32_t) countSerializer.GetSerializeSize()));
}

// Exporting function
extern "C" void init(Handle<Object> target) {
  HandleScope scope;
  Kerberos::Initialize(target);
}

NODE_MODULE(kerberos, Kerberos::Initialize);