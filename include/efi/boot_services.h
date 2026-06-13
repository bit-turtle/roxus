#include <stdint.h>

#include "table_header.h"

#include "types.h"

#ifndef EFI_BOOT_SERVICES
#define EFI_BOOT_SERVICES

struct efi_memory_descriptor {
  uint32_t type;
  efi_physical_address_t physicalStart;
  efi_virtual_address_t virtualStart;
  uint64_t numberOfPages;
  uint64_t attribute;
};

#define EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL   0x00000001
#define EFI_OPEN_PROTOCOL_GET_PROTOCOL         0x00000002
#define EFI_OPEN_PROTOCOL_TEST_PROTOCOL        0x00000004
#define EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER  0x00000008
#define EFI_OPEN_PROTOCOL_BY_DRIVER            0x00000010
#define EFI_OPEN_PROTOCOL_EXCLUSIVE            0x00000020

enum efi_allocate_type {
  EFI_ALLOCATE_ANY_PAGES,
  EFI_ALLOCATE_MAX_ADDRESS,
  EFI_ALLOCATE_ADDRESS,
  EFI_MAX_ALLOCATE_TYPE
};

struct efi_boot_services {
  struct efi_table_header header;
  // Task Priority
  efi_uint_t (*raiseTPL)(efi_uint_t);
  void (*restoreTPL)(efi_uint_t);
  // Memory
  efi_status_t (*allocatePages)(enum efi_allocate_type, enum efi_memory_type, efi_uint_t, efi_physical_address_t*);
  efi_status_t (*freePages)(efi_physical_address_t, efi_uint_t);
  efi_status_t (*getMemoryMap)(efi_uint_t*, struct efi_memory_descriptor*, efi_uint_t*, efi_uint_t*, uint32_t*);
  efi_status_t (*allocatePool)(enum efi_memory_type, efi_uint_t, void**);
  efi_status_t (*freePool)(void*);
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
  efi_status_t (*handleProtocol)(efi_handle_t, struct efi_guid*, void**);
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
  efi_status_t (*stall)(efi_uint_t);
  efi_status_t (*setWatchdogTimer)(efi_uint_t, uint64_t, efi_uint_t, efi_char_t*);
  // DriverSupport
  void* connectController;
  void* disconnectController;
  // Protocol
  efi_status_t (*openProtocol)(efi_handle_t, struct efi_guid*, void**, efi_handle_t, efi_handle_t, uint32_t);
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
