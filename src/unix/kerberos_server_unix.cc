#include <memory>

#include "../kerberos_worker.h"
#include "../kerberos_server.h"

KerberosServer::~KerberosServer() {
    if (_state != NULL) {
        authenticate_gss_server_clean(_state);
        _state = NULL;
    }
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
