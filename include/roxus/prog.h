// Program Interface

#include <roxus/ring.h>
#include <utf8/utf8.h>
#include <efi/types.h>
#include <stdbool.h>

struct program_interface {
	struct ring input;
	struct ring output;
	int return_val;
	int argc;
	char *argv[];
};
