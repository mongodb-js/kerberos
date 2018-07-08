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

#include "kerberos_gss.h"

#include "base64.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

static gss_result* gss_success_result(int ret);
static gss_result* gss_error_result(OM_uint32 err_maj, OM_uint32 err_min);
static gss_result* gss_error_result_with_message(const char* message);
static gss_result* gss_error_result_with_message_and_code(const char* mesage, int code);

gss_client_state* gss_client_state_new() {
    gss_client_state* state = (gss_client_state*)malloc(sizeof(gss_client_state));
    state->username = NULL;
    state->response = NULL;
    state->responseConf = 0;
    state->context_complete = false;

    return state;
}

gss_server_state* gss_server_state_new() {
    gss_server_state* state = (gss_server_state*)malloc(sizeof(gss_server_state));
    state->username = NULL;
    state->response = NULL;
    state->targetname = NULL;
    state->context_complete = false;

    return state;
}

gss_result* server_principal_details(const char* service, const char* hostname) {
    char match[1024];
    size_t match_len = 0;
    char* details = NULL;
    gss_result* result = NULL;

    int code;
    krb5_context kcontext;
    krb5_keytab kt = NULL;
    krb5_kt_cursor cursor = NULL;
    krb5_keytab_entry entry;
    char* pname = NULL;

    // Generate the principal prefix we want to match
    snprintf(match, 1024, "%s/%s@", service, hostname);
    match_len = strlen(match);

    code = krb5_init_context(&kcontext);
    if (code) {
        result =
            gss_error_result_with_message_and_code("Cannot initialize Kerberos5 context", code);
        return result;
    }

    if ((code = krb5_kt_default(kcontext, &kt))) {
        result = gss_error_result_with_message_and_code("Cannot get default keytab", code);
        goto end;
    }

    if ((code = krb5_kt_start_seq_get(kcontext, kt, &cursor))) {
        result =
            gss_error_result_with_message_and_code("Cannot get sequence cursor from keytab", code);
        goto end;
    }

    while ((code = krb5_kt_next_entry(kcontext, kt, &entry, &cursor)) == 0) {
        if ((code = krb5_unparse_name(kcontext, entry.principal, &pname))) {
            result = gss_error_result_with_message_and_code(
                "Cannot parse principal name from keytab", code);
            goto end;
        }

        if (strncmp(pname, match, match_len) == 0) {
            details = (char*)malloc(strlen(pname) + 1);
            strcpy(details, pname);
            krb5_free_unparsed_name(kcontext, pname);
            krb5_free_keytab_entry_contents(kcontext, &entry);
            break;
        }

        krb5_free_unparsed_name(kcontext, pname);
        krb5_free_keytab_entry_contents(kcontext, &entry);
    }

    if (details == NULL) {
        result = gss_error_result_with_message_and_code("Principal not found in keytab", -1);
    } else {
        result = gss_success_result(AUTH_GSS_COMPLETE);
        result->data = details;
    }
end:
    if (cursor)
        krb5_kt_end_seq_get(kcontext, kt, &cursor);
    if (kt)
        krb5_kt_close(kcontext, kt);
    krb5_free_context(kcontext);

    return result;
}

