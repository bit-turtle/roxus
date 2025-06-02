// A Blue Screen of Death (Prints EFI error codes)

#include "bsod.h"

efi_status_t bsod(struct efi_system_table* system, efi_status_t error) {
  // Set Screen Color to Blue
  efi_status_t status;
  status |= system->output->setAttribute(system->output, EFI_WHITE | EFI_BACKGROUND_BLUE);
  status |= system->output->clearScreen(system->output);
  system->output->outputString(system->output, u"The Blue Screen of Death!\n\r");
  if (status != EFI_SUCCESS) system->output->outputString(system->output, u"(Failed To Set Console State)\n\r");
  system->output->outputString(system->output, u"\nStatus: ");
  // Display status of error
  switch (error) {
    // Success Codes
    case EFI_SUCCESS:
      system->output->outputString(system->output, u"EFI_SUCCESS");
      break;
    // Error Codes
    case EFI_LOAD_ERROR:
      system->output->outputString(system->output, u"EFI_LOAD_ERROR");
      break;
    case EFI_INVALID_PARAMETER:
      system->output->outputString(system->output, u"EFI_INVALID_PARAMETER");
      break;
    case EFI_UNSUPPORTED:
      system->output->outputString(system->output, u"EFI_UNSUPPORTED");
      break;
    case EFI_BAD_BUFFER_SIZE:
      system->output->outputString(system->output, u"EFI_BAD_BUFFER_SIZE");
      break;
    case EFI_BUFFER_TOO_SMALL:
      system->output->outputString(system->output, u"EFI_BUFFER_TOO_SMALL");
      break;
    case EFI_NOT_READY:
      system->output->outputString(system->output, u"EFI_NOT_READY");
      break;
    case EFI_DEVICE_ERROR:
      system->output->outputString(system->output, u"EFI_DEVICE_ERROR");
      break;
    case EFI_WRITE_PROTECTED:
      system->output->outputString(system->output, u"EFI_WRITE_PROTECTED");
      break;
    case EFI_OUT_OF_RESOURCES:
      system->output->outputString(system->output, u"EFI_OUT_OF_RESOURCES");
      break;
    case EFI_VOLUME_CORRUPTED:
      system->output->outputString(system->output, u"EFI_VOLUME_CORRUPTED");
      break;
    case EFI_VOLUME_FULL:
      system->output->outputString(system->output, u"EFI_VOLUME_FULL");
      break;
    case EFI_NO_MEDIA:
      system->output->outputString(system->output, u"EFI_NO_MEDIA");
      break;
    case EFI_MEDIA_CHANGED:
      system->output->outputString(system->output, u"EFI_MEDIA_CHANGED");
      break;
    case EFI_NOT_FOUND:
      system->output->outputString(system->output, u"EFI_NOT_FOUND");
      break;
    case EFI_ACCESS_DENIED:
      system->output->outputString(system->output, u"EFI_ACCESS_DENIED");
      break;
    case EFI_NO_RESPONSE:
      system->output->outputString(system->output, u"EFI_NO_RESPONSE");
      break;
    case EFI_NO_MAPPING:
      system->output->outputString(system->output, u"EFI_NO_MAPPING");
      break;
    case EFI_TIMEOUT:
      system->output->outputString(system->output, u"EFI_TIMEOUT");
      break;
    case EFI_NOT_STARTED:
      system->output->outputString(system->output, u"EFI_NOT_STARTED");
      break;
    case EFI_ALREADY_STARTED:
      system->output->outputString(system->output, u"EFI_ALREADY_STARTED");
      break;
    case EFI_ABORTED:
      system->output->outputString(system->output, u"EFI_ABORTED");
      break;
    case EFI_ICMP_ERROR:
      system->output->outputString(system->output, u"EFI_ICMP_ERROR");
      break;
    case EFI_TFTP_ERROR:
      system->output->outputString(system->output, u"EFI_TFTP_ERROR");
      break;
    case EFI_PROTOCOL_ERROR:
      system->output->outputString(system->output, u"EFI_PROTOCOL_ERROR");
      break;
    case EFI_INCOMPATIBLE_VERSION:
      system->output->outputString(system->output, u"EFI_INCOMPATIBLE_VERSION");
      break;
    case EFI_SECURITY_VIOLATION:
      system->output->outputString(system->output, u"EFI_SECURITY_VIOLATION");
      break;
    case EFI_CRC_ERROR:
      system->output->outputString(system->output, u"EFI_CRC_ERROR");
      break;
    case EFI_END_OF_MEDIA:
      system->output->outputString(system->output, u"EFI_END_OF_MEDIA");
      break;
    case EFI_END_OF_FILE:
      system->output->outputString(system->output, u"EFI_END_OF_FILE");
      break;
    case EFI_INVALID_LANGUAGE:
      system->output->outputString(system->output, u"EFI_INVALID_LANGUAGE");
      break;
    case EFI_COMPROMISED_DATA:
      system->output->outputString(system->output, u"EFI_COMPROMISED_DATA");
      break;
    case EFI_IP_ADDRESS_CONFLICT:
      system->output->outputString(system->output, u"EFI_IP_ADDRESS_CONFLICT");
      break;
    case EFI_HTTP_ERROR:
      system->output->outputString(system->output, u"EFI_HTTP_ERROR");
      break;
    // Warning Codes
    case EFI_WARN_UNKNOWN_GLYPH:
      system->output->outputString(system->output, u"EFI_WARN_UNKNOWN_GLYPH");
      break;
    case EFI_WARN_DELETE_FAILURE:
      system->output->outputString(system->output, u"EFI_WARN_DELETE_FAILURE");
      break;
    case EFI_WARN_WRITE_FAILURE:
      system->output->outputString(system->output, u"EFI_WARN_WRITE_FAILURE");
      break;
    case EFI_WARN_BUFFER_TOO_SMALL:
      system->output->outputString(system->output, u"EFI_WARN_BUFFER_TOO_SMALL");
      break;
    case EFI_WARN_STALE_DATA:
      system->output->outputString(system->output, u"EFI_WARN_STALE_DATA");
      break;
    case EFI_WARN_FILE_SYSTEM:
      system->output->outputString(system->output, u"EFI_WARN_FILE_SYSTEM");
      break;
    case EFI_WARN_RESET_REQUIRED:
      system->output->outputString(system->output, u"EFI_WARN_RESET_REQUIRED");
      break;
    // Roxus Errors
    case ROXUS_END:
      system->output->outputString(system->output, u"ROXUS_END");
      break;
    default:
      if (error & EFI_ERROR_BIT)
        system->output->outputString(system->output, u"(Unknown Error)");
      else
        system->output->outputString(system->output, u"(Unknown Warning)");
  }
  system->output->outputString(system->output, u"\n\n\rPress any key to continue...");
  struct efi_input_key key;
  do {
    status = system->input->readKey(system->input, &key);
  }
  while (status != EFI_SUCCESS);
  system->output->setAttribute(system->output, EFI_WHITE | EFI_BACKGROUND_BLACK);
  system->output->clearScreen(system->output);
  return error;
}
