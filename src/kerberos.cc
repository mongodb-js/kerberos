#include "kerberos.h"

using v8::FunctionTemplate;

class DummyWorker : public Nan::AsyncWorker {
 public:
  DummyWorker(Nan::Callback *callback)
    : AsyncWorker(callback, "kerberos:DummyWorker") {}
  ~DummyWorker() {}

  void Execute () {}
};

NAN_METHOD(AuthGSSClientInit) {
  v8::MaybeLocal<v8::String> service = Nan::To<v8::String>(info[0]);
  v8::MaybeLocal<v8::Object> options = Nan::To<v8::Object>(info[1]);
  Nan::Callback *callback = new Nan::Callback(Nan::To<v8::Function>(info[2]).ToLocalChecked());

  AsyncQueueWorker(new DummyWorker(callback));
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

NAN_METHOD(AuthGSSClientResponse) {
  v8::MaybeLocal<v8::Object> context = Nan::To<v8::Object>(info[0]);
  Nan::Callback *callback = new Nan::Callback(Nan::To<v8::Function>(info[1]).ToLocalChecked());

  AsyncQueueWorker(new DummyWorker(callback));
}

NAN_METHOD(AuthGSSClientResponseConf) {
  v8::MaybeLocal<v8::Object> context = Nan::To<v8::Object>(info[0]);
  Nan::Callback *callback = new Nan::Callback(Nan::To<v8::Function>(info[1]).ToLocalChecked());

  AsyncQueueWorker(new DummyWorker(callback));
}

NAN_METHOD(AuthGSSClientUserName) {
  v8::MaybeLocal<v8::Object> context = Nan::To<v8::Object>(info[0]);
  Nan::Callback *callback = new Nan::Callback(Nan::To<v8::Function>(info[1]).ToLocalChecked());

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
  v8::MaybeLocal<v8::String> service = Nan::To<v8::String>(info[0]);
  Nan::Callback *callback = new Nan::Callback(Nan::To<v8::Function>(info[2]).ToLocalChecked());

  AsyncQueueWorker(new DummyWorker(callback));
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

NAN_METHOD(AuthGSSServerResponse) {
  v8::MaybeLocal<v8::Object> context = Nan::To<v8::Object>(info[0]);
  Nan::Callback *callback = new Nan::Callback(Nan::To<v8::Function>(info[1]).ToLocalChecked());

  AsyncQueueWorker(new DummyWorker(callback));
}

NAN_METHOD(AuthGSSServerUserName) {
  v8::MaybeLocal<v8::Object> context = Nan::To<v8::Object>(info[0]);
  Nan::Callback *callback = new Nan::Callback(Nan::To<v8::Function>(info[1]).ToLocalChecked());

  AsyncQueueWorker(new DummyWorker(callback));
}

NAN_METHOD(AuthGSSServerTargetName) {
  v8::MaybeLocal<v8::Object> context = Nan::To<v8::Object>(info[0]);
  Nan::Callback *callback = new Nan::Callback(Nan::To<v8::Function>(info[1]).ToLocalChecked());

  AsyncQueueWorker(new DummyWorker(callback));
}

NAN_MODULE_INIT(Init) {
  // Client
  Nan::Set(target, Nan::New("authGSSClientInit").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(AuthGSSClientInit)).ToLocalChecked());
  Nan::Set(target, Nan::New("authGSSClientClean").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(AuthGSSClientClean)).ToLocalChecked());
  Nan::Set(target, Nan::New("authGSSClientStep").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(AuthGSSClientStep)).ToLocalChecked());
  Nan::Set(target, Nan::New("authGSSClientResponse").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(AuthGSSClientResponse)).ToLocalChecked());
  Nan::Set(target, Nan::New("authGSSClientResponseConf").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(AuthGSSClientResponseConf)).ToLocalChecked());
  Nan::Set(target, Nan::New("authGSSClientUserName").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(AuthGSSClientUserName)).ToLocalChecked());
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
  Nan::Set(target, Nan::New("authGSSServerResponse").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(AuthGSSServerResponse)).ToLocalChecked());
  Nan::Set(target, Nan::New("authGSSServerUserName").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(AuthGSSServerUserName)).ToLocalChecked());
  Nan::Set(target, Nan::New("authGSSServerTargetName").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(AuthGSSServerTargetName)).ToLocalChecked());
}

NODE_MODULE(kerberos, Init)
