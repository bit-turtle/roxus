// References to EFI Protocols

#include "efi/system_table.h"
#include "efi/protocols.h"
#include "efi/types.h"

// Setup

void roxus_setup(efi_handle_t handle, struct efi_system_table* system);

extern struct efi_system_table* system_table;
extern struct efi_boot_services* boot_services;
extern struct efi_runtime_services* runtime_services;
extern struct efi_simple_text_output_protocol* text_output;
extern struct efi_simple_text_input_protocol* text_input;
extern struct efi_graphics_output_protocol* graphics_output;

extern struct efi_simple_file_system_protocol* filesystem;

// Utility

efi_status_t print(efi_char_t* string);
efi_status_t print_ascii(char* string);
efi_status_t clear_screen();
