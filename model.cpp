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
#include "debug.h"

Model::Model()
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

    //std::cout << vertex_.size() << std::endl;
    //glDrawArrays(GL_LINES, 0, vertex_.size()/3);
    SCH_CHECK_GL( glDrawElements(GL_TRIANGLES, index_.size(), IndexEnum, &index_[0]) );

    SCH_CHECK_GL( glDisableClientState(GL_VERTEX_ARRAY) );
    SCH_CHECK_GL( glDisableClientState(GL_COLOR_ARRAY) );
    SCH_CHECK_GL( glDisableClientState(GL_NORMAL_ARRAY) );
}
