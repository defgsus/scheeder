/***************************************************************************

Copyright (C) 2014  stefan.berke @ modular-audio-graphics.com

This source is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either
version 3.0 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this software; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

****************************************************************************/

#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>
#include "opengl.h"

/** Returns the readable name for an opengl error */
const char * glErrorName(GLenum error);

/** Executes the command and calls glGetError() and
    prints the error, if any. */
#define SCH_CHECK_GL(command__)             \
{                                           \
    command__;                              \
    if (GLenum err__ = glGetError())        \
    {                                       \
        std::cerr << "opengl error "        \
            << glErrorName(err__)           \
            << " for command "              \
            << #command__                   \
            << " in " << __FILE__           \
            << ": " << __LINE__ << "\n";    \
    }                                       \
}

#endif // DEBUG_H
