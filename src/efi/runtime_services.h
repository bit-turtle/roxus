#include <stdint.h>

#include "table_header.h"
#include "types.h"

#ifndef EFI_RUNTIME_SERVICES
#define EFI_RUNTIME_SERVICES

struct efi_time {
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint8_t pad1;
  uint32_t nanosecond;
  int16_t timezone;
  uint8_t daylight;
  uint8_t pad2;
};

struct efi_time_capabilities {
  uint32_t resolution;
  uint32_t accuracy;
  bool setsToZero;
};

enum efi_reset_type_t {
  efi_reset_cold,
  efi_reset_warm,
  efi_reset_shutdown,
  efi_reset_platform_specific
};

struct efi_runtime_services {
  struct efi_table_header header;
  // Time
  efi_status_t (*getTime)(struct efi_time* time, struct efi_time_capabilities* capabilities);
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
  void (*resetSystem)(enum efi_reset_type_t, efi_status_t, efi_uint_t, void*);
  // Capsule (UEFI 2.0)
  void* updateCapsule;
  void* queryCapsuleCapabilities;
  // Misc (UEFI 2.0)
  void* queryVariableInfo;
};

#endif
