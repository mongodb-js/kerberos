#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <v8.h>
#include <node.h>
#include <node_buffer.h>
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>

#include "security_context.h"

static Handle<Value> VException(const char *msg) {
  HandleScope scope;
  return ThrowException(Exception::Error(String::New(msg)));
};

Persistent<FunctionTemplate> SecurityContext::constructor_template;

SecurityContext::SecurityContext(Persistent<Object> value) : ObjectWrap() {
  this->value = value;
}

SecurityContext::~SecurityContext() {
}

Handle<Value> SecurityContext::New(const Arguments &args) {
  HandleScope scope;  
  Persistent<Object> doublePObj;
  
  if(args.Length() != 1 && !args[0]->IsString()) {
    return VException("There must be 1 argument passed in where the first argument is a string");
  }
  
  // Decode the string
  doublePObj = Persistent<Object>::New(args[0]->ToObject());
  // Create code object
  SecurityContext *security_obj = new SecurityContext(doublePObj);
  // Wrap it
  security_obj->Wrap(args.This());
  // Return the object
  return args.This();    
}

static Persistent<String> value_Double;

void SecurityContext::Initialize(Handle<Object> target) {
  // Grab the scope of the call from Node
  HandleScope scope;
  // Define a new function template
  Local<FunctionTemplate> t = FunctionTemplate::New(New);
  constructor_template = Persistent<FunctionTemplate>::New(t);
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("SecurityContext"));

  target->Set(String::NewSymbol("SecurityContext"), constructor_template->GetFunction());  
}
