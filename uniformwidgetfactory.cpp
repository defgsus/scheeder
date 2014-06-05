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

bool UniformWidgetFactory::isSupported(GLenum type) const
{
    return type == GL_FLOAT
        || type == GL_FLOAT_VEC2
        || type == GL_FLOAT_VEC3
        || type == GL_FLOAT_VEC4;
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
        QLabel * l = new QLabel(uniform->name(), w);
        lv->addWidget(l);

        // controls
        QHBoxLayout * lh = new QHBoxLayout;
        lv->addLayout(lh);

            switch(uniform->type())
            {
                case GL_FLOAT:
                {
                    QDoubleSpinBox * sb;
                    lh->addWidget( sb = getFloatWidget_(uniform, w, 0) );
                }
                break;
                case GL_FLOAT_VEC2:
                {
                    QDoubleSpinBox * sb;
                    lh->addWidget( sb = getFloatWidget_(uniform, w, 0) );
                    lh->addWidget( sb = getFloatWidget_(uniform, w, 1) );
                }
                break;
                case GL_FLOAT_VEC3:
                {
                    QDoubleSpinBox * sb;
                    lh->addWidget( sb = getFloatWidget_(uniform, w, 0) );
                    lh->addWidget( sb = getFloatWidget_(uniform, w, 1) );
                    lh->addWidget( sb = getFloatWidget_(uniform, w, 2) );
                }
                break;
                case GL_FLOAT_VEC4:
                {
                    QDoubleSpinBox * sb;
                    lh->addWidget( sb = getFloatWidget_(uniform, w, 0) );
                    lh->addWidget( sb = getFloatWidget_(uniform, w, 1) );
                    lh->addWidget( sb = getFloatWidget_(uniform, w, 2) );
                    lh->addWidget( sb = getFloatWidget_(uniform, w, 3) );
                }
                break;
            }

    return w;
}

QDoubleSpinBox * UniformWidgetFactory::getFloatWidget_(
        Uniform * uniform, QWidget *parent, int vecIndex)
{
    // prepare a nice spinbox
    auto w = new QDoubleSpinBox(parent);
    w->setRange(-100000000., 100000000.);

    w->setValue(uniform->floats[vecIndex]);

    /*  This shows how to connect to actual functions rather than Qt slots,
        which is possible since Qt 5.
        We further use a C++11 lambda instead of a function because it makes
        life a bit easier here. This way, each spinbox widget connects
        to a unique (anonymous) function which changes the one Uniform struct
        assigned to the widet. Without lambdas, the way would be to attach
        the Uniform to the userData of the widget and write a generic slot.

        One slight caveat of connecting to functions instead of slots is that
        we can't use the SIGNAL() macro anymore. So the signal has to be the
        actual function too. In the case of the QDoubleSpinBox this is expressed
        as: &QDoubleSpinBox::valueChanged
        BUT there are actually two overloaded versions of valueChanged(), one
        with a double parameter and one with a QString. The C++ parser needs to
        know which one to use and this is only possible with this rather
        complicated looking static_cast below.
        It's, however, not needed for functions that are not overloaded.
    */
    connect(w,
        static_cast<void(QDoubleSpinBox::*)(double)>( &QDoubleSpinBox::valueChanged ),
        [=](double value)
        {
            uniform->floats[vecIndex] = value;
            uniformChanged(uniform);
        }
    );
    /*  One other thing worth noting:
        Connecting directly to functions bypasses the signal/slot mechanism and
        may not be safe to use in a multi-threaded environment! */

    return w;
}
