#include <memory>

#include "../kerberos.h"
#include "../kerberos_client.h"
#include "../kerberos_common.h"
#include "../kerberos_server.h"
#include "../kerberos_worker.h"

NAN_METHOD(InitializeClient) {
    std::string service(*Nan::Utf8String(info[0]));
    v8::Local<v8::Object> options = Nan::To<v8::Object>(info[1]).ToLocalChecked();
    Nan::Callback* callback = new Nan::Callback(Nan::To<v8::Function>(info[2]).ToLocalChecked());
    Nan::ThrowError('`initializeClient` is not implemented yet for windows');
}

NAN_METHOD(InitializeServer) {
    std::string service(*Nan::Utf8String(info[0]));
    Nan::Callback* callback = new Nan::Callback(Nan::To<v8::Function>(info[1]).ToLocalChecked());
    Nan::ThrowError('`initializeServer` is not implemented yet for windows');
}

NAN_METHOD(PrincipalDetails) {
    std::string service(*Nan::Utf8String(info[0]));
    std::string hostname(*Nan::Utf8String(info[1]));
    Nan::Callback* callback = new Nan::Callback(Nan::To<v8::Function>(info[2]).ToLocalChecked());
    Nan::ThrowError('`principalDetails` is not implemented yet for windows');
}

NAN_METHOD(CheckPassword) {
    std::string username(*Nan::Utf8String(info[0]));
    std::string password(*Nan::Utf8String(info[1]));
    std::string service(*Nan::Utf8String(info[2]));
    std::string defaultRealm(*Nan::Utf8String(info[3]));
    Nan::Callback* callback = new Nan::Callback(Nan::To<v8::Function>(info[4]).ToLocalChecked());
    Nan::ThrowError('`checkPassword` is not implemented yet for windows');
}
