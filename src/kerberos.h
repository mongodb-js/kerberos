#ifndef KERBEROS_NATIVE_EXTENSION_H
#define KERBEROS_NATIVE_EXTENSION_H

// We generally only target N-API version 4, but the instance data
// feature is only available in N-API version 6. However, it is
// available in all Node.js versions that have N-API version 4
#define NAPI_VERSION 6
// as an experimental feature (that has not been changed since then).
#define NAPI_EXPERIMENTAL
#define NODE_API_EXPERIMENTAL_NOGC_ENV_OPT_OUT

#include <napi.h>
#include "kerberos_common.h"

namespace node_kerberos {

bool kerberos_libraries_available(std::string* error_out);

class KerberosServer : public Napi::ObjectWrap<KerberosServer> {
   public:
    static Napi::Function Init(Napi::Env env);
    static Napi::Object NewInstance(Napi::Env env, std::shared_ptr<krb_server_state> state);

    std::shared_ptr<krb_server_state> state() const;

   private:
    Napi::Value UserNameGetter(const Napi::CallbackInfo& info);
    Napi::Value ResponseGetter(const Napi::CallbackInfo& info);
    Napi::Value TargetNameGetter(const Napi::CallbackInfo& info);
    Napi::Value ContextCompleteGetter(const Napi::CallbackInfo& info);

    void Step(const Napi::CallbackInfo& info);

   private:
    friend class Napi::ObjectWrap<KerberosServer>;
    explicit KerberosServer(const Napi::CallbackInfo& info);

    std::shared_ptr<krb_server_state> _state;
};

class KerberosClient : public Napi::ObjectWrap<KerberosClient> {
   public:
    static Napi::Function Init(Napi::Env env);
    static Napi::Object NewInstance(Napi::Env env, std::shared_ptr<krb_client_state> state);

    std::shared_ptr<krb_client_state> state() const;

   private:
    Napi::Value UserNameGetter(const Napi::CallbackInfo& info);
    Napi::Value ResponseGetter(const Napi::CallbackInfo& info);
    Napi::Value ResponseConfGetter(const Napi::CallbackInfo& info);
    Napi::Value ContextCompleteGetter(const Napi::CallbackInfo& info);

    void Step(const Napi::CallbackInfo& info);
    void UnwrapData(const Napi::CallbackInfo& info);
    void WrapData(const Napi::CallbackInfo& info);

    int ParseWrapOptionsProtect(const Napi::Object& options);

   private:
    friend class Napi::ObjectWrap<KerberosClient>;
    explicit KerberosClient(const Napi::CallbackInfo& info);

    std::shared_ptr<krb_client_state> _state;
};

void PrincipalDetails(const Napi::CallbackInfo& info);
void InitializeClient(const Napi::CallbackInfo& info);
void InitializeServer(const Napi::CallbackInfo& info);
void CheckPassword(const Napi::CallbackInfo& info);

// NOTE: explicitly used for unit testing `defineOperation`, not meant to be exported
void TestMethod(const Napi::CallbackInfo& info);

std::string ToStringWithNonStringAsEmpty(Napi::Value value);

}

#endif  // KERBEROS_NATIVE_EXTENSION_H
