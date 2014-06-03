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

#include "glsl.h"
#include "debug.h"

Glsl::Glsl()
{
}


void Glsl::setVertexSource(const QString &text)
{
    vertSource_ = text;
}

void Glsl::setFragmentSource(const QString &text)
{
    fragSource_ = text;
}


bool Glsl::compile()
{
    // delete previous shader object
    SCH_CHECK_GL( if (glIsProgram(shader_)) glDeleteProgram(shader_) );

    log_ = "";

    // create shader object
    SCH_CHECK_GL( shader_ = glCreateProgramObjectARB() );

    // test if working
    if (!glIsProgram(shader_))
    {
        log_ += "could not create ProgramObject\n";
        return false;
    }

    // compile the vertex shader
    if (!compileShader_(GL_VERTEX_SHADER_ARB, "vertex shader", vertSource_))
        return false;

    // compile the fragment shader
    if (!compileShader_(GL_FRAGMENT_SHADER_ARB, "fragment shader", vertSource_))
        return false;

    // link program object
    SCH_CHECK_GL( glLinkProgramARB(shader_) );

    GLint linked;
    SCH_CHECK_GL( glGetProgramiv(shader_, GL_LINK_STATUS, &linked) );
    if (!linked)
    {
        log_ += "shader programm link error\n";
        shader_ = -1;
        return false;
    }

    return true;
}


bool Glsl::compileShader_(GLenum type, const QString& typeName, const QString &source)
{
    int shadername;
    SCH_CHECK_GL( shadername = glCreateShaderObjectARB(type) );
    if (!glIsShader(shadername))
    {
        log_ += "error creating " + typeName + " ShaderObject\n";
        return false;
    }

    // get the latin1 char source
    const GLchar * src[1];
    src[0] = source.toStdString().c_str();

    // attach source
    SCH_CHECK_GL( glShaderSource(shadername, 1, src, 0) );
    // compile
    SCH_CHECK_GL( glCompileShaderARB(shadername) );

    // check compile status
    bool compiled = false;
    GLint cc;
    SCH_CHECK_GL( glGetShaderiv(shadername, GL_COMPILE_STATUS, &cc); )
    if (!cc)
    {
        log_ += typeName + " compile ERROR\n";
        //MO_CHECK_GL( glGetShaderiv(shadername, GL_SHADER_SOURCE_LENGTH, &cc); )
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
        SCH_CHECK_GL( glGetInfoLogARB(shadername, blen, &slen, &compiler_log[0]) );
        log_ += "compiler log:\n" + QString(&compiler_log[0]) + "\n";
        // error_line_(compiler_log, code));
    }

    // attach to programObject
    SCH_CHECK_GL( glAttachObjectARB(shader_, shadername) );

    return compiled;
}
