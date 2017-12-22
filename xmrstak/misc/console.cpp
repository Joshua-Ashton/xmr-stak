/*
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
  *
  * Additional permission under GNU GPL version 3 section 7
  *
  * If you modify this Program, or any covered work, by linking or combining
  * it with OpenSSL (or a modified version of that library), containing parts
  * covered by the terms of OpenSSL License and SSLeay License, the licensors
  * of this Program grant you additional permission to convey the resulting work.
  *
  */

#include "xmrstak/misc/console.hpp"

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>

int get_key()
{
	return 0;
}

void set_colour(out_colours cl)
{
	
}

void reset_colour()
{

}

#else
#include <termios.h>
#include <unistd.h>
#include <stdio.h>

int get_key()
{
}

void set_colour(out_colours cl)
{
}

void reset_colour()
{
}
#endif // _WIN32

inline void comp_localtime(const time_t* ctime, tm* stime)
{
#ifdef _WIN32
	localtime_s(stime, ctime);
#else
	localtime_r(ctime, stime);
#endif // __WIN32
}

printer::printer()
{
	verbose_level = LINF;
	logfile = nullptr;
	b_flush_stdout = false;
}

bool printer::open_logfile(const char* file)
{
	return false;
}

void printer::print_msg(verbosity verbose, const char* fmt, ...)
{
}

void printer::print_str(const char* str)
{
}

//Do a press any key for the windows folk. *insert any key joke here*
#ifdef _WIN32
void win_exit(size_t code)
{
	size_t envSize = 0;
	getenv_s(&envSize, nullptr, 0, "XMRSTAK_NOWAIT");
	if(envSize == 0)
	{
		printer::inst()->print_str("Press any key to exit.");
		get_key();
	}
	std::exit(code);
}

#else
void win_exit(size_t code) 
{ 
	std::exit(code);
}
#endif // _WIN32
