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

#include <memory> // for std::shared_ptr

#include <QString>

#include "opengl.h"


/** Container for a GLSL uniform. */
struct Uniform
{
    // ------- public member ---------

    /** A vector of floats,
        for types GL_FLOAT, GL_FLOAT_VEC2, GL_FLOAT_VEC3 and GL_FLOAT_VEC4. */
    GLfloat floats[4];

    /** Name as in the shader source */
    const QString& name() const { return name_; }
    /** Type of the uniform (as OpenGL enum) */
    GLenum type() const { return type_; }
    /** Number of instances (for arrays) */
    GLint size() const { return size_; }
    /** Uniform location, to send the stuff over */
    GLint location() const { return location_; }

    friend class Glsl;
    friend void privateUniformDeleter(Uniform*);

    // ----------- private area -----------
private:

    /** Constructor (initializes all to zero) */
    Uniform();
    /** Private destructor to avoid stupid things. */
    ~Uniform() { }

    void copyValuesFrom_(Uniform*);

    QString name_;
    GLenum type_;
    GLint size_;
    GLint location_;

};


class Glsl
{
public:

    // ---------------- ctor -----------------

    Glsl();

    // ----------- query ---------------------

    /** Return the log from the last compilation */
    const QString& log() const { return log_; }

    /** Has the source changed and shader needs recompilation? */
    bool sourceChanged() const { return sourceChanged_; }

    /** Is the shader ready to use? */
    bool ready() const { return ready_; }

    /** Returns if the shader has been activated.
        @note If, after activation, activate() or deactivate() is called on a
        different shader, this value will not reflect the GPU state! */
    bool activated() const { return activated_; }

    /** Returns the number of used uniforms of this shader.
        Can be called after succesful compilation. */
    size_t numUniforms() const { return uniforms_.size(); }

    /** Returns a pointer to a uniform attached to this shader.
        The public members of the Uniform struct can be manipulated.
        Can be called after succesful compilation.
        @p index must be < numUniforms() */
    Uniform * getUniform(size_t index) { return uniforms_[index].get(); }

    GLint positionLocation() const { return attribPosition_; }
    GLint normalLocation() const { return attribNormal_; }
    GLint colorLocation() const { return attribColor_; }

    // ---------- source/compiler ------------

    /** Sets the source for the vertex shader. Previous content will be overwritten. */
    void setVertexSource(const QString& text);

    /** Sets the source for the fragment shader. Previous content will be overwritten. */
    void setFragmentSource(const QString& text);

    /** Tries to compile the shader.
        Any previous program will be destroyed but the values of uniforms are kept.
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
    void sendUniform(const Uniform * uniform);

    /** Sends all uniform values to the GPU.
        @note The shader must be activated. */
    void sendUniforms();

private:

    /** Gets the standardized attributes. */
    void getAttributes_();

    /** Gets all used uniforms and populates the uniforms_ list */
    void getUniforms_();

    /** Compiles one of the vertex/fragment shaders and attaches to current programObject */
    bool compileShader_(GLenum type, const QString& typeName, const QString& source);

    QString vertSource_,
            fragSource_,
            log_;

    GLenum shader_;

    bool sourceChanged_, ready_, activated_;

    std::vector<std::shared_ptr<Uniform>>
        uniforms_,
        oldUniforms_;

    // --- attributes ---

    GLint attribPosition_,
          attribNormal_,
          attribColor_,
          attribProjection_,
          attribTransformation_;

    QString
          attribNamePosition_,
          attribNameNormal_,
          attribNameColor_,
          attribNameProjection_,
          attribNameTransformation_;
};

#endif // GLSL_H
