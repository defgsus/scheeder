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

#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include "basic3dwidget.h"

// forward decls.
class Model;
class Glsl;

/** Class to render a Model */
class RenderWidget : public Basic3DWidget
{
    Q_OBJECT
public:
    explicit RenderWidget(QWidget *parent = 0);
    ~RenderWidget();

signals:

    /** Emitted when shader was compiled after source-change,
        regardless of success. */
    void shaderCompiled();

public slots:

    /** Applies AppSettings */
    void reconfigure();

    /** Sets the Model to be rendered.
        Ownership of class is taken! */
    void setModel(Model * m);

    /** Sets the Shader to be used for rendering.
        Ownership of class is taken! */
    void setShader(Glsl * s);

    /** Please compile the shader in next paintGL() */
    void requestCompileShader();

protected:

    virtual void paintGL();

    /** Sets the OpenGL state to the current set options */
    void applyOptions_();

private:

    Model * model_;
    Glsl * shader_;

    bool requestCompile_;

    // options
    bool doDepthTest_,
         doCullFace_,
         doFrontFaceCCW_,
         doDrawCoords_;
};

#endif // RENDERWIDGET_H
