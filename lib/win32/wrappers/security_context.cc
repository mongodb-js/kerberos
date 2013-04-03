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

#include "security_context.h"
#include "security_buffer_descriptor.h"

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

Persistent<FunctionTemplate> SecurityContext::constructor_template;

SecurityContext::SecurityContext() : ObjectWrap() {
}

SecurityContext::~SecurityContext() {
}

Handle<Value> SecurityContext::New(const Arguments &args) {
  HandleScope scope;    
  // Create code object
  SecurityContext *security_obj = new SecurityContext();
  // Wrap it
  security_obj->Wrap(args.This());
  // Return the object
  return args.This();    
}

Handle<Value> SecurityContext::InitializeContext(const Arguments &args) {
  HandleScope scope;  
  char *service_principal_name_str = NULL, *input_str = NULL, *decoded_input_str = NULL;
  BYTE *out_bound_data_str = NULL;
  int decoded_input_str_length = NULL;
  // Store reference to security credentials
  SecurityCredentials *security_credentials = NULL;
  // Status of operation
  SECURITY_STATUS status;

  // We need 3 parameters
  if(args.Length() != 3)
    return VException("Initialize must be called with either [credential:SecurityCredential, servicePrincipalName:string, input:string]");

  // First parameter must be an instance of SecurityCredentials
  if(!SecurityCredentials::HasInstance(args[0]))
    return VException("First parameter for Initialize must be an instance of SecurityCredentials");

  // Second parameter must be a string
  if(!args[1]->IsString())
    return VException("Second parameter for Initialize must be a string");

  // Third parameter must be a base64 encoded string
  if(!args[2]->IsString())
    return VException("Second parameter for Initialize must be a string");

  // Let's unpack the values
  Local<String> service_principal_name = args[1]->ToString();
  service_principal_name_str = (char *)calloc(service_principal_name->Utf8Length() + 1, sizeof(char));
  service_principal_name->WriteUtf8(service_principal_name_str);

  // Unpack the user name
  Local<String> input = args[2]->ToString();

  if(input->Utf8Length() > 0) {
    input_str = (char *)calloc(input->Utf8Length() + 1, sizeof(char));
    input->WriteUtf8(input_str);

    // Now let's get the base64 decoded string
    decoded_input_str = (char *)base64_decode(input_str, &decoded_input_str_length);
  }

  // Unpack the Security credentials
  security_credentials = ObjectWrap::Unwrap<SecurityCredentials>(args[0]->ToObject());

  // Create Security context instance
  Local<Object> security_context_value = constructor_template->GetFunction()->NewInstance();
  security_context_value->Get(String::New("initialize"));
  // Unwrap the security context
  SecurityContext *security_context = ObjectWrap::Unwrap<SecurityContext>(security_context_value);
  // Add a reference to the security_credentials
  security_context->security_credentials = security_credentials;

  // Structures used for c calls
  SecBufferDesc ibd, obd;
  SecBuffer ib, ob;
  // package info
  SecPkgInfo m_PkgInfo;

  // 
  // Prepare data structure for returned data from SSPI
  ob.BufferType = SECBUFFER_TOKEN;
  ob.cbBuffer = m_PkgInfo.cbMaxToken;
  // Allocate space for return data
  out_bound_data_str = new BYTE[ob.cbBuffer + sizeof(DWORD)];
  ob.pvBuffer = out_bound_data_str;
  // prepare buffer description
  obd.cBuffers  = 1;
  obd.ulVersion = SECBUFFER_VERSION;
  obd.pBuffers  = &ob;

  //
  // Prepare the data we are passing to the SSPI method
  if(input->Utf8Length() > 0) {
    ib.BufferType = SECBUFFER_TOKEN;
    ib.cbBuffer   = decoded_input_str_length;
    ib.pvBuffer   = decoded_input_str;
    // prepare buffer description
    ibd.cBuffers  = 1;
    ibd.ulVersion = SECBUFFER_VERSION;
    ibd.pBuffers  = &ib;    
  }

  // Perform initialization step
  status = _sspi_initializeSecurityContext(
      &security_credentials->m_Credentials
    , NULL
    , const_cast<TCHAR*>(service_principal_name_str)
    , 0x02  // MUTUAL
    , 0
    , 0     // Network
    , input->Utf8Length() > 0 ? &ibd : NULL
    , 0
    , &security_context->m_Context
    , &obd
    , &security_context->CtxtAttr
    , &security_context->Expiration
  );

  // If we have a ok or continue let's prepare the result
  if(status == SEC_E_OK 
    || status == SEC_I_COMPLETE_NEEDED
    || status == SEC_I_CONTINUE_NEEDED
    || status == SEC_I_COMPLETE_AND_CONTINUE
  ) {
    security_context->hasContext = true;
    security_context->payload = base64_encode((const unsigned char *)ob.pvBuffer, ob.cbBuffer);
  } else {
    LPSTR err_message = DisplaySECError(status);

    if(err_message != NULL) {
      return VExceptionErrNo(err_message, status);
    } else {
      return VExceptionErrNo("Unknown error", status);
    }
  }

  // Make object persistent
  // Persistent<Object> persistent = Persistent<Object>::New(security_context_value);
  // Return security context
  return scope.Close(security_context_value);
}

Handle<Value> SecurityContext::PayloadGetter(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  // Unpack the context object
  SecurityContext *context = ObjectWrap::Unwrap<SecurityContext>(info.Holder());
  // Return the low bits
  return scope.Close(String::New(context->payload));
}


Handle<Value> SecurityContext::InitalizeStep(const Arguments &args) {
  HandleScope scope;
  printf("========================= InitalizeStep\n");

  char *service_principal_name_str = NULL, *input_str = NULL, *decoded_input_str = NULL;
  BYTE *out_bound_data_str = NULL;
  int decoded_input_str_length = NULL;
  // Status of operation
  SECURITY_STATUS status;

  // We need 3 parameters
  if(args.Length() != 2)
    return VException("Initialize must be called with [servicePrincipalName:string, input:string]");

  // Second parameter must be a string
  if(!args[0]->IsString())
    return VException("First parameter for Initialize must be a string");

  // Third parameter must be a base64 encoded string
  if(!args[1]->IsString())
    return VException("Second parameter for Initialize must be a string");

  // Let's unpack the values
  Local<String> service_principal_name = args[0]->ToString();
  service_principal_name_str = (char *)calloc(service_principal_name->Utf8Length() + 1, sizeof(char));
  service_principal_name->WriteUtf8(service_principal_name_str);

  printf("================================== service_principal_name\n");
  printf("%s\n", service_principal_name_str);

  // Unpack the user name
  Local<String> input = args[1]->ToString();

  if(input->Utf8Length() > 0) {
    input_str = (char *)calloc(input->Utf8Length() + 1, sizeof(char));
    input->WriteUtf8(input_str);

    printf("================================== input_str\n");
    printf("%s\n", input_str);

    // Now let's get the base64 decoded string
    decoded_input_str = (char *)base64_decode(input_str, &decoded_input_str_length);
  }

  // Structures used for c calls
  SecBufferDesc ibd, obd;
  SecBuffer ib, ob;
  // package info
  SecPkgInfo m_PkgInfo;

  // 
  // Prepare data structure for returned data from SSPI
  ob.BufferType = SECBUFFER_TOKEN;
  ob.cbBuffer = m_PkgInfo.cbMaxToken;
  // Allocate space for return data
  out_bound_data_str = new BYTE[ob.cbBuffer + sizeof(DWORD)];
  ob.pvBuffer = out_bound_data_str;
  // prepare buffer description
  obd.cBuffers  = 1;
  obd.ulVersion = SECBUFFER_VERSION;
  obd.pBuffers  = &ob;

  //
  // Prepare the data we are passing to the SSPI method
  if(input->Utf8Length() > 0) {
    ib.BufferType = SECBUFFER_TOKEN;
    ib.cbBuffer   = decoded_input_str_length;
    ib.pvBuffer   = decoded_input_str;
    // prepare buffer description
    ibd.cBuffers  = 1;
    ibd.ulVersion = SECBUFFER_VERSION;
    ibd.pBuffers  = &ib;    
  }

  // Unpack the long object
  SecurityContext *security_context = ObjectWrap::Unwrap<SecurityContext>(args.This());  
  SecurityCredentials *security_credentials = security_context->security_credentials;

  // Perform initialization step
  status = _sspi_initializeSecurityContext(
      &security_credentials->m_Credentials
    , security_context->hasContext == true ? &security_context->m_Context : NULL
    , const_cast<TCHAR*>(service_principal_name_str)
    , 0x02  // MUTUAL
    , 0
    , 0     // Network
    , input->Utf8Length() > 0 ? &ibd : NULL
    , 0
    , &security_context->m_Context
    , &obd
    , &security_context->CtxtAttr
    , &security_context->Expiration
  );

  // If we have a ok or continue let's prepare the result
  if(status == SEC_E_OK 
    || status == SEC_I_COMPLETE_NEEDED
    || status == SEC_I_CONTINUE_NEEDED
    || status == SEC_I_COMPLETE_AND_CONTINUE
  ) {
    // Set the new payload
    if(security_context->payload != NULL) free(security_context->payload);
    security_context->payload = base64_encode((const unsigned char *)ob.pvBuffer, ob.cbBuffer);
  } else {
    LPSTR err_message = DisplaySECError(status);

    if(err_message != NULL) {
      return VExceptionErrNo(err_message, status);
    } else {
      return VExceptionErrNo("Unknown error", status);
    }
  }

  return scope.Close(Null());
}
    
Handle<Value> SecurityContext::EncryptMessage(const Arguments &args) {
  HandleScope scope;
  SECURITY_STATUS status;

  if(args.Length() != 2)
    return VException("EncryptMessage takes an instance of SecurityBufferDescriptor and an integer flag");  
  if(!SecurityBufferDescriptor::HasInstance(args[0]))
    return VException("EncryptMessage takes an instance of SecurityBufferDescriptor and an integer flag");  
  if(!args[1]->IsUint32())
    return VException("EncryptMessage takes an instance of SecurityBufferDescriptor and an integer flag");  

  // Unpack the security context
  SecurityContext *security_context = ObjectWrap::Unwrap<SecurityContext>(args.This());

  // Unpack the descriptor
  SecurityBufferDescriptor *descriptor = ObjectWrap::Unwrap<SecurityBufferDescriptor>(args[0]->ToObject());

  // Let's execute encryption
  status = _sspi_EncryptMessage(
      &security_context->m_Context
    , (unsigned long)args[1]->ToInteger()->Value()
    , &descriptor->secBufferDesc
    , 0
  );

  // We've got ok
  if(status == SEC_E_OK) {
    int bytesToAllocate = (int)descriptor->bufferSize();    
    // Free up existing payload
    if(security_context->payload != NULL) free(security_context->payload);
    // Save the payload
    security_context->payload = base64_encode((unsigned char *)descriptor->toBuffer(), bytesToAllocate);
  } else {
    LPSTR err_message = DisplaySECError(status);

    if(err_message != NULL) {
      return VExceptionErrNo(err_message, status);
    } else {
      return VExceptionErrNo("Unknown error", status);
    }
  }

  return scope.Close(Null());
}

