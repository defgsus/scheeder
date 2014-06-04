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

/* Following are the keywords and variable names of GLSL.
 * TODO: Would be great to divide them into different GLSL versions.
 *       Right now, they are all from 1.2 to 4+
 */

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

const QString glsl_functions[] =
{
    "radians", "degrees", "sin", "cos", "tan", "asin", "acos", "atan",
    "sinh", "cosh", "tanh", "asinh", "acosh", "atanh",
    "pow", "exp", "log", "exp2", "log2", "sqrt", "inversesqrt",
    "abs", "sign", "floor", "trunc", "round", "roundEven", "ceil", "fract", "mod",
    "modf", "min", "max", "clamp", "mix", "step", "smoothstep", "isnan", "isinf",
    "floatBitsToInt", "floatBitsToUint", "intBitsToFloat", "uintBitsToFloat",
    "fma", "frexp", "ldexp",
    "packUnorm2x16", "packUnorm4x8", "packSnorm4x8",
    "unpackUnorm2x16", "unpackUnorm4x8", "unpackSnorm4x8",
    "packDouble2x32", "unpackDouble2x32",
    "length", "distance", "dot", "cross", "normalize", "ftransform",
    "faceforward", "reflect", "refract",
    "matrixCompMult", "outerProduct", "transpose", "determinant", "inverse",
    "lessThan", "lessThanEqual", "greaterThan", "greaterThanEqual",
    "equal", "notEqual", "any", "all", "not",
    "uaddCarry", "usubBorrow", "umulExtended", "imulExtended",
    "bitfieldExtract", "bitfieldInsert", "bitfieldReverse", "bitCount", "findLSB", "findMSB",
    "textureSize", "textureQueryLod", "textureQueryLevels",
    "texture", "textureProj", "textureLod",
    "textureOffset", "texelFetch", "texelFetchOffset", "textureProjOffset", "textureLodOffset",
    "textureProjLod", "textureProjLodOffset", "textureGrad", "textureGradOffset",
    "textureProjGrad", "textureProjGradOffset",
    "textureGather", "textureGatherOffset", "textureGatherOffsets",
    "texture1D", "texture1DProj", "texture1DLod", "texture1DProjLod",
    "texture2D", "texture2DProj", "texture2DLod", "texture2DProjLod",
    "texture3D", "texture3DProj", "texture3DLod", "texture3DProjLod",
    "textureCube", "textureCubeLod",
    "shadow1D", "shadow1DProj", "shadow1DLod", "shadow1DProjLod",
    "shadow2D", "shadow2DProj", "shadow2DLod", "shadow2DProjLod",
    "atomicCounterIncrement", "atomicCounterDecrement", "atomicCounter",
    "atomicAdd", "atomicMin", "atomicMax", "atomicAnd", "atomicOr", "atomicXor",
    "atomicExchange", "atomicCompSwap",
    "imageSize", "imageLoad", "imageStore",
    "imageAtomicAdd", "imageAtomicMin", "imageAtomicMax", "imageAtomicAnd", "imageAtomicOr", "imageAtomicXor",
    "dFdx", "dFdy", "fwidth",
    "interpolateAtCentroid", "interpolateAtSample", "interpolateAtOffset",
    "noise1", "noise2", "noise3", "noise4",
    "EmitStreamVertex", "EndStreamPrimitive", "EmitVertex", "EndPrimitive",
    "barrier", "memoryBarrier", "memoryBarrierAtomicCounter", "memoryBarrierBuffer",
    "memoryBarrierShared", "memoryBarrier", "memoryBarrierImage", "groupMemoryBarrier"
};
const int num_glsl_functions =
        sizeof(glsl_functions) / sizeof(glsl_functions[0]);

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
    "gl_DepthRange",
    "gl_FragColor", "gl_FragData", "gl_FragDepth",
    "gl_Normal", "gl_Color", "gl_SecondaryColor",
    "gl_NormalScale", "gl_ClipPlane",
    "gl_PointParamaters", "gl_MaterialParameters", "gl_LightSourceParameters",
    "gl_LightModelParameters", "gl_LightModelProducts", "gl_LightProducts",
    "gl_FogParameters",
    "gl_FogCoord",
    "gl_FrontColor", "gl_BackColor", "gl_FrontSecondaryColor", "gl_BackSecondaryColor",
    "gl_TexCoord", "gl_FogFragCoord",
    "gl_ModelViewMatrix", "gl_ProjectionMatrix", "gl_ModelViewProjectionMatrix",
    "gl_TextureMatrix", "gl_NormalMatrix",
    "gl_ModelViewMatrixInverse", "gl_ProjectionMatrixInverse",
    "gl_ModelViewProjectionMatrixInverse", "gl_TextureMatrixInverse"
    "gl_ModelViewMatrixTranspose", "gl_ProjectionMatrixTranspose",
    "gl_ModelViewProjectionMatrixTranspose", "gl_TextureMatrixTranspose"
    "gl_ModelViewMatrixInverseTranspose", "gl_ProjectionMatrixInverseTranspose",
    "gl_ModelViewProjectionMatrixInverseTranspose", "gl_TextureMatrixInverseTranspose"
    "gl_MultiTexCoord0", "gl_MultiTexCoord1", "gl_MultiTexCoord2", "gl_MultiTexCoord3",
    "gl_MultiTexCoord4", "gl_MultiTexCoord5", "gl_MultiTexCoord6", "gl_MultiTexCoord7",
    // constants
    "gl_MaxTextureUnits", "gl_MaxVertexAttribs", "gl_MaxVertexUniformComponents",
    "gl_MaxVaryingFloats", "gl_MaxVaryingComponents", "gl_MaxVertextTextureImageUnits",
    "gl_MaxCombinedTextureImageUnits", "gl_MaxTextureImageUnits", "gl_MaxFragmentUniformComponents",
    "gl_MaxDrawBuffers", "gl_MaxClipDistances",
    "gl_MaxClipPlanes", "gl_MaxTextureCoords",

};
const int num_glsl_variables = sizeof(glsl_variables) / sizeof(glsl_variables[0]);



