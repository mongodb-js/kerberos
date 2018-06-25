#include <memory>

#include "kerberos.h"
#include "kerberos_context.h"
#include "kerberos_worker.h"

using v8::FunctionTemplate;

#define GSS_MECH_OID_KRB5 9
#define GSS_MECH_OID_SPNEGO 6

static char krb5_mech_oid_bytes [] = "\x2a\x86\x48\x86\xf7\x12\x01\x02\x02";
gss_OID_desc krb5_mech_oid = { 9, &krb5_mech_oid_bytes };

static char spnego_mech_oid_bytes[] = "\x2b\x06\x01\x05\x05\x02";
gss_OID_desc spnego_mech_oid = { 6, &spnego_mech_oid_bytes };

class DummyWorker : public Nan::AsyncWorker {
 public:
  DummyWorker(Nan::Callback *callback)
    : AsyncWorker(callback, "kerberos:DummyWorker") {}
  ~DummyWorker() {}

  void Execute () {}
};

NAN_INLINE std::string StringOptionValue(v8::Local<v8::Object> options,
                                         const char* _key) {
  Nan::HandleScope scope;
  v8::Local<v8::String> key = Nan::New(_key).ToLocalChecked();
  return !options.IsEmpty()
    && options->Has(key)
    && options->Get(key)->IsString()
    ? std::string(*(Nan::Utf8String(options->Get(key))))
    : std::string();
}

NAN_INLINE uint32_t UInt32OptionValue(v8::Local<v8::Object> options,
                                      const char* _key,
                                      uint32_t def) {
  Nan::HandleScope scope;
  v8::Local<v8::String> key = Nan::New(_key).ToLocalChecked();
  return !options.IsEmpty()
    && options->Has(key)
    && options->Get(key)->IsNumber()
    ? options->Get(key)->Uint32Value()
    : def;
}

NAN_METHOD(AuthGSSClientInit) {
  std::string service(*Nan::Utf8String(info[0]));
  v8::Local<v8::Object> options = Nan::To<v8::Object>(info[1]).ToLocalChecked();
  Nan::Callback* callback = new Nan::Callback(Nan::To<v8::Function>(info[2]).ToLocalChecked());

  std::string principal = StringOptionValue(options, "principal");
  uint32_t gss_flags = UInt32OptionValue(options, "gssFlags", 0);
  uint32_t mech_oid_int = UInt32OptionValue(options, "mechOID", 0);
  gss_OID mech_oid = GSS_C_NO_OID;
  if (mech_oid_int == GSS_MECH_OID_KRB5) {
    mech_oid = &krb5_mech_oid;
  } else if (mech_oid_int == GSS_MECH_OID_SPNEGO) {
    mech_oid = &spnego_mech_oid;
  }

  AsyncQueueWorker(new ClientInitWorker(service, principal, gss_flags, GSS_C_NO_OID, callback));
}

NAN_METHOD(AuthGSSClientClean) {
  v8::MaybeLocal<v8::Object> context = Nan::To<v8::Object>(info[0]);
  Nan::Callback *callback = new Nan::Callback(Nan::To<v8::Function>(info[1]).ToLocalChecked());

  AsyncQueueWorker(new DummyWorker(callback));
}

NAN_METHOD(AuthGSSClientStep) {
  v8::MaybeLocal<v8::Object> context = Nan::To<v8::Object>(info[0]);
  v8::MaybeLocal<v8::String> challenge = Nan::To<v8::String>(info[1]);
  Nan::Callback *callback = new Nan::Callback(Nan::To<v8::Function>(info[2]).ToLocalChecked());

  AsyncQueueWorker(new DummyWorker(callback));
}

NAN_METHOD(AuthGSSClientUnwrap) {
  v8::MaybeLocal<v8::Object> context = Nan::To<v8::Object>(info[0]);
  v8::MaybeLocal<v8::String> challenge = Nan::To<v8::String>(info[1]);
  Nan::Callback *callback = new Nan::Callback(Nan::To<v8::Function>(info[2]).ToLocalChecked());

  AsyncQueueWorker(new DummyWorker(callback));
}

NAN_METHOD(AuthGSSClientWrap) {
  v8::MaybeLocal<v8::String> service = Nan::To<v8::String>(info[0]);
  v8::MaybeLocal<v8::Object> options = Nan::To<v8::Object>(info[1]);
  Nan::Callback *callback = new Nan::Callback(Nan::To<v8::Function>(info[2]).ToLocalChecked());

  AsyncQueueWorker(new DummyWorker(callback));
}

NAN_METHOD(AuthGSSServerInit) {
  std::string service = *(Nan::Utf8String(info[0]));
  Nan::Callback* callback = new Nan::Callback(Nan::To<v8::Function>(info[2]).ToLocalChecked());

  AsyncQueueWorker(new ServerInitWorker(service, callback));
}

NAN_METHOD(AuthGSSServerClean) {
  v8::MaybeLocal<v8::Object> context = Nan::To<v8::Object>(info[0]);
  Nan::Callback *callback = new Nan::Callback(Nan::To<v8::Function>(info[1]).ToLocalChecked());

  AsyncQueueWorker(new DummyWorker(callback));
}

NAN_METHOD(AuthGSSServerStep) {
  v8::MaybeLocal<v8::Object> context = Nan::To<v8::Object>(info[0]);
  v8::MaybeLocal<v8::String> challenge = Nan::To<v8::String>(info[1]);
  Nan::Callback *callback = new Nan::Callback(Nan::To<v8::Function>(info[2]).ToLocalChecked());

  AsyncQueueWorker(new DummyWorker(callback));
}

NAN_MODULE_INIT(Init) {
  // Custom types
  KerberosClientContext::Init(target);
  KerberosServerContext::Init(target);

  // Client
  Nan::Set(target, Nan::New("authGSSClientInit").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(AuthGSSClientInit)).ToLocalChecked());
  Nan::Set(target, Nan::New("authGSSClientClean").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(AuthGSSClientClean)).ToLocalChecked());
  Nan::Set(target, Nan::New("authGSSClientStep").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(AuthGSSClientStep)).ToLocalChecked());
  Nan::Set(target, Nan::New("authGSSClientUnwrap").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(AuthGSSClientUnwrap)).ToLocalChecked());
  Nan::Set(target, Nan::New("authGSSClientWrap").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(AuthGSSClientWrap)).ToLocalChecked());

  // Server
  Nan::Set(target, Nan::New("authGSSServerInit").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(AuthGSSServerInit)).ToLocalChecked());
  Nan::Set(target, Nan::New("authGSSServerClean").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(AuthGSSServerClean)).ToLocalChecked());
  Nan::Set(target, Nan::New("authGSSServerStep").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(AuthGSSServerStep)).ToLocalChecked());
}

NODE_MODULE(kerberos, Init)
