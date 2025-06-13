                                                              // Simple Terminal

#include <stddef.h>
#include "libc.h"

#include "term.h"

#include "input.h"
#include "output.h"

#include "string.h"
#include "bsod.h"
#include "roxus.h"
#include "image.h"

efi_status_t term() {
  efi_status_t status;

  // Open Filesystem
  struct efi_file_protocol* dir = NULL;
  status = filesystem->openVolume(filesystem, &dir);
  if (status != EFI_SUCCESS) print(u"Failed to open filesystem!");

  bool running = true;
  efi_char_t buffer[256];
  while (running) {
    print(u"\n\r> ");
    input(buffer, 256);
    status = command(buffer, &dir, &running);
    if (status != EFI_SUCCESS) print(u"Error!");
  }

  return EFI_SUCCESS;
}

efi_status_t run(struct efi_file_protocol* file, struct efi_file_protocol** dir) {
  efi_char_t* buffer = NULL;
  // Allocate buffer
  struct efi_guid info_guid = EFI_FILE_INFO_ID;
  struct efi_file_info info;
  efi_uint_t info_size = sizeof(info);
  efi_status_t status = file->getInfo(file, &info_guid, &info_size, &info);
  if (status != EFI_SUCCESS) return status;
  buffer = malloc(info.fileSize);
  if (buffer == NULL)
    return EFI_BUFFER_TOO_SMALL;
  // Load File
  efi_uint_t size = info.fileSize;
  status = file->read(file, &size, buffer);
  if (status != EFI_SUCCESS)
    return status;
  print(u"Script loaded!");
  // Run script
  print(buffer);
  efi_char_t* start = buffer;
  efi_char_t* end = buffer;
  while (*++end != u'\0') {
    if (*end == u'\n') *end = u'\0';
    print(start);
    start = end+1;
  }
  // Dealloc script
  free(buffer);

  return EFI_SUCCESS;
}

