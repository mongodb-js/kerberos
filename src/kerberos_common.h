#ifndef KERBEROS_COMMON_H
#define KERBEROS_COMMON_H

#if defined(__linux__) || defined(__APPLE__)
#include "unix/kerberos_gss.h"

namespace node_kerberos {
typedef gss_client_state krb_client_state;
typedef gss_server_state krb_server_state;
typedef gss_result krb_result;
}
#else
#include "win32/kerberos_sspi.h"

namespace node_kerberos {
typedef sspi_client_state krb_client_state;
typedef sspi_server_state krb_server_state;
typedef sspi_result krb_result;
}
#endif

#endif
