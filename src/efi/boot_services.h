#include <stdint.h>

#include "table_header.h"

struct efi_boot_services {
  struct efi_table_header header;
  // Task Priority
  void* raiseTPL;
  void* restoreTPL;
  // Memory
  void* allocatePages;
  void* freePages;
  void* getMemoryMap;
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
  void* installProtocolInterface;
  void* reinstallProtocolInterface;
  void* uninstallProtocolInterface;
  void* handleProtocol;
  void* reserved;
  void* registerProtocolNotify;
  void* locateHandle;
  void* locateDevicePath;
  void* installConfigurationTable;
  // Image
  void* imageUnload;
  void* imageStart;
  void* exit;
  void* exitBootServices;
  // Misc
  void* getNextMonotonicCount;
  void* stall;
  void* setWatchdogTimer;
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
  void* locateProtocol;
  void* installMultipleProtocolInterfaces;
  void* uninstallMultipleProtocolInterfaces;
  // CRC32
  void* calculateCRC32;
  // Misc
  void* copyMem;
  void* setMem;
  void* createEventEx;
};
