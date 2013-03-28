#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <v8.h>
#include <node.h>
#include <node_buffer.h>
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>

#include "security_credentials.h"

static LPSTR DisplaySECError(DWORD ErrCode);

static Handle<Value> VException(const char *msg) {
  HandleScope scope;
  return ThrowException(Exception::Error(String::New(msg)));
};

static Handle<Value> VExceptionErrNo(const char *msg, const int errorNumber) {
  HandleScope scope;

  Local<Value> err = Exception::Error(String::New(msg));
  Local<Object> obj = err->ToObject();
  obj->Set(NODE_PSYMBOL("code"), Int32::New(errorNumber));
  return ThrowException(err);
};

Persistent<FunctionTemplate> SecurityCredentials::constructor_template;

SecurityCredentials::SecurityCredentials() : ObjectWrap() {
}

SecurityCredentials::~SecurityCredentials() {
}

Handle<Value> SecurityCredentials::New(const Arguments &args) {
  HandleScope scope;  

  // Create security credentials instance
  SecurityCredentials *security_credentials = new SecurityCredentials();
  // Wrap it
  security_credentials->Wrap(args.This());
  // Return the object
  return args.This();
}

Handle<Value> SecurityCredentials::Aquire(const Arguments &args) {
  HandleScope scope;  
  char *package_str, *username_str, *password_str, *domain_str;
  // Status of operation
  SECURITY_STATUS status;

  // Unpack the variables
  if(args.Length() != 2 && args.Length() != 3 && args.Length() != 4)
    return VException("Aquire must be called with either [package:string, username:string, [password:string, domain:string]]");

  if(!args[0]->IsString())
    return VException("Aquire must be called with either [package:string, username:string, [password:string, domain:string]]");

  if(!args[1]->IsString())
    return VException("Aquire must be called with either [package:string, username:string, [password:string, domain:string]]");

  if(args.Length() == 3 && !args[2]->IsString())
    return VException("Aquire must be called with either [package:string, username:string, [password:string, domain:string]]");

  if(args.Length() == 4 && !args[3]->IsString())
    return VException("Aquire must be called with either [package:string, username:string, [password:string, domain:string]]");

  // Unpack the package
  Local<String> package = args[0]->ToString();
  package_str = (char *)calloc(package->Utf8Length() + 1, sizeof(char));
  package->WriteUtf8(package_str);

  // Unpack the user name
  Local<String> username = args[1]->ToString();
  username_str = (char *)calloc(username->Utf8Length() + 1, sizeof(char));
  username->WriteUtf8(username_str);

  // If we have a password
  if(args.Length() == 3) {
    Local<String> password = args[2]->ToString();
    password_str = (char *)calloc(password->Utf8Length() + 1, sizeof(char));
    password->WriteUtf8(password_str);    
  }

  // If we have a domain
  if(args.Length() == 4) {
    Local<String> domain = args[3]->ToString();
    domain_str = (char *)calloc(domain->Utf8Length() + 1, sizeof(char));
    domain->WriteUtf8(domain_str);    
  }

  // Create Security instance
  // Local<Value> argv[] = {};
  Local<Object> security_credentials_value = constructor_template->GetFunction()->NewInstance();

  // Unwrap the credentials
  SecurityCredentials *security_credentials = ObjectWrap::Unwrap<SecurityCredentials>(security_credentials_value);

  // If we have domain string
  if(domain_str != NULL) {
    security_credentials->m_Identity.Domain = USTR(_tcsdup(domain_str));
    security_credentials->m_Identity.DomainLength = (unsigned long)_tcslen(domain_str);
  }

  // Set up the user
  security_credentials->m_Identity.User = USTR(_tcsdup(username_str));
  security_credentials->m_Identity.UserLength = (unsigned long)_tcslen(username_str);

  // If we have a password string
  if(password_str != NULL) {
    // Set up the password
    security_credentials->m_Identity.Password = USTR(_tcsdup(password_str));
    security_credentials->m_Identity.PasswordLength = (unsigned long)_tcslen(password_str);    
  }

  #ifdef _UNICODE
      security_credentials->m_Identity.Flags = SEC_WINNT_AUTH_IDENTITY_UNICODE;
  #else
      security_credentials->m_Identity.Flags = SEC_WINNT_AUTH_IDENTITY_ANSI;
  #endif

  // Attempt to acquire credentials
  status = _sspi_AcquireCredentialsHandle(
    NULL,
    package_str,
    SECPKG_CRED_OUTBOUND,
    NULL, 
    password_str != NULL ? &security_credentials->m_Identity : NULL,
    NULL, NULL,
    &security_credentials->m_Credentials,
    &security_credentials->Expiration
  );

  // We have an error
  if(status != SEC_E_OK) {
    LPSTR err_message = DisplaySECError(status);

    if(err_message != NULL) {
      return VExceptionErrNo(err_message, status);
    } else {
      return VExceptionErrNo("Unknown error", status);
    }
  }

  // Return the object
  return scope.Close(security_credentials_value);
}

