#ifndef KERBEROS_NATIVE_EXTENSION_H
#define KERBEROS_NATIVE_EXTENSION_H

#include <nan.h>
#include "kerberos_common.h"

class KerberosServer : public Nan::ObjectWrap {
   public:
    static NAN_MODULE_INIT(Init);
    static v8::Local<v8::Object> NewInstance(std::shared_ptr<krb_server_state> state);

    std::shared_ptr<krb_server_state> state() const;

   private:
    static Nan::Persistent<v8::Function> constructor;

    static NAN_GETTER(UserNameGetter);
    static NAN_GETTER(ResponseGetter);
    static NAN_GETTER(TargetNameGetter);
    static NAN_GETTER(ContextCompleteGetter);

    static NAN_METHOD(Step);

   private:
    explicit KerberosServer(std::shared_ptr<krb_server_state> server_state);

    std::shared_ptr<krb_server_state> _state;
};

class KerberosClient : public Nan::ObjectWrap {
   public:
    static NAN_MODULE_INIT(Init);
    static v8::Local<v8::Object> NewInstance(std::shared_ptr<krb_client_state> state);

    std::shared_ptr<krb_client_state> state() const;

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
    explicit KerberosClient(std::shared_ptr<krb_client_state> client_state);

    std::shared_ptr<krb_client_state> _state;
};

NAN_METHOD(PrincipalDetails);
NAN_METHOD(InitializeClient);
NAN_METHOD(InitializeServer);
NAN_METHOD(CheckPassword);

// NOTE: explicitly used for unit testing `defineOperation`, not meant to be exported
NAN_METHOD(TestMethod);

#endif  // KERBEROS_NATIVE_EXTENSION_H
