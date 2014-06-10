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

#include <QFile>
#include <QTextStream>
#include <QFont>
#include <QDebug>

#include "sourcewidget.h"
#include "glslhighlighter.h"

SourceWidget::SourceWidget(QWidget *parent) :
    QTextEdit   (parent),
    modified_   (false)
{
    // --- set default colors ---

    QPalette pal = palette();

    pal.setColor(QPalette::Base, QColor(61,61,61));
    pal.setColor(QPalette::Text, QColor(200,200,200));

    setPalette(pal);

    // --- setup font ---

    QFont f("Monospace");
    f.setStyleHint(QFont::Monospace);
    setFont(f);
    setTabStopWidth(QFontMetrics(font()).width("    "));

    // attach syntax highlighter
    highlighter_ = new GlslHighlighter(document());

    // time after which a recompile() signal is issued
    timer_.setInterval(2000);
    timer_.setSingleShot(true);
    connect(&timer_, SIGNAL(timeout()), this, SIGNAL(recompile()));

    // change modified state on text-edit
    connect(this, SIGNAL(textChanged()), this, SLOT(slotTextChanged()) );

    // send cursor position to mainwindow
    connect(this, &QTextEdit::cursorPositionChanged, [=]()
    {
        auto c = textCursor();
        statusMessage( QString("%1 : %2")
                       .arg(c.blockNumber() + 1)
                       .arg(c.columnNumber() + 1) );
    });
}

SourceWidget::~SourceWidget()
{
    delete highlighter_;
}

bool SourceWidget::loadFile(const QString &fn)
{
    QFile file(fn);
    if (!file.open(QFile::ReadOnly))
    {
        qDebug() << "error opening file" << fn;
        return false;
    }

    QTextStream in(&file);

    setPlainText(in.readAll());

    modified_ = false;
    filename_ = fn;
    return true;
}

bool SourceWidget::saveFile(const QString &fn)
{
    QFile file(fn);
    if (!file.open(QFile::WriteOnly | QFile::Truncate))
    {
        qDebug() << "error writing file" << fn;
        return false;
    }

    QTextStream out(&file);

    out << toPlainText();

    modified_ = false;
    filename_ = fn;
    return true;
}

void SourceWidget::slotTextChanged()
{
    modified_ = true;

    timer_.start();
}
