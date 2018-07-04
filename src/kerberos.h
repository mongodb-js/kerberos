
#ifndef KERBEROS_NATIVE_EXTENSION_H
#define KERBEROS_NATIVE_EXTENSION_H

#include <nan.h>

NAN_METHOD(ServerPrincipalDetails);
NAN_METHOD(InitializeClient);
NAN_METHOD(InitializeServer);
NAN_METHOD(CheckPassword);

#endif
