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

#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <QMouseEvent>

#include "basic3dwidget.h"

Basic3DWidget::Basic3DWidget(QWidget *parent) :
    QGLWidget(parent)
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
    rotationMatrix_ =
            glm::rotate(Mat4(), d, Vec3(1,0,0))
            * rotationMatrix_;
    updateGL();
}

void Basic3DWidget::viewRotateY(Float d)
{
    if (!d) return;
    rotationMatrix_ =
            glm::rotate(Mat4(), d, Vec3(0,1,0))
            * rotationMatrix_;
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
}

void Basic3DWidget::resizeGL(int w, int h)
{
    // calculate a perspective transformation
    projectionMatrix_ = glm::perspective(63.f, (float)w/h, 0.1f, 1000.0f);

    // set the viewport to the widget's size
    glViewport(0,0,w,h);

    // set the projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(projectionMatrix_));
    // initialize the transformation matrix
    // (although this is overwritten in paintGL();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


void Basic3DWidget::paintGL()
{
    // setup transformation matrix
    glLoadMatrixf(glm::value_ptr(transformationMatrix()));

    // clear screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
