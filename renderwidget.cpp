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

#include "renderwidget.h"
#include "appsettings.h"
#include "model.h"
#include "glsl.h"
#include "debug.h"


RenderWidget::RenderWidget(QWidget *parent,
                           const QGLFormat& format) :
    Basic3DWidget   (parent, format),
    model_          (0),
    newModel_       (0),
    shader_         (0),
    newShader_      (0),
    requestCompile_ (false),
    requestTextureUpdate_(false),
    doAnimation_    (false)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(256,256);

    for (int i=0; i<4; ++i)
        texture_[i] = -1;

    reconfigure();
}

RenderWidget::~RenderWidget()
{
    if (model_)
        delete model_;
    if (shader_)
        delete shader_;
}

void RenderWidget::reconfigure()
{
    doDepthTest_ = appSettings->getValue("RenderSettings/doDepthTest").toBool();
    doCullFace_ = appSettings->getValue("RenderSettings/doCullFace").toBool();
    doFrontFaceCCW_ = appSettings->getValue("RenderSettings/doFrontFaceCCW").toBool();
    doDrawCoords_ = appSettings->getValue("RenderSettings/doDrawCoords").toBool();

    // set image filenames
    for (int i=0; i<SCH_MAX_TEXTURES; ++i)
    {
        QString fn = appSettings->getValue(QString("image%1").arg(i)).toString();
        if (imageFile_[i] != fn)
            setImage(i, fn);
    }

    update();
}

void RenderWidget::setModel(Model * m)
{
    newModel_ = m;
    update();
}

void RenderWidget::setShader(Glsl *s)
{
    newShader_ = s;
    update();
}

void RenderWidget::requestCompileShader()
{
    requestCompile_ = true;
    update();
}

void RenderWidget::initializeGL()
{
    Basic3DWidget::initializeGL();
}

void RenderWidget::paintGL()
{
    //glGetError(); /* clear previous errors */

    applyOptions_();

    // clear screen and such
    Basic3DWidget::paintGL();

#ifndef SCH_USE_QT_OPENGLFUNC
    if (doDrawCoords_)
        drawCoords_(10);
#endif

    if (requestTextureUpdate_)
    {
        requestTextureUpdate_ = false;
        initTextures_();
    }

    bool sendAttributes = false;

    // if there's a new model ..
    if (newModel_)
    {
        if (model_)
        {
            // .. delete the opengl resources
            model_->releaseGL();
            delete model_;
        }
        // and exchange
        model_ = newModel_;
        newModel_ = 0;
        // we need to tell the model the attribute locations
        sendAttributes = true;
    }

    // replace shader
    if (newShader_)
    {
        // remove old resources
        if (shader_)
        {
            if (shader_->ready())
                shader_->releaseGL();
            delete shader_;
        }
        // exchange
        shader_ = newShader_;
        newShader_ = 0;
    }

    if (shader_)
    {
        if (requestCompile_)
        {
            requestCompile_ = false;
            // (re-)compile shader
            if (shader_->compile())
                // and send the (possibly new) vertex attribute locations
                // to the model
                sendAttributes = true;
            // also tell mainwindow
            // to update the uniform widgets
            emit shaderCompiled();
        }

        // activate shader and update uniform values
        if (shader_->ready())
        {
            shader_->activate();
            shader_->sendUniforms();
            sendSpecialUniforms_();
        }
    }

    if (model_)
    {
        // compile model vao with new attribute locations
        if (sendAttributes && shader_)
            model_->setShaderLocations(shader_->getShaderLocations());

        // and finally draw
        if (model_->isVAO())
            model_->draw();
        else
            model_->drawOldschool();
    }

    if (shader_ && shader_->activated())
        shader_->deactivate();


    // render again!
    if (doAnimation_)
        update();
}

void RenderWidget::sendSpecialUniforms_()
{
    if ((int)shader_->getShaderLocations().projection>=0)
    {
        SCH_CHECK_GL( glUniformMatrix4fv(
                        shader_->getShaderLocations().projection,
                        1, GL_FALSE, glm::value_ptr(projectionMatrix())) );
    }
    if ((int)shader_->getShaderLocations().view>=0)
    {
        SCH_CHECK_GL( glUniformMatrix4fv(
                        shader_->getShaderLocations().view,
                        1, GL_FALSE, glm::value_ptr(transformationMatrix())) );
    }
    if ((int)shader_->getShaderLocations().time>=0)
    {
        SCH_CHECK_GL( glUniform1f(
                          shader_->getShaderLocations().time, getTime()) );
    }
    if ((int)shader_->getShaderLocations().aspect>=0)
    {
        SCH_CHECK_GL( glUniform1f(
                          shader_->getShaderLocations().aspect,
                          (float)width() / height()) );
    }

}

void RenderWidget::startAnimation()
{
    doAnimation_ = true;
    timer_.start();
    update();
}

float RenderWidget::getTime() const
{
    return (float)timer_.elapsed() / 1000.f;
}

void RenderWidget::applyOptions_()
{
    if (doDepthTest_)
        SCH_CHECK_GL( glEnable(GL_DEPTH_TEST) )
    else
        SCH_CHECK_GL( glDisable(GL_DEPTH_TEST) );

    if (doCullFace_)
        SCH_CHECK_GL( glEnable(GL_CULL_FACE) )
    else
        SCH_CHECK_GL( glDisable(GL_CULL_FACE) );

    if (doFrontFaceCCW_)
        SCH_CHECK_GL( glFrontFace(GL_CCW) )
    else
        SCH_CHECK_GL( glFrontFace(GL_CW) );
}

void RenderWidget::setImage(uint index, const QString &filename)
{
    imageFile_[index] = filename;
    requestTextureUpdate_ = true;
}

void RenderWidget::initTextures_()
{
    glEnable(GL_TEXTURE_2D);

    for (int i=0; i<SCH_MAX_TEXTURES; ++i)
    {
        SCH_CHECK_GL( glActiveTexture(GL_TEXTURE0 + i) );

        // delete previous
        if (texture_[i] != -1)
            deleteTexture(texture_[i]);

        texture_[i] = imageFile_[i].isEmpty()? -1 : bindTexture(QImage(imageFile_[i]));

        // filtering/interpolation mode for min- & maxifying
        SCH_CHECK_GL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST) );
        SCH_CHECK_GL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) );

        // repeat texture coordinates when outside range [0,1]
        SCH_CHECK_GL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT) );
        SCH_CHECK_GL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT) );
    }
}
