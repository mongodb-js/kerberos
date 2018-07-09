#include <memory>

#include "../kerberos_client.h"
#include "../kerberos_worker.h"

NAN_METHOD(KerberosClient::Step) {
    KerberosClient* client = Nan::ObjectWrap::Unwrap<KerberosClient>(info.This());
    std::string challenge(*Nan::Utf8String(info[0]));
    Nan::Callback* callback = new Nan::Callback(Nan::To<v8::Function>(info[1]).ToLocalChecked());

    KerberosWorker::Run(callback, "kerberos:ClientStep", [=](KerberosWorker::SetOnFinishedHandler onFinished) {
        std::shared_ptr<gss_result> result(
            authenticate_gss_client_step(client->state(), challenge.c_str(), NULL), ResultDeleter);

        return onFinished([=](KerberosWorker* worker) {
            Nan::HandleScope scope;
            if (result->code == AUTH_GSS_ERROR) {
                v8::Local<v8::Value> argv[] = {Nan::New(result->message).ToLocalChecked(), Nan::Null()};
                worker->Call(2, argv);
                return;
            }

            v8::Local<v8::Value> response = Nan::Null();
            if (client->state()->response != NULL) {
                response = Nan::New(client->state()->response).ToLocalChecked();
            }

            v8::Local<v8::Value> argv[] = {Nan::Null(), response};
            worker->Call(2, argv);
        });
    });
}

NAN_METHOD(KerberosClient::UnwrapData) {
    KerberosClient* client = Nan::ObjectWrap::Unwrap<KerberosClient>(info.This());
    std::string challenge(*Nan::Utf8String(info[0]));
    Nan::Callback* callback = new Nan::Callback(Nan::To<v8::Function>(info[1]).ToLocalChecked());

    KerberosWorker::Run(callback, "kerberos:ClientUnwrap", [=](KerberosWorker::SetOnFinishedHandler onFinished) {
        std::shared_ptr<gss_result> result(
            authenticate_gss_client_unwrap(client->state(), challenge.c_str()), ResultDeleter);

        return onFinished([=](KerberosWorker* worker) {
            Nan::HandleScope scope;
            if (result->code == AUTH_GSS_ERROR) {
                v8::Local<v8::Value> argv[] = {Nan::New(result->message).ToLocalChecked(), Nan::Null()};
                worker->Call(2, argv);
                return;
            }

            v8::Local<v8::Value> argv[] = {Nan::Null(), Nan::Null()};
            worker->Call(2, argv);
        });
    });
}

NAN_METHOD(KerberosClient::WrapData) {
    KerberosClient* client = Nan::ObjectWrap::Unwrap<KerberosClient>(info.This());
    std::string challenge(*Nan::Utf8String(info[0]));
    v8::Local<v8::Object> options = Nan::To<v8::Object>(info[1]).ToLocalChecked();
    Nan::Callback* callback = new Nan::Callback(Nan::To<v8::Function>(info[2]).ToLocalChecked());
    std::string user = StringOptionValue(options, "user");

    int protect = 0; // NOTE: this should be an option

    KerberosWorker::Run(callback, "kerberos:ClientWrap", [=](KerberosWorker::SetOnFinishedHandler onFinished) {
        std::shared_ptr<gss_result> result(authenticate_gss_client_wrap(
            client->state(), challenge.c_str(), user.c_str(), protect), ResultDeleter);

        return onFinished([=](KerberosWorker* worker) {
            Nan::HandleScope scope;
            if (result->code == AUTH_GSS_ERROR) {
                v8::Local<v8::Value> argv[] = {Nan::New(result->message).ToLocalChecked(), Nan::Null()};
                worker->Call(2, argv);
                return;
            }

            v8::Local<v8::Value> argv[] = {Nan::Null(), Nan::Null()};
            worker->Call(2, argv);
        });
    });
}
