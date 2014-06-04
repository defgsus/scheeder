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

#ifndef GLSL_H
#define GLSL_H

#include <QString>

#include "opengl.h"
#include "vector.h"

class Glsl
{
public:
    Glsl();

    // ----------- query ---------------------

    /** Return the log from the last compilation */
    const QString& log() const { return log_; }

    /** Is the shader ready to use? */
    bool ready() const { return ready_; }

    // ---------- source/compiler ------------

    /** Sets the source for the vertex shader. Previous content will be overwritten. */
    void setVertexSource(const QString& text);

    /** Sets the source for the fragment shader. Previous content will be overwritten. */
    void setFragmentSource(const QString& text);

    /** Tries to compile the shader.
        Any previous program will be destroyed.
        @returns true on success, also sets ready() to true. */
    bool compile();

    // ------------ usage --------------------

    void activate();

    void deactivate();

private:

    /** Compiles one of the vertex/fragment shaders and attaches to programObject */
    bool compileShader_(GLenum type, const QString& typeName, const QString& source);

    QString vertSource_,
            fragSource_,
            log_;

    GLenum shader_;

    bool ready_;
};

#endif // GLSL_H
