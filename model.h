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

#ifndef MODEL_H
#define MODEL_H

#include <vector>

#include "opengl.h"

/** Abstraction around GL-arrays.

 */
class Model
{
public:
    // ------- types ---------

    typedef GLfloat VertexType;
    typedef GLfloat NormalType;
    typedef GLfloat ColorType;
    typedef GLfloat TextureCoordType;
    typedef GLuint  IndexType;

    // ------ enums ----------

    static const GLenum VertexEnum       = GL_FLOAT;
    static const GLenum NormalEnum       = GL_FLOAT;
    static const GLenum ColorEnum        = GL_FLOAT;
    static const GLenum TextureCoordEnum = GL_FLOAT;
    static const GLenum IndexEnum        = GL_UNSIGNED_INT;

    // -------- ctor ---------

    Model();

    // ------- query ---------

    /** Returns number of vertices in the Model */
    int numVertices() const { return vertex_.size() / 3; }

    /** Returns number of triangles in the Model */
    int numTriangles() const { return index_.size() / 3; }

    // --------- state -----------------------

    /** Sets the current color. Any subsequent call to addVertex will
        use this color. */
    void setColor(ColorType r, ColorType g, ColorType b, ColorType a)
    { curR_ = r; curG_ = g; curB_ = b; curA_ = a; }

    // -------- vertex/triangle handling -----

    /** Clear ALL contents */
    void clear();

    int addVertex(VertexType x, VertexType y, VertexType z)
    { return addVertex(x,y,z, 0,0,0, curR_, curG_, curB_, curA_); }

    int addVertex(VertexType x, VertexType y, VertexType z,
                  NormalType nx, NormalType ny, NormalType nz,
                  ColorType r, ColorType g, ColorType b, ColorType a);

    void addTriangle(IndexType p1, IndexType p2, IndexType p3);

    // ------- convenience functions -------

    /** Automatically calculates all normals for each triangle.
        Normals that share multiple triangles will be averaged. */
    void calculateTriangleNormals();

    /** Executes all drawing commands. */
    void draw() const;

private:

    std::vector<VertexType>       vertex_;
    std::vector<NormalType>       normal_;
    std::vector<ColorType>        color_;
    std::vector<TextureCoordType> texcoord_;
    std::vector<IndexType>        index_;

    ColorType
        curR_, curG_, curB_, curA_;
};

#endif // MODEL_H
