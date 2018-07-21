#include <memory>

#include "../kerberos.h"
#include "../kerberos_client.h"
#include "../kerberos_common.h"
#include "../kerberos_server.h"
#include "../kerberos_worker.h"

#define GSS_MECH_OID_KRB5 9
#define GSS_MECH_OID_SPNEGO 6
#define GSS_MECH_OID_KRB5_STR L"Kerberos"
#define GSS_MECH_OID_SPNEGO_STR L"Negotiate"

const wchar_t* to_wstring(const v8::String::Utf8Value& str) {
    const char *bytes = *str;
    unsigned int sizeOfStr = MultiByteToWideChar(CP_UTF8, 0, bytes, -1, NULL, 0);
    wchar_t *output = new wchar_t[sizeOfStr];
    MultiByteToWideChar(CP_UTF8, 0, bytes, -1, output, sizeOfStr);
    return output;
}

NAN_INLINE std::wstring WStringOptionValue(v8::Local<v8::Object> options, const char* _key) {
    Nan::HandleScope scope;
    v8::Local<v8::String> key = Nan::New(_key).ToLocalChecked();
    return !options.IsEmpty() && options->Has(key) && options->Get(key)->IsString()
               ? std::wstring(to_wstring(v8::String::Utf8Value(options->Get(key)->ToString())))
               : std::wstring();
}

NAN_METHOD(InitializeClient) {
    std::wstring service(to_wstring(v8::String::Utf8Value(info[0]->ToString())));
    v8::Local<v8::Object> options = Nan::To<v8::Object>(info[1]).ToLocalChecked();
    Nan::Callback* callback = new Nan::Callback(Nan::To<v8::Function>(info[2]).ToLocalChecked());

    ULONG gss_flags = 0;
    std::wstring user = WStringOptionValue(options, "user");
    std::wstring domain = WStringOptionValue(options, "domain");
    std::wstring password = WStringOptionValue(options, "password");

    uint32_t mech_oid_int = UInt32OptionValue(options, "mechOID", 0);
    std::wstring mech_oid = GSS_MECH_OID_KRB5_STR;
    if (mech_oid_int == GSS_MECH_OID_SPNEGO) {
        mech_oid = GSS_MECH_OID_SPNEGO_STR;
    }

    KerberosWorker::Run(callback, "kerberos:InitializeClient", [=](KerberosWorker::SetOnFinishedHandler onFinished) {
        sspi_client_state* client_state = sspi_client_state_new();
        std::shared_ptr<sspi_result> result(auth_sspi_client_init(
            (WCHAR*)service.c_str(), gss_flags, (WCHAR*)user.c_str(), user.length(),
            (WCHAR*)domain.c_str(), domain.length(), (WCHAR*)password.c_str(), password.length(),
            (WCHAR*)mech_oid.c_str(), client_state), ResultDeleter);

        // must clean up state if we won't be using it, smart pointers won't help here unfortunately
        // because we can't `release` a shared pointer.
        if (result->code == AUTH_GSS_ERROR) {
            free(client_state);
        }

        return onFinished([=](KerberosWorker* worker) {
            Nan::HandleScope scope;
            if (result->code == AUTH_GSS_ERROR) {
                v8::Local<v8::Value> argv[] = {Nan::New(result->message).ToLocalChecked(), Nan::Null()};
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
    Nan::ThrowError("`initializeServer` is not implemented yet for windows");
}

NAN_METHOD(PrincipalDetails) {
    std::string service(*Nan::Utf8String(info[0]));
    std::string hostname(*Nan::Utf8String(info[1]));
    Nan::Callback* callback = new Nan::Callback(Nan::To<v8::Function>(info[2]).ToLocalChecked());
    Nan::ThrowError("`principalDetails` is not implemented yet for windows");
}

NAN_METHOD(CheckPassword) {
    std::string username(*Nan::Utf8String(info[0]));
    std::string password(*Nan::Utf8String(info[1]));
    std::string service(*Nan::Utf8String(info[2]));
    std::string defaultRealm(*Nan::Utf8String(info[3]));
    Nan::Callback* callback = new Nan::Callback(Nan::To<v8::Function>(info[4]).ToLocalChecked());
    Nan::ThrowError("`checkPassword` is not implemented yet for windows");
}
