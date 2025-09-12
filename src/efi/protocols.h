// Protocol GUID
#include <stdint.h>

#include "types.h"
#include "system_table.h"

#ifndef EFI_PROTOCOLS
#define EFI_PROTOCOLS

// Protocol Structures

// Loaded Image
struct efi_loaded_image_protocol {
  // Information
  uint32_t revision;
  efi_handle_t parent;
  struct efi_system_table* system;
  // Source Location
  efi_handle_t device;
  struct efi_device_path_protocol* filepath;
  void* reserved;
  // Load Options
  uint32_t loadOptionsSize;
  void* loadOptions;
  // Load Location
  void* imageBase;
  uint64_t imageSize;
  enum efi_memory_type imageCodeType;
  enum efi_memory_type imageDataType;
  efi_status_t (*unload)(efi_handle_t);
};

// GOP
struct efi_pixel_bitmask {
  uint32_t red;
  uint32_t green;
  uint32_t blue;
  uint32_t reserved;
};
enum efi_graphics_pixel_format {
  EFI_PIXEL_FORMAT_RGBX8,
  EFI_PIXEL_FORMAT_BGRX8,
  EFI_PIXEL_FORMAT_BITMASK,
  EFI_PIXEL_FORMAT_BLT_ONLY,
  EFI_PIXEL_FORMAT_MAX
};
struct efi_graphics_output_mode_information {
  uint32_t version;
  uint32_t horizontalResolution;
  uint32_t verticalResolution;
  enum efi_graphics_pixel_format pixelFormat;
  struct efi_pixel_bitmask pixelInformation;
  uint32_t pixelsPerScanline;
};
struct efi_graphics_output_protocol_mode {
  uint32_t maxMode;
  uint32_t mode;
  struct efi_graphics_output_mode_information* info;
  efi_uint_t sizeOfInfo;
  efi_physical_address_t framebuffer;
  efi_uint_t framebufferSize;
};
struct efi_graphics_output_blt_pixel {
  uint8_t blue;
  uint8_t green;
  uint8_t red;
  uint8_t reserved;
};
enum efi_graphics_output_blt_operation {
  EFI_BLT_VIDEO_FILL,
  EFI_BLT_VIDEO_TO_BLT_BUFFER,
  EFI_BLT_BUFFER_TO_VIDEO,
  EFI_BLT_VIDEO_TO_VIDEO,
  EFI_GRAPHICS_OUTPUT_BLT_OPERATION_MAX
};
struct efi_graphics_output_protocol {
  efi_status_t (*queryMode)(struct efi_graphics_output_protocol*, uint32_t, efi_uint_t*, struct efi_graphics_output_mode_information**);
  efi_status_t (*setMode)(struct efi_graphics_output_protocol*, uint32_t);
  efi_status_t (*blt)(struct efi_graphics_output_protocol*, struct efi_graphics_output_blt_pixel*, enum efi_graphics_output_blt_operation, efi_uint_t, efi_uint_t, efi_uint_t, efi_uint_t, efi_uint_t, efi_uint_t, efi_uint_t);
  struct efi_graphics_output_protocol_mode* mode;
};

