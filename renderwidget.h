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
    explicit RenderWidget(QWidget *parent = 0,
                          const QGLFormat& format = QGLFormat());
    ~RenderWidget();

    /** Return current animation time in seconds. */
    float getTime() const;

signals:

    /** Emitted when shader was compiled after source-change,
        regardless of success. */
    void shaderCompiled();

public slots:

    /** Applies AppSettings */
    void reconfigure();

    /** Sets the image filename for slot [0, SCH_MAX_TEXTURES-1] */
    void setImage(uint index, const QString& filename);

    /** Sets the Model to be rendered.
        Ownership of class is taken! */
    void setModel(Model * m);

    /** Sets the Shader to be used for rendering.
        Ownership of class is taken! */
    void setShader(Glsl * s);

    /** Please compile the shader in next paintGL() */
    void requestCompileShader();

    /** Starts continuiosly rerendering the scene. */
    void startAnimation();

    /** Stops rerendering the scene all over. */
    void stopAnimation() { doAnimation_ = false; }

protected:

    virtual void initializeGL();

    virtual void paintGL();

    /** Sets the OpenGL state to the current set options */
    void applyOptions_();

    /** Send specific uniform values (like projection matrix...) */
    void sendSpecialUniforms_();

    void initTextures_();

private:

    Model * model_, * newModel_;
    Glsl * shader_, * newShader_;

    QString imageFile_[SCH_MAX_TEXTURES];
    GLint texture_[SCH_MAX_TEXTURES];

    bool requestCompile_,
         requestTextureUpdate_,
         doAnimation_;

    QTime timer_;

    // options
    bool doDepthTest_,
         doCullFace_,
         doFrontFaceCCW_,
         doDrawCoords_;
};

#endif // RENDERWIDGET_H
