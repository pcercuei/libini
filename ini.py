from ctypes import POINTER, Structure, cdll, c_char_p, c_int, c_uint, byref
from sys import argv

def _checkOpen(result, func, arguments):
	if result:
		return result
	else:
		raise IOError("Failed to open INI file: '%s'" % arguments[0])

def _checkRead(result, func, arguments):
	if result == -1:
		raise SyntaxError("Error occured while parsing INI file")
	return result

def _init():
	class _INI(Structure):
		pass
	IniPtr = POINTER(_INI)

	lib = cdll.LoadLibrary('libini.so.0')

	ini_open = lib.ini_open
	ini_open.restype = IniPtr
	ini_open.archtypes = (c_char_p, )
	ini_open.errcheck = _checkOpen
	global _ini_open
	_ini_open = ini_open

	ini_close = lib.ini_close
	ini_close.restype = None
	ini_close.archtypes = (IniPtr, )
	global _ini_close
	_ini_close = ini_close

	ini_next_section = lib.ini_next_section
	ini_next_section.restype = c_int
	ini_next_section.archtypes = (IniPtr, c_char_p)
	ini_next_section.errcheck = _checkRead
	global _ini_next_section
	_ini_next_section = ini_next_section

	ini_read_pair = lib.ini_read_pair
	ini_read_pair.restype = c_int
	ini_read_pair.archtypes = (IniPtr, c_char_p, c_char_p)
	ini_read_pair.errcheck = _checkRead
	global _ini_read_pair
	_ini_read_pair = ini_read_pair

_init()

class INI(object):

	def __init__(self, path):
		self._ini = _ini_open(path)
	
	def __del__(self):
		_ini_close(self._ini)

	def next_section(self):
		s = c_char_p()
		res = _ini_next_section(self._ini, byref(s))
		if res == 1:
			return s.value

	def read_pair(self):
		key = c_char_p()
		val = c_char_p()
		res = _ini_read_pair(self._ini, byref(key), byref(val))
		if res == 1:
			return (key.value, val.value)
		return ((),())

def main():
	if len(argv) != 2:
		print "Usage: ini.py [INI_FILE]..."
		return
	ini = INI(argv[1])

	while True:
		name = ini.next_section()
		if not name:
			print 'End.'
			break

		print 'In section: ' + name
		while True:
			key, value = ini.read_pair()
			if not key:
				print 'End of section.'
				break
			print 'Reading key: ' + key + ' value: ' + value

if __name__ == '__main__':
	main()
