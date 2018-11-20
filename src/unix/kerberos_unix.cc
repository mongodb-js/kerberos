#include <memory>

#include "../kerberos.h"
#include "../kerberos_worker.h"

#define GSS_MECH_OID_KRB5 9
#define GSS_MECH_OID_SPNEGO 6

static char krb5_mech_oid_bytes[] = "\x2a\x86\x48\x86\xf7\x12\x01\x02\x02";
gss_OID_desc krb5_mech_oid = {9, &krb5_mech_oid_bytes};

static char spnego_mech_oid_bytes[] = "\x2b\x06\x01\x05\x05\x02";
gss_OID_desc spnego_mech_oid = {6, &spnego_mech_oid_bytes};

/// KerberosClient
KerberosClient::~KerberosClient() {
    if (_state != NULL) {
        authenticate_gss_client_clean(_state);
        _state = NULL;
    }
}

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
                v8::Local<v8::Value> argv[] = {Nan::Error(result->message), Nan::Null()};
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
                v8::Local<v8::Value> argv[] = {Nan::Error(result->message), Nan::Null()};
                worker->Call(2, argv);
                return;
            }

            v8::Local<v8::Value> argv[] = {Nan::Null(), Nan::New(client->state()->response).ToLocalChecked()};
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
                v8::Local<v8::Value> argv[] = {Nan::Error(result->message), Nan::Null()};
                worker->Call(2, argv);
                return;
            }

            v8::Local<v8::Value> argv[] = {Nan::Null(), Nan::New(client->state()->response).ToLocalChecked()};
            worker->Call(2, argv);
        });
    });
}

/// KerberosServer
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
                v8::Local<v8::Value> argv[] = {Nan::Error(result->message), Nan::Null()};
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

/// Global Methods
NAN_METHOD(InitializeClient) {
    std::string service(*Nan::Utf8String(info[0]));
    v8::Local<v8::Object> options = Nan::To<v8::Object>(info[1]).ToLocalChecked();
    Nan::Callback* callback = new Nan::Callback(Nan::To<v8::Function>(info[2]).ToLocalChecked());

    std::string principal = StringOptionValue(options, "principal");
    uint32_t gss_flags =
        UInt32OptionValue(options, "gssFlags", GSS_C_MUTUAL_FLAG | GSS_C_SEQUENCE_FLAG);
    uint32_t mech_oid_int = UInt32OptionValue(options, "mechOID", 0);
    gss_OID mech_oid = GSS_C_NO_OID;
    if (mech_oid_int == GSS_MECH_OID_KRB5) {
        mech_oid = &krb5_mech_oid;
    } else if (mech_oid_int == GSS_MECH_OID_SPNEGO) {
        mech_oid = &spnego_mech_oid;
    }

    KerberosWorker::Run(callback, "kerberos:InitializeClient", [=](KerberosWorker::SetOnFinishedHandler onFinished) {
        gss_client_state* client_state = gss_client_state_new();
        std::shared_ptr<gss_result> result(authenticate_gss_client_init(
            service.c_str(), principal.c_str(), gss_flags, NULL, mech_oid, client_state), ResultDeleter);

        // must clean up state if we won't be using it, smart pointers won't help here unfortunately
        // because we can't `release` a shared pointer.
        if (result->code == AUTH_GSS_ERROR) {
            free(client_state);
        }

        return onFinished([=](KerberosWorker* worker) {
            Nan::HandleScope scope;
            if (result->code == AUTH_GSS_ERROR) {
                v8::Local<v8::Value> argv[] = {Nan::Error(result->message), Nan::Null()};
                worker->Call(2, argv);
                return;
            }

            v8::Local<v8::Value> argv[] = {Nan::Null(), KerberosClient::NewInstance(client_state)};
            worker->Call(2, argv);
        });
    });
}

NAN_METHOD(InitializeServer) {
    std::string service(*Nan::Utf8String(info[0]));
    Nan::Callback* callback = new Nan::Callback(Nan::To<v8::Function>(info[1]).ToLocalChecked());

    KerberosWorker::Run(callback, "kerberos:InitializeServer", [=](KerberosWorker::SetOnFinishedHandler onFinished) {
        gss_server_state* server_state = gss_server_state_new();
        std::shared_ptr<gss_result> result(
            authenticate_gss_server_init(service.c_str(), server_state), ResultDeleter);

        // must clean up state if we won't be using it, smart pointers won't help here unfortunately
        // because we can't `release` a shared pointer.
        if (result->code == AUTH_GSS_ERROR) {
            free(server_state);
        }

        return onFinished([=](KerberosWorker* worker) {
            Nan::HandleScope scope;
            if (result->code == AUTH_GSS_ERROR) {
                v8::Local<v8::Value> argv[] = {Nan::Error(result->message), Nan::Null()};
                worker->Call(2, argv);
                return;
            }

            v8::Local<v8::Value> argv[] = {Nan::Null(), KerberosServer::NewInstance(server_state)};
            worker->Call(2, argv);
        });
    });
}

NAN_METHOD(PrincipalDetails) {
    std::string service(*Nan::Utf8String(info[0]));
    std::string hostname(*Nan::Utf8String(info[1]));
    Nan::Callback* callback = new Nan::Callback(Nan::To<v8::Function>(info[2]).ToLocalChecked());

    KerberosWorker::Run(callback, "kerberos:PrincipalDetails", [=](KerberosWorker::SetOnFinishedHandler onFinished) {
        std::shared_ptr<gss_result> result(
            server_principal_details(service.c_str(), hostname.c_str()), ResultDeleter);

        return onFinished([=](KerberosWorker* worker) {
            Nan::HandleScope scope;
            if (result->code == AUTH_GSS_ERROR) {
                v8::Local<v8::Value> argv[] = {Nan::Error(result->message), Nan::Null()};
                worker->Call(2, argv);
                return;
            }

            v8::Local<v8::Value> argv[] = {Nan::Null(), Nan::New(result->data).ToLocalChecked()};
            worker->Call(2, argv);
        });
    });
}

NAN_METHOD(CheckPassword) {
    std::string username(*Nan::Utf8String(info[0]));
    std::string password(*Nan::Utf8String(info[1]));
    std::string service(*Nan::Utf8String(info[2]));

    std::string defaultRealm;
    Nan::Callback* callback;
    if (info[3]->IsFunction()) {
        callback = new Nan::Callback(Nan::To<v8::Function>(info[3]).ToLocalChecked());
    } else {
        defaultRealm = *Nan::Utf8String(info[3]);
        callback = new Nan::Callback(Nan::To<v8::Function>(info[4]).ToLocalChecked());
    }

    KerberosWorker::Run(callback, "kerberos:CheckPassword", [=](KerberosWorker::SetOnFinishedHandler onFinished) {
        std::shared_ptr<gss_result> result(authenticate_user_krb5pwd(
            username.c_str(), password.c_str(), service.c_str(), defaultRealm.c_str()), ResultDeleter);

        return onFinished([=](KerberosWorker* worker) {
            Nan::HandleScope scope;
            if (result->code == AUTH_GSS_ERROR) {
                v8::Local<v8::Value> argv[] = {Nan::Error(result->message), Nan::Null()};
                worker->Call(2, argv);
            } else {
                v8::Local<v8::Value> argv[] = {Nan::Null(), Nan::Null()};
                worker->Call(2, argv);
            }
        });
    });
}
