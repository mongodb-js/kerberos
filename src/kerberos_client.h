#ifndef KERBEROS_CLIENT_H
#define KERBEROS_CLIENT_H

#include "common.h"

class KerberosClient : public Nan::ObjectWrap {
   public:
    static NAN_MODULE_INIT(Init);
    static v8::Local<v8::Object> NewInstance(gss_client_state* state);

    gss_client_state* state() const;

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
    friend class ClientStepWorker;
    explicit KerberosClient(gss_client_state* client_state);
    ~KerberosClient();

    gss_client_state* _state;
    bool _contextComplete;
};

#endif
