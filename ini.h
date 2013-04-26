#ifndef __INI_H
#define __INI_H

#include <stdlib.h>

struct INI;

struct INI *ini_open(const char *file);
struct INI *ini_open_mem(char *buf, size_t len);

void ini_close(struct INI *ini);

/* Jump to the next section.
 * if 'name' is set, the pointer passed as argument
 * points to the name of the section.
 * XXX: the pointer will be invalid as soon as ini_close() is called.
 *
 * Returns:
 * 	-1 if an error occured,
 * 	EOF if no more section can be found,
 * 	0 otherwise.
 */
int ini_open_section(struct INI *ini, const char **name);

/* Read a key/value pair.
 * 'key' and 'value' must be valid pointers. The pointers passed as arguments
 * will point to the key and value read.
 * XXX: the pointers will be invalid as soon as ini_close() is called.
 *
 * Returns:
 *  -1 if an error occured,
 *  EOF if no more key/value pairs can be found,
 *  0 otherwise.
 */
int ini_read_key_value(struct INI *ini, const char **key, const char **value);

#endif