// Simple FS
// Modes
#define EFI_FILE_MODE_READ       0x0000000000000001
#define EFI_FILE_MODE_WRITE      0x0000000000000002
#define EFI_FILE_MODE_CREATE     0x8000000000000000
// Attributes
#define EFI_FILE_READ_ONLY       0x0000000000000001
#define EFI_FILE_HIDDEN          0x0000000000000002
#define EFI_FILE_SYSTEM          0x0000000000000004
#define EFI_FILE_RESERVED        0x0000000000000008
#define EFI_FILE_DIRECTORY       0x0000000000000010
#define EFI_FILE_ARCHIVE         0x0000000000000020
#define EFI_FILE_VALID_ATTR      0x0000000000000037
// Structures
struct efi_file_io_token {
  efi_event event;
  efi_status_t status;
  efi_uint_t bufferSize;
  void* buffer;
};
struct efi_file_info {
  uint64_t size;
  uint64_t fileSize;
  uint64_t physicalSize;
  struct efi_time createTime;
  struct efi_time lastAccessTime;
  struct efi_time modificationTime;
  uint64_t attribute;
  efi_char_t filename[256];
};
struct efi_file_protocol {
  uint64_t revision;
  efi_status_t (*open)(struct efi_file_protocol*, struct efi_file_protocol**, efi_char_t*, uint64_t, uint64_t);
  efi_status_t (*close)(struct efi_file_protocol*);
  efi_status_t (*remove)(struct efi_file_protocol*);
  efi_status_t (*read)(struct efi_file_protocol*, efi_uint_t*, void*);
  efi_status_t (*write)(struct efi_file_protocol*, efi_uint_t*, void*);
  efi_status_t (*getPosition)(struct efi_file_protocol*, uint64_t*);
  efi_status_t (*setPosition)(struct efi_file_protocol*, uint64_t);
  efi_status_t (*getInfo)(struct efi_file_protocol*, struct efi_guid*, efi_uint_t*, void*);
  efi_status_t (*setInfo)(struct efi_file_protocol*, struct efi_guid*, efi_uint_t, void*);
  efi_status_t (*flush)(struct efi_file_protocol*);
  // Revision 2
  efi_status_t (*openEx)(struct efi_file_protocol*, struct efi_file_protocol**, efi_char_t*, uint64_t, uint64_t, struct efi_file_io_token*);
  efi_status_t (*readEx)(struct efi_file_protocol*, struct efi_file_io_token*);
  efi_status_t (*writeEx)(struct efi_file_protocol*, struct efi_file_io_token*);
  efi_status_t (*flushEx)(struct efi_file_protocol*, struct efi_file_io_token*);
};
struct efi_simple_file_system_protocol {
  uint64_t revision;
  efi_status_t (*openVolume)(struct efi_simple_file_system_protocol*, struct efi_file_protocol**);
};

// Simple Pointer
struct efi_simple_pointer_mode {
	uint64_t resolutionX;
	uint64_t resolutionY;
	uint64_t resolutionZ;
	bool leftButton;
	bool rightButton;
};
struct efi_simple_pointer_state {
	int32_t movementX;
	int32_t movementY;
	int32_t movementZ;
	bool leftButton;
	bool rightButton;
};
struct efi_simple_pointer_protocol {
	efi_status_t (*reset)(struct efi_simple_pointer_protocol*, bool);
	efi_status_t (*getState)(struct efi_simple_pointer_protocol*, struct efi_simple_pointer_state*);
	efi_event waitForInput;
	struct efi_simple_pointer_mode* mode;

};

