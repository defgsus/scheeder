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

#include "modelfactory.h"
#include "model.h"

ModelFactory::ModelFactory()
{
}

Model * ModelFactory::createCube(float s) const
{
    return createBox(s, s, s);
}

Model * ModelFactory::createBox(
        float sidelength_x, float sidelength_y, float sidelength_z) const
{
    Model * m = new Model;

    float
        sx = sidelength_x * 0.5f,
        sy = sidelength_y * 0.5f,
        sz = sidelength_z * 0.5f;

    // --- vertices / corner-points ---


    m->setColor(0.1,1.0,0.5, 1.);

    // front-bottom-left
    int fbl = m->addVertex(-sx, -sy,  sz),
    // front-bottom-right
        fbr = m->addVertex( sx, -sy,  sz),
    // front-top-right
        ftr = m->addVertex( sx,  sy,  sz),
    // front-top-left
        ftl = m->addVertex(-sx,  sy,  sz);

    m->setColor(1,0.5,0.1, 1.);

    // back-bottom-left
    int bbl = m->addVertex(-sx, -sy, -sz),
    // aso..
        bbr = m->addVertex( sx, -sy, -sz),
        btr = m->addVertex( sx,  sy, -sz),
        btl = m->addVertex(-sx,  sy, -sz);

    // --- surfaces / triangles ---

    // front
    m->addTriangle(ftr, ftl, fbl);
    m->addTriangle(ftr, fbl, fbr);
    // right
    m->addTriangle(ftr, fbr, btr);
    m->addTriangle(fbr, bbr, btr);
    // back
    m->addTriangle(btr, bbr, btl);
    m->addTriangle(btl, bbr, bbl);
    // left
    m->addTriangle(ftl, btl, bbl);
    m->addTriangle(ftl, bbl, fbl);
    // top
    m->addTriangle(ftr, btr, btl);
    m->addTriangle(ftr, btl, ftl);
    // bottom
    m->addTriangle(fbr, fbl, bbl);
    m->addTriangle(fbr, bbl, bbr);

    m->calculateTriangleNormals();

    return m;
}
