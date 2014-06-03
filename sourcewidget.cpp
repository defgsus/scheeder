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

#include "sourcewidget.h"
#include "glslhighlighter.h"

SourceWidget::SourceWidget(QWidget *parent) :
    QTextEdit(parent)
{
    // set default colors

    QPalette pal = palette();

    pal.setColor(QPalette::Base, QColor(61,61,61));
    pal.setColor(QPalette::Text, QColor(200,200,200));

    setPalette(pal);

    // attach syntax highlighter
    highlighter_ = new GlslHighlighter(document());
}

SourceWidget::~SourceWidget()
{
    delete highlighter_;
}
