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


RenderWidget::RenderWidget(QWidget *parent) :
    Basic3DWidget   (parent),
    model_          (0),
    shader_         (0),
    requestCompile_ (false)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(256,256);

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

    update();
}

void RenderWidget::setModel(Model * m)
{
    if (model_)
        delete model_;

    model_ = m;

    update();
}

void RenderWidget::setShader(Glsl *s)
{
    if (shader_)
        delete shader_;

    shader_ = s;

    update();
}

void RenderWidget::requestCompileShader()
{
    requestCompile_ = true;
    update();
}

void RenderWidget::paintGL()
{
    applyOptions_();

    Basic3DWidget::paintGL();

    if (doDrawCoords_)
        drawCoords_(10);

    if (shader_)
    {
        if (requestCompile_)
        {
            requestCompile_ = false;
            shader_->compile();
            emit shaderCompiled();
        }
        shader_->activate();
        shader_->sendUniforms();
    }

    if (model_)
        model_->draw();

    if (shader_)
        shader_->deactivate();
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

