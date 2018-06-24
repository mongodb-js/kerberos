#ifndef KERBEROS_CONTEXT_H
#define KERBEROS_CONTEXT_H

#include "nan.h"
#include "kerberos_gss.h"

class KerberosClientContext : public Nan::ObjectWrap {
 public:
    static NAN_MODULE_INIT(Init);

 private:
    static Nan::Persistent<v8::Function> constructor;

    static NAN_GETTER(UserNameGetter);
    static NAN_GETTER(ResponseGetter);
    static NAN_GETTER(ResponseConfGetter);

 private:
    explicit KerberosClientContext(gss_client_state* client_state);
    ~KerberosClientContext();

    gss_client_state* _state;
};

class KerberosServerContext : public Nan::ObjectWrap {
 public:
    static NAN_MODULE_INIT(Init);

 private:
    static Nan::Persistent<v8::Function> constructor;

    static NAN_GETTER(UserNameGetter);
    static NAN_GETTER(ResponseGetter);
    static NAN_GETTER(TargetNameGetter);

 private:
    explicit KerberosServerContext(gss_server_state* server_state);
    ~KerberosServerContext();

    gss_server_state* _state;
}

#endif
