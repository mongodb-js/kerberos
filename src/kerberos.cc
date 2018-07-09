#include "kerberos.h"
#include "kerberos_client.h"
#include "kerberos_server.h"

NAN_MODULE_INIT(Init) {
    // Custom types
    KerberosClient::Init(target);
    KerberosServer::Init(target);

    Nan::Set(target,
             Nan::New("initializeClient").ToLocalChecked(),
             Nan::GetFunction(Nan::New<v8::FunctionTemplate>(InitializeClient)).ToLocalChecked());
    Nan::Set(target,
             Nan::New("initializeServer").ToLocalChecked(),
             Nan::GetFunction(Nan::New<v8::FunctionTemplate>(InitializeServer)).ToLocalChecked());
    Nan::Set(target,
             Nan::New("principalDetails").ToLocalChecked(),
             Nan::GetFunction(Nan::New<v8::FunctionTemplate>(PrincipalDetails)).ToLocalChecked());
    Nan::Set(target,
             Nan::New("checkPassword").ToLocalChecked(),
             Nan::GetFunction(Nan::New<v8::FunctionTemplate>(CheckPassword)).ToLocalChecked());
}

NODE_MODULE(kerberos, Init)
