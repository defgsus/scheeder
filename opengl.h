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
/** @file
    @brief system-independent wrapper for opengl headers.
*/
#ifndef OPENGL_H
#define OPENGL_H


#ifdef __APPLE__

#   include <gl.h>
#   include <glext.h>

#else

    // enable openGL extensions
#   define GL_GLEXT_PROTOTYPES
#   include <GL/gl.h>
#   include <GL/glext.h>

#endif


/** Exchange of common vertex attribute locations.
    @note This is totally specific to this application
*/
struct ShaderAttributeLocations
{
    // vertex attributes
    GLuint
        position,
        normal,
        color;
    // uniforms
    GLuint
        projection,
        view;
};


#endif // OPENGL_H
