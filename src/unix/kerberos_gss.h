/**
 * Copyright (c) 2006-2013 Apple Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 **/

#ifndef KERBEROS_GSS_H
#define KERBEROS_GSS_H

extern "C" {
    #include <gssapi/gssapi.h>
    #include <gssapi/gssapi_generic.h>
    #include <gssapi/gssapi_krb5.h>
}

#include <string>

inline const char* krb5_get_err_text(const krb5_context&, krb5_error_code code) {
    return error_message(code);
}

#define AUTH_GSS_ERROR -1
#define AUTH_GSS_COMPLETE 1
#define AUTH_GSS_CONTINUE 0

#define GSS_AUTH_P_NONE 1
#define GSS_AUTH_P_INTEGRITY 2
#define GSS_AUTH_P_PRIVACY 4

typedef struct {
    int code;
    std::string message;
    std::string data;
} gss_result;

typedef struct {
    gss_ctx_id_t context;
    gss_name_t server_name;
    gss_OID mech_oid;
    long int gss_flags;
    gss_cred_id_t client_creds;
    char* username;
    char* response;
    int responseConf;
    bool context_complete;
} gss_client_state;

typedef struct {
    gss_ctx_id_t context;
    gss_name_t server_name;
    gss_name_t client_name;
    gss_cred_id_t server_creds;
    gss_cred_id_t client_creds;
    char* username;
    char* targetname;
    char* response;
    bool context_complete;
} gss_server_state;

gss_client_state* gss_client_state_new();
gss_server_state* gss_server_state_new();

gss_result server_principal_details(const char* service, const char* hostname);

gss_result authenticate_gss_client_init(const char* service,
                                        const char* principal,
                                        long int gss_flags,
                                        gss_server_state* delegatestate,
                                        gss_OID mech_oid,
                                        gss_client_state* state);

int authenticate_gss_client_clean(gss_client_state* state);
gss_result authenticate_gss_client_step(gss_client_state* state,
                                        const char* challenge,
                                        struct gss_channel_bindings_struct* channel_bindings);
gss_result authenticate_gss_client_unwrap(gss_client_state* state, const char* challenge);
gss_result authenticate_gss_client_wrap(gss_client_state* state,
                                        const char* challenge,
                                        const char* user,
                                        int protect);
gss_result authenticate_gss_server_init(const char* service, gss_server_state* state);
int authenticate_gss_server_clean(gss_server_state* state);
gss_result authenticate_gss_server_step(gss_server_state* state, const char* challenge);

gss_result authenticate_user_krb5pwd(const char* user,
                                     const char* pswd,
                                     const char* service,
                                     const char* default_realm);

#endif
