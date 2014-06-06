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

#ifndef MODELFACTORY_H
#define MODELFACTORY_H

// forwards
class Model;

/** @brief Creates predefined Shapes/Models
*/
class ModelFactory
{
public:
    ModelFactory();

    /** Create a cube model */
    Model * createCube(float sidelength) const;

    /** Create a box model */
    Model * createBox(float sidelength_x, float sidelength_y, float sidelength_z) const;

    Model * createUVSphere(float radius, unsigned int segmentsU, unsigned int segmentsV);

    /** Create the famous OpenGL teapot */
    Model * createTeapot(float scale);
};

#endif // MODELFACTORY_H
