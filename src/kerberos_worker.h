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

class ClientCleanWorker : public Nan::AsyncWorker {
 public:
  ClientCleanWorker(KerberosClientContext* context, Nan::Callback *callback)
    : AsyncWorker(callback, "kerberos:ClientCleanWorker"),
      _context(context)
  {}

  virtual void Execute() {
    _context->destroy();
  }

 private:
  KerberosClientContext* _context;
};

class ClientStepWorker : public Nan::AsyncWorker {
 public:
  ClientStepWorker(KerberosClientContext* context, std::string challenge, Nan::Callback *callback)
    : AsyncWorker(callback, "kerberos:ClientStepWorker"),
      _context(context),
      _challenge(challenge)
  {}

  virtual void Execute() {
    std::unique_ptr<gss_result, FreeDeleter> result(
      authenticate_gss_client_step(_context->state(), _challenge.c_str(), NULL));
    if (result->code == AUTH_GSS_ERROR) {
      SetErrorMessage(result->message);
      return;
    }
  }

 private:
  KerberosClientContext* _context;
  std::string _challenge;
};

class ClientWrapWorker : public Nan::AsyncWorker {
 public:
  ClientWrapWorker(KerberosClientContext* context, std::string challenge, std::string user, int protect, Nan::Callback *callback)
    : AsyncWorker(callback, "kerberos:ClientWrapWorker"),
      _context(context),
      _challenge(challenge),
      _user(user),
      _protect(protect)
  {}

  virtual void Execute() {
    std::unique_ptr<gss_result, FreeDeleter> result(
      authenticate_gss_client_wrap(_context->state(), _challenge.c_str(), _user.c_str(), _protect));
    if (result->code == AUTH_GSS_ERROR) {
      SetErrorMessage(result->message);
      return;
    }
  }

 private:
  KerberosClientContext* _context;
  std::string _challenge;
  std::string _user;
  int _protect;
};

class ClientUnwrapWorker : public Nan::AsyncWorker {
 public:
  ClientUnwrapWorker(KerberosClientContext* context, std::string challenge, Nan::Callback *callback)
    : AsyncWorker(callback, "kerberos:ClientUnwrapWorker"),
      _context(context),
      _challenge(challenge)
  {}

  virtual void Execute() {
    std::unique_ptr<gss_result, FreeDeleter> result(
      authenticate_gss_client_unwrap(_context->state(), _challenge.c_str()));
    if (result->code == AUTH_GSS_ERROR) {
      SetErrorMessage(result->message);
      return;
    }
  }

 private:
  KerberosClientContext* _context;
  std::string _challenge;
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

class ServerCleanWorker : public Nan::AsyncWorker {
 public:
  ServerCleanWorker(KerberosServerContext* context, Nan::Callback *callback)
    : AsyncWorker(callback, "kerberos:ServerCleanWorker"),
      _context(context)
  {}

  virtual void Execute() {
    _context->destroy();
  }

 private:
  KerberosServerContext* _context;
};

class ServerStepWorker : public Nan::AsyncWorker {
 public:
  ServerStepWorker(KerberosServerContext* context, std::string challenge, Nan::Callback *callback)
    : AsyncWorker(callback, "kerberos:ServerStepWorker"),
      _context(context),
      _challenge(challenge)
  {}

  virtual void Execute() {
    std::unique_ptr<gss_result, FreeDeleter> result(
      authenticate_gss_server_step(_context->state(), _challenge.c_str()));
    if (result->code == AUTH_GSS_ERROR) {
      SetErrorMessage(result->message);
      return;
    }
  }

 private:
  KerberosServerContext* _context;
  std::string _challenge;
};


#endif // KERBEROS_WORKER_H
