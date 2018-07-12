#include "kerberos_sspi.h"

static sspi_result* sspi_success_result(INT ret);
static sspi_result* sspi_error_result(DWORD errCode, const SEC_CHAR* msg);
static sspi_result* sspi_error_result_with_message(WCHAR* message);
static sspi_result* sspi_error_result_with_message_and_code(WCHAR* mesage, INT code);

sspi_client_state* sspi_client_state_new() {
    sspi_client_state* state = (sspi_client_state*)malloc(sizeof(sspi_client_state));
    state->username = NULL;
    state->response = NULL;
    state->responseConf = 0;
    state->context_complete = FALSE;

    return state;
}

// sspi_server_state* sspi_server_state_new() {
//     sspi_server_state* state = (sspi_server_state*)malloc(sizeof(sspi_server_state));
//     state->username = NULL;
//     state->response = NULL;
//     state->targetname = NULL;
//     state->context_complete = false;

//     return state;
// }

VOID
auth_sspi_client_clean(sspi_client_state* state) {
    if (state->haveCtx) {
        DeleteSecurityContext(&state->ctx);
        state->haveCtx = 0;
    }
    if (state->haveCred) {
        FreeCredentialsHandle(&state->cred);
        state->haveCred = 0;
    }
    if (state->spn != NULL) {
        free(state->spn);
        state->spn = NULL;
    }
    if (state->response != NULL) {
        free(state->response);
        state->response = NULL;
    }
    if (state->username != NULL) {
        free(state->username);
        state->username = NULL;
    }
}

sspi_result*
auth_sspi_client_init(WCHAR* service,
                      ULONG flags,
                      WCHAR* user,
                      ULONG ulen,
                      WCHAR* domain,
                      ULONG dlen,
                      WCHAR* password,
                      ULONG plen,
                      WCHAR* mechoid,
                      sspi_client_state* state) {
    SECURITY_STATUS status;
    SEC_WINNT_AUTH_IDENTITY_W authIdentity;
    TimeStamp ignored;

    state->response = NULL;
    state->username = NULL;
    state->qop = SECQOP_WRAP_NO_ENCRYPT;
    state->flags = flags;
    state->haveCred = 0;
    state->haveCtx = 0;
    state->spn = _wcsdup(service);
    if (state->spn == NULL) {
        return sspi_error_result_with_message("Ran out of memory assigning service");
    }

    /* Convert RFC-2078 format to SPN */
    if (!wcschr(state->spn, L'/')) {
        WCHAR* ptr = wcschr(state->spn, L'@');
        if (ptr) {
            *ptr = L'/';
        }
    }

    if (user) {
        authIdentity.User = user;
        authIdentity.UserLength = ulen;
        authIdentity.Domain = domain;
        authIdentity.DomainLength = dlen;
        authIdentity.Password = password;
        authIdentity.PasswordLength = plen;
        authIdentity.Flags = SEC_WINNT_AUTH_IDENTITY_UNICODE;
    }

    /* Note that the first paramater, pszPrincipal, appears to be
     * completely ignored in the Kerberos SSP. For more details see
     * https://github.com/mongodb-labs/winkerberos/issues/11.
     * */
    status = AcquireCredentialsHandleW(/* Principal */
                                       NULL,
                                       /* Security package name */
                                       mechoid,
                                       /* Credentials Use */
                                       SECPKG_CRED_OUTBOUND,
                                       /* LogonID (We don't use this) */
                                       NULL,
                                       /* AuthData */
                                       user ? &authIdentity : NULL,
                                       /* Always NULL */
                                       NULL,
                                       /* Always NULL */
                                       NULL,
                                       /* CredHandle */
                                       &state->cred,
                                       /* Expiry (Required but unused by us) */
                                       &ignored);
    if (status != SEC_E_OK) {
        return sspi_error_result(status, "AcquireCredentialsHandle");
    }

    state->haveCred = 1;
    return sspi_success_result(AUTH_SSPI_COMPLETE);
}

static sspi_result* sspi_success_result(int ret) {
    sspi_result* result = (sspi_result*)malloc(sizeof(sspi_result));
    result->code = ret;
    result->message = NULL;
    return result;
}

static sspi_result* sspi_error_result(DWORD errCode, const SEC_CHAR* msg) {
    SEC_CHAR* err;
    DWORD status;
    DWORD flags = (FORMAT_MESSAGE_ALLOCATE_BUFFER |
                   FORMAT_MESSAGE_FROM_SYSTEM |
                   FORMAT_MESSAGE_IGNORE_INSERTS);
    status = FormatMessageA(flags,
                            NULL,
                            errCode,
                            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                            (LPTSTR)&err,
                            0,
                            NULL);

    sspi_result* result = (sspi_result*)malloc(sizeof(sspi_result));
    result->code = AUTH_SSPI_ERROR;
    result->message = (WCHAR*)malloc(sizeof(WCHAR) * 1024 + 2);
    if (status) {
        swprintf(result->message, "%s: %s", msg, err);
    } else {
        swprintf(result->message, "%s", msg);
    }

    return result;
}

static sspi_result* sspi_error_result_with_message(WCHAR* message) {
    sspi_result* result = (sspi_result*)malloc(sizeof(sspi_result));
    result->code = AUTH_SSPI_ERROR;
    result->message = _wcsdup(message);
    return result;
}

static sspi_result* sspi_error_result_with_message_and_code(WCHAR* message, INT code) {
    sspi_result* result = (sspi_result*)malloc(sizeof(sspi_result));
    result->code = AUTH_SSPI_ERROR;
    result->message = (WCHAR*)malloc(wcslen(message) + 20);
    swprintf(result->message, "%s (%d)", message, code);
    return result;
}
