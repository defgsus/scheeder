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

#include <QMouseEvent>

#include "basic3dwidget.h"
#include "debug.h"

Basic3DWidget::Basic3DWidget(QWidget *parent, const QGLFormat& f) :
    QGLWidget(f, parent)
{
    viewInit();
}


void Basic3DWidget::viewInit(Float distanceZ)
{
    distanceZ_ = distanceZ;
    rotationMatrix_ = Mat4();
    updateGL();
}

void Basic3DWidget::viewRotateX(Float d)
{
    if (!d) return;
#ifndef SCH_GLM_USE_DEGREE
    d /= (360.f / TWO_PI)
#endif
    rotationMatrix_ =
            glm::rotate(Mat4(), d, Vec3(1,0,0))
            * rotationMatrix_;
    updateGL();
}

void Basic3DWidget::viewRotateY(Float d)
{
    if (!d) return;
#ifndef SCH_GLM_USE_DEGREE
    d /= (360.f / TWO_PI)
#endif
    rotationMatrix_ =
            glm::rotate(Mat4(), d, Vec3(0,1,0))
            * rotationMatrix_;
    updateGL();
}

void Basic3DWidget::viewZoom(Float zoom)
{
    distanceZ_ -= zoom;
    updateGL();
}

Mat4 Basic3DWidget::transformationMatrix() const
{
    Mat4 m = glm::translate(Mat4(), Vec3(0,0,-distanceZ_));
    return m * rotationMatrix_;
}

void Basic3DWidget::mousePressEvent(QMouseEvent * e)
{
    // store the last mouse-click position so we can calculate a delta
    lastMousePos_ = e->pos();
}

void Basic3DWidget::mouseMoveEvent(QMouseEvent * e)
{
    // calculate the delta between the current and last mouse position
    int dx = lastMousePos_.x() - e->x(),
        dy = lastMousePos_.y() - e->y();
    lastMousePos_ = e->pos();

    if (e->buttons() & Qt::LeftButton)
    {
        viewRotateX(dy);
        viewRotateY(dx);
    }
    if (e->buttons() & Qt::RightButton)
    {
        viewZoom(dy * 0.1);
    }
}

void Basic3DWidget::resizeGL(int w, int h)
{
    // calculate a perspective transformation
    projectionMatrix_ = glm::perspective(63.f, (float)w/h, 0.1f, 1000.0f);

    // set the viewport to the widget's size
    glViewport(0,0,w,h);


#ifndef SCH_USE_QT_OPENGLFUNC
    // set the projection matrix
    /* NOTE: This is pretty much openGL v1-2 stuff.
     * In later versions projection and view matrices are
     * passed to the shaders directly.
     * They are still here, to enable some easy immidiate-drawing
     * of the coordinate system */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glLoadMatrixf(glm::value_ptr(projectionMatrix_));
    // initialize the transformation matrix
    // (although this is overwritten in paintGL();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
#endif
}


void Basic3DWidget::paintGL()
{
#ifndef SCH_USE_QT_OPENGLFUNC
    // setup transformation matrix
    SCH_CHECK_GL( glLoadMatrixf(glm::value_ptr(transformationMatrix())) );
#endif

    // clear screen
    SCH_CHECK_GL( glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) );
}


void Basic3DWidget::drawCoords_(int len)
{
    // NOTE: slow immidiate-mode drawing

    const GLfloat s = 0.1;
    glBegin(GL_LINES);

        glColor3f(1,0,0);
        for (int i=0; i<len; ++i)
        {
            glVertex3f(i,0,0);
            glVertex3f(i+1,0,0);
            glVertex3f(i+1,-s,0);
            glVertex3f(i+1,s,0);
        }
        glColor3f(0,1,0);
        for (int i=0; i<len; ++i)
        {
            glVertex3f(0,i,0);
            glVertex3f(0,i+1,0);
            glVertex3f(-s,i+1,0);
            glVertex3f(s,i+1,0);
        }
        glColor3f(0,0,1);
        for (int i=0; i<len; ++i)
        {
            glVertex3f(0,0,i);
            glVertex3f(0,0,i+1);
            glVertex3f(0,-s,i+1);
            glVertex3f(0,s,i+1);
        }

    glEnd();
}

