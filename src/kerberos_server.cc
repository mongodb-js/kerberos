#include "kerberos_server.h"

Nan::Persistent<v8::Function> KerberosServer::constructor;
NAN_MODULE_INIT(KerberosServer::Init) {
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>();
    tpl->SetClassName(Nan::New("KerberosServer").ToLocalChecked());
    Nan::SetPrototypeMethod(tpl, "step", Step);

    v8::Local<v8::ObjectTemplate> itpl = tpl->InstanceTemplate();
    itpl->SetInternalFieldCount(1);

    Nan::SetAccessor(itpl, Nan::New("username").ToLocalChecked(), KerberosServer::UserNameGetter);
    Nan::SetAccessor(itpl, Nan::New("response").ToLocalChecked(), KerberosServer::ResponseGetter);
    Nan::SetAccessor(
        itpl, Nan::New("targetName").ToLocalChecked(), KerberosServer::TargetNameGetter);
    Nan::SetAccessor(
        itpl, Nan::New("contextComplete").ToLocalChecked(), KerberosServer::ContextCompleteGetter);

    constructor.Reset(Nan::GetFunction(tpl).ToLocalChecked());
    Nan::Set(target,
             Nan::New("KerberosServer").ToLocalChecked(),
             Nan::GetFunction(tpl).ToLocalChecked());
}

v8::Local<v8::Object> KerberosServer::NewInstance(krb_server_state* state) {
    Nan::EscapableHandleScope scope;
    v8::Local<v8::Function> ctor = Nan::New<v8::Function>(KerberosServer::constructor);
    v8::Local<v8::Object> object = Nan::NewInstance(ctor).ToLocalChecked();
    KerberosServer* class_instance = new KerberosServer(state);
    class_instance->Wrap(object);
    return scope.Escape(object);
}

KerberosServer::KerberosServer(krb_server_state* state) : _state(state) {}

krb_server_state* KerberosServer::state() const {
    return _state;
}

NAN_GETTER(KerberosServer::UserNameGetter) {
    KerberosServer* server = Nan::ObjectWrap::Unwrap<KerberosServer>(info.This());
    (server->_state->username == NULL)
        ? info.GetReturnValue().Set(Nan::Null())
        : info.GetReturnValue().Set(Nan::New(server->_state->username).ToLocalChecked());
}

NAN_GETTER(KerberosServer::ResponseGetter) {
    KerberosServer* server = Nan::ObjectWrap::Unwrap<KerberosServer>(info.This());
    (server->_state->response == NULL)
        ? info.GetReturnValue().Set(Nan::Null())
        : info.GetReturnValue().Set(Nan::New(server->_state->response).ToLocalChecked());
}

NAN_GETTER(KerberosServer::TargetNameGetter) {
    KerberosServer* server = Nan::ObjectWrap::Unwrap<KerberosServer>(info.This());
    (server->_state->targetname == NULL)
        ? info.GetReturnValue().Set(Nan::Null())
        : info.GetReturnValue().Set(Nan::New(server->_state->targetname).ToLocalChecked());
}

NAN_GETTER(KerberosServer::ContextCompleteGetter) {
    KerberosServer* server = Nan::ObjectWrap::Unwrap<KerberosServer>(info.This());
    info.GetReturnValue().Set(Nan::New(server->_state->context_complete));
}
