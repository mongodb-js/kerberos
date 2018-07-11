#ifndef KERBEROS_CLIENT_H
#define KERBEROS_CLIENT_H

#include "kerberos_common.h"

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

#endif  // KERBEROS_CLIENT_H
