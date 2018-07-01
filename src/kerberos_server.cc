#include "kerberos_server.h"

Nan::Persistent<v8::Function> KerberosServer::constructor;
NAN_MODULE_INIT(KerberosServer::Init) {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>();
  tpl->SetClassName(Nan::New("KerberosServer").ToLocalChecked());
  Nan::SetPrototypeMethod(tpl, "step", Step);

  v8::Local<v8::ObjectTemplate> itpl = tpl->InstanceTemplate();
  itpl->SetInternalFieldCount(1);

  Nan::SetAccessor(itpl, Nan::New("username").ToLocalChecked(), KerberosServer::UserNameGetter);
  Nan::SetAccessor(itpl, Nan::New("response").ToLocalChecked(), KerberosServer::ResponseGetter);
  Nan::SetAccessor(itpl, Nan::New("targetName").ToLocalChecked(), KerberosServer::TargetNameGetter);

  constructor.Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(target, Nan::New("KerberosServer").ToLocalChecked(), Nan::GetFunction(tpl).ToLocalChecked());
}

v8::Local<v8::Object> KerberosServer::NewInstance(gss_server_state* state) {
  Nan::EscapableHandleScope scope;
  v8::Local<v8::Function> ctor = Nan::New<v8::Function>(KerberosServer::constructor);
  v8::Local<v8::Object> object = Nan::NewInstance(ctor).ToLocalChecked();
  KerberosServer *class_instance = new KerberosServer(state);
  class_instance->Wrap(object);
  return scope.Escape(object);
}

KerberosServer::KerberosServer(gss_server_state* state)
  : _state(state)
{}

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
  KerberosServer* server = Nan::ObjectWrap::Unwrap<KerberosServer>(info.Holder());
  (server->_state->username == NULL) ?
    info.GetReturnValue().Set(Nan::Null()) :
    info.GetReturnValue().Set(Nan::New(server->_state->username).ToLocalChecked());
}

NAN_GETTER(KerberosServer::ResponseGetter) {
  KerberosServer* server = Nan::ObjectWrap::Unwrap<KerberosServer>(info.Holder());
  (server->_state->response == NULL) ?
    info.GetReturnValue().Set(Nan::Null()) :
    info.GetReturnValue().Set(Nan::New(server->_state->response).ToLocalChecked());
}

NAN_GETTER(KerberosServer::TargetNameGetter) {
  KerberosServer* server = Nan::ObjectWrap::Unwrap<KerberosServer>(info.Holder());
  (server->_state->targetname == NULL) ?
    info.GetReturnValue().Set(Nan::Null()) :
    info.GetReturnValue().Set(Nan::New(server->_state->targetname).ToLocalChecked());
}

class ServerStepWorker : public Nan::AsyncWorker {
 public:
  ServerStepWorker(KerberosServer* client, std::string challenge, Nan::Callback *callback)
    : AsyncWorker(callback, "kerberos:ServerStepWorker"),
      _client(client),
      _challenge(challenge)
  {}

  virtual void Execute() {
    std::unique_ptr<gss_result, FreeDeleter> result(
      authenticate_gss_server_step(_client->state(), _challenge.c_str()));
    if (result->code == AUTH_GSS_ERROR) {
      SetErrorMessage(result->message);
      return;
    }
  }

 private:
  KerberosServer* _client;
  std::string _challenge;
};

NAN_METHOD(KerberosServer::Step) {
  KerberosServer* client =
    Nan::ObjectWrap::Unwrap<KerberosServer>(info[0]->ToObject());
  std::string challenge(*Nan::Utf8String(info[1]));
  Nan::Callback *callback = new Nan::Callback(Nan::To<v8::Function>(info[2]).ToLocalChecked());

  AsyncQueueWorker(new ServerStepWorker(client, challenge, callback));
}
