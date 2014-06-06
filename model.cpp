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

#include "model.h"
#include "vector.h"
#include "debug.h"

Model::Model()
    :   curR_   (.5f),
        curG_   (.5f),
        curB_   (.5f),
        curA_   (1.f)
{
}

void Model::clear()
{
    vertex_.clear();
    normal_.clear();
    color_.clear();
    texcoord_.clear();
    index_.clear();
}

int Model::addVertex(
                VertexType x, VertexType y, VertexType z,
                NormalType nx, NormalType ny, NormalType nz,
                ColorType r, ColorType g, ColorType b, ColorType a)
{
    vertex_.push_back(x);
    vertex_.push_back(y);
    vertex_.push_back(z);

    normal_.push_back(nx);
    normal_.push_back(ny);
    normal_.push_back(nz);

    color_.push_back(r);
    color_.push_back(g);
    color_.push_back(b);
    color_.push_back(a);

    return numVertices() - 1;
}

void Model::addTriangle(IndexType p1, IndexType p2, IndexType p3)
{
    index_.push_back(p1);
    index_.push_back(p2);
    index_.push_back(p3);
}

void Model::draw() const
{
    SCH_CHECK_GL( glEnableClientState(GL_COLOR_ARRAY) );
    SCH_CHECK_GL( glEnableClientState(GL_NORMAL_ARRAY) );
    SCH_CHECK_GL( glEnableClientState(GL_VERTEX_ARRAY) );

    SCH_CHECK_GL( glVertexPointer(3, VertexEnum, 0, &vertex_[0]) );
    SCH_CHECK_GL( glNormalPointer(NormalEnum, 0, &normal_[0]) );
    SCH_CHECK_GL( glColorPointer(4, ColorEnum,  0, &color_[0]) );

    //glDrawArrays(GL_LINES, 0, vertex_.size()/3);
    SCH_CHECK_GL( glDrawElements(GL_TRIANGLES, index_.size(), IndexEnum, &index_[0]) );

    SCH_CHECK_GL( glDisableClientState(GL_VERTEX_ARRAY) );
    SCH_CHECK_GL( glDisableClientState(GL_COLOR_ARRAY) );
    SCH_CHECK_GL( glDisableClientState(GL_NORMAL_ARRAY) );
}


void Model::calculateTriangleNormals()
{
    // first clear the normal array
    normal_.resize(vertex_.size());
    for (size_t i=0; i<normal_.size(); ++i)
        normal_[i] = 0;

    // an array to memorize the number of
    // adjacent vertices for each vertex
    std::vector<size_t> nr_adds(numVertices());

    // for each triangle
    for (size_t i=0; i<index_.size()/3; ++i)
    {
        // index of each triangle corner
        size_t
            v1 = index_[i*3],
            v2 = index_[i*3+1],
            v3 = index_[i*3+2];
        // vector of each triangle corner
        Vec3
            p1 = Vec3(vertex_[v1*3], vertex_[v1*3+1], vertex_[v1*3+2]),
            p2 = Vec3(vertex_[v2*3], vertex_[v2*3+1], vertex_[v2*3+2]),
            p3 = Vec3(vertex_[v3*3], vertex_[v3*3+1], vertex_[v3*3+2]);

        // calculate the normal of the triangle
        Vec3 n = glm::normalize( glm::cross( p2-p1, p3-p1 ) );

        // copy/add to normals array
        normal_[v1*3  ] += n[0];
        normal_[v1*3+1] += n[1];
        normal_[v1*3+2] += n[2];
        normal_[v2*3  ] += n[0];
        normal_[v2*3+1] += n[1];
        normal_[v2*3+2] += n[2];
        normal_[v3*3  ] += n[0];
        normal_[v3*3+1] += n[1];
        normal_[v3*3+2] += n[2];

        // memorize the count
        nr_adds[v1]++;
        nr_adds[v2]++;
        nr_adds[v3]++;
    }

    // finally normalize the normals :)
    for (size_t i=0; i<normal_.size(); ++i)
    {
        if (nr_adds[i/3])
            normal_[i] /= nr_adds[i/3];
    }

}