efi_status_t command(efi_char_t* command, struct efi_file_protocol** dir, bool* running) {
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
  else if (streq(argv[0], u"clear"))
    clear_screen();
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
      system_table->runtime_services->resetSystem(reset, EFI_SUCCESS, 0, NULL);
    else {
      status = system_table->output->outputString(system_table->output, u"Invalid Reset Type\n\rValid Types: shutdown, warm, cold, platform-specific");
      if (status != EFI_SUCCESS) return status;
    }
  }
  else if (streq(argv[0], u"echo")) {
    for (int i = 1; i < argc; i++) {
      if (i != 1) {
        status = system_table->output->outputString(system_table->output, u" ");
        if (status != EFI_SUCCESS) return status;
      }
      status = output(system_table, argv[i]);
      if (status != EFI_SUCCESS) return status;
    }
  }
  else if (streq(argv[0], u"bsod")) {
    bsod(system_table, EFI_SUCCESS);
  }
  // File
  else if (streq(argv[0], u"cd")) {
    if (argc > 1) {
      struct efi_file_protocol* newdir;
      status = (*dir)->open(*dir, &newdir, argv[1], EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
      if (status != EFI_SUCCESS) {
        print(u"Failed to open directory");
      }
      else {
        status = (*dir)->close(*dir);
        if (status != EFI_SUCCESS) print(u"Failed to close original dir");
        *dir = newdir;
        print(argv[1]);
      }
    }
    else print(u"Expected directory name");
  }
  else if (streq(argv[0], u"ls")) {
    struct efi_file_protocol* directory;
    status = (*dir)->open(*dir, &directory, u".", EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
    if (status == EFI_SUCCESS) {
      while (true) {
        struct efi_file_info info;
        efi_uint_t size = sizeof(info);
        status = directory->read(directory, &size, &info);
        if (size == 0) break;
        print(info.filename);
        print(u"\n\r");
      }
    }
  }
  else if (streq(argv[0], u"run")) {
    struct efi_file_protocol* file;
    status = (*dir)->open(*dir, &file, argv[1], EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
    if (status != EFI_SUCCESS) {
      print(u"Failed to open file: ");
      print(argv[1]);
      print(u"\n\r");
    }
    else {
      status = run(file, dir);
      if (status != EFI_SUCCESS) {
        print(u"Error in script: ");
        print(argv[1]);
        print(u"\n\r");
      }
      else {
        print(u"\r\nDone!");
      }
    }
  }
  else if (streq(argv[0], u"ri")) {
    struct efi_file_protocol* file;
    status = (*dir)->open(*dir, &file, argv[1], EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
    if (status != EFI_SUCCESS) {
      print(u"Failed to open file: ");
      print(argv[1]);
      print(u"\n\r");
    }
    else {
      uint32_t width, height;
      struct efi_graphics_output_blt_pixel* buffer;
      status = load_image(file, &buffer, &width, &height);
      if (status != EFI_SUCCESS) {
        print(u"Failed to read image: ");
        print(argv[1]);
        print(u"\n\r");
      }
      else {
        if (argc > 2) {
          uint32_t newwidth = getInt(argv[2]);
          uint32_t newheight = (argc > 3) ? getInt(argv[3]) : newwidth;
          struct efi_graphics_output_blt_pixel* newbuffer;
          print(u"Resizing!");
          status = resize_image(buffer, width, height, &newbuffer, newwidth, newheight);
          print(u"Resized");
          free(buffer);
          print(u"Freed");
          if (status == EFI_SUCCESS) {
            buffer = newbuffer;
            width = newwidth;
            height = newheight;
          }
          else print(u"Failed to resize image");
        }
        graphics_output->blt(graphics_output, buffer, EFI_BLT_BUFFER_TO_VIDEO, 0, 0, 0, 0, width, height, 0);
      }
      free(buffer);
    }
  }
  else if (streq(argv[0], u"cat")) for (efi_uint_t i = 1; i < argc; i++) {
    struct efi_file_protocol* file;
    status = (*dir)->open(*dir, &file, argv[i], EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
    if (status != EFI_SUCCESS) {
      print(u"Failed to open file: ");
      print(argv[i]);
      print(u"\n\r");
    }
    else {
      efi_uint_t size = 256*256;
      uint8_t* buffer = malloc(size);
      status = file->read(file, &size, buffer);
      if (status != EFI_SUCCESS) {
        print(u"Failed to read file: ");
        print(argv[i]);
        print(u"\n\r");
      }
      else {
        print(u"Read File: ");
        print(argv[1]);
        print(u"\n\rFile Size (bytes): ");
        efi_char_t num[4];
        print(itoa(size, num, 10));
        print(u"\n\rFile Data:\n\r");
        for (efi_uint_t c = 0; c < size; c++) {
          efi_char_t ch[2];
          ch[0] = buffer[c];
          ch[1] = u'\0';
          print(ch);
          // Convert to CRLF
          if (ch[0] == u'\n') print(u"\r");

        }
        print(u"\n\r---------[END OF FILE]---------\n\r");
        status = file->close(file);
        if (status != EFI_SUCCESS) print(u"Failed to close file");
      }
      free(buffer);
    }
  }
  else if (streq(argv[0], u"time")) {
    struct efi_time time;
    struct efi_time_capabilities capabilities;
    status = system_table->runtime_services->getTime(&time, &capabilities);
    if (status != EFI_SUCCESS) return status;
    for (int i = 1; i < argc; i++) {
      if (i != 1) {
        status = system_table->output->outputString(system_table->output, u":");
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
        status = system_table->output->outputString(system_table->output, itoa(value, str, 10));
      else
        status = system_table->output->outputString(system_table->output, u"?");
      if (status != EFI_SUCCESS) return status;
    }
  }
  // graphics_output Test Commands
  else if (streq(argv[0], u"gopinfo")) {
    efi_char_t buf[128];
    system_table->output->outputString(system_table->output, u"Mode: ");
    system_table->output->outputString(system_table->output, itoa(graphics_output->mode->mode, buf, 10));
    system_table->output->outputString(system_table->output, u"/");
    system_table->output->outputString(system_table->output, itoa(graphics_output->mode->maxMode, buf, 10));
    system_table->output->outputString(system_table->output, u"\n\rResolution: ");
    system_table->output->outputString(system_table->output, itoa(graphics_output->mode->info->horizontalResolution, buf, 10));
    system_table->output->outputString(system_table->output, u":");
    system_table->output->outputString(system_table->output, itoa(graphics_output->mode->info->verticalResolution, buf, 10));
    system_table->output->outputString(system_table->output, u"\n\rFramebuffer Size: ");
    system_table->output->outputString(system_table->output, itoa(graphics_output->mode->framebufferSize, buf, 10));
  }
  else if (streq(argv[0], u"gopmode")) {
    if (argc > 1) {
      uint32_t mode = getInt(argv[1]);
      if (mode < graphics_output->mode->maxMode) {
        // Set graphics_output Mode
        status = graphics_output->setMode(graphics_output, mode);
        // Failure message
        if (status != EFI_SUCCESS)
          system_table->output->outputString(system_table->output, u"Failed to set GOP Mode!");
        // Success Message
        else
          system_table->output->outputString(system_table->output, u"GOP Mode Set!");
      }
      else {
        system_table->output->outputString(system_table->output, u"Mode must be less than max mode");
      }
    }
    else {
      system_table->output->outputString(system_table->output, u"Invalid Parameter");
    }
  }
  else if (streq(argv[0], u"goptest")) {
    struct efi_graphics_output_blt_pixel pixel;
    pixel.red = getInt(argv[1]);
    pixel.green = getInt(argv[2]);
    pixel.blue = getInt(argv[3]);
    graphics_output->blt(graphics_output, &pixel, EFI_BLT_VIDEO_FILL, 0, 0, 0, 0, 500, 500, 0);
  }
  else if (streq(argv[0], u"textinfo")) {
    efi_char_t buf[128];
    system_table->output->outputString(system_table->output, u"Mode: ");
    system_table->output->outputString(system_table->output, itoa(system_table->output->mode->mode, buf, 10));
    system_table->output->outputString(system_table->output, u"/");
    system_table->output->outputString(system_table->output, itoa(system_table->output->mode->maxMode, buf, 10));
    system_table->output->outputString(system_table->output, u"\n\rAttribute: ");
    system_table->output->outputString(system_table->output, itoa(system_table->output->mode->attribute, buf, 10));
  }
  else if (streq(argv[0], u"textmode")) {
    if (argc > 1) {
      uint32_t mode = getInt(argv[1]);
      if (mode < system_table->output->mode->maxMode) {
        // Set Text Mode
        status = system_table->output->setMode(system_table->output, mode);
        // Failure Message
        if (status != EFI_SUCCESS)
          system_table->output->outputString(system_table->output, u"Failed to set Text Mode!");
        // Success Message
        else
          system_table->output->outputString(system_table->output, u"Text Mode Set!");
      }
      else {
        system_table->output->outputString(system_table->output, u"Mode must be less than max mode");
      }
    }
    else {
      system_table->output->outputString(system_table->output, u"Invalid Parameter");
    }
  }
  else {
    status = system_table->output->outputString(system_table->output, u"Unknown Command");
    if (status != EFI_SUCCESS) return status;
  }

  return EFI_SUCCESS;
}
