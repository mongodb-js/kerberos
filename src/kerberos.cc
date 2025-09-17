#include "kerberos.h"

#include "kerberos_worker.h"

/// KerberosClient
namespace node_kerberos {

using namespace Napi;

namespace {
struct InstanceData {
    Reference<Function> KerberosClientCtor;
    Reference<Function> KerberosServerCtor;
};

static constexpr napi_property_attributes writable_and_configurable =
    static_cast<napi_property_attributes>(static_cast<int>(napi_writable) |
                                          static_cast<int>(napi_configurable));

inline String NewMaybeWideString(Env env, const char* str) {
    return String::New(env, str);
}
#ifdef _WIN32
inline String NewMaybeWideString(Env env, const WCHAR* str) {
    static_assert(sizeof(std::wstring::value_type) == sizeof(std::u16string::value_type),
                  "wstring and u16string have the same value type on Windows");
    std::wstring wstr(str);
    std::u16string u16(wstr.begin(), wstr.end());
    return String::New(env, u16);
}
#endif
}  // namespace

std::string ToStringWithNonStringAsEmpty(Napi::Value value) {
    if (!value.IsString()) {
        return std::string();
    }
    return value.As<String>();
}

int KerberosClient::ParseWrapOptionsProtect(const Napi::Object& options) {
    if (!options.Has("protect"))
        return 0;

    if (!options.Get("protect").IsBoolean()) {
        throw TypeError::New(options.Env(), "options.protect must be a boolean.");
    }

    bool protect = options.Get("protect").As<Boolean>();
    return protect ? 1 : 0;
}

Function KerberosClient::Init(Napi::Env env) {
    return DefineClass(
        env,
        "KerberosClient",
        {InstanceMethod("step", &KerberosClient::Step, writable_and_configurable),
         InstanceMethod("wrap", &KerberosClient::WrapData, writable_and_configurable),
         InstanceMethod("unwrap", &KerberosClient::UnwrapData, writable_and_configurable),
         InstanceAccessor("username", &KerberosClient::UserNameGetter, nullptr),
         InstanceAccessor("response", &KerberosClient::ResponseGetter, nullptr),
         InstanceAccessor("responseConf", &KerberosClient::ResponseConfGetter, nullptr),
         InstanceAccessor("contextComplete", &KerberosClient::ContextCompleteGetter, nullptr)});
}

Object KerberosClient::NewInstance(Napi::Env env, std::shared_ptr<krb_client_state> state) {
    InstanceData* instance_data = env.GetInstanceData<InstanceData>();
    Object obj = instance_data->KerberosClientCtor.Value().New({});
    KerberosClient* instance = KerberosClient::Unwrap(obj);
    instance->_state = std::move(state);
    return obj;
}

KerberosClient::KerberosClient(const CallbackInfo& info) : ObjectWrap(info) {}

std::shared_ptr<krb_client_state> KerberosClient::state() const {
    return _state;
}

Value KerberosClient::UserNameGetter(const CallbackInfo& info) {
    const auto* username = state()->username;
    if (username == nullptr)
        return Env().Null();
    return NewMaybeWideString(Env(), username);
}

Value KerberosClient::ResponseGetter(const CallbackInfo& info) {
    const auto* response = state()->response;
    if (response == nullptr)
        return Env().Null();
    return NewMaybeWideString(Env(), response);
}

Value KerberosClient::ResponseConfGetter(const CallbackInfo& info) {
    return Number::New(Env(), state()->responseConf);
}

Value KerberosClient::ContextCompleteGetter(const CallbackInfo& info) {
    return Boolean::New(Env(), state()->context_complete);
}

/// KerberosServer
Function KerberosServer::Init(Napi::Env env) {
    return DefineClass(
        env,
        "KerberosServer",
        {InstanceMethod("step", &KerberosServer::Step, writable_and_configurable),
         InstanceAccessor("username", &KerberosServer::UserNameGetter, nullptr),
         InstanceAccessor("response", &KerberosServer::ResponseGetter, nullptr),
         InstanceAccessor("targetName", &KerberosServer::TargetNameGetter, nullptr),
         InstanceAccessor("contextComplete", &KerberosServer::ContextCompleteGetter, nullptr)});
}

Object KerberosServer::NewInstance(Napi::Env env, std::shared_ptr<krb_server_state> state) {
    InstanceData* instance_data = env.GetInstanceData<InstanceData>();
    Object obj = instance_data->KerberosServerCtor.Value().New({});
    KerberosServer* instance = KerberosServer::Unwrap(obj);
    instance->_state = std::move(state);
    return obj;
}

KerberosServer::KerberosServer(const CallbackInfo& info) : ObjectWrap(info) {}

std::shared_ptr<krb_server_state> KerberosServer::state() const {
    return _state;
}

Value KerberosServer::UserNameGetter(const CallbackInfo& info) {
    const auto* username = state()->username;
    if (username == nullptr)
        return Env().Null();
    return NewMaybeWideString(Env(), username);
}

Value KerberosServer::ResponseGetter(const CallbackInfo& info) {
    const auto* response = state()->response;
    if (response == nullptr)
        return Env().Null();
    return NewMaybeWideString(Env(), response);
}

Value KerberosServer::TargetNameGetter(const CallbackInfo& info) {
    const auto* targetname = state()->targetname;
    if (targetname == nullptr)
        return Env().Null();
    return NewMaybeWideString(Env(), targetname);
}

Value KerberosServer::ContextCompleteGetter(const CallbackInfo& info) {
    return Boolean::New(Env(), state()->context_complete);
}

Value GetDefinedNapiVersion(const CallbackInfo& info) {
    return String::New(info.Env(), std::to_string(NAPI_VERSION));
}

static Object Init(Env env, Object exports) {
    std::string libraries_unavailable_error;
    if (!kerberos_libraries_available(&libraries_unavailable_error)) {
        throw Error::New(env, libraries_unavailable_error);
    }
    Function KerberosClientCtor = KerberosClient::Init(env);
    Function KerberosServerCtor = KerberosServer::Init(env);
    exports["KerberosClient"] = KerberosClientCtor;
    exports["KerberosServer"] = KerberosServerCtor;
    env.SetInstanceData(new InstanceData{Reference<Function>::New(KerberosClientCtor, 1),
                                         Reference<Function>::New(KerberosServerCtor, 1)});
    exports["initializeClient"] = Function::New(env, InitializeClient);
    exports["initializeServer"] = Function::New(env, InitializeServer);
    exports["principalDetails"] = Function::New(env, PrincipalDetails);
    exports["checkPassword"] = Function::New(env, CheckPassword);
    exports["definedNapiVersion"] = Function::New(env, GetDefinedNapiVersion);
    return exports;
}

NODE_API_MODULE(kerberos, Init)

}  // namespace node_kerberos
