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

#ifndef VECTOR_H
#define VECTOR_H

#include <iostream>

/* http://glm.g-truc.net */
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/core/func_geometric.hpp>
#include <glm/gtc/type_ptr.hpp>


// --------- our floating-point numeric type --------

typedef float Float;


// -------- basic vector and matrix types -------

typedef glm::detail::tvec2<Float> Vec2;
typedef glm::detail::tvec3<Float> Vec3;
typedef glm::detail::tvec4<Float> Vec4;

typedef glm::detail::tmat3x3<Float> Mat3;
typedef glm::detail::tmat4x4<Float> Mat4;


// ------------ iostream bindings ---------------

/** stream output of a vector */
template <typename F>
std::ostream& operator << (std::ostream& s, const glm::detail::tvec2<F>& v)
{
    s << '<' << v[0] << ", " << v[1] << '>';
    return s;
}

/** stream output of a vector */
template <typename F>
std::ostream& operator << (std::ostream& s, const glm::detail::tvec3<F>& v)
{
    s << '<' << v[0] << ", " << v[1] << ", " << v[2] << '>';
    return s;
}

/** stream output of a vector */
template <typename F>
std::ostream& operator << (std::ostream& s, const glm::detail::tvec4<F>& v)
{
    s << '<' << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << '>';
    return s;
}

/** stream output of a 3by3 matrix */
template <typename F>
std::ostream& operator << (std::ostream& s, const glm::detail::tmat3x3<F>& m)
{
    for (int i=0; i<3; ++i)
    {
        s << "<" << m[0][i] << ", " << m[1][i] << ", " << m[2][i] << ">" << std::endl;
    }
    return s;
}

/** stream output of a 4by4 matrix */
template <typename F>
std::ostream& operator << (std::ostream& s, const glm::detail::tmat4x4<F>& m)
{
    for (int i=0; i<4; ++i)
    {
        s << "<";
        for (int j=0; j<3; ++j)
            s << m[j][i] << ", ";
        s << m[3][i] << ">" << std::endl;
    }
    return s;
}

#endif // VECTOR_H
