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

#ifndef BASIC3DWIDGET_H
#define BASIC3DWIDGET_H

#include <QGLWidget>

#include "vector.h"

/** @brief Helper for setting up a projection/transformation
    matrix with mouse interaction.

    <p>This is a base class.
    You should derive a class and override paintGL().
    Like so:</p>
    @code
    void Derived::paintGL()
    {
        // clear screen and set transformation matrix
        Basic3DWidget::paintGL();

        // draw something
        drawCoords_(10);
        ...
    }
    @endcode
 */
class Basic3DWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit Basic3DWidget(QWidget *parent = 0);

    /** Returns the current transformation matrix */
    Mat4 transformationMatrix() const;

signals:

public slots:

    /** Initialize the transformation matrix */
    void viewInit(Float distanceZ = 10.f);
    /** Continously rotate around the x-axis */
    void viewRotateX(Float degree);
    /** Continously rotate around the y-axis */
    void viewRotateY(Float degree);
    /** Zooms in (+) or out (-) */
    void viewZoom(Float zoom);

protected:

    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);

    //virtual void initializeGL();

    /** Sets the viewport and the projection matrix */
    virtual void resizeGL(int w, int h);

    /** Clears buffer and applies the transformation matrix */
    virtual void paintGL();

    // ---- helper functions for derrived classes ----

    /** Draws a coordinate system. @p len is length in units */
    void drawCoords_(int len);

private:

    Mat4
        projectionMatrix_,
        rotationMatrix_;
    Float distanceZ_;

    QPoint lastMousePos_;

};

#endif // BASIC3DWIDGET_H
