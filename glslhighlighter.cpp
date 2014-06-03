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

#include "glslhighlighter.h"

/* http://www.opengl.org/registry/doc/GLSLangSpec.4.10.6.clean.pdf */
const QString glsl_keywords[] =
{
    "attribute", "const", "uniform", "varying", "layout",
    "centroid", "flat", "smooth", "noperspective", "patch",
    "sample", "break", "continue", "do", "for", "while", "switch",
    "case", "default", "if", "else", "subroutine", "in", "out", "inout",
    "float", "double", "int", "void", "bool", "true", "false",
    "invariant", "discard", "return",
    "mat2", "mat3", "mat4", "dmat2", "dmat3", "dmat4",
    "mat2x2", "mat2x3", "mat2x4", "dmat2x2", "dmat2x3", "dmat2x4",
    "mat3x2", "mat3x3", "mat3x4", "dmat3x2", "dmat3x3", "dmat3x4",
    "mat4x2", "mat4x3", "mat4x4", "dmat4x2", "dmat4x3", "dmat4x4",
    "vec2", "vec3", "vec4", "ivec2", "ivec3", "ivec4",
    "bvec2", "bvec3", "bvec4", "dvec2", "dvec3", "dvec4",
    "uint", "uvec2", "uvec3", "uvec4",
    "lowp", "mediump", "highp", "precision",
    "sampler1D", "sampler2D", "sampler3D", "samplerCube",
    "sampler1DShadow", "sampler2DShadow", "samplerCubeShadow",
    "sampler1DArray", "sampler2DArray",
    "sampler1DArrayShadow", "sampler2DArrayShadow",
    "isampler1D", "isampler2D", "isampler3D", "isamplerCube",
    "isampler1DArray", "isampler2DArray",
    "usampler1D", "usampler2D", "usampler3D", "usamplerCube",
    "usampler1DArray", "usampler2DArray",
    "sampler2DRect", "sampler2DRectShadow", "isampler2DRect", "usampler2DRect",
    "samplerBuffer", "isamplerBuffer", "usamplerBuffer",
    "sampler2DMS", "isampler2DMS", "usampler2DMS",
    "sampler2DMSArray", "isampler2DMSArray", "usampler2DMSArray",
    "samplerCubeArray", "samplerCubeArrayShadow", "isamplerCubeArray", "usamplerCubeArray",
    "struct"
};
const int num_glsl_keywords = sizeof(glsl_keywords) / sizeof(glsl_keywords[0]);

const QString glsl_reserved_keywords[] =
{
    "common", "partition", "active",
    "asm",
    "class", "union", "enum", "typedef", "template", "this", "packed",
    "goto", "inline", "noinline", "volatile", "public", "static", "extern",
    "external", "interface",
    "long", "short", "half", "fixed", "unsigned", "superp",
    "input", "output",
    "hvec2", "hvec3", "hvec4", "fvec2", "fvec3", "fvec4",
    "sampler3DRect",
    "filter",
    "image1D", "image2D", "image3D", "imageCube",
    "iimage1D", "iimage2D", "iimage3D", "iimageCube",
    "uimage1D", "uimage2D", "uimage3D", "uimageCube",
    "image1DArray", "image2DArray",
    "iimage1DArray", "iimage2DArray",
    "uimage1DArray", "uimage2DArray",
    "image1DShadow", "image2DShadow",
    "image1DArrayShadow", "image2DArrayShadow",
    "imageBuffer", "iimageBuffer", "uimageBuffer",
    "sizeof", "cast",
    "namespace", "using",
    "row_major"
};
const int num_glsl_reserved_keywords =
        sizeof(glsl_reserved_keywords) / sizeof(glsl_reserved_keywords[0]);

/* https://www.opengl.org/wiki/Built-in_Variable_%28GLSL%29 */
const QString glsl_variables[] =
{
    "gl_VertexID", "gl_InstanceID",
    "gl_PerVertex",
    "gl_Position", "gl_PointSize", "gl_ClipDistance",
    "gl_Vertex",
    "gl_PatchVerticesIn", "gl_PrimitiveID", "gl_InvocationID",
    "gl_MaxPatchVertices",
    "gl_TessLevelOuter", "gl_TessLevelInner", "gl_TessCoord",
    "gl_PrimitiveIDIn",
    "gl_Layer", "gl_ViewportIndex",
    "gl_FragCoord", "gl_FrontFacing", "gl_PointCoord",
    "gl_SampleID", "gl_SamplePosition", "gl_SampleMask", "gl_SampleMaskIn",
    "gl_DepthRange", "gl_FragDepth"
};
const int num_glsl_variables = sizeof(glsl_variables) / sizeof(glsl_variables[0]);



GlslHighlighter::GlslHighlighter(QTextDocument *parent)
    :   QSyntaxHighlighter(parent)
{
    QTextCharFormat
        keywordFormat,
        reservedKeywordFormat,
        variableFormat;

    // -- styles for each category --

    // keywords
    keywordFormat.setFontWeight(QFont::Bold);
    keywordFormat.setForeground(QBrush(QColor(220,220,220)));
    // reserved keywords
    reservedKeywordFormat.setFontWeight(QFont::Bold);
    reservedKeywordFormat.setForeground(QBrush(QColor(220,50,50)));
    // built-in variables
    variableFormat.setFontWeight(QFont::Bold);
    variableFormat.setForeground(QBrush(QColor(200,220,200)));
    // comments
    commentFormat_.setForeground(QBrush(QColor(120,120,120)));

    HighlightingRule rule;

    // setup keywords
    for (int i=0; i<num_glsl_keywords; ++i)
    {
        // match whole-word with word boundaries
        rule.pattern = QRegExp( "\\b" + glsl_keywords[i] + "\\b" );
        rule.format = keywordFormat;
        rules_.append(rule);
    }

    // setup reserved keywords

    for (int i=0; i<num_glsl_reserved_keywords; ++i)
    {
        // match whole-word with word boundaries
        rule.pattern = QRegExp( "\\b" + glsl_reserved_keywords[i] + "\\b" );
        rule.format = reservedKeywordFormat;
        rules_.append(rule);
    }

    // setup variables

    for (int i=0; i<num_glsl_variables; ++i)
    {
        rule.pattern = QRegExp( glsl_variables[i] );
        rule.pattern.setPatternSyntax(QRegExp::FixedString);
        rule.format = variableFormat;
        rules_.append(rule);
    }

    // single line comments
    rule.pattern = QRegExp("//[^\n]*");
    rule.format = commentFormat_;
    rules_.append(rule);
    // multiline comments
    commentStartExpression_ = QRegExp("/\\*");
    commentEndExpression_ = QRegExp("\\*/");
}


void GlslHighlighter::highlightBlock(const QString &text)
{
    // apply rules
    foreach (const HighlightingRule &rule, rules_)
    {
        QRegExp expression(rule.pattern);
        // find the string
        int index = expression.indexIn(text);
        while (index >= 0)
        {
            int length = expression.matchedLength();
            // set format
            setFormat(index, length, rule.format);
            // find again
            index = expression.indexIn(text, index + length);
        }
    }

    // multiline comments
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = commentStartExpression_.indexIn(text);

    while (startIndex >= 0)
    {
        int endIndex = commentEndExpression_.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1)
        {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        }
        else
        {
            commentLength = endIndex - startIndex
                            + commentEndExpression_.matchedLength();
        }
        setFormat(startIndex, commentLength, commentFormat_);
        startIndex = commentStartExpression_.indexIn(text, startIndex + commentLength);
    }
}