// Simple Network Protocol
#define EFI_SIMPLE_NETWORK_PROTOCOL_REVISION 0x00010000
#define MAX_MCAST_FILTER_CNT 16
#define EFI_SIMPLE_NETWORK_RECEIVE_UNICAST 0x01
#define EFI_SIMPLE_NETWORK_RECEIVE_MULTICAST 0x02
#define EFI_SIMPLE_NETWORK_RECEIVE_BROADCAST 0x04
#define EFI_SIMPLE_NETWORK_RECEIVE_PROMISCUOUS 0x08
#define EFI_SIMPLE_NETWORK_RECEIVE_PROMISCUOUS_MULTICAST 0x10
enum efi_simple_network_state {
  EfiSimpleNetworkStopped,
  EfiSimpleNetworkStarted,
  EfiSimpleNetworkInitialized,
  EfiSimpleNetworkMaxState
};
struct efi_simple_network_mode {
  uint32_t state;
  uint32_t hwAddressSize;
  uint32_t mediaHeaderSize;
  uint32_t nvRamSize;
  uint32_t nvRamAccessSize;
  uint32_t receiveFilterMask;
  uint32_t receiveFilterSetting;
  uint32_t maxMCastFilterCount;
  uint32_t mCastFilterCount;
  struct efi_mac_address mCastFilter[MAX_MCAST_FILTER_CNT];
  struct efi_mac_address currentAddress;
  struct efi_mac_address broadcasAddress;
  struct efi_mac_address permanentAddress;
  uint8_t ifType;
  bool maxAddressChangeable;
  bool multipleTxSupported;
  bool mediaPresentSupported;
  bool mediaPresent;
};
struct efi_network_statistics {
  uint64_t rxTotalFrames;
  uint64_t rxGoodFrames;
  uint64_t rxUndersizeFrames;
  uint64_t rxOversizeFrames;
  uint64_t rxDroppedFrames;
  uint64_t rxUnicastFrames;
  uint64_t rxBroadcastFrames;
  uint64_t rxMulticastFrames;
  uint64_t rxCrcErrorFrames;
  uint64_t rxTotalBytes;
  uint64_t txTotalFrames;
  uint64_t txGoodFrames;
  uint64_t txUndersizeFrames;
  uint64_t txOversizeFrames;
  uint64_t txDroppedFrames;
  uint64_t txUnicastFrames;
  uint64_t txBroadcastFrames;
  uint64_t txMulticastFrames;
  uint64_t txCrcErrorFrames;
  uint64_t txTotalBytes;
  uint64_t collisions;
  uint64_t unsupportedProtocol;
  uint64_t rxDuplicatedFrames;
  uint64_t rxDecryptErrorFrames;
  uint64_t txErrorFrames;
  uint64_t txRetryFrames;
};
struct efi_simple_network_protocol {
  uint64_t revision;
  efi_status_t (*start)(struct efi_simple_network_protocol*);
  efi_status_t (*stop)(struct efi_simple_network_protocol*);
  efi_status_t (*initialize)(struct efi_simple_network_protocol*, efi_uint_t, efi_uint_t);
  efi_status_t (*reset)(struct efi_simple_network_protocol*, bool);
  efi_status_t (*shutdown)(struct efi_simple_network_protocol*);
  efi_status_t (*receiveFilters)(struct efi_simple_network_protocol*, uint32_t, uint32_t, bool, efi_uint_t, struct efi_mac_address);
  efi_status_t (*stationAddress)(struct efi_simple_network_protocol*, bool, struct efi_mac_address*);
  efi_status_t (*statistics)(struct efi_simple_network_protocol*, bool, efi_uint_t*, struct efi_network_statistics*);
  efi_status_t (*mCastIpToMac)(struct efi_simple_network_protocol*, bool, union efi_ip_address*, struct efi_mac_address*);
  efi_status_t (*nvData)(struct efi_simple_network_protocol*, bool, efi_uint_t, efi_uint_t, void*);
  efi_status_t (*getStatus)(struct efi_simple_network_protocol*, uint32_t*, void**);
  efi_status_t (*transmit)(struct efi_simple_network_protocol*, efi_uint_t, efi_uint_t, void*, struct efi_mac_address*, struct efi_mac_address*, uint16_t);
  efi_status_t (*receive)(struct efi_simple_network_protocol*, efi_uint_t*, efi_uint_t*, void*, struct efi_mac_address*, struct efi_mac_address*, uint16_t);
  efi_event waitForPacket;
  struct efi_simple_network_mode* mode;  
};

// GUID List
#define EFI_LOADED_IMAGE_PROTOCOL_GUID \
  {0x5B1B31A1,0x9562,0x11d2,\
    {0x8E,0x3F,0x00,0xA0,0xC9,0x69,0x72,0x3B}}

#define EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID \
 {0x9042a9de,0x23dc,0x4a38,\
  {0x96,0xfb,0x7a,0xde,0xd0,0x80,0x51,0x6a}}

#define EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID \
 {0x0964e5b22,0x6459,0x11d2,\
  {0x8e,0x39,0x00,0xa0,0xc9,0x69,0x72,0x3b}}

#define EFI_FILE_INFO_ID \
 {0x09576e92,0x6d3f,0x11d2,\
  {0x8e,0x39,0x00,0xa0,0xc9,0x69,0x72,0x3b}}

#define EFI_SIMPLE_NETWORK_PROTOCOL_GUID \
  {0xA19832B9,0xAC25,0x11D3,\
    {0x9A,0x2D,0x00,0x90,0x27,0x3f,0xc1,0x4d}}

#define EFI_SIMPLE_POINTER_PROTOCOL_GUID \
 {0x31878c87,0xb75,0x11d5,\
  {0x9a,0x4f,0x00,0x90,0x27,0x3f,0xc1,0x4d}}

#endif
