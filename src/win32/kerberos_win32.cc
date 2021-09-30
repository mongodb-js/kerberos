#include <memory>

#include "../kerberos.h"
#include "../kerberos_worker.h"

namespace node_kerberos {

using namespace Napi;

inline std::wstring ToWStringWithNonStringAsEmpty(Value value) {
    static_assert(sizeof(std::wstring::value_type) == sizeof(std::u16string::value_type),
        "wstring and u16string have the same value type on Windows");
    if (!value.IsString()) {
        return std::wstring();
    }
    std::u16string u16 = value.As<String>();
    return std::wstring(u16.begin(), u16.end());
}

#define GSS_MECH_OID_KRB5 9
#define GSS_MECH_OID_SPNEGO 6
#define GSS_MECH_OID_KRB5_STR L"Kerberos"
#define GSS_MECH_OID_SPNEGO_STR L"Negotiate"

#define GSS_C_MUTUAL_FLAG 2
#define GSS_C_REPLAY_FLAG 4
#define GSS_C_SEQUENCE_FLAG 8

/// KerberosClient
void KerberosClient::Step(const CallbackInfo& info) {
    auto state = this->state();
    std::string challenge = info[0].ToString();
    Function callback = info[1].As<Function>();

    KerberosWorker::Run(callback, "kerberos:ClientStep", [=](KerberosWorker::SetOnFinishedHandler onFinished) {
        sspi_result result =
            auth_sspi_client_step(state.get(), (SEC_CHAR*)challenge.c_str(), NULL);

        return onFinished([=](KerberosWorker* worker) {
            Napi::Env env = worker->Env();
            if (result.code == AUTH_GSS_ERROR) {
                worker->Call(std::initializer_list<napi_value>
                    { Error::New(env, result.message).Value(), env.Null() });
                return;
            }

            Napi::Value response = env.Null();
            if (state->response != nullptr) {
                response = String::New(env, state->response);
            }

            worker->Call(std::initializer_list<napi_value>
                { env.Null(), response });
        });
    });
}

void KerberosClient::UnwrapData(const CallbackInfo& info) {
    auto state = this->state();
    std::string challenge = info[0].ToString();
    Function callback = info[1].As<Function>();

    KerberosWorker::Run(callback, "kerberos:ClientUnwrap", [=](KerberosWorker::SetOnFinishedHandler onFinished) {
        sspi_result result =
            auth_sspi_client_unwrap(state.get(), (SEC_CHAR*)challenge.c_str());

        return onFinished([=](KerberosWorker* worker) {
            Napi::Env env = worker->Env();
            if (result.code == AUTH_GSS_ERROR) {
                worker->Call(std::initializer_list<napi_value>
                    { Error::New(env, result.message).Value(), env.Null() });
                return;
            }

            worker->Call(std::initializer_list<napi_value>
                { env.Null(), String::New(env, state->response) });
        });
    });
}

void KerberosClient::WrapData(const CallbackInfo& info) {
    auto state = this->state();
    std::string challenge = info[0].ToString();
    Object options = info[1].ToObject();
    Function callback = info[2].As<Function>();
    std::string user = ToStringWithNonStringAsEmpty(options["user"]);
    int protect = 0; // NOTE: this should be an option

    KerberosWorker::Run(callback, "kerberos:ClientWrap", [=](KerberosWorker::SetOnFinishedHandler onFinished) {
        sspi_result result = auth_sspi_client_wrap(
            state.get(), (SEC_CHAR*)challenge.c_str(), (SEC_CHAR*)user.c_str(), user.length(), protect);

        return onFinished([=](KerberosWorker* worker) {
            Napi::Env env = worker->Env();

            if (result.code == AUTH_GSS_ERROR) {
                worker->Call(std::initializer_list<napi_value>
                    { Error::New(env, result.message).Value(), env.Null() });
                return;
            }

            worker->Call(std::initializer_list<napi_value>
                { env.Null(), String::New(env, state->response) });
        });
    });
}

/// KerberosServer
void KerberosServer::Step(const CallbackInfo& info) {
    throw Error::New(info.Env(), "`KerberosServer::Step` is not implemented yet for windows");
}

/// Global Methods
void InitializeClient(const CallbackInfo& info) {
    std::wstring service = ToWStringWithNonStringAsEmpty(info[0]);
    Object options = info[1].ToObject();
    Function callback = info[2].As<Function>();

    std::wstring user = ToWStringWithNonStringAsEmpty(options["user"]);
    std::wstring domain = ToWStringWithNonStringAsEmpty(options["domain"]);
    std::wstring password = ToWStringWithNonStringAsEmpty(options["password"]);
    Value flags_v = options["flags"];
    ULONG gss_flags = flags_v.IsNumber() ? flags_v.As<Number>().Uint32Value() : GSS_C_MUTUAL_FLAG|GSS_C_SEQUENCE_FLAG;
    Value mech_oid_v = options["mechOID"];
    uint32_t mech_oid_int = mech_oid_v.IsNumber() ? mech_oid_v.As<Number>().Uint32Value() : 0;
    std::wstring mech_oid = GSS_MECH_OID_KRB5_STR;
    if (mech_oid_int == GSS_MECH_OID_SPNEGO) {
        mech_oid = GSS_MECH_OID_SPNEGO_STR;
    }

    KerberosWorker::Run(callback, "kerberos:InitializeClient", [=](KerberosWorker::SetOnFinishedHandler onFinished) {
        auto client_state = std::make_shared<sspi_client_state>();
        sspi_result result = auth_sspi_client_init(
            (WCHAR*)service.c_str(), gss_flags, (WCHAR*)user.c_str(), user.length(),
            (WCHAR*)domain.c_str(), domain.length(), (WCHAR*)password.c_str(), password.length(),
            (WCHAR*)mech_oid.c_str(), client_state.get());

        return onFinished([=](KerberosWorker* worker) {
            Napi::Env env = worker->Env();
            if (result.code == AUTH_GSS_ERROR) {
                worker->Call(std::initializer_list<napi_value>
                    { Error::New(env, result.message).Value(), env.Null() });
                return;
            }

            worker->Call(std::initializer_list<napi_value>
                { env.Null(), KerberosClient::NewInstance(env, std::move(client_state)) });
        });
    });
}

void InitializeServer(const CallbackInfo& info) {
    throw Error::New(info.Env(), "`initializeServer` is not implemented yet for windows");
}

void PrincipalDetails(const CallbackInfo& info) {
    throw Error::New(info.Env(), "`principalDetails` is not implemented yet for windows");
}

void CheckPassword(const CallbackInfo& info) {
    throw Error::New(info.Env(), "`checkPassword` is not implemented yet for windows");
}

}