gss_result* authenticate_gss_client_init(const char* service,
                                         const char* principal,
                                         long int gss_flags,
                                         gss_server_state* delegatestate,
                                         gss_OID mech_oid,
                                         gss_client_state* state) {
    OM_uint32 maj_stat;
    OM_uint32 min_stat;
    gss_buffer_desc name_token = GSS_C_EMPTY_BUFFER;
    gss_buffer_desc principal_token = GSS_C_EMPTY_BUFFER;
    gss_result* ret = NULL;

    state->server_name = GSS_C_NO_NAME;
    state->mech_oid = mech_oid;
    state->context = GSS_C_NO_CONTEXT;
    state->gss_flags = gss_flags;
    state->client_creds = GSS_C_NO_CREDENTIAL;
    state->username = NULL;
    state->response = NULL;

    // Import server name first
    name_token.length = strlen(service);
    name_token.value = (char*)service;

    maj_stat =
        gss_import_name(&min_stat, &name_token, gss_krb5_nt_service_name, &state->server_name);

    if (GSS_ERROR(maj_stat)) {
        ret = gss_error_result(maj_stat, min_stat);
        goto end;
    }

    // Use the delegate credentials if they exist
    if (delegatestate && delegatestate->client_creds != GSS_C_NO_CREDENTIAL) {
        state->client_creds = delegatestate->client_creds;
    }
    // If available use the principal to extract its associated credentials
    else if (principal && *principal) {
        gss_name_t name;
        principal_token.length = strlen(principal);
        principal_token.value = (char*)principal;

        maj_stat = gss_import_name(&min_stat, &principal_token, GSS_C_NT_USER_NAME, &name);
        if (GSS_ERROR(maj_stat)) {
            ret = gss_error_result(maj_stat, min_stat);
            goto end;
        }

        maj_stat = gss_acquire_cred(&min_stat,
                                    name,
                                    GSS_C_INDEFINITE,
                                    GSS_C_NO_OID_SET,
                                    GSS_C_INITIATE,
                                    &state->client_creds,
                                    NULL,
                                    NULL);
        if (GSS_ERROR(maj_stat)) {
            ret = gss_error_result(maj_stat, min_stat);
            goto end;
        }

        maj_stat = gss_release_name(&min_stat, &name);
        if (GSS_ERROR(maj_stat)) {
            ret = gss_error_result(maj_stat, min_stat);
            goto end;
        }
    }

    ret = gss_success_result(AUTH_GSS_COMPLETE);
end:
    return ret;
}

int authenticate_gss_client_clean(gss_client_state* state) {
    OM_uint32 min_stat;
    int ret = AUTH_GSS_COMPLETE;

    if (state->context != GSS_C_NO_CONTEXT)
        gss_delete_sec_context(&min_stat, &state->context, GSS_C_NO_BUFFER);
    if (state->server_name != GSS_C_NO_NAME)
        gss_release_name(&min_stat, &state->server_name);
    if (state->client_creds != GSS_C_NO_CREDENTIAL && !(state->gss_flags & GSS_C_DELEG_FLAG))
        gss_release_cred(&min_stat, &state->client_creds);
    if (state->username != NULL) {
        free(state->username);
        state->username = NULL;
    }
    if (state->response != NULL) {
        free(state->response);
        state->response = NULL;
    }

    return ret;
}

gss_result* authenticate_gss_client_step(gss_client_state* state,
                                         const char* challenge,
                                         struct gss_channel_bindings_struct* channel_bindings) {
    OM_uint32 maj_stat;
    OM_uint32 min_stat;
    gss_buffer_desc input_token = GSS_C_EMPTY_BUFFER;
    gss_buffer_desc output_token = GSS_C_EMPTY_BUFFER;
    gss_result* ret = NULL;
    int temp_ret = AUTH_GSS_CONTINUE;

    // Always clear out the old response
    if (state->response != NULL) {
        free(state->response);
        state->response = NULL;
    }

    // If there is a challenge (data from the server) we need to give it to GSS
    if (challenge && *challenge) {
        size_t len;
        input_token.value = base64_decode(challenge, &len);
        if (input_token.value == NULL) {
            ret = gss_error_result_with_message("Ran out of memory decoding challenge");
            goto end;
        }

        input_token.length = len;
    }

    // Do GSSAPI step
    maj_stat = gss_init_sec_context(&min_stat,
                                    state->client_creds,
                                    &state->context,
                                    state->server_name,
                                    state->mech_oid,
                                    (OM_uint32)state->gss_flags,
                                    0,
                                    channel_bindings,
                                    &input_token,
                                    NULL,
                                    &output_token,
                                    NULL,
                                    NULL);

    if ((maj_stat != GSS_S_COMPLETE) && (maj_stat != GSS_S_CONTINUE_NEEDED)) {
        ret = gss_error_result(maj_stat, min_stat);
        goto end;
    }

    temp_ret = (maj_stat == GSS_S_COMPLETE) ? AUTH_GSS_COMPLETE : AUTH_GSS_CONTINUE;
    // Grab the client response to send back to the server
    if (output_token.length) {
        state->response =
            base64_encode((const unsigned char*)output_token.value, output_token.length);
        if (state->response == NULL) {
            ret = gss_error_result_with_message("Ran out of memory encoding response");
            goto end;
        }

        maj_stat = gss_release_buffer(&min_stat, &output_token);
    }

    // Try to get the user name if we have completed all GSS operations
    if (temp_ret == AUTH_GSS_COMPLETE) {
        state->context_complete = true;

        gss_name_t gssuser = GSS_C_NO_NAME;
        maj_stat = gss_inquire_context(
            &min_stat, state->context, &gssuser, NULL, NULL, NULL, NULL, NULL, NULL);
        if (GSS_ERROR(maj_stat)) {
            ret = gss_error_result(maj_stat, min_stat);
            goto end;
        }

        gss_buffer_desc name_token;
        name_token.length = 0;
        maj_stat = gss_display_name(&min_stat, gssuser, &name_token, NULL);
        if (GSS_ERROR(maj_stat)) {
            if (name_token.value) {
                gss_release_buffer(&min_stat, &name_token);
            }
            gss_release_name(&min_stat, &gssuser);

            ret = gss_error_result(maj_stat, min_stat);
            goto end;
        } else {
            if (state->username != NULL) {
                free(state->username);
                state->username = NULL;
            }
            state->username = (char*)malloc(name_token.length + 1);
            if (state->username == NULL) {
                ret = gss_error_result_with_message("Ran out of memory allocating username");
                goto end;
            }
            strncpy(state->username, (char*)name_token.value, name_token.length);
            state->username[name_token.length] = 0;
            gss_release_buffer(&min_stat, &name_token);
            gss_release_name(&min_stat, &gssuser);
        }
    }

    ret = gss_success_result(temp_ret);
end:
    if (output_token.value) {
        gss_release_buffer(&min_stat, &output_token);
    }
    if (input_token.value) {
        free(input_token.value);
    }
    return ret;
}