GlslHighlighter::GlslHighlighter(QTextDocument *parent)
    :   QSyntaxHighlighter(parent)
{
    QTextCharFormat
        keywordFormat,
        reservedKeywordFormat,
        functionFormat,
        variableFormat;

    // -- styles for each category --

    // keywords
    keywordFormat.setFontWeight(QFont::Bold);
    keywordFormat.setForeground(QBrush(QColor(220,220,220)));
    // reserved keywords
    reservedKeywordFormat.setFontWeight(QFont::Bold);
    reservedKeywordFormat.setForeground(QBrush(QColor(220,50,50)));
    // built-in functions
    functionFormat.setFontWeight(QFont::Bold);
    functionFormat.setForeground(QBrush(QColor(200,200,220)));
    // built-in variables
    variableFormat.setFontWeight(QFont::Bold);
    variableFormat.setForeground(QBrush(QColor(200,220,200)));
    // comments
    commentFormat_.setForeground(QBrush(QColor(140,140,140)));

    // temporary
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
        rule.pattern = QRegExp( "\\b" + glsl_reserved_keywords[i] + "\\b" );
        rule.format = reservedKeywordFormat;
        rules_.append(rule);
    }

    // setup functions

    for (int i=0; i<num_glsl_functions; ++i)
    {
        rule.pattern = QRegExp( "\\b" + glsl_functions[i] + "\\b" );
        rule.format = functionFormat;
        rules_.append(rule);
    }

    // setup variables

    for (int i=0; i<num_glsl_variables; ++i)
    {
        rule.pattern = QRegExp( "\\b" + glsl_variables[i] + "\\b" );
        rule.format = variableFormat;
        rules_.append(rule);
    }

    // setup single line comments
    rule.pattern = QRegExp("//[^\n]*");
    rule.format = commentFormat_;
    rules_.append(rule);

    // setup multiline comments
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


    // --- multiline comments ---

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
