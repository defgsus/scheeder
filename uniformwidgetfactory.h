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


/** This class creates widgets for controlling uniforms.
 */
class UniformWidgetFactory : public QObject
{
    Q_OBJECT
public:
    explicit UniformWidgetFactory(QObject * parent);

    /** Returns true if a widget can be created for the uniform type. */
    bool isSupported(unsigned int type) const;

    /** Creates a widget for the given uniform.
        The returned widget will be a child of @p parent and
        will contain a number of sub-widgets in a sub-layout,
        depending on the uniform type.
        Editing the widgets will directly change the contents of
        the passed Uniform struct and also issue a uniformChanged() signal.
        @note The returned widget's lifetime must not exceed the
        lifetime of the uniform! This would probably lead to segfaults. */
    QWidget * getWidget(Uniform * uniform, QWidget * parent);

signals:

    /** This signal is send whenever the Uniform is changed by an edit action. */
    void uniformChanged(Uniform *);

private:

    QDoubleSpinBox * getFloatWidget_(Uniform *, QWidget * parent, int vecIndex);
};

#endif // UNIFORMWIDGETFACTORY_H
