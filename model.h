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



/** Container for geometry data and abstraction around GL-arrays.

    This Model is quite custom and less generic.
    It only renders triangles. It will create a vertex-array-object
    from the passed data and draw it.
 */
class Model
#ifdef SCH_USE_QT_OPENGLFUNC
        : protected QOpenGLFunctions_3_3_Core
#endif
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

    /** Returns if a vertex array object has been initialized for this model. */
    bool isVAO() const { return isVAO_; }

    // --------- state -----------------------

    /** Sets the current color. Any subsequent call to the
        easy form of addVertex() will use this color. */
    void setColor(ColorType r, ColorType g, ColorType b, ColorType a)
        { curR_ = r; curG_ = g; curB_ = b; curA_ = a; }

    /** Sets the current normal. Any subsequent call to the
        easy form of addVertex() will use this normal. */
    void setNormal(NormalType nx, NormalType ny, NormalType nz)
        { curNx_ = nx; curNy_ = ny; curNz_ = nz; }

    void setTexCoord(TextureCoordType u, TextureCoordType v)
        { curU_ = u; curV_ = v; }

    // -------- vertex/triangle handling -----

    /** Clear ALL contents */
    void clear();

    /** Adds a vertex (point) with currently set normal and color.
        @returns the index of the vertex.
        @see setColor(), setNormal() */
    IndexType addVertex(VertexType x, VertexType y, VertexType z)
        { return addVertex(x,y,z, curNx_,curNy_,curNz_, curR_, curG_, curB_, curA_, curU_, curV_); }

    /** Adds a vertex (point) with normal and color.
        @returns the index of the vertex. */
    IndexType addVertex(VertexType x, VertexType y, VertexType z,
                  NormalType nx, NormalType ny, NormalType nz,
                  ColorType r, ColorType g, ColorType b, ColorType a,
                  TextureCoordType u, TextureCoordType v);

    /** Connects three previously created indices to form a triangle. */
    void addTriangle(IndexType p1, IndexType p2, IndexType p3);

    // ------- convenience functions -------

    /** Automatically calculates all normals for each triangle.
        Normals that share multiple triangles will be averaged. */
    void calculateTriangleNormals();

    /** Makes every vertex in the model unique.
        After this call, every triangle will have it's unique vertices. */
    void unGroupVertices();

    // ------------- opengl / drawing ---------------

    /** @{ */
    /** All these functions need to be called from within an opengl context! */

    /** Release existing opengl resources.
        Call this before deleting the model. */
    void releaseGL();

    /** Transmits the vertex attribute locations from the shader.
        This needs to be called for Model to create it's vertex array object */
    void setShaderLocations(const ShaderLocations&);

    /** Draws the vertex array object.
        @note This needs a shader working with the vertex attributes. */
    void draw();

    /** Draws the model through oldschool opengl arrays */
    void drawOldschool();

    /** @} */

private:

    /** Creates the vertexArrayObject from the initialized data. */
    void createVAO_();

#ifdef SCH_USE_QT_OPENGLFUNC
    void initQtOpenGl_();
#endif

    std::vector<VertexType>       vertex_;
    std::vector<NormalType>       normal_;
    std::vector<ColorType>        color_;
    std::vector<TextureCoordType> texcoord_;
    std::vector<IndexType>        index_;

    ColorType
        curR_, curG_, curB_, curA_;
    NormalType
        curNx_, curNy_, curNz_;
    TextureCoordType
        curU_, curV_;

    ShaderLocations attribs_;

    /** vertex array object */
    GLuint buffers_[4], vao_;
    bool isVAO_;

#ifdef SCH_USE_QT_OPENGLFUNC
    bool isGlFuncInitialized_;
#endif

};

#endif // MODEL_H
