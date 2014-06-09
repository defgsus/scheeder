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

#include <QDebug>
#include <QStringList>

#include "glsl.h"
#include "debug.h"
#include "appsettings.h"

void privateUniformDeleter(Uniform * u) { delete u; }


Uniform::Uniform()
    :   type_    (0),
        size_    (0),
        location_(0)
{
    floats[0] = floats[1] = floats[2] = floats[3] = 0.f;
}

void Uniform::copyValuesFrom_(Uniform * u)
{
    floats[0] = u->floats[0];
    floats[1] = u->floats[1];
    floats[2] = u->floats[2];
    floats[3] = u->floats[3];
}


Glsl::Glsl()
    :   shader_         (-1),
        sourceChanged_  (false),
        ready_          (false),
        activated_      (false)
#ifdef SCH_USE_QT_OPENGLFUNC
        ,isGlFuncInitialized_(false)
#endif
{
}


void Glsl::setVertexSource(const QString &text)
{
    vertSource_ = text;
    sourceChanged_ = true;
}

void Glsl::setFragmentSource(const QString &text)
{
    fragSource_ = text;
    sourceChanged_ = true;
}


bool Glsl::compile()
{
    // temporarily save uniforms
//    auto unicopy = uniforms_;

#ifdef SCH_USE_QT_OPENGLFUNC
    if (!isGlFuncInitialized_)
    {
        initializeOpenGLFunctions();
        isGlFuncInitialized_ = true;
    }
#endif

    // init state
    ready_ = false;
    sourceChanged_ = false;
    log_ = "";
    uniforms_.clear();

    // delete previous shader object
    SCH_CHECK_GL( if (glIsProgram(shader_)) glDeleteProgram(shader_) );

    // create shader object
    SCH_CHECK_GL( shader_ = glCreateProgram() );

    // test if working
    if (!glIsProgram(shader_))
    {
        log_ += "could not create ProgramObject\n";
        return false;
    }

    // compile the vertex shader
    if (!compileShader_(GL_VERTEX_SHADER, "vertex shader", vertSource_))
    {
        return false;
    }

    // compile the fragment shader
    if (!compileShader_(GL_FRAGMENT_SHADER, "fragment shader", fragSource_))
        return false;

    // link program object
    SCH_CHECK_GL( glLinkProgram(shader_) );

    GLint linked;
    SCH_CHECK_GL( glGetProgramiv(shader_, GL_LINK_STATUS, &linked) );
    if (!linked)
    {
        log_ += "shader programm link error\n";
        shader_ = -1;
        return false;
    }

    getSpecialLocations_();
    getUniforms_();

    // keep copy of previous uniforms
    oldUniforms_ = uniforms_;

    return ready_ = true;
}


bool Glsl::compileShader_(GLenum type, const QString& typeName, const QString &source)
{
    if (source.isEmpty())
        return false;

    int shadername;
    SCH_CHECK_GL( shadername = glCreateShader(type) );
    if (!glIsShader(shadername))
    {
        log_ += "error creating " + typeName + " ShaderObject\n";
        return false;
    }

    // get the latin1 char source
    // NOTE: QString::toStdString() is a temporary, we can't just
    // take the pointer to it because the pointet-at memory might
    // have been deallocated already.
    // This is the totally safe way.
    // (unless the driver tries a buffer overflow attack :)
    std::vector<GLchar> src(source.size()+1);
    memcpy(&src[0], source.toStdString().c_str(), source.size());
    const GLchar * psrc[1];
    psrc[0] = &src[0];

    // attach source
    SCH_CHECK_GL( glShaderSource(shadername, 1, psrc, 0) );
    // compile
    SCH_CHECK_GL( glCompileShader(shadername) );

    // check compile status
    bool compiled = false;
    GLint cc;
    SCH_CHECK_GL( glGetShaderiv(shadername, GL_COMPILE_STATUS, &cc); )
    if (!cc)
    {
        log_ += typeName + " compile ERROR\n";
    }
    else
    {
        log_ += typeName + " compiled..\n";
        compiled = true;
    }

    // print compiler log
    GLint blen = 0;
    GLsizei slen = 0;
    SCH_CHECK_GL( glGetShaderiv(shadername, GL_INFO_LOG_LENGTH , &blen) );
    if (blen > 1)
    {
        std::vector<GLchar> compiler_log(blen+1);
        SCH_CHECK_GL( glGetShaderInfoLog(shadername, blen, &slen, &compiler_log[0]) );
        log_ += "compiler log:\n" + QString(&compiler_log[0]) + "\n";
        // error_line_(compiler_log, code));
    }

    // attach to programObject
    SCH_CHECK_GL( glAttachShader(shader_, shadername) );

    return compiled;
}


