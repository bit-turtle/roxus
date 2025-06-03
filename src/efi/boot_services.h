#include <stdint.h>

#include "table_header.h"

#include "types.h"
#include "handle.h"
#include "protocol_guid.h"

#ifndef EFI_BOOT_SERVICES
#define EFI_BOOT_SERVICES

struct efi_memory_descriptor {
  uint32_t type;
  efi_physical_address_t physicalStart;
  efi_virtual_address_t virtualStart;
  uint64_t numberOfPages;
  uint64_t attribute;
};

struct efi_boot_services {
  struct efi_table_header header;
  // Task Priority
  efi_uint_t (*raiseTPL)(efi_uint_t);
  void (*restoreTPL)(efi_uint_t);
  // Memory
  void* allocatePages;
  void* freePages;
  efi_status_t (*getMemoryMap)(efi_uint_t*, struct efi_memory_descriptor*, efi_uint_t*, efi_uint_t*, uint32_t*);
  void* allocatePool;
  void* freePool;
  // Event & Timer
  void* createEvent;
  void* setTimer;
  void* waitForEvent;
  void* signalEvent;
  void* closeEvent;
  void* checkEvent;
  // Protocol Handler
  efi_status_t (*installProtocolInterface)(efi_handle_t, struct efi_guid*, enum efi_interface_type, void*);
  void* reinstallProtocolInterface;
  void* uninstallProtocolInterface;
  void* handleProtocol;
  void* reserved;
  void* registerProtocolNotify;
  void* locateHandle;
  void* locateDevicePath;
  void* installConfigurationTable;
  // Image
  void* loadImage;
  void* startImage;
  void* exit;
  void* unloadImage;
  efi_status_t (*exitBootServices)(efi_handle_t*, efi_uint_t);
  // Misc
  void* getNextMonotonicCount;
  void* stall;
  efi_status_t (*setWatchdogTimer)(efi_uint_t, uint64_t, efi_uint_t, efi_char_t*);
  // DriverSupport
  void* connectController;
  void* disconnectController;
  // Protocol
  void* openProtocol;
  void* closeProtocol;
  void* openProtocolInformation;
  // Library
  void* protocolsPerHandle;
  void* locateHandleBuffer;
  efi_status_t (*locateProtocol)(struct efi_guid*, void*, void*);
  void* installMultipleProtocolInterfaces;
  void* uninstallMultipleProtocolInterfaces;
  // CRC32
  void* calculateCRC32;
  // Misc
  void* copyMem;
  void* setMem;
  void* createEventEx;
};

#endif
