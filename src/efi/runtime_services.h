#include <stdint.h>

#include "table_header.h"

#ifndef EFI_RUNTIME_SERVICES
#define EFI_RUNTIME_SERVICES

struct efi_runtime_services {
  struct efi_table_header header;
  // Time
  void* getTime;
  void* setTime;
  void* getWakeupTime;
  void* setWakeupTime;
  // Virtual Memory
  void* setVirtualAddressMap;
  void* convertPointer;
  // Variable
  void* getVariable;
  void* getNextVariableName;
  void* setVariable;
  // Misc
  void* getNextHighMonoCount;
  void* resetSystem;
  // Capsule (UEFI 2.0)
  void* updateCapsule;
  void* queryCapsuleCapabilities;
  // Misc (UEFI 2.0)
  void* queryVariableInfo;
};

#endif