gss_result* authenticate_gss_client_unwrap(gss_client_state* state, const char* challenge) {
    OM_uint32 maj_stat;
    OM_uint32 min_stat;
    gss_buffer_desc input_token = GSS_C_EMPTY_BUFFER;
    gss_buffer_desc output_token = GSS_C_EMPTY_BUFFER;
    int conf = 0;
    gss_result* ret = NULL;

    // Always clear out the old response
    if (state->response != NULL) {
        free(state->response);
        state->response = NULL;
        state->responseConf = 0;
    }

    // If there is a challenge (data from the server) we need to give it to GSS
    if (challenge && *challenge) {
        size_t len;
        input_token.value = base64_decode(challenge, &len);
        input_token.length = len;
    }

    // Do GSSAPI step
    maj_stat = gss_unwrap(&min_stat, state->context, &input_token, &output_token, &conf, NULL);

    if (maj_stat != GSS_S_COMPLETE) {
        ret = gss_error_result(maj_stat, min_stat);
        goto end;
    }

    // Grab the client response
    if (output_token.length) {
        state->response =
            base64_encode((const unsigned char*)output_token.value, output_token.length);
        state->responseConf = conf;
        maj_stat = gss_release_buffer(&min_stat, &output_token);
    }

    ret = gss_success_result(AUTH_GSS_COMPLETE);
end:
    if (output_token.value)
        gss_release_buffer(&min_stat, &output_token);
    if (input_token.value)
        free(input_token.value);

    return ret;
}

gss_result* authenticate_gss_client_wrap(gss_client_state* state,
                                         const char* challenge,
                                         const char* user,
                                         int protect) {
    OM_uint32 maj_stat;
    OM_uint32 min_stat;
    gss_buffer_desc input_token = GSS_C_EMPTY_BUFFER;
    gss_buffer_desc output_token = GSS_C_EMPTY_BUFFER;
    char buf[4096];
    unsigned long buf_size;
    gss_result* ret = NULL;

    // Always clear out the old response
    if (state->response != NULL) {
        free(state->response);
        state->response = NULL;
    }

    if (challenge && *challenge) {
        size_t len;
        input_token.value = base64_decode(challenge, &len);
        input_token.length = len;
    }

    if (user) {
        // get bufsize
        // server_conf_flags = ((char*) input_token.value)[0];
        ((char*)input_token.value)[0] = 0;
        buf_size = ntohl(*((long*)input_token.value));
        free(input_token.value);
#ifdef PRINTFS
        printf("User: %s, %c%c%c\n",
               user,
               server_conf_flags & GSS_AUTH_P_NONE ? 'N' : '-',
               server_conf_flags & GSS_AUTH_P_INTEGRITY ? 'I' : '-',
               server_conf_flags & GSS_AUTH_P_PRIVACY ? 'P' : '-');
        printf("Maximum GSS token size is %ld\n", buf_size);
#endif

        // agree to terms (hack!)
        buf_size = htonl(buf_size);  // not relevant without integrity/privacy
        memcpy(buf, &buf_size, 4);
        buf[0] = GSS_AUTH_P_NONE;
        // server decides if principal can log in as user
        strncpy(buf + 4, user, sizeof(buf) - 4);
        input_token.value = buf;
        input_token.length = 4 + strlen(user);
    }

    // Do GSSAPI wrap
    maj_stat = gss_wrap(
        &min_stat, state->context, protect, GSS_C_QOP_DEFAULT, &input_token, NULL, &output_token);

    if (maj_stat != GSS_S_COMPLETE) {
        ret = gss_error_result(maj_stat, min_stat);
        goto end;
    }

    // Grab the client response to send back to the server
    if (output_token.length) {
        state->response =
            base64_encode((const unsigned char*)output_token.value, output_token.length);
        ;
        maj_stat = gss_release_buffer(&min_stat, &output_token);
    }

    ret = gss_success_result(AUTH_GSS_COMPLETE);
end:
    if (output_token.value)
        gss_release_buffer(&min_stat, &output_token);

    if (!user && input_token.value)
        gss_release_buffer(&min_stat, &input_token);

    return ret;
}

gss_result* authenticate_gss_server_init(const char* service, gss_server_state* state) {
    OM_uint32 maj_stat;
    OM_uint32 min_stat;
    size_t service_len;
    gss_buffer_desc name_token = GSS_C_EMPTY_BUFFER;
    gss_result* ret = NULL;

    state->context = GSS_C_NO_CONTEXT;
    state->server_name = GSS_C_NO_NAME;
    state->client_name = GSS_C_NO_NAME;
    state->server_creds = GSS_C_NO_CREDENTIAL;
    state->client_creds = GSS_C_NO_CREDENTIAL;
    state->username = NULL;
    state->targetname = NULL;
    state->response = NULL;

    // Server name may be empty which means we aren't going to create our own creds
    service_len = strlen(service);
    if (service_len != 0) {
        // Import server name first
        name_token.length = strlen(service);
        name_token.value = (char*)service;

        maj_stat = gss_import_name(
            &min_stat, &name_token, GSS_C_NT_HOSTBASED_SERVICE, &state->server_name);

        if (GSS_ERROR(maj_stat)) {
            ret = gss_error_result(maj_stat, min_stat);
            goto end;
        }

        // Get credentials
        maj_stat = gss_acquire_cred(&min_stat,
                                    state->server_name,
                                    GSS_C_INDEFINITE,
                                    GSS_C_NO_OID_SET,
                                    GSS_C_ACCEPT,
                                    &state->server_creds,
                                    NULL,
                                    NULL);

        if (GSS_ERROR(maj_stat)) {
            ret = gss_error_result(maj_stat, min_stat);
            goto end;
        }
    }

    ret = gss_success_result(AUTH_GSS_COMPLETE);
end:
    return ret;
}

int authenticate_gss_server_clean(gss_server_state* state) {
    OM_uint32 min_stat;
    int ret = AUTH_GSS_COMPLETE;

    if (state->context != GSS_C_NO_CONTEXT)
        gss_delete_sec_context(&min_stat, &state->context, GSS_C_NO_BUFFER);
    if (state->server_name != GSS_C_NO_NAME)
        gss_release_name(&min_stat, &state->server_name);
    if (state->client_name != GSS_C_NO_NAME)
        gss_release_name(&min_stat, &state->client_name);
    if (state->server_creds != GSS_C_NO_CREDENTIAL)
        gss_release_cred(&min_stat, &state->server_creds);
    if (state->client_creds != GSS_C_NO_CREDENTIAL)
        gss_release_cred(&min_stat, &state->client_creds);
    if (state->username != NULL) {
        free(state->username);
        state->username = NULL;
    }
    if (state->targetname != NULL) {
        free(state->targetname);
        state->targetname = NULL;
    }
    if (state->response != NULL) {
        free(state->response);
        state->response = NULL;
    }

    return ret;
}

