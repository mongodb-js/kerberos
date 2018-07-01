#ifndef KERBEROS_SERVER_H
#define KERBEROS_SERVER_H

#include "nan.h"
#include "common.h"

class KerberosServer : public Nan::ObjectWrap {
 public:
    static NAN_MODULE_INIT(Init);
    static v8::Local<v8::Object> NewInstance(gss_server_state* state);

    gss_server_state* state() const;

 private:
    static Nan::Persistent<v8::Function> constructor;

    static NAN_GETTER(UserNameGetter);
    static NAN_GETTER(ResponseGetter);
    static NAN_GETTER(TargetNameGetter);

    static NAN_METHOD(Step);

 private:
    explicit KerberosServer(gss_server_state* server_state);
    ~KerberosServer();

    gss_server_state* _state;
};

#endif
