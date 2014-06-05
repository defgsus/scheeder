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

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QDoubleSpinBox>

#include "uniformwidgetfactory.h"
#include "glsl.h"

UniformWidgetFactory::UniformWidgetFactory(QObject * parent)
    :   QObject(parent)
{
}


QWidget * UniformWidgetFactory::getWidget(Uniform * uniform, QWidget *parent)
{
    // a container for the controls
    QWidget * w = new QWidget(parent);
    // set it's background
    w->setAutoFillBackground(true);
    w->setBackgroundRole(QPalette::AlternateBase);

    QVBoxLayout * lv = new QVBoxLayout(w);
    lv->setMargin(1);

        // name label
        QLabel * l = new QLabel(uniform->name, w);
        lv->addWidget(l);

        // controls
        QHBoxLayout * lh = new QHBoxLayout;
        lv->addLayout(lh);

            switch(uniform->type)
            {
                case GL_FLOAT_VEC3:
                {
                    QDoubleSpinBox * sb;
                    lh->addWidget( sb = getFloatWidget_(uniform, w, 0) );
                    lh->addWidget( sb = getFloatWidget_(uniform, w, 1) );
                    lh->addWidget( sb = getFloatWidget_(uniform, w, 2) );
                }
            }

    return w;
}

QDoubleSpinBox * UniformWidgetFactory::getFloatWidget_(
        Uniform * uniform, QWidget *parent, int vecIndex)
{
    auto w = new QDoubleSpinBox(parent);
    w->setRange(-100000000., 100000000.);

    connect(w,
        static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
        [=](double value)
        {
            uniform->floats[vecIndex] = value;
            uniformChanged(uniform);
        }
        );

    return w;
}
