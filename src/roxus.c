#include "roxus.h"

#include "bsod.h"
#include "string.h"
#include <stddef.h>

// System
efi_handle_t system_handle;
struct efi_system_table* system_table;
struct efi_boot_services* boot_services;
struct efi_runtime_services* runtime_services;
struct efi_simple_text_output_protocol* text_output;
struct efi_simple_text_input_protocol* text_input;
struct efi_graphics_output_protocol* graphics_output;

struct efi_loaded_image_protocol* loaded_image;
struct efi_simple_file_system_protocol* filesystem;

// Utility
efi_status_t print(efi_char_t* string) {
  return system_table->output->outputString(system_table->output, string);
}
efi_status_t clear_screen() {
  return system_table->output->clearScreen(system_table->output);
}

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
  status = system_table->boot_services->openProtocol(loaded_image->device, &fs_guid, (void**)&filesystem, system_handle, NULL, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
  if (status != EFI_SUCCESS) return true;

  return false;
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

  // Clear Screen
  text_output->clearScreen(text_output);

  // Welcome Message
  print(u"Welcome to Roxus!\n\r");

  // Warnings
  if (watchdogerror || goperror || texterror || fserror) {
    if (watchdogerror)
      print(u"Error: Failed to stop Watchdog Timer\n\r");
    if (goperror)
      print(u"Error: Failed to initialize Graphics Output\n\r");
    if (texterror)
      print(u"Error: Failed to set Text Mode\n\r");
    if (fserror)
      print(u"Error: Failed to open File System\n\r");
  }
  else
    print(u"All Systems Operational\n\r");
}
