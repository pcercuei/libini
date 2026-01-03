
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ini.h>

int main(int argc, char **argv)
{
	struct INI *ini;
	char *key, *value;

	if (argc < 2) {
		printf("USAGE: test [INI_FILE]...\n");
		return EXIT_SUCCESS;
	}

	ini = ini_open(argv[1]);
	if (!ini)
		return EXIT_FAILURE;
	printf("INI file opened.\n");

	while (1) {
		const char *buf;
		char *name;
		size_t name_len;
		int res = ini_next_section(ini, &buf, &name_len);
		if (!res) {
			printf("End of file.\n");
			break;
		}
		if (res < 0) {
			printf("ERROR: code %i\n", res);
			goto error;
		}

		name = malloc(name_len + 1);
		if (!name) goto error;
		name[name_len] = '\0';
		memcpy(name, buf, name_len);
		printf("Opening section: \'%s\'\n", name);
		free(name);

		while (1) {
			const char *buf2;
			size_t key_len, value_len;
			res = ini_read_pair(ini, &buf, &key_len, &buf2, &value_len);
			if (!res) {
				printf("No more data.\n");
				break;
			}
			if (res < 0) {
				printf("ERROR: code %i\n", res);
				goto error;
			}

			key = malloc(key_len + 1);
			if (!key) goto error;
			key[key_len] = '\0';
			memcpy(key, buf, key_len);

			value = malloc(value_len + 1);
			if (!value) goto key_error;
			value[value_len] = '\0';
			memcpy(value, buf2, value_len);

			printf("Reading key: \'%s\' value: \'%s\'\n", key, value);
			free(key);
			key = NULL; /* satisfy gcc's static analyzers */
			free(value);
		}
	}

	ini_close(ini);
	return EXIT_SUCCESS;
key_error:
	free(key);
error:
	ini_close(ini);
	return EXIT_FAILURE;
}
