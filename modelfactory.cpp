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
#include "vector.h"
#include "teapot_data.h"

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

    // set color for front of box
    m->setColor(0.1,1.0,0.5, 1.);

    // front-bottom-left
    m->setTexCoord(0,0);
    int fbl = m->addVertex(-sx, -sy,  sz);
    // front-bottom-right
    m->setTexCoord(1,0);
    int fbr = m->addVertex( sx, -sy,  sz);
    // front-top-right
    m->setTexCoord(1,1);
    int ftr = m->addVertex( sx,  sy,  sz);
    // front-top-left
    m->setTexCoord(0,1);
    int ftl = m->addVertex(-sx,  sy,  sz);

    // color for back of box
    m->setColor(1,0.5,0.1, 1.);

    // back-bottom-left
    m->setTexCoord(0,1);
    int bbl = m->addVertex(-sx, -sy, -sz);
    // aso..
    m->setTexCoord(1,1);
    int bbr = m->addVertex( sx, -sy, -sz);
    m->setTexCoord(1,0);
    int btr = m->addVertex( sx,  sy, -sz);
    m->setTexCoord(0,0);
    int btl = m->addVertex(-sx,  sy, -sz);

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


Model * ModelFactory::createUVSphere(float rad, unsigned int segu, unsigned int segv)
{
    Model * m = new Model;

    // top point
    m->setTexCoord(0,1);
    m->addVertex(0, rad, 0);

    for (unsigned int v = 1; v<segv; ++v)
    {
        // current vertex offset
        int rown = m->numVertices();

        // body vertices
        for (unsigned int u = 0; u<segu; ++u)
        {
            Vec3 p = pointOnSphere((float)u / segu, (float)v / segv);

            m->setTexCoord((float)u / segu, (float)v / segv);
            m->addVertex(p.x * rad, p.y * rad, p.z * rad);
        }

        // triangles on each 'row'
        for (unsigned int u = 0; u<segu; ++u)
        {
            // connect to top point
            if (v==1)
            {
                m->addTriangle(0, rown + u, rown + (u + 1) % segu);
            }
            else
            {
                // connect
                m->addTriangle(
                // previous row
                    rown - segu + u,
                // with this row
                    rown + u, rown + (u + 1) % segu);
                // .. and the other triangle of the quad
                m->addTriangle(rown - segu + (u + 1) % segu,
                               rown - segu + u,
                               rown + (u + 1) % segu);
            }
        }
    }

    int rown = m->numVertices() - segu - 1;

    // bottom point
    m->setTexCoord(0,0);
    m->addVertex(0, -rad, 0);

    // connect to bottom point
    for (unsigned int u = 0; u<segu; ++u)
    {
        m->addTriangle(m->numVertices()-1,
                       rown + (u + 1) % segu, rown + u);
    }

    m->calculateTriangleNormals();

    return m;
}


Model * ModelFactory::createTeapot(float scale)
{
    Model * m = new Model;

    float maxy = 0.001;
    for (int i=0; i<TeapotNS::numVertices; ++i)
        maxy = std::max(maxy, TeapotNS::vertices[i][1]);

    // create vertices
    for (int i=0; i<TeapotNS::numVertices; ++i)
    {
        // calculate 'some' texture coordinates for the teapot
        float ang = atan2(TeapotNS::vertices[i][0], TeapotNS::vertices[i][2]);
        m->setTexCoord(ang / TWO_PI + 0.5f, TeapotNS::vertices[i][1] / maxy);

        // just copy the data
        m->addVertex(TeapotNS::vertices[i][0] * scale,
                     TeapotNS::vertices[i][1] * scale,
                     TeapotNS::vertices[i][2] * scale);
    }

    // create triangles
    for (int i=0; i<TeapotNS::numFaces; ++i)
    {
        m->addTriangle(TeapotNS::faces[i][1],
                       TeapotNS::faces[i][2],
                       TeapotNS::faces[i][3]);
    }

    m->calculateTriangleNormals();

    return m;
}
