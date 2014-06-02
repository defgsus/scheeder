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
#include "model.h"

RenderWidget::RenderWidget(QWidget *parent) :
    Basic3DWidget   (parent),
    model_          (0)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(256,256);
}

RenderWidget::~RenderWidget()
{
    if (model_)
        delete model_;
}

void RenderWidget::setModel(Model * m)
{
    if (model_)
        delete model_;

    model_ = m;

    repaint();
}

void RenderWidget::paintGL()
{
    Basic3DWidget::paintGL();

    drawCoords_(10);

    if (model_)
        model_->draw();
}


