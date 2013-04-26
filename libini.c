
#include <stdbool.h>
#include <stdio.h>

#include "ini.h"

struct INI {
	char *buf, *end, *curr;
	bool free_buf_on_exit;
};

static struct INI *_ini_open_mem(char *buf, size_t len, bool free_buf_on_exit)
{
	struct INI *ini = malloc(sizeof(*ini));
	if (!ini) {
		perror("Unable to allocate memory");
		return NULL;
	}

	ini->buf = ini->curr = buf;
	ini->end = buf + len;
	ini->free_buf_on_exit = free_buf_on_exit;
	return ini;
}

struct INI *ini_open_mem(char *buf, size_t len)
{
	return _ini_open_mem(buf, len, false);
}

struct INI *ini_open(const char *file)
{
	FILE *f;
	char *buf;
	size_t len, pos = 0;
	struct INI *ini = NULL;
	
	f = fopen(file, "r");
	if (!f) {
		perror("Unable to open file");
		return NULL;
	}

	fseek(f, 0, SEEK_END);
	len = ftell(f);

	if (!len) {
		fprintf(stderr, "ERROR: File is empty\n");
		goto error_fclose;
	}

	buf = malloc(len);
	if (!buf) {
		perror("Unable to allocate memory");
		goto error_fclose;
	}

	rewind(f);

	do {
		pos += fread(buf + pos, 1, len - pos, f);
	} while (pos < len);

	ini = _ini_open_mem(buf, len, true);

error_fclose:
	fclose(f);
	return ini;
}

void ini_close(struct INI *ini)
{
	if (ini->free_buf_on_exit)
		free(ini->buf);
	free(ini);
}

static bool skip_comments(struct INI *ini)
{
	char *curr = ini->curr;
	char *end = ini->end;

	while (curr != end) {
		if (*curr == '\n')
			curr++;
		else if (*curr == '#')
			do { curr++; } while (curr != end && *curr != '\n');
		else
			break;
	}

	ini->curr = curr;
	return curr == end;
}

static bool skip_line(struct INI *ini)
{
	char *curr = ini->curr;
	char *end = ini->end;

	for (; curr != end && *curr != '\n'; curr++);
	if (curr == end) {
		ini->curr = end;
		return true;
	} else {
		ini->curr = curr + 1;
		return false;
	}
}

int ini_open_section(struct INI *ini, const char **name)
{
	const char *_name;
	if (ini->curr == ini->end)
		return EOF; /* EOF: no more sections */

	if (ini->curr == ini->buf) {
		if (skip_comments(ini) || *ini->curr != '[') {
			fprintf(stderr, "Malformed INI file (missing section header)\n");
			return -1;
		}
	} else while (*ini->curr != '[' && !skip_line(ini));

	if (ini->curr == ini->end)
		return EOF; /* EOF: no more sections */

	_name = ++ini->curr;
	do {
		ini->curr++;
		if (ini->curr == ini->end || *ini->curr == '\n') {
			fprintf(stderr, "Malformed INI file (malformed section header)\n");
			return -1;
		}
	} while (*ini->curr != ']');

	*ini->curr++ = '\0';
	if (name)
		*name = _name;
	return 0;
}

int ini_read_key_value(struct INI *ini, const char **key, const char **value)
{
	char *_key, *_value;
	char *curr, *end = ini->end;

	if (skip_comments(ini))
		return EOF;
	curr = _key = ini->curr;

	if (*curr == '[')
		return EOF;

	while (true) {
		curr++;

		if (curr == end || *curr == '\n') {
			fprintf(stderr, "ERROR: Unexpected end of line\n");
			return -1;

		} else if (*curr == '=') {
			*curr++ = '\0';
			break;

		} else if (*curr <= ' ')
			*curr = '\0';
	}

	/* Skip whitespaces. */
	while (curr != end && (*curr == ' ' || *curr == '\t')) curr++;
	if (curr == end) {
		fprintf(stderr, "ERROR: Unexpected end of line\n");
		return -1;
	}

	_value = curr++;

	while (curr != end && *curr != '\n') curr++;
	if (curr == end) {
		fprintf(stderr, "ERROR: Unexpected end of line\n");
		return -1;
	}

	*curr++ = '\0';
	ini->curr = curr;
	*key = _key;
	*value = _value;
	return 0;
}
