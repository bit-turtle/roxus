#include "roxus.h"

#include "bsod.h"
#include "string.h"
#include "libc.h"
#include <stddef.h>

// System
efi_handle_t system_handle;
struct efi_system_table* system_table;
struct efi_boot_services* boot_services;
struct efi_runtime_services* runtime_services;
struct efi_simple_text_output_protocol* text_output;
int32_t text_output_height, text_output_width;
struct efi_simple_text_input_protocol* text_input;
struct efi_graphics_output_protocol* graphics_output;

struct efi_loaded_image_protocol* loaded_image;
struct efi_simple_file_system_protocol* filesystem;

struct efi_simple_network_protocol* network = NULL;

// Mouse Pointer
struct roxus_pointer pointer;
struct efi_simple_pointer_protocol* simple_pointer = NULL;
struct efi_absolute_pointer_protocol* absolute_pointer = NULL;

// Utility
efi_status_t screenshot(struct efi_graphics_output_blt_pixel** image) {
  *image = malloc(sizeof(struct efi_graphics_output_blt_pixel) * graphics_output->mode->info->horizontalResolution * graphics_output->mode->info->verticalResolution);
  if (*image == NULL)
    return EFI_BUFFER_TOO_SMALL;
  graphics_output->blt(graphics_output, *image, EFI_BLT_VIDEO_TO_BLT_BUFFER, 0, 0, 0, 0, graphics_output->mode->info->horizontalResolution, graphics_output->mode->info->verticalResolution, 0);
  return EFI_SUCCESS;
}
struct efi_graphics_output_blt_pixel* mainscroll = NULL;
struct efi_graphics_output_blt_pixel* scrollback[64] = {NULL};
efi_status_t print(efi_char_t* string) {
  if (mainscroll != NULL) {
    graphics_output->blt(graphics_output, mainscroll, EFI_BLT_BUFFER_TO_VIDEO, 0, 0, 0, 0, graphics_output->mode->info->horizontalResolution, graphics_output->mode->info->verticalResolution, graphics_output->mode->info->horizontalResolution);
    free(mainscroll);
    mainscroll = NULL;
  }
  efi_char_t* end = string;
  unsigned lines = 0;
  while(*end != u'\0') {
    if (*end == u'\n')
      lines++;
    end++;
  };
  if (false && text_output->mode->cursorRow+lines >= text_output_height) {
    struct efi_graphics_output_blt_pixel** scroll = NULL;
    for (int i = 0; scrollback[i] == NULL; i++)
      scroll = &scrollback[i];
    screenshot(scroll);
    clear_screen();
  }
  return system_table->output->outputString(system_table->output, string);
}
efi_status_t print_ascii(char* string) {
  efi_status_t status;
  efi_char_t* buffer;
  unsigned length = 0;
  while (string[length++] != '\0');
  buffer = malloc(sizeof(efi_char_t)*length);
  for (int i = 0; i < length; i++)
    buffer[i] = string[i];
  status = print(buffer);
  free(buffer);
  return status;
}
efi_status_t clear_screen() {
  return system_table->output->clearScreen(system_table->output);
}

// Roxus Protocols

// Roxus Pointer
bool roxus_pointer_exists() {
  return (simple_pointer != NULL || absolute_pointer != NULL);
}
bool roxus_pointer_absolute() {
  return (absolute_pointer != NULL);
}
struct roxus_pointer_state roxus_pointer_state = {0,0,0,false,false};
struct roxus_pointer_state roxus_pointer_get_state() {
  if (absolute_pointer != NULL) {
    efi_status_t status;
    struct efi_absolute_pointer_state state;
    status = absolute_pointer->getState(absolute_pointer, &state);
    if (status == EFI_SUCCESS) {
      roxus_pointer_state.x = state.currentX;
      roxus_pointer_state.y = state.currentY;
      roxus_pointer_state.z = state.currentZ;
      roxus_pointer_state.left = state.activeButtons&EFI_ABS_AltActive;
      roxus_pointer_state.right = state.activeButtons&EFI_ABSP_TouchActive;
    }
  }
  if (simple_pointer != NULL) {
    efi_status_t status;
    struct efi_simple_pointer_state state;
    status = simple_pointer->getState(simple_pointer, &state);
    if (status == EFI_SUCCESS) {
      roxus_pointer_state.x += state.movementX;
      roxus_pointer_state.y += state.movementY;
      roxus_pointer_state.z += state.movementZ;
      roxus_pointer_state.left = state.leftButton;
      roxus_pointer_state.right = state.rightButton;
    }
  }
  return roxus_pointer_state;
}
struct roxus_pointer pointer = {
  roxus_pointer_exists,
  roxus_pointer_absolute,
  roxus_pointer_get_state
};