gss_result* authenticate_gss_server_step(gss_server_state* state, const char* challenge) {
    OM_uint32 maj_stat;
    OM_uint32 min_stat;
    gss_buffer_desc input_token = GSS_C_EMPTY_BUFFER;
    gss_buffer_desc output_token = GSS_C_EMPTY_BUFFER;
    gss_name_t target_name = GSS_C_NO_NAME;
    // int ret = AUTH_GSS_CONTINUE;
    gss_result* ret = NULL;

    // Always clear out the old response
    if (state->response != NULL) {
        free(state->response);
        state->response = NULL;
    }

    // If there is a challenge (data from the server) we need to give it to GSS
    if (challenge && *challenge) {
        size_t len;
        input_token.value = base64_decode(challenge, &len);
        input_token.length = len;
    } else {
        ret = gss_error_result_with_message("No challenge parameter in request from client");
        goto end;
    }

    maj_stat = gss_accept_sec_context(&min_stat,
                                      &state->context,
                                      state->server_creds,
                                      &input_token,
                                      GSS_C_NO_CHANNEL_BINDINGS,
                                      &state->client_name,
                                      NULL,
                                      &output_token,
                                      NULL,
                                      NULL,
                                      &state->client_creds);

    if (GSS_ERROR(maj_stat)) {
        ret = gss_error_result(maj_stat, min_stat);
        goto end;
    }

    // Grab the server response to send back to the client
    if (output_token.length) {
        state->response =
            base64_encode((const unsigned char*)output_token.value, output_token.length);
        ;
        maj_stat = gss_release_buffer(&min_stat, &output_token);
    }

    // Get the user name
    maj_stat = gss_display_name(&min_stat, state->client_name, &output_token, NULL);
    if (GSS_ERROR(maj_stat)) {
        ret = gss_error_result(maj_stat, min_stat);
        goto end;
    }
    state->username = (char*)malloc(output_token.length + 1);
    strncpy(state->username, (char*)output_token.value, output_token.length);
    state->username[output_token.length] = 0;

    // Get the target name if no server creds were supplied
    if (state->server_creds == GSS_C_NO_CREDENTIAL) {
        maj_stat = gss_inquire_context(
            &min_stat, state->context, NULL, &target_name, NULL, NULL, NULL, NULL, NULL);
        if (GSS_ERROR(maj_stat)) {
            ret = gss_error_result(maj_stat, min_stat);
            goto end;
        }

        // Free output token if necessary before reusing
        if (output_token.length)
            gss_release_buffer(&min_stat, &output_token);

        maj_stat = gss_display_name(&min_stat, target_name, &output_token, NULL);
        if (GSS_ERROR(maj_stat)) {
            ret = gss_error_result(maj_stat, min_stat);
            goto end;
        }
        state->targetname = (char*)malloc(output_token.length + 1);
        strncpy(state->targetname, (char*)output_token.value, output_token.length);
        state->targetname[output_token.length] = 0;
    }

    ret = gss_success_result(AUTH_GSS_COMPLETE);
    state->context_complete = true;
end:
    if (target_name != GSS_C_NO_NAME)
        gss_release_name(&min_stat, &target_name);
    if (output_token.length)
        gss_release_buffer(&min_stat, &output_token);
    if (input_token.value)
        free(input_token.value);
    return ret;
}

