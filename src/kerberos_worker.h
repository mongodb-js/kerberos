#ifndef KERBEROS_WORKER_H
#define KERBEROS_WORKER_H

#include "nan.h"

// Provide a default custom delter for the `gss_result` type
struct FreeDeleter {
    void operator()(void* result) { free(result); }
};

class ClientInitWorker : public Nan::AsyncWorker {
 public:
  ClientInitWorker(std::string service, std::string principal, long int gss_flags, gss_OID mech_oid, Nan::Callback *callback)
    : AsyncWorker(callback, "kerberos:ClientInitWorker"),
      _service(service),
      _principal(principal),
      _gss_flags(gss_flags),
      _mech_oid(mech_oid),
      _client_state(NULL)
  {}

  virtual void Execute() {
    std::unique_ptr<gss_client_state, FreeDeleter> state(gss_client_state_new());
    std::unique_ptr<gss_result, FreeDeleter> result(
      authenticate_gss_client_init(_service.c_str(), _principal.c_str(), _gss_flags, _mech_oid, state.get()));

    if (result->code == AUTH_GSS_ERROR) {
      SetErrorMessage(result->message);
      return;
    }

    _client_state = state.release();
  }

 protected:
  virtual void HandleOKCallback() {
    Nan::HandleScope scope;
    v8::Local<v8::Object> context = KerberosClientContext::NewInstance(_client_state);
    v8::Local<v8::Value> argv[] = { Nan::Null(), context };
    callback->Call(2, argv, async_resource);
  }

 private:
  std::string _service;
  std::string _principal;
  long int _gss_flags;
  gss_OID _mech_oid;
  gss_client_state* _client_state;

};

class ServerInitWorker : public Nan::AsyncWorker {
 public:
  ServerInitWorker(std::string service, Nan::Callback *callback)
    : AsyncWorker(callback, "kerberos:ServerInitWorker"),
      _service(service),
      _server_state(NULL)
  {}

  virtual void Execute() {
    std::unique_ptr<gss_server_state, FreeDeleter> state(gss_server_state_new());
    std::unique_ptr<gss_result, FreeDeleter> result(
      authenticate_gss_server_init(_service.c_str(), state.get()));

    if (result->code == AUTH_GSS_ERROR) {
      SetErrorMessage(result->message);
      return;
    }

    _server_state = state.release();
  }

 protected:
  virtual void HandleOKCallback() {
    Nan::HandleScope scope;
    v8::Local<v8::Object> context = KerberosServerContext::NewInstance(_server_state);
    v8::Local<v8::Value> argv[] = { Nan::Null(), context };
    callback->Call(2, argv, async_resource);
  }

 private:
  std::string _service;
  gss_server_state* _server_state;

};

#endif // KERBEROS_WORKER_H
