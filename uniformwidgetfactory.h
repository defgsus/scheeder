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

#ifndef UNIFORMWIDGETFACTORY_H
#define UNIFORMWIDGETFACTORY_H

#include <QObject>

// forwards
class QWidget;
class QDoubleSpinBox;
class Uniform;

class UniformWidgetFactory : public QObject
{
    Q_OBJECT
public:
    explicit UniformWidgetFactory(QObject * parent);

    QWidget * getWidget(Uniform * uniform, QWidget * parent);

signals:

    void uniformChanged(Uniform *);

private:

    QDoubleSpinBox * getFloatWidget_(Uniform *, QWidget * parent, int vecIndex);
};

#endif // UNIFORMWIDGETFACTORY_H