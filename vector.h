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
/** @file

    @brief definition of vector and matrix types.

    <p>This file includes the glm library and all usefull functions from it
    and defines the common types of vectors and matrices.</p>
*/


#ifndef VECTOR_H
#define VECTOR_H

#include <iostream>

/* http://glm.g-truc.net */
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>
//#include <glm/core/func_geometric.hpp>
//#include <glm/detail/func_geometric.hpp>
#include <glm/gtc/type_ptr.hpp>

// ----------- good-to-have constants -------------

/** the infamous PI */
#ifndef PI
#define PI (3.1415926535897932384626433832795)
#endif

/** 2.0 * PI */
#ifndef TWO_PI
#define TWO_PI (6.283185307179586476925286766559)
#endif

// TODO

// --------- our floating-point numeric type --------

typedef float Float;


// -------- basic vector and matrix types -------

typedef glm::vec2 Vec2;
typedef glm::vec3 Vec3;
typedef glm::vec4 Vec4;

typedef glm::mat3 Mat3;
typedef glm::mat4 Mat4;

// ------------- some functions -----------------

/** Returns a point on a unit sphere (radius = 1.0). <br>
Given a point P = <0,1,0>, and v = rotation around z, and u = rotation around y <br>
then the result is the rotated P. <br>
u and v are in the range of 0..1 for the whole sphere */
inline Vec3 pointOnSphere(Float u, Float v)
{
    u *= TWO_PI,
    v *= PI;
    auto P = Vec3(
    // rotate a point (0,1,0) around z
        -sin(v), std::cos(v), 0 );
    // rotate this point around y
    P.z = -P.x * std::sin(u);
    P.x =  P.x * std::cos(u);
    return P;
}

// ------------ iostream bindings ---------------

/** stream output of a vector */
inline std::ostream& operator << (std::ostream& s, const Vec2& v)
{
    s << '<' << v[0] << ", " << v[1] << '>';
    return s;
}

/** stream output of a vector */
inline std::ostream& operator << (std::ostream& s, const Vec3& v)
{
    s << '<' << v[0] << ", " << v[1] << ", " << v[2] << '>';
    return s;
}

/** stream output of a vector */
inline std::ostream& operator << (std::ostream& s, const Vec4& v)
{
    s << '<' << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << '>';
    return s;
}

/** stream output of a 3by3 matrix */
inline std::ostream& operator << (std::ostream& s, const Mat3& m)
{
    for (int i=0; i<3; ++i)
    {
        s << "<" << m[0][i] << ", " << m[1][i] << ", " << m[2][i] << ">" << std::endl;
    }
    return s;
}

/** stream output of a 4by4 matrix */
inline std::ostream& operator << (std::ostream& s, const Mat4& m)
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
