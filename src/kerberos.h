#ifndef KERBEROS_NATIVE_EXTENSION_H
#define KERBEROS_NATIVE_EXTENSION_H

#include <nan.h>
#include "kerberos_common.h"

class KerberosServer : public Nan::ObjectWrap {
   public:
    static NAN_MODULE_INIT(Init);
    static v8::Local<v8::Object> NewInstance(krb_server_state* state);

    krb_server_state* state() const;

   private:
    static Nan::Persistent<v8::Function> constructor;

    static NAN_GETTER(UserNameGetter);
    static NAN_GETTER(ResponseGetter);
    static NAN_GETTER(TargetNameGetter);
    static NAN_GETTER(ContextCompleteGetter);

    static NAN_METHOD(Step);

   private:
    explicit KerberosServer(krb_server_state* server_state);
    ~KerberosServer();

    krb_server_state* _state;
};

class KerberosClient : public Nan::ObjectWrap {
   public:
    static NAN_MODULE_INIT(Init);
    static v8::Local<v8::Object> NewInstance(krb_client_state* state);

    krb_client_state* state() const;

   private:
    static Nan::Persistent<v8::Function> constructor;

    static NAN_GETTER(UserNameGetter);
    static NAN_GETTER(ResponseGetter);
    static NAN_GETTER(ResponseConfGetter);
    static NAN_GETTER(ContextCompleteGetter);

    static NAN_METHOD(Step);
    static NAN_METHOD(UnwrapData);
    static NAN_METHOD(WrapData);

   private:
    explicit KerberosClient(krb_client_state* client_state);
    ~KerberosClient();

    krb_client_state* _state;
};

NAN_METHOD(PrincipalDetails);
NAN_METHOD(InitializeClient);
NAN_METHOD(InitializeServer);
NAN_METHOD(CheckPassword);

#endif  // KERBEROS_NATIVE_EXTENSION_H