void SecurityCredentials::Initialize(Handle<Object> target) {
  // Grab the scope of the call from Node
  HandleScope scope;
  // Define a new function template
  Local<FunctionTemplate> t = FunctionTemplate::New(New);
  constructor_template = Persistent<FunctionTemplate>::New(t);
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("SecurityCredentials"));

  // Class methods
  NODE_SET_METHOD(constructor_template->GetFunction(), "aquire", Aquire);

  // Set the class on the target module
  target->Set(String::NewSymbol("SecurityCredentials"), constructor_template->GetFunction());  

  // Attempt to load the security.dll library
  load_library();
}

static LPSTR DisplaySECError(DWORD ErrCode) {
  LPSTR pszName = NULL; // WinError.h

  switch(ErrCode) {
    case SEC_E_BUFFER_TOO_SMALL:
      pszName = "SEC_E_BUFFER_TOO_SMALL - The message buffer is too small. Used with the Digest SSP.";
      break;

    case SEC_E_CRYPTO_SYSTEM_INVALID:
      pszName = "SEC_E_CRYPTO_SYSTEM_INVALID - The cipher chosen for the security context is not supported. Used with the Digest SSP."; 
      break;
    case SEC_E_INCOMPLETE_MESSAGE:
      pszName = "SEC_E_INCOMPLETE_MESSAGE - The data in the input buffer is incomplete. The application needs to read more data from the server and call DecryptMessage (General) again."; 
      break;

    case SEC_E_INVALID_HANDLE:
      pszName = "SEC_E_INVALID_HANDLE - A context handle that is not valid was specified in the phContext parameter. Used with the Digest and Schannel SSPs."; 
      break;

    case SEC_E_INVALID_TOKEN:
      pszName = "SEC_E_INVALID_TOKEN - The buffers are of the wrong type or no buffer of type SECBUFFER_DATA was found. Used with the Schannel SSP."; 
      break;
        
    case SEC_E_MESSAGE_ALTERED:
      pszName = "SEC_E_MESSAGE_ALTERED - The message has been altered. Used with the Digest and Schannel SSPs."; 
      break;
        
    case SEC_E_OUT_OF_SEQUENCE:
      pszName = "SEC_E_OUT_OF_SEQUENCE - The message was not received in the correct sequence."; 
      break;
        
    case SEC_E_QOP_NOT_SUPPORTED:
      pszName = "SEC_E_QOP_NOT_SUPPORTED - Neither confidentiality nor integrity are supported by the security context. Used with the Digest SSP."; 
      break;
        
    case SEC_I_CONTEXT_EXPIRED:
      pszName = "SEC_I_CONTEXT_EXPIRED - The message sender has finished using the connection and has initiated a shutdown."; 
      break;
        
    case SEC_I_RENEGOTIATE:
      pszName = "SEC_I_RENEGOTIATE - The remote party requires a new handshake sequence or the application has just initiated a shutdown."; 
      break;
        
    case SEC_E_ENCRYPT_FAILURE:
      pszName = "SEC_E_ENCRYPT_FAILURE - The specified data could not be encrypted."; 
      break;
        
    case SEC_E_DECRYPT_FAILURE:
      pszName = "SEC_E_DECRYPT_FAILURE - The specified data could not be decrypted."; 
      break;
    case -1:
      pszName = "Failed to load security.dll library"; 
      break;

  }

  return pszName;
}
