/*
 * Copyright 2016 MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef KERBEROS_SSPI_H
#define KERBEROS_SSPI_H

#define SECURITY_WIN32 1 /* Required for SSPI */

#include <windows.h>
#include <sspi.h>
#include <string>

namespace node_kerberos {

#define AUTH_GSS_ERROR -1
#define AUTH_GSS_COMPLETE 1
#define AUTH_GSS_CONTINUE 0

typedef struct {
    int code;
    std::string message;
    std::string data;
} sspi_result;

struct sspi_client_state {
    CredHandle cred;
    CtxtHandle ctx;
    WCHAR* spn = nullptr;
    SEC_CHAR* response = nullptr;
    SEC_CHAR* username = nullptr;
    ULONG flags;
    UCHAR haveCred = 0;
    UCHAR haveCtx = 0;
    ULONG qop;

    INT responseConf = 0;
    BOOL context_complete = FALSE;

    sspi_client_state() {}
    sspi_client_state(const sspi_client_state&) = delete;
    sspi_client_state& operator=(const sspi_client_state&) = delete;
    ~sspi_client_state();
};

struct sspi_server_state {
    // This is unused currently
    WCHAR* username = nullptr;
    WCHAR* response = nullptr;
    BOOL context_complete = FALSE;
    char* targetname = nullptr;
};

sspi_client_state* sspi_client_state_new();
VOID auth_sspi_client_clean(sspi_client_state* state);
sspi_result auth_sspi_client_init(WCHAR* service,
                                   ULONG flags,
                                   WCHAR* user,
                                   ULONG ulen,
                                   WCHAR* domain,
                                   ULONG dlen,
                                   WCHAR* password,
                                   ULONG plen,
                                   WCHAR* mechoid,
                                   sspi_client_state* state);

sspi_result auth_sspi_client_step(sspi_client_state* state, SEC_CHAR* challenge, SecPkgContext_Bindings* sec_pkg_context_bindings);
sspi_result auth_sspi_client_unwrap(sspi_client_state* state, SEC_CHAR* challenge);
sspi_result auth_sspi_client_wrap(sspi_client_state* state, SEC_CHAR* data, SEC_CHAR* user, ULONG ulen, INT protect);

}

#endif