// Setup
bool watchdog_setup() {
  efi_status_t status = system_table->boot_services->setWatchdogTimer(0, 0x10000, 0, NULL);
  if (status != EFI_SUCCESS && status != EFI_UNSUPPORTED)
    return true;
  return false;
}
bool gop_setup() {
  efi_status_t status;
  // Locate GOP
  struct efi_guid gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
  status = system_table->boot_services->locateProtocol(&gop_guid, NULL, &graphics_output);
  if (status != EFI_SUCCESS) return true;
  // Start GOP (If not started)
  struct efi_graphics_output_mode_information* info;
  efi_uint_t infosize, nativeMode = 0;
  status = graphics_output->queryMode(graphics_output, graphics_output->mode==NULL?0:graphics_output->mode->mode, &infosize, &info);
  if (status == EFI_NOT_STARTED) {
    // default to mode zero
    status = graphics_output->setMode(graphics_output, 0);
    if (status != EFI_SUCCESS) return true;
  }

  return false;
}
bool text_setup() {
  text_output = system_table->output;
  text_input = system_table->input;
  // Find best text mode
  efi_status_t status;
  efi_uint_t best_width = 0, best_height = 0;
  efi_uint_t best_mode = 0;
  for (efi_uint_t mode = 0; mode < text_output->mode->maxMode; mode++) {
    efi_uint_t width, height;
    status = text_output->queryMode(text_output, mode, &width, &height);
    if (status == EFI_SUCCESS)
    if (width > best_width || height > best_height ) {
      best_width = width;
      best_height = height;
      best_mode = mode;
    }
  }
  text_output_width = best_width;
  text_output_height = best_height;
  status = text_output->setMode(text_output, best_mode);
  if (status != EFI_SUCCESS)
    return true;
  return false;
}
bool fs_setup() {
  efi_status_t status;
  // Locate Loaded Image Protocol
  struct efi_guid loaded_image_guid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
  status = system_table->boot_services->openProtocol(system_handle, &loaded_image_guid, (void**)&loaded_image, system_handle, NULL, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
  if (status != EFI_SUCCESS) return true;
  // Open Simple Filesystem Protocol
  struct efi_guid fs_guid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
  status |= system_table->boot_services->openProtocol(loaded_image->device, &fs_guid, (void**)&filesystem, system_handle, NULL, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
  if (status != EFI_SUCCESS) return true;

  return false;
}

bool network_setup() {
  efi_status_t status;
  struct efi_guid network_guid = EFI_SIMPLE_NETWORK_PROTOCOL_GUID;
  // Locate Simple Network Protocol
  status = system_table->boot_services->locateProtocol(&network_guid, NULL, &network);
  if (status != EFI_SUCCESS) return true;
  // Initialize Network
  status |= network->start(network);
  status |= network->initialize(network,0,0);
  if (status != EFI_SUCCESS) return true;
  
  return false;
}

bool mouse_setup() {
  efi_status_t status;
  bool mouse = false;
  struct efi_guid simple_pointer_guid = EFI_SIMPLE_POINTER_PROTOCOL_GUID;
  struct efi_guid absolute_pointer_guid = EFI_ABSOLUTE_POINTER_PROTOCOL_GUID;
  // Locate Simple Pointer Protocol
  status = system_table->boot_services->locateProtocol(&simple_pointer_guid, NULL, &simple_pointer);
  if (status == EFI_SUCCESS)
    mouse = true;
  // Locate Simple Pointer Protocol
  status = system_table->boot_services->locateProtocol(&absolute_pointer_guid, NULL, &absolute_pointer);
  if (status == EFI_SUCCESS)
    mouse = true;

  return !mouse;
}

void roxus_setup(efi_handle_t handle, struct efi_system_table *system) {
  system_handle = handle;
  system_table = system;
  boot_services = system_table->boot_services;
  runtime_services = system_table->runtime_services;
  // Setup
  // Disable Watchdog Timer
  bool watchdogerror = watchdog_setup();
  // Setup GOP
  bool goperror = gop_setup();
  // Setup Text Output
  bool texterror = text_setup();
  // Setup Filesystem
  bool fserror = fs_setup();
  // Setup Network
  bool neterror = network_setup();
  // Setup Mouse Pointer
  bool mouseerror = mouse_setup();

  // Clear Screen
  text_output->clearScreen(text_output);

  // Welcome Message
  print(u"Welcome to Roxus!\n\r");

  // Warnings
  if (watchdogerror || goperror || texterror || fserror || neterror || mouseerror) {
    if (watchdogerror)
      print(u"Error: Failed to stop Watchdog Timer\n\r");
    if (goperror)
      print(u"Error: Failed to initialize Graphics Output\n\r");
    if (texterror)
      print(u"Error: Failed to set Text Mode\n\r");
    if (fserror)
      print(u"Error: Failed to open File System\n\r");
    if (neterror)
      print(u"Error: Failed to open Network\n\r");
    if (mouseerror)
      print(u"Error: Failed to get Mouse Pointer\n\r");
  }
  else
    print(u"All Systems Operational\n\r");
}