Handle<Value> SecurityContext::DecryptMessage(const Arguments &args) {
  HandleScope scope;
  unsigned long quality = 0;
  SECURITY_STATUS status;

  if(args.Length() != 1)
    return VException("DecryptMessage takes an instance of SecurityBufferDescriptor");
  if(!SecurityBufferDescriptor::HasInstance(args[0]))
    return VException("DecryptMessage takes an instance of SecurityBufferDescriptor");

  // Unpack the security context
  SecurityContext *security_context = ObjectWrap::Unwrap<SecurityContext>(args.This());

  // Unpack the descriptor
  SecurityBufferDescriptor *descriptor = ObjectWrap::Unwrap<SecurityBufferDescriptor>(args[0]->ToObject());

  // Let's execute encryption
  status = _sspi_DecryptMessage(
      &security_context->m_Context
    , &descriptor->secBufferDesc
    , 0
    , (unsigned long)&quality
  );

  printf("================== quality :: %ul\n", quality);

  // We've got ok
  if(status == SEC_E_OK) {
    int bytesToAllocate = (int)descriptor->bufferSize();    
    // Free up existing payload
    if(security_context->payload != NULL) free(security_context->payload);
    // Save the payload
    security_context->payload = base64_encode((unsigned char *)descriptor->toBuffer(), bytesToAllocate);
  } else {
    LPSTR err_message = DisplaySECError(status);

    if(err_message != NULL) {
      return VExceptionErrNo(err_message, status);
    } else {
      return VExceptionErrNo("Unknown error", status);
    }
  }

  return scope.Close(Null());
}

Handle<Value> SecurityContext::QueryContextAttributes(const Arguments &args) {
  HandleScope scope;
  SECURITY_STATUS status;

  if(args.Length() != 1)
    return VException("Decryption method takes a an integer Attribute specifier");
  if(!args[0]->IsInt32())
    return VException("Decryption method takes a an integer Attribute specifier");

  // Unpack the security context
  SecurityContext *security_context = ObjectWrap::Unwrap<SecurityContext>(args.This());
  uint32_t attribute = args[0]->ToInt32()->Value();

  // Check what attribute we are asking for
  if(attribute == SECPKG_ATTR_SIZES) {
    SecPkgContext_Sizes sizes;

    // Let's grab the query context attribute
    status = _sspi_QueryContextAttributes(
      &security_context->m_Context,
      attribute,
      &sizes
    );  
    
    if(status == SEC_E_OK) {
      Local<Object> value = Object::New();
      value->Set(String::New("maxToken"), Integer::New(sizes.cbMaxToken));
      value->Set(String::New("maxSignature"), Integer::New(sizes.cbMaxSignature));
      value->Set(String::New("blockSize"), Integer::New(sizes.cbBlockSize));
      value->Set(String::New("securityTrailer"), Integer::New(sizes.cbSecurityTrailer));
      return scope.Close(value);
    } else {
      LPSTR err_message = DisplaySECError(status);

      if(err_message != NULL) {
        return VExceptionErrNo(err_message, status);
      } else {
        return VExceptionErrNo("Unknown error", status);
      }      
    }
  }

  return scope.Close(Null());
}

void SecurityContext::Initialize(Handle<Object> target) {
  // Grab the scope of the call from Node
  HandleScope scope;
  // Define a new function template
  Local<FunctionTemplate> t = FunctionTemplate::New(New);
  constructor_template = Persistent<FunctionTemplate>::New(t);
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("SecurityContext"));

  // Class methods
  NODE_SET_METHOD(constructor_template, "initializeSync", InitializeContext);
  
  // Set up method for the instance
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "initializeSync", InitalizeStep);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "encryptMessageSync", EncryptMessage);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "decryptMessageSync", DecryptMessage);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "queryContextAttributesSync", QueryContextAttributes);

  // Getters for correct serialization of the object  
  constructor_template->InstanceTemplate()->SetAccessor(String::NewSymbol("payload"), PayloadGetter);

  // Set template class name
  target->Set(String::NewSymbol("SecurityContext"), constructor_template->GetFunction());  
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
