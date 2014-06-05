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

#include <memory> // for std::auto_ptr

#include <QString>

#include "opengl.h"

/** Container for a GLSL uniform. */
struct Uniform
{
    /** Name as in the shader */
    QString name;
    /** Type of the uniform */
    GLenum type;
    /** Number of instances (for arrays) */
    GLint size;
    /** Uniform location, to send the stuff over */
    GLint location;

    GLfloat floats[4];

    /** Constructor (clears all contents) */
    Uniform();
};


class Glsl
{
public:

    // ---------------- ctor -----------------

    Glsl();

    // ----------- query ---------------------

    /** Return the log from the last compilation */
    const QString& log() const { return log_; }

    /** Is the shader ready to use? */
    bool ready() const { return ready_; }

    /** Returns the number of used uniforms of this shader.
        Can be called after succesful compilation. */
    size_t numUniforms() const { return uniforms_.size(); }

    /** Returns a pointer to a uniform attached to this shader.
        Can be called after succesful compilation.
        @p index must be < numUniforms() */
    Uniform * getUniform(size_t index) { return uniforms_[index].get(); }

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

    /** Activates the shader. Subsequent OpenGL calls will
        be affected by the shader's workings. */
    void activate();

    /** Turns the shader off. */
    void deactivate();

    /** Sets the GPU-value of the uniform to the contents provided by
        the @p uniform parameter.
        @note The shader must be activated. */
    void setUniform(const Uniform * uniform);

private:

    void getUniforms_();

    /** Compiles one of the vertex/fragment shaders and attaches to programObject */
    bool compileShader_(GLenum type, const QString& typeName, const QString& source);

    QString vertSource_,
            fragSource_,
            log_;

    GLenum shader_;

    bool ready_;

    std::vector<std::auto_ptr<Uniform>> uniforms_;
};

#endif // GLSL_H
