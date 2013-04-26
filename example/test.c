
#include <stdio.h>
#include <stdlib.h>

#include <ini.h>

int main(int argc, char **argv)
{
	struct INI *ini;
	const char *name;
	const char *key, *value;

	if (argc < 2) {
		printf("USAGE: test [INI_FILE]...\n");
		return EXIT_SUCCESS;
	}

	ini = ini_open(argv[1]);
	if (!ini)
		return EXIT_FAILURE;
	printf("INI file opened.\n");

	while (1) {
		int res = ini_next_section(ini, &name);
		if (!res) {
			printf("End of file.\n");
			break;
		}
		if (res == -1) {
			printf("ERROR: code %i\n", res);
			goto error;
		}

		printf("Opening section: \'%s\'\n", name);

		while (1) {
			res = ini_read_pair(ini, &key, &value);
			if (!res) {
				printf("No more data.\n");
				break;
			}
			if (res == -1) {
				printf("ERROR: code %i\n", res);
				goto error;
			}
			printf("Reading key: \'%s\' value: \'%s\'\n", key, value);
		}
	}

	ini_close(ini);
	return EXIT_SUCCESS;

error:
	ini_close(ini);
	return EXIT_FAILURE;
}
