/* Copyright, 2010 Tux Target
 * Copyright, 2003 Melting Pot
 *
 * This file is part of Tux Target.
 * Tux Target is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.

 * Tux Target is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with Tux Target; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 */

#include <nel/misc/types_nl.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <list>
#include <deque>
#include <string>
#include <vector>
#include <sstream>
#include <utility>
#include <algorithm>
#include <exception>

#ifdef NL_OS_WINDOWS
#	include <winsock2.h>
#	undef min
#	undef max
#elif defined NL_OS_UNIX
#	include <unistd.h>
#	include <sys/time.h>
#	include <sys/types.h>
#endif

#include <nel/misc/path.h>
#include <nel/misc/debug.h>
#include <nel/misc/vector.h>
#include <nel/misc/matrix.h>
#include <nel/misc/common.h>
#include <nel/misc/command.h>
#include <nel/misc/fast_mem.h>
#include <nel/misc/variable.h>
#include <nel/misc/singleton.h>
#include <nel/misc/system_info.h>
#include <nel/misc/config_file.h>
#include <nel/misc/mem_displayer.h>

#include <nel/net/service.h>
