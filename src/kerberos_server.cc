#include <memory>

#include "kerberos_server.h"
#include "kerberos_worker.h"

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

v8::Local<v8::Object> KerberosServer::NewInstance(gss_server_state* state) {
    Nan::EscapableHandleScope scope;
    v8::Local<v8::Function> ctor = Nan::New<v8::Function>(KerberosServer::constructor);
    v8::Local<v8::Object> object = Nan::NewInstance(ctor).ToLocalChecked();
    KerberosServer* class_instance = new KerberosServer(state);
    class_instance->Wrap(object);
    return scope.Escape(object);
}

KerberosServer::KerberosServer(gss_server_state* state) : _state(state), _contextComplete(false) {}

KerberosServer::~KerberosServer() {
    if (_state != NULL) {
        authenticate_gss_server_clean(_state);
        _state = NULL;
    }
}

gss_server_state* KerberosServer::state() const {
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
    info.GetReturnValue().Set(Nan::New(server->_contextComplete));
}

NAN_METHOD(KerberosServer::Step) {
    KerberosServer* server = Nan::ObjectWrap::Unwrap<KerberosServer>(info.This());
    std::string challenge(*Nan::Utf8String(info[0]));
    Nan::Callback* callback = new Nan::Callback(Nan::To<v8::Function>(info[1]).ToLocalChecked());

    KerberosWorker::Run(callback, "kerberos:ServerStep", [=](KerberosWorker::SetOnFinishedHandler onFinished) {
        std::shared_ptr<gss_result> result(
            authenticate_gss_server_step(server->state(), challenge.c_str()), ResultDeleter);

        return onFinished([=](KerberosWorker* worker) {
            Nan::HandleScope scope;
            if (result->code == AUTH_GSS_ERROR) {
                v8::Local<v8::Value> argv[] = {Nan::New(result->message).ToLocalChecked(), Nan::Null()};
                worker->Call(2, argv);
                return;
            }

            v8::Local<v8::Value> response = Nan::Null();
            if (server->state()->response != NULL) {
                response = Nan::New(server->state()->response).ToLocalChecked();
            }

            v8::Local<v8::Value> argv[] = {Nan::Null(), response};
            worker->Call(2, argv);
        });
    });
}
