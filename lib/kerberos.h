#ifndef KERBEROS_H
#define KERBEROS_H

#include <gssapi/gssapi.h>
#include <gssapi/gssapi_generic.h>
#include <gssapi/gssapi_krb5.h>

#include <node.h>
#include <node_object_wrap.h>
#include <v8.h>

using namespace v8;
using namespace node;

class Kerberos : public ObjectWrap {

public:
  Kerberos();
  ~Kerberos() {};

  // Constructor used for creating new Kerberos objects from C++
  static Persistent<FunctionTemplate> constructor_template;

  // Initialize function for the object
  static void Initialize(Handle<Object> target);

  // Method available
  static Handle<Value> AuthGSSClientInit(const Arguments &args);  

private:
  static Handle<Value> New(const Arguments &args);  
};

#endif