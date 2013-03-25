#include "kerberos_sspi.h"
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

int load_library() {
  DWORD err;
  HINSTANCE hDLL = LoadLibrary("security.dll");
  /*printf("================ ponter :: %p\n", ptr);*/
  
  if(hDLL != NULL) {
    printf("Library has been loaded\n");
  } else {
    err = GetLastError();
    printf("Couldn't load dll\n");
  }

  return 0;
}