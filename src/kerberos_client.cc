#include "kerberos_client.h"
#include "kerberos_worker.h"

Nan::Persistent<v8::Function> KerberosClient::constructor;
NAN_MODULE_INIT(KerberosClient::Init) {
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>();
    tpl->SetClassName(Nan::New("KerberosClient").ToLocalChecked());
    Nan::SetPrototypeMethod(tpl, "step", Step);
    Nan::SetPrototypeMethod(tpl, "wrap", WrapData);
    Nan::SetPrototypeMethod(tpl, "unwrap", UnwrapData);

    v8::Local<v8::ObjectTemplate> itpl = tpl->InstanceTemplate();
    itpl->SetInternalFieldCount(1);

    Nan::SetAccessor(itpl, Nan::New("username").ToLocalChecked(), KerberosClient::UserNameGetter);
    Nan::SetAccessor(itpl, Nan::New("response").ToLocalChecked(), KerberosClient::ResponseGetter);
    Nan::SetAccessor(
        itpl, Nan::New("responseConf").ToLocalChecked(), KerberosClient::ResponseConfGetter);
    Nan::SetAccessor(
        itpl, Nan::New("contextComplete").ToLocalChecked(), KerberosClient::ContextCompleteGetter);

    constructor.Reset(Nan::GetFunction(tpl).ToLocalChecked());
    Nan::Set(target,
             Nan::New("KerberosClient").ToLocalChecked(),
             Nan::GetFunction(tpl).ToLocalChecked());
}

v8::Local<v8::Object> KerberosClient::NewInstance(gss_client_state* state) {
    Nan::EscapableHandleScope scope;
    v8::Local<v8::Function> ctor = Nan::New<v8::Function>(KerberosClient::constructor);
    v8::Local<v8::Object> object = Nan::NewInstance(ctor).ToLocalChecked();
    KerberosClient* class_instance = new KerberosClient(state);
    class_instance->Wrap(object);
    return scope.Escape(object);
}

KerberosClient::KerberosClient(gss_client_state* state) : _state(state) {}

KerberosClient::~KerberosClient() {
    if (_state != NULL) {
        authenticate_gss_client_clean(_state);
        _state = NULL;
    }
}

gss_client_state* KerberosClient::state() const {
    return _state;
}

NAN_GETTER(KerberosClient::UserNameGetter) {
    KerberosClient* client = Nan::ObjectWrap::Unwrap<KerberosClient>(info.This());
    (client->_state->username == NULL)
        ? info.GetReturnValue().Set(Nan::Null())
        : info.GetReturnValue().Set(Nan::New(client->_state->username).ToLocalChecked());
}

NAN_GETTER(KerberosClient::ResponseGetter) {
    KerberosClient* client = Nan::ObjectWrap::Unwrap<KerberosClient>(info.This());
    (client->_state->response == NULL)
        ? info.GetReturnValue().Set(Nan::Null())
        : info.GetReturnValue().Set(Nan::New(client->_state->response).ToLocalChecked());
}

NAN_GETTER(KerberosClient::ResponseConfGetter) {
    KerberosClient* client = Nan::ObjectWrap::Unwrap<KerberosClient>(info.This());
    info.GetReturnValue().Set(Nan::New(client->_state->responseConf));
}

NAN_GETTER(KerberosClient::ContextCompleteGetter) {
    KerberosClient* client = Nan::ObjectWrap::Unwrap<KerberosClient>(info.This());
    info.GetReturnValue().Set(Nan::New(client->_state->context_complete));
}
