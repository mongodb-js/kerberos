#include "kerberos.h"
#include <stdlib.h>
#include <tchar.h>
#include "base64.h"

#ifndef ARRAY_SIZE
# define ARRAY_SIZE(a) (sizeof((a)) / sizeof((a)[0]))
#endif

#define SECURITY_WIN32  1

// SEC_WINNT_AUTH_IDENTITY makes it unusually hard
// to compile for both Unicode and ansi, so I use this macro:
#ifdef _UNICODE
#define USTR(str) (str)
#else
#define USTR(str) ((unsigned char*)(str))
#endif

Persistent<FunctionTemplate> Kerberos::constructor_template;

// VException object (causes throw in calling code)
static Handle<Value> VException(const char *msg) {
  HandleScope scope;
  return ThrowException(Exception::Error(String::New(msg)));
}

Kerberos::Kerberos() : ObjectWrap() {
}

void Kerberos::Initialize(v8::Handle<v8::Object> target) {
  // Grab the scope of the call from Node
  HandleScope scope;
  // Define a new function template
  Local<FunctionTemplate> t = FunctionTemplate::New(Kerberos::New);
  constructor_template = Persistent<FunctionTemplate>::New(t);
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("Kerberos"));

  // Set up method for the Kerberos instance
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "acquireAlternateCredentials", AcquireAlternateCredentials);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "prepareOutboundPackage", PrepareOutboundPackage);  
  // NODE_SET_PROTOTYPE_METHOD(constructor_template, "authGSSClientUnwrap", AuthGSSClientUnwrap);
  // NODE_SET_PROTOTYPE_METHOD(constructor_template, "authGSSClientWrap", AuthGSSClientWrap);
  // NODE_SET_PROTOTYPE_METHOD(constructor_template, "authGSSClientClean", AuthGSSClientClean);

  // Set the symbol
  target->ForceSet(String::NewSymbol("Kerberos"), constructor_template->GetFunction());
}

Handle<Value> Kerberos::New(const Arguments &args) {
  // Load the security.dll library
  load_library();
  // Create a Kerberos instance
  Kerberos *kerberos = new Kerberos();
  kerberos->m_HaveContext = false;
  // Return the kerberos object
  kerberos->Wrap(args.This());
  return args.This();
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// SSPI Methods
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Handle<Value> Kerberos::AcquireAlternateCredentials(const Arguments &args) {
  HandleScope scope;
  // printf("============= Acquire :: 0\n");

  // C String pointers
  char *domain_str          = NULL;
  char *user_name_str       = NULL;
  char *password_str        = NULL;

  // Status of operation
  SECURITY_STATUS status;

  // Unpack the long object
  Kerberos *kerberos = ObjectWrap::Unwrap<Kerberos>(args.This());

  // Unpack the arguments
  Local<String> user = args[0]->ToString();
  Local<String> password = args[1]->ToString();

  if(args.Length() == 3 && args[2]->IsString()) {
    Local<String> domain = args[2]->ToString();
    // Create c strings of the values passed in
    domain_str = (char *)calloc(domain->Utf8Length() + 1, sizeof(char));
    domain->WriteUtf8(domain_str);
  }

  user_name_str = (char *)calloc(user->Utf8Length() + 1, sizeof(char));
  user->WriteUtf8(user_name_str);

  password_str = (char *)calloc(password->Utf8Length() + 1, sizeof(char));
  password->WriteUtf8(password_str);

  // printf("============= Acquire :: 0:1\n");
  // printf("%s :: %s\n", user_name_str, password_str);

  // Set up domain
  if(domain_str != NULL) {

    // printf("============= Acquire :: 0:2\n");
    kerberos->m_Identity.Domain = USTR(_tcsdup(domain_str));
    kerberos->m_Identity.DomainLength = (unsigned long)_tcslen(domain_str);    
  } else {
    kerberos->m_Identity.Domain = NULL;
    kerberos->m_Identity.DomainLength = 0;
  }

  // Set up the user
  kerberos->m_Identity.User = USTR(_tcsdup(user_name_str));
  kerberos->m_Identity.UserLength = (unsigned long)_tcslen(user_name_str);

  // Set up the password
  kerberos->m_Identity.Password = USTR(_tcsdup(password_str));
  kerberos->m_Identity.PasswordLength = (unsigned long)_tcslen(password_str);

#ifdef _UNICODE
    kerberos->m_Identity.Flags          = SEC_WINNT_AUTH_IDENTITY_UNICODE;
#else
    kerberos->m_Identity.Flags          = SEC_WINNT_AUTH_IDENTITY_ANSI;
#endif

  // printf("============= Acquire :: 1\n");
  // Try to acquire credentials
  status = _kerberos_AcquireCredentialsHandle(
      NULL,
      "Kerberos",
      SECPKG_CRED_OUTBOUND,
      NULL, 
      &kerberos->m_Identity,
      NULL, NULL,
      &kerberos->m_Credentials,
      &kerberos->Expiration      
    );

  // printf("============= Acquire :: 2\n");
  // printf("=================== status :: %d :: %d\n", status, SEC_E_OK);
  return scope.Close(String::New("hello"));
}

Handle<Value> Kerberos::PrepareOutboundPackage(const Arguments &args) {
  HandleScope scope;
  // Variables used
  char *in_bound_data_str = NULL;
  int in_bound_length = 0;
  char *temp;
  char *target_str = NULL;
  char *payload = NULL;
  BYTE *out_bound_data_str = NULL;
  SecBufferDesc   ibd, obd;
  SecBuffer       ib,  ob;
  SECURITY_STATUS status;
  Local<String> inbound;
  Local<String> target;  

  // Unpack the long object
  Kerberos *kerberos = ObjectWrap::Unwrap<Kerberos>(args.This());

  // Unpack the principle
  target = args[0]->ToString();
  // Create c strings of the values passed in
  target_str = (char *)calloc(target->Utf8Length() + 1, sizeof(char));
  target->WriteUtf8(target_str);    

  // Unpack the inbound data if any
  if(args[1]->IsString()) {
    inbound = args[1]->ToString();
    in_bound_length = inbound->Utf8Length();
    // Create c strings of the values passed in
    in_bound_data_str = (char *)calloc(in_bound_length + 1, sizeof(char));    
    inbound->WriteUtf8(in_bound_data_str);    
    // Keep reference to we can get rid of the memory
    temp = in_bound_data_str;
    // Now let's get the base64 decoded string
    in_bound_data_str = (char *)base64_decode(in_bound_data_str, &in_bound_length);
    free(temp);
  }

  // prepare outbound buffer
  ob.BufferType = SECBUFFER_TOKEN;
  ob.cbBuffer   = kerberos->m_PkgInfo.cbMaxToken;
  // Allocate space for return data
  out_bound_data_str = new BYTE[ob.cbBuffer + sizeof(DWORD)];
  if(out_bound_data_str == NULL) {
    return scope.Close(String::New("error"));
  }
  // Point buffer to allocated space
  ob.pvBuffer = out_bound_data_str;

  // prepare buffer description
  obd.cBuffers  = 1;
  obd.ulVersion = SECBUFFER_VERSION;
  obd.pBuffers  = &ob;

  if(in_bound_data_str != NULL) {

    printf("================= strlen(in_bound_data_str) :: %lu\n", in_bound_length);
    // prepare inbound buffer
    ib.BufferType = SECBUFFER_TOKEN;
    ib.cbBuffer   = in_bound_length;
    ib.pvBuffer   = in_bound_data_str;
    // prepare buffer description
    ibd.cBuffers  = 1;
    ibd.ulVersion = SECBUFFER_VERSION;
    ibd.pBuffers  = &ib;    
  }

  // prepare our context
  DWORD      CtxtAttr;
  // TimeStamp  Expiration;

  // printf("=============== target ::%s\n", target_str);

  if(kerberos->m_HaveContext == true) {
    printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^ GOT CONTEXT\n");
  }

  if(in_bound_data_str != NULL) {
    printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^ INBOUND DATA\n");
  }

  status = _kerberos_initializeSecurityContext ( 
    &kerberos->m_Credentials,
    kerberos->m_HaveContext ? &kerberos->m_Context : NULL,
    const_cast<TCHAR*>(target_str),
    0x2, // MUTUAL
    0,  
    0,   // Network
    in_bound_data_str != NULL ? &ibd : NULL,
    0,
    &kerberos->m_Context,
    &obd,
    &kerberos->CtxtAttr,
    &kerberos->Expiration 
  );

  if((status == SEC_I_COMPLETE_NEEDED) || (status == SEC_I_COMPLETE_AND_CONTINUE)) {
    printf("==================== SEC_I_COMPLETE_NEEDED/SEC_I_COMPLETE_AND_CONTINUE");
  }


    switch ( status )
    {
    case SEC_E_OK:
        // we should now have a context
        kerberos->m_HaveContext = true;
        payload = base64_encode((const unsigned char *)ob.pvBuffer, ob.cbBuffer);
        // printf("payload :: %s\n", payload);
        return scope.Close(String::New(payload));
        break;
    case SEC_I_COMPLETE_NEEDED:
        // m_State = AuthSuccess;   // we're done here
        // printf("SEC_I_COMPLETE_NEEDED\n");
        break;
    case SEC_I_CONTINUE_NEEDED:
    case SEC_I_COMPLETE_AND_CONTINUE:
        // we should now have a context
        kerberos->m_HaveContext = true;
        // printf("SEC_I_CONTINUE_NEEDED/SEC_I_COMPLETE_AND_CONTINUE\n");
        payload = base64_encode((const unsigned char *)ob.pvBuffer, ob.cbBuffer);
        // printf("payload :: %s\n", payload);
        return scope.Close(String::New(payload));
        // m_State = AuthContinue;  // keep on going
        break;
    case SEC_E_LOGON_DENIED:
        // printf("SEC_E_LOGON_DENIED\n");
        // m_State = AuthFailed;    // logon denied
        break;
    default:
        printf("ERROR\n");
        break;
        // m_State = AuthFailed;
        // make sure we don't leak memory
        // FreeBuffer ( pOutbound ); 
        // THROWEXE ( ErrorAuthFailed, status );
    }

  return scope.Close(String::New(""));
}


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// UV Lib callbacks
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void Kerberos::Process(uv_work_t* work_req) {
  // // Grab the worker
  // Worker *worker = static_cast<Worker*>(work_req->data);
  // // Execute the worker code
  // worker->execute(worker);
}

void Kerberos::After(uv_work_t* work_req) {
  // Grab the scope of the call from Node
  v8::HandleScope scope;

  // // Get the worker reference
  // Worker *worker = static_cast<Worker*>(work_req->data);

  // // If we have an error
  // if(worker->error) {
  //   v8::Local<v8::Value> err = v8::Exception::Error(v8::String::New(worker->error_message));
  //   Local<Object> obj = err->ToObject();
  //   obj->Set(NODE_PSYMBOL("code"), Int32::New(worker->error_code));
  //   v8::Local<v8::Value> args[2] = { err, v8::Local<v8::Value>::New(v8::Null()) };
  //   // Execute the error
  //   v8::TryCatch try_catch;
  //   // Call the callback
  //   worker->callback->Call(v8::Context::GetCurrent()->Global(), ARRAY_SIZE(args), args);
  //   // If we have an exception handle it as a fatalexception
  //   if (try_catch.HasCaught()) {
  //     node::FatalException(try_catch);
  //   }
  // } else {
  //   // // Map the data
  //   v8::Handle<v8::Value> result = worker->mapper(worker);
  //   // Set up the callback with a null first
  //   v8::Handle<v8::Value> args[2] = { v8::Local<v8::Value>::New(v8::Null()), result};
  //   // Wrap the callback function call in a TryCatch so that we can call
  //   // node's FatalException afterwards. This makes it possible to catch
  //   // the exception from JavaScript land using the
  //   // process.on('uncaughtException') event.
  //   v8::TryCatch try_catch;
  //   // Call the callback
  //   worker->callback->Call(v8::Context::GetCurrent()->Global(), ARRAY_SIZE(args), args);
  //   // If we have an exception handle it as a fatalexception
  //   if (try_catch.HasCaught()) {
  //     node::FatalException(try_catch);
  //   }
  // }

  // // Clean up the memory
  // worker->callback.Dispose();
  // delete worker;
}


// // Exporting function
// extern "C" void init(Handle<Object> target) {
//   HandleScope scope;
//   Kerberos::Initialize(target);
// }

NODE_MODULE(kerberos, Kerberos::Initialize);