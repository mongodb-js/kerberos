#include "kerberos_sspi.h"
#include <stdlib.h>
#include <stdio.h>

static HINSTANCE _kerberos_security_dll; 

/**
 * Encrypt A Message
 */
SECURITY_STATUS SEC_ENTRY _kerberos_EncryptMessage(PCtxtHandle phContext, unsigned long fQOP, PSecBufferDesc pMessage, unsigned long MessageSeqNo) {
  // Create function pointer instance
  encryptMessage_fn pfn_encryptMessage = NULL;

  // Return error if library not loaded
  if(_kerberos_security_dll == NULL) return -1;

  // Map function to library method
  pfn_encryptMessage = (encryptMessage_fn)GetProcAddress(_kerberos_security_dll, "EncryptMessage");
  // Check if the we managed to map function pointer
  if(!pfn_encryptMessage) {
    printf("GetProcAddress failed.\n");
    return -2;
  }

  // Call the function
  return (*pfn_encryptMessage)(phContext, fQOP, pMessage, MessageSeqNo);
}

/**
 * Acquire Credentials
 */
SECURITY_STATUS SEC_ENTRY _kerberos_AcquireCredentialsHandle(
  LPSTR pszPrincipal, LPSTR pszPackage, unsigned long fCredentialUse,
  void * pvLogonId, void * pAuthData, SEC_GET_KEY_FN pGetKeyFn, void * pvGetKeyArgument,
  PCredHandle phCredential, PTimeStamp ptsExpiry
) {
  // Create function pointer instance
  acquireCredentialsHandle_fn pfn_acquireCredentialsHandle = NULL;

  // Return error if library not loaded
  if(_kerberos_security_dll == NULL) return -1;
  // Map function
  pfn_acquireCredentialsHandle = (acquireCredentialsHandle_fn)GetProcAddress(_kerberos_security_dll, "AcquireCredentialsHandleA");

  // Check if the we managed to map function pointer
  if(!pfn_acquireCredentialsHandle) {
    printf("GetProcAddress failed.\n");
    return -2;
  }

  // Call the function
  return (*pfn_acquireCredentialsHandle)(pszPrincipal, pszPackage, fCredentialUse,
      pvLogonId, pAuthData, pGetKeyFn, pvGetKeyArgument, phCredential, ptsExpiry
    );
}

/**
 * Delete Security Context
 */
SECURITY_STATUS SEC_ENTRY _kerberos_DeleteSecurityContext(PCtxtHandle phContext) {
  // Create function pointer instance
  deleteSecurityContext_fn pfn_deleteSecurityContext = NULL;

  // Return error if library not loaded
  if(_kerberos_security_dll == NULL) return -1;
  // Map function
  pfn_deleteSecurityContext = (deleteSecurityContext_fn)GetProcAddress(_kerberos_security_dll, "DeleteSecurityContext");

  // Check if the we managed to map function pointer
  if(!pfn_deleteSecurityContext) {
    printf("GetProcAddress failed.\n");
    return -2;
  }

  // Call the function
  return (*pfn_deleteSecurityContext)(phContext);
}

/**
 * Decrypt Message
 */
SECURITY_STATUS SEC_ENTRY _kerberos_DecryptMessage(PCtxtHandle phContext, PSecBufferDesc pMessage, unsigned long MessageSeqNo, unsigned long pfQOP) {
  // Create function pointer instance
  decryptMessage_fn pfn_decryptMessage = NULL;

  // Return error if library not loaded
  if(_kerberos_security_dll == NULL) return -1;
  // Map function
  pfn_decryptMessage = (decryptMessage_fn)GetProcAddress(_kerberos_security_dll, "DecryptMessage");

  // Check if the we managed to map function pointer
  if(!pfn_decryptMessage) {
    printf("GetProcAddress failed.\n");
    return -2;
  }

  // Call the function
  return (*pfn_decryptMessage)(phContext, pMessage, MessageSeqNo, pfQOP);
}

/**
 * Initialize Security Context
 */
SECURITY_STATUS SEC_ENTRY _kerberos_SaslInitializeSecurityContext(
  PCredHandle phCredential, PCtxtHandle phContext,
  LPSTR pszTargetName, unsigned long fContextReq, 
  unsigned long Reserved1, unsigned long TargetDataRep, 
  PSecBufferDesc pInput, unsigned long Reserved2,
  PCtxtHandle phNewContext, PSecBufferDesc pOutput,
  unsigned long * pfContextAttr, PTimeStamp ptsExpiry
) {
  // Create function pointer instance
  saslInitializeSecurityContext_fn pfn_saslInitializeSecurityContext = NULL;

  // Return error if library not loaded
  if(_kerberos_security_dll == NULL) return -1;
  // Map function
  pfn_saslInitializeSecurityContext = (saslInitializeSecurityContext_fn)GetProcAddress(_kerberos_security_dll, "SaslInitializeSecurityContext");

  // Check if the we managed to map function pointer
  if(!pfn_saslInitializeSecurityContext) {
    printf("GetProcAddress failed.\n");
    return -2;
  }

  // Call the function
  return (*pfn_saslInitializeSecurityContext)(
    phCredential, phContext, pszTargetName, fContextReq, 
    Reserved1, TargetDataRep, pInput, Reserved2,
    phNewContext, pOutput, pfContextAttr, ptsExpiry
  );
}
/**
 * Query Context Attributes
 */
SECURITY_STATUS SEC_ENTRY _kerberos_QueryContextAttributes(
  PCtxtHandle phContext, unsigned long ulAttribute, void * pBuffer
) {
  // Create function pointer instance
  queryContextAttributes_fn pfn_queryContextAttributes = NULL;

  // Return error if library not loaded
  if(_kerberos_security_dll == NULL) return -1;
  // Map function
  pfn_queryContextAttributes = (queryContextAttributes_fn)GetProcAddress(_kerberos_security_dll, "QueryContextAttributes");

  // Check if the we managed to map function pointer
  if(!pfn_queryContextAttributes) {
    printf("GetProcAddress failed.\n");
    return -2;
  }

  // Call the function
  return (*pfn_queryContextAttributes)(
    phContext, ulAttribute, pBuffer
  );
}

/**
 * Load security.dll dynamically
 */
int load_library() {
  DWORD err;
  // Load the library
  _kerberos_security_dll = LoadLibrary("security.dll");

  // Check if the library loaded
  if(_kerberos_security_dll == NULL) {
    err = GetLastError();
    return err;
  }

  return 0;


/*  DWORD err;
  HINSTANCE hDLL = LoadLibrary("security.dll");
  
  if(hDLL != NULL) {
    printf("Library has been loaded\n");
  } else {
    err = GetLastError();
    printf("Couldn't load dll\n");
  }

  // Create function pointer
  typedef DWORD (WINAPI *encryptMessage_fn)(PCtxtHandle phContext, ULONG fQOP, PSecBufferDesc pMessage, ULONG MessageSeqNo);
*/

/*  HMODULE winmmDLL = LoadLibraryA("winmm.dll");

  if (!winmmDLL) {
    printf("Couldn't load dll\n");
      return 1;
  }

  pfnTimeGetTime = (timeGetTime_fn)GetProcAddress(winmmDLL, "timeGetTime");

  if (!pfnTimeGetTime) {
    printf("GetProcAddress failed.\n");
      return 2;
  }

  printf("============== %d\n", (*pfnTimeGetTime)());
  return 0;
*/}