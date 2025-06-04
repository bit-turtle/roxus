// Simple Terminal

#include <stddef.h>

#include "term.h"

#include "input.h"
#include "output.h"

#include "string.h"
#include "bsod.h"

efi_status_t term(struct efi_system_table* system, struct efi_graphics_output_protocol* gop) {
  efi_status_t status;

  bool running = true;
  efi_char_t buffer[256];
  while (running) {
    status = system->output->outputString(system->output, u"> ");
    if (status != EFI_SUCCESS) return status;
    status = input(system, buffer, 256);
    if (status != EFI_SUCCESS) return status;
    status = command(system, gop, buffer, &running);
    if (status != EFI_SUCCESS) return status;
  }

  return EFI_SUCCESS;
}

efi_status_t command(struct efi_system_table* system, struct efi_graphics_output_protocol* gop, efi_char_t* command, bool* running) {
  efi_status_t status;

  efi_uint_t argc = 1;
  efi_char_t* argv[256];
  argv[0] = command;

  // Parse Command
  while (*command != u'\0') {
    if (*command == u' ') {
      *command++ = u'\0';
      argv[argc++] = command;
    }
    else {
      command++;
    }
  }

  // Lookup Program
  if (streq(argv[0], u"exit")) {
    *running = false;
  }
  else if (streq(argv[0], u"clear")) {
    status = system->output->clearScreen(system->output);
    if (status != EFI_SUCCESS) return status;
  }
  else if (streq(argv[0], u"reset")) {
    bool valid = true;
    enum efi_reset_type_t reset;
    if (argc > 1) {
      if (streq(argv[1], u"shutdown"))
        reset = efi_reset_shutdown;
      else if (streq(argv[1], u"cold"))
        reset = efi_reset_cold;
      else if (streq(argv[1], u"warm"))
        reset = efi_reset_warm;
      else if (streq(argv[1], u"platform-specific"))
        reset = efi_reset_platform_specific;
      else valid = false;
    }
    else valid = false;
    if (valid)
      system->runtime_services->resetSystem(reset, EFI_SUCCESS, 0, NULL);
    else {
      status = system->output->outputString(system->output, u"Invalid Reset Type\n\rValid Types: shutdown, warm, cold, platform-specific");
      if (status != EFI_SUCCESS) return status;
    }
  }
  else if (streq(argv[0], u"echo")) {
    for (int i = 1; i < argc; i++) {
      if (i != 1) {
        status = system->output->outputString(system->output, u" ");
        if (status != EFI_SUCCESS) return status;
      }
      status = output(system, argv[i]);
      if (status != EFI_SUCCESS) return status;
    }
  }
  else if (streq(argv[0], u"bsod")) {
    bsod(system, EFI_SUCCESS);
  }
  else if (streq(argv[0], u"time")) {
    struct efi_time time;
    struct efi_time_capabilities capabilities;
    status = system->runtime_services->getTime(&time, &capabilities);
    if (status != EFI_SUCCESS) return status;
    for (int i = 1; i < argc; i++) {
      if (i != 1) {
        status = system->output->outputString(system->output, u":");
        if (status != EFI_SUCCESS) return status;
      }
      efi_char_t str[16];
      str[0] = u'\0';
      int value;
      bool valid = true;
      if (streq(argv[i], u"year")) value = time.year;
      else if (streq(argv[i], u"month")) value = time.month;
      else if (streq(argv[i], u"day")) value = time.day;
      else if (streq(argv[i], u"hour")) value = time.hour;
      else if (streq(argv[i], u"minute")) value = time.minute;
      else if (streq(argv[i], u"second")) value = time.second;
      else if (streq(argv[i], u"nanosecond")) value = time.nanosecond;
      else if (streq(argv[i], u"timezone")) value = time.timezone;
      else if (streq(argv[i], u"daylight")) value = time.daylight;
      else if (streq(argv[i], u"accuracy")) value = capabilities.accuracy;
      else if (streq(argv[i], u"resolution")) value = capabilities.resolution;
      else if (streq(argv[i], u"setstozero")) value = capabilities.setsToZero;
      else valid = false;
      if (valid)
        status = system->output->outputString(system->output, itoa(value, str, 10));
      else
        status = system->output->outputString(system->output, u"?");
      if (status != EFI_SUCCESS) return status;
    }
  }
  // GOP Test Commands
  else if (streq(argv[0], u"gopinfo")) {
    efi_char_t buf[128];
    system->output->outputString(system->output, u"Mode: ");
    system->output->outputString(system->output, itoa(gop->mode->mode, buf, 10));
    system->output->outputString(system->output, u"/");
    system->output->outputString(system->output, itoa(gop->mode->maxMode, buf, 10));
    system->output->outputString(system->output, u"\n\rResolution: ");
    system->output->outputString(system->output, itoa(gop->mode->info->horizontalResolution, buf, 10));
    system->output->outputString(system->output, u":");
    system->output->outputString(system->output, itoa(gop->mode->info->verticalResolution, buf, 10));
    system->output->outputString(system->output, u"\n\rFramebuffer Size: ");
    system->output->outputString(system->output, itoa(gop->mode->framebufferSize, buf, 10));
  }
  else if (streq(argv[0], u"gopmode")) {
    if (argc > 1) {
      uint32_t mode = getInt(argv[1]);
      if (mode < gop->mode->maxMode) {
        // Set GOP Mode
        status = gop->setMode(gop, mode);
        // Failure message
        if (status != EFI_SUCCESS)
          system->output->outputString(system->output, u"Failed to set GOP Mode!");
        // Success Message
        else
          system->output->outputString(system->output, u"GOP Mode Set!");
      }
      else {
        system->output->outputString(system->output, u"Mode must be less than max mode");
      }
    }
    else {
      system->output->outputString(system->output, u"Invalid Parameter");
    }
  }
  else if (streq(argv[0], u"goptest")) {
    struct efi_graphics_output_blt_pixel pixel;
    pixel.red = getInt(argv[1]);
    pixel.green = getInt(argv[2]);
    pixel.blue = getInt(argv[3]);
    gop->blt(gop, &pixel, EFI_BLT_VIDEO_FILL, 0, 0, 0, 0, 500, 500, 0);
  }
  else if (streq(argv[0], u"textinfo")) {
    efi_char_t buf[128];
    system->output->outputString(system->output, u"Mode: ");
    system->output->outputString(system->output, itoa(system->output->mode->mode, buf, 10));
    system->output->outputString(system->output, u"/");
    system->output->outputString(system->output, itoa(system->output->mode->maxMode, buf, 10));
    system->output->outputString(system->output, u"\n\rAttribute: ");
    system->output->outputString(system->output, itoa(system->output->mode->attribute, buf, 10));
  }
  else if (streq(argv[0], u"textmode")) {
    if (argc > 1) {
      uint32_t mode = getInt(argv[1]);
      if (mode < system->output->mode->maxMode) {
        // Set Text Mode
        status = system->output->setMode(system->output, mode);
        // Failure Message
        if (status != EFI_SUCCESS)
          system->output->outputString(system->output, u"Failed to set Text Mode!");
        // Success Message
        else
          system->output->outputString(system->output, u"Text Mode Set!");
      }
      else {
        system->output->outputString(system->output, u"Mode must be less than max mode");
      }
    }
    else {
      system->output->outputString(system->output, u"Invalid Parameter");
    }
  }
  else {
    status = system->output->outputString(system->output, u"Unknown Command");
    if (status != EFI_SUCCESS) return status;
  }

  return EFI_SUCCESS;
}
