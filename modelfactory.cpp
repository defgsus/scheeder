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


Model * ModelFactory::createCube(float sidelength) const
{
    Model * m = new Model;

    float s = sidelength * 0.5f;

    // --- vertices / corner-points ---

    // front-bottom-left
    int fbl = m->addVertex(-s, -s,  s),
    // front-bottom-right
        fbr = m->addVertex( s, -s,  s),
    // front-top-right
        ftr = m->addVertex( s,  s,  s),
    // front-top-left
        ftl = m->addVertex(-s,  s,  s),
    // back-bottom-left
        bbl = m->addVertex(-s, -s, -s),
    // aso..
        bbr = m->addVertex( s, -s, -s),
        btr = m->addVertex( s,  s, -s),
        btl = m->addVertex(-s,  s, -s);

    // --- surfaces / triangles ---

    // front
    m->addTriangle(ftr, ftl, fbl);
    m->addTriangle(ftr, fbl, fbr);
    // right
    m->addTriangle(ftr, fbr, btr);
    m->addTriangle(fbr, bbr, btr);
    // back
    m->addTriangle(btr, btl, bbl);
    m->addTriangle(btr, bbl, bbr);
    // left
    m->addTriangle(ftl, btl, bbl);
    m->addTriangle(ftl, bbl, fbl);
    // top
    m->addTriangle(ftr, btr, btl);
    m->addTriangle(ftr, btl, ftl);
    // bottom
    m->addTriangle(fbr, fbl, bbl);
    m->addTriangle(fbr, bbr, bbl);


    return m;
}
