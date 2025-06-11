#include <stdio.h>
#include <stdint.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main(int argc, char* argv[]) {
  if (argc != 3) {
    puts("usage: roxusimg [inputfile] [outputfile]");
    return EXIT_FAILURE;
  }

  int width, height, ok, channels;
  ok = stbi_info(argv[1], &width, &height, &channels);
  if (!ok) {
    puts("Unsupported format");
    return EXIT_FAILURE;
  }
  uint8_t* buffer = stbi_load(argv[1], &width, &height, &channels, 4);
  uint32_t size[2] = { width, height };
  uint64_t pixels = width*height;

  FILE* output = fopen(argv[2], "w");
  fwrite(size, sizeof(uint32_t[2]), 1, output);
  fwrite(buffer, sizeof(uint8_t[4]), pixels, output);
  fclose(output);

  return EXIT_SUCCESS;
}