void Glsl::activate()
{
    if (!ready())
        return;

    SCH_CHECK_GL( glUseProgram(shader_) );
    activated_ = true;
}

void Glsl::deactivate()
{
    // TODO:
    // On OSX this gives GL_INVALID_OPERATION
    // although the spec says that's the way
    SCH_CHECK_GL( glUseProgram(0) );
    activated_ = false;
}

void Glsl::getSpecialLocations_()
{
    SCH_CHECK_GL( attribs_.position = glGetAttribLocation(shader_,
                    appSettings->getValue("ShaderAttributes/position").toString().toStdString().c_str()) );
    SCH_CHECK_GL( attribs_.normal = glGetAttribLocation(shader_,
                    appSettings->getValue("ShaderAttributes/normal").toString().toStdString().c_str()) );
    SCH_CHECK_GL( attribs_.color = glGetAttribLocation(shader_,
                    appSettings->getValue("ShaderAttributes/color").toString().toStdString().c_str()) );
    SCH_CHECK_GL( attribs_.projection = glGetUniformLocation(shader_,
                    appSettings->getValue("ShaderUniforms/projection").toString().toStdString().c_str()) );
    SCH_CHECK_GL( attribs_.view = glGetUniformLocation(shader_,
                    appSettings->getValue("ShaderUniforms/view").toString().toStdString().c_str()) );
    SCH_CHECK_GL( attribs_.time = glGetUniformLocation(shader_,
                    appSettings->getValue("ShaderUniforms/time").toString().toStdString().c_str()) );
    SCH_CHECK_GL( attribs_.aspect = glGetUniformLocation(shader_,
                    appSettings->getValue("ShaderUniforms/aspect").toString().toStdString().c_str()) );
}

void Glsl::getUniforms_()
{
    // get number of used uniforms
    GLint numu;
    SCH_CHECK_GL( glGetProgramiv(shader_, GL_ACTIVE_UNIFORMS, &numu) );

    // get max length of variable names
    GLint labelLength;
#ifndef GL_MAX_LABEL_LENGTH
    labelLength = 256;
#else
    SCH_CHECK_GL( glGetIntegerv(GL_MAX_LABEL_LENGTH, &labelLength) );
#endif

    // don't expose these to user
    QStringList specialUniforms = appSettings->getShaderUniforms();

    // get each uniform data
    for (int i=0; i<numu; ++i)
    {
        Uniform * u = new Uniform;

        u->location_ = i;

        // plain old char* strings always need a bit of extra code ..
        GLsizei length;
        std::vector<GLchar> name(labelLength);
        SCH_CHECK_GL(
            glGetActiveUniform(shader_, i, name.size(), &length, &u->size_, &u->type_, &name[0])
            );
        name.resize(length);
        u->name_ = QString(&name[0]);

        // check for special uniforms
        if (specialUniforms.contains(u->name_))
        {
            delete u;
            continue;
        }

        // keep in list
        uniforms_.push_back(std::shared_ptr<Uniform>(u, privateUniformDeleter));

        // see if we have values from previous uniforms
        for (auto j = oldUniforms_.begin(); j!=oldUniforms_.end(); ++j)
        if (j->get()->name_ == u->name_ && j->get()->type_ == u->type_)
        {
            u->copyValuesFrom_(j->get());
            break;
        }
    }
}

void Glsl::sendUniform(const Uniform * u)
{
    switch (u->type())
    {
    case GL_FLOAT:
        SCH_CHECK_GL( glUniform1f(u->location_, u->floats[0]) );
    break;
    case GL_FLOAT_VEC2:
        SCH_CHECK_GL( glUniform2f(u->location_, u->floats[0], u->floats[1]) );
    break;
    case GL_FLOAT_VEC3:
        SCH_CHECK_GL( glUniform3f(u->location_, u->floats[0], u->floats[1], u->floats[2]) );
    break;
    case GL_FLOAT_VEC4:
        SCH_CHECK_GL( glUniform4f(u->location_, u->floats[0], u->floats[1], u->floats[2], u->floats[3]) );
    break;
//    default:
//        qDebug() << "unsupported uniform type" << u->type_;
    }
}

void Glsl::sendUniforms()
{
    for (size_t i=0; i<numUniforms(); ++i)
        sendUniform(getUniform(i));
}

void Glsl::releaseGL()
{
    SCH_CHECK_GL( glDeleteProgram(shader_) );
    ready_ = activated_ = false;
}
