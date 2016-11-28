#ifndef _WDGS_DEBUG_H
#define _WDGS_DEBUG_H

#include <iostream>

#ifdef WIN32
#include <Windows.h>
#endif

namespace WDGS
{
	struct dbgstream : public std::ostream, public std::streambuf
	{
		dbgstream() : std::ostream(this) {}

#ifdef WIN32
		std::streambuf::int_type sputc(std::streambuf::char_type ch)
		{
			static char str[2] = { 0 };
			str[0] = ch;
			OutputDebugStringA(str);
			return ch;
		}

		virtual std::streamsize xsputn(const std::streambuf::char_type *ptr, std::streamsize count)
		{
			std::string str(ptr, (size_t)count);
			OutputDebugStringA(str.c_str());
			return count;
		}

		virtual std::streambuf::int_type overflow(std::streambuf::int_type c)
		{
			static char str[2] = { 0 };
			str[0] = c;
			OutputDebugStringA(str);
			return c;
		}

#else

		virtual std::streambuf::int_type overflow(std::streambuf::int_type c)
		{
			std::clog.put(c);
			return c;
		}

#endif

	};

	extern dbgstream cdbg;
}

#endif
