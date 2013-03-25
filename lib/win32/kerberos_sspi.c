#include "kerberos_sspi.h"
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

int load_library() {
  void *ptr = LoadLibrary("security.dll");
  printf("================ ponter :: %p\n", ptr);

  return 0;
}