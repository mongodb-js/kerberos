#include <memory>

#include "../kerberos_worker.h"
#include "../kerberos_server.h"

KerberosServer::~KerberosServer() {
    // if (_state != NULL) {
    //     authenticate_gss_server_clean(_state);
    //     _state = NULL;
    // }
}

NAN_METHOD(KerberosServer::Step) {
    KerberosServer* server = Nan::ObjectWrap::Unwrap<KerberosServer>(info.This());
    std::string challenge(*Nan::Utf8String(info[0]));
    Nan::Callback* callback = new Nan::Callback(Nan::To<v8::Function>(info[1]).ToLocalChecked());
    Nan::ThrowError("`KerberosServer::Step` is not implemented yet for windows");
}
