#include "kerberos_context.h"

Nan::Persistent<v8::Function> KerberosClientContext::constructor;
NAN_MODULE_INIT(KerberosClientContext::Init) {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>();
  tpl->SetClassName(Nan::New("KerberosClientContext").ToLocalChecked());

  v8::Local<v8::ObjectTemplate> itpl = tpl->InstanceTemplate();
  itpl->SetInternalFieldCount(1);

  Nan::SetAccessor(itpl, Nan::New("username").ToLocalChecked(), KerberosClientContext::UserNameGetter);
  Nan::SetAccessor(itpl, Nan::New("response").ToLocalChecked(), KerberosClientContext::ResponseGetter);
  Nan::SetAccessor(itpl, Nan::New("responseConf").ToLocalChecked(), KerberosClientContext::ResponseConfGetter);

  constructor.Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(target, Nan::New("KerberosClientContext").ToLocalChecked(), Nan::GetFunction(tpl).ToLocalChecked());
}

v8::Local<v8::Object> KerberosClientContext::NewInstance(gss_client_state* state) {
  Nan::EscapableHandleScope scope;
  v8::Local<v8::Function> ctor = Nan::New<v8::Function>(KerberosClientContext::constructor);
  v8::Local<v8::Object> object = Nan::NewInstance(ctor).ToLocalChecked();
  KerberosClientContext *class_instance = new KerberosClientContext(state);
  class_instance->Wrap(object);
  return scope.Escape(object);
}

KerberosClientContext::KerberosClientContext(gss_client_state* state)
  : _state(state)
{}

KerberosClientContext::~KerberosClientContext()
{
}

void KerberosClientContext::destroy() {
  authenticate_gss_client_clean(_state);
}

NAN_GETTER(KerberosClientContext::UserNameGetter) {
  KerberosClientContext* context =
    Nan::ObjectWrap::Unwrap<KerberosClientContext>(info.Holder());

  (context->_state->username == NULL) ?
    info.GetReturnValue().Set(Nan::Null()) :
    info.GetReturnValue().Set(Nan::New(context->_state->username).ToLocalChecked());
}

NAN_GETTER(KerberosClientContext::ResponseGetter) {
  KerberosClientContext* context =
    Nan::ObjectWrap::Unwrap<KerberosClientContext>(info.Holder());

  (context->_state->response == NULL) ?
    info.GetReturnValue().Set(Nan::Null()) :
    info.GetReturnValue().Set(Nan::New(context->_state->response).ToLocalChecked());
}

NAN_GETTER(KerberosClientContext::ResponseConfGetter) {
  KerberosClientContext* context =
    Nan::ObjectWrap::Unwrap<KerberosClientContext>(info.Holder());

  info.GetReturnValue().Set(Nan::New(context->_state->responseConf));
}

Nan::Persistent<v8::Function> KerberosServerContext::constructor;
NAN_MODULE_INIT(KerberosServerContext::Init) {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>();
  tpl->SetClassName(Nan::New("KerberosServerContext").ToLocalChecked());

  v8::Local<v8::ObjectTemplate> itpl = tpl->InstanceTemplate();
  itpl->SetInternalFieldCount(1);

  Nan::SetAccessor(itpl, Nan::New("username").ToLocalChecked(), KerberosServerContext::UserNameGetter);
  Nan::SetAccessor(itpl, Nan::New("response").ToLocalChecked(), KerberosServerContext::ResponseGetter);
  Nan::SetAccessor(itpl, Nan::New("targetName").ToLocalChecked(), KerberosServerContext::TargetNameGetter);

  constructor.Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(target, Nan::New("KerberosServerContext").ToLocalChecked(), Nan::GetFunction(tpl).ToLocalChecked());
}

v8::Local<v8::Object> KerberosServerContext::NewInstance(gss_server_state* state) {
  Nan::EscapableHandleScope scope;
  v8::Local<v8::Function> ctor = Nan::New<v8::Function>(KerberosServerContext::constructor);
  v8::Local<v8::Object> object = Nan::NewInstance(ctor).ToLocalChecked();
  KerberosServerContext *class_instance = new KerberosServerContext(state);
  class_instance->Wrap(object);
  return scope.Escape(object);
}

KerberosServerContext::KerberosServerContext(gss_server_state* state)
  : _state(state)
{}

KerberosServerContext::~KerberosServerContext()
{
  // TODO: destroy the state with `authenticate_gss_server_clean` if it hasn't been already
}

void KerberosServerContext::destroy() {
  authenticate_gss_server_clean(_state);
}

NAN_GETTER(KerberosServerContext::UserNameGetter) {
  KerberosServerContext* context =
    Nan::ObjectWrap::Unwrap<KerberosServerContext>(info.Holder());

  (context->_state->username == NULL) ?
    info.GetReturnValue().Set(Nan::Null()) :
    info.GetReturnValue().Set(Nan::New(context->_state->username).ToLocalChecked());
}

NAN_GETTER(KerberosServerContext::ResponseGetter) {
  KerberosServerContext* context =
    Nan::ObjectWrap::Unwrap<KerberosServerContext>(info.Holder());

  (context->_state->response == NULL) ?
    info.GetReturnValue().Set(Nan::Null()) :
    info.GetReturnValue().Set(Nan::New(context->_state->response).ToLocalChecked());
}

NAN_GETTER(KerberosServerContext::TargetNameGetter) {
  KerberosServerContext* context =
    Nan::ObjectWrap::Unwrap<KerberosServerContext>(info.Holder());

  (context->_state->targetname == NULL) ?
    info.GetReturnValue().Set(Nan::Null()) :
    info.GetReturnValue().Set(Nan::New(context->_state->targetname).ToLocalChecked());
}
