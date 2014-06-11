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

    @note On Unix systems, it seems to be okay to just include the
    system's opengl headers, since they normally represent the API version
    that is supported by the system.

    <p>On Windows, however, only OpenGL 1.2 is supported by the system headers.
    It is generally nescessary to query the function entries for higher API
    functions at runtime. Qt does this quite conveniently (no need for libGLEW
    or something like that). But it is nescessary to change the application
    a bit. The SCH_USE_QT_OPENGLFUNC is used for that. Every class that uses
    opengl functionallity must derive from QOpenGLFunctions and call
    initializeOpenGLFunctions() once the opengl context is available.
    There is a isInitialized() function to query for that, but it seems
    it got forgotton to declare (undefined reference error). So my classes
    have their own flags which adds just a little-bit of code noise..</p>

    <p>UPDATE: Well, the only system that does not seems to care about the
    API version is linux. OSX seems to have some issues as well (Thanks
    Martin for pointing out). Probably best to generally use the
    QOpenGLFunctions wrapper. TODO for next release...</p>

*/
#ifndef OPENGL_H
#define OPENGL_H

#include <QtCore> // for os flags

#ifdef Q_OS_OSX

#   include <gl.h>
#   include <glext.h>

#elif defined(Q_OS_LINUX)

    // enable openGL extensions
#   define GL_GLEXT_PROTOTYPES
#   include <GL/gl.h>
#   include <GL/glext.h>

#elif defined(Q_OS_WIN)

#   include <QOpenGLFunctions_3_3_Core>
#   define SCH_USE_QT_OPENGLFUNC

#endif


/** Maximum number of texture slots */
#define SCH_MAX_TEXTURES 4

/** Exchange of common vertex attribute and uniform locations.
    @note This is totally specific to this application. The
    code names of these are defined in AppSettings.
*/
struct ShaderLocations
{
    // vertex attributes
    GLuint
        position,
        normal,
        color;
    // uniforms
    GLuint
        projection,
        view,
        time,
        aspect;
};


#endif // OPENGL_H