gss_result* authenticate_user_krb5pwd(const char* user,
                                      const char* pswd,
                                      const char* service,
                                      const char* default_realm) {
    krb5_context kcontext = NULL;
    krb5_error_code code;
    krb5_principal client = NULL;
    krb5_principal server = NULL;
    gss_result* result = NULL;
    int ret = 0;
    char* name = NULL;
    char* p = NULL;

    // for verify
    krb5_creds creds;
    krb5_get_init_creds_opt gic_options;
    krb5_error_code verifyRet;
    char *vName = NULL;

    code = krb5_init_context(&kcontext);
    if (code) {
        result =
            gss_error_result_with_message_and_code("Cannot initialize Kerberos5 context", code);
        return result;
    }

    ret = krb5_parse_name(kcontext, service, &server);
    if (ret) {
        result = gss_error_result_with_message_and_code(krb5_get_err_text(kcontext, ret), ret);
        goto end;
    }

    code = krb5_unparse_name(kcontext, server, &name);
    if (code) {
        result = gss_error_result_with_message_and_code(krb5_get_err_text(kcontext, code), code);
        goto end;
    }

    free(name);
    name = NULL;
    name = (char*)malloc(256);
    if (name == NULL) {
        result = gss_error_result_with_message("Ran out of memory allocating name");
        goto end;
    }

    p = strchr((char*)user, '@');
    if (p == NULL) {
        snprintf(name, 256, "%s@%s", user, default_realm);
    } else {
        snprintf(name, 256, "%s", user);
    }

    code = krb5_parse_name(kcontext, name, &client);
    if (code) {
        result = gss_error_result_with_message_and_code(krb5_get_err_text(kcontext, code), code);
        goto end;
    }

    // verify krb5 user
    memset(&creds, 0, sizeof(creds));

    verifyRet = krb5_unparse_name(kcontext, client, &vName);
    if (verifyRet == 0) {
        free(vName);
    }

    krb5_get_init_creds_opt_init(&gic_options);
    verifyRet = krb5_get_init_creds_password(
        kcontext, &creds, client, (char*)pswd, NULL, NULL, 0, NULL, &gic_options);
    if (verifyRet) {
        result = gss_error_result_with_message_and_code(krb5_get_err_text(kcontext, verifyRet),
                                                        verifyRet);
        krb5_free_cred_contents(kcontext, &creds);
        goto end;
    }

    krb5_free_cred_contents(kcontext, &creds);
    result = gss_success_result(1);

end:
    if (name) {
        free(name);
    }
    if (client) {
        krb5_free_principal(kcontext, client);
    }
    if (server) {
        krb5_free_principal(kcontext, server);
    }
    krb5_free_context(kcontext);

    return result;
}

static gss_result* gss_success_result(int ret) {
    gss_result* result = (gss_result*)malloc(sizeof(gss_result));
    result->code = ret;
    result->message = NULL;
    return result;
}

static gss_result* gss_error_result(OM_uint32 err_maj, OM_uint32 err_min) {
    OM_uint32 maj_stat, min_stat;
    OM_uint32 msg_ctx = 0;
    gss_buffer_desc status_string;
    char buf_maj[512];
    char buf_min[512];
    gss_result* result;

    do {
        maj_stat = gss_display_status(
            &min_stat, err_maj, GSS_C_GSS_CODE, GSS_C_NO_OID, &msg_ctx, &status_string);
        if (GSS_ERROR(maj_stat))
            break;
        strncpy(buf_maj, (char*)status_string.value, sizeof(buf_maj));
        gss_release_buffer(&min_stat, &status_string);

        maj_stat = gss_display_status(
            &min_stat, err_min, GSS_C_MECH_CODE, GSS_C_NULL_OID, &msg_ctx, &status_string);
        if (!GSS_ERROR(maj_stat)) {
            strncpy(buf_min, (char*)status_string.value, sizeof(buf_min));
            gss_release_buffer(&min_stat, &status_string);
        }
    } while (!GSS_ERROR(maj_stat) && msg_ctx != 0);

    result = (gss_result*)malloc(sizeof(gss_result));
    result->code = AUTH_GSS_ERROR;
    result->message = (char*)malloc(sizeof(char) * 1024 + 2);
    sprintf(result->message, "%s: %s", buf_maj, buf_min);

    return result;
}

static gss_result* gss_error_result_with_message(const char* message) {
    gss_result* result = (gss_result*)malloc(sizeof(gss_result));
    result->code = AUTH_GSS_ERROR;
    result->message = strdup(message);
    return result;
}

static gss_result* gss_error_result_with_message_and_code(const char* message, int code) {
    gss_result* result = (gss_result*)malloc(sizeof(gss_result));
    result->code = AUTH_GSS_ERROR;
    result->message = (char*)malloc(strlen(message) + 20);
    sprintf(result->message, "%s (%d)", message, code);
    return result;
}

#if defined(__clang__)
#pragma clang diagnostic pop
#endif
