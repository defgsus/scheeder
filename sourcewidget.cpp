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
#include <QCompleter>
#include <QStringListModel>
#include <QAbstractItemView>
#include <QScrollBar>
#include <QKeyEvent>

#include "sourcewidget.h"
#include "glslhighlighter.h"
#include "glslsyntax.h"

SourceWidget::SourceWidget(QWidget *parent) :
    QPlainTextEdit   (parent),
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
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(slotCursorChanged()));

    // ---- setup auto complete ----

    // create stringlist from all glsl words
    GlslSyntax syntax;
    QStringList words;
    words << syntax.functions() << syntax.keywords()
          << syntax.reservedKeywords() << syntax.variables();
    words.sort(Qt::CaseInsensitive);

    // create completer
    completer_ = new QCompleter(words, this);
    completer_->setCaseSensitivity(Qt::CaseInsensitive);
    completer_->setCompletionMode(QCompleter::PopupCompletion);
    completer_->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    completer_->setWrapAround(true);
    completer_->setWidget(this);
    connect(completer_, SIGNAL(activated(QString)), this, SLOT(slotInsertCompletion(QString)));
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

void SourceWidget::keyPressEvent(QKeyEvent * e)
{
    if ((e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
        && completer_->popup()->isVisible()
        && !completer_->currentCompletion().isEmpty())
    {
        e->accept();
        slotInsertCompletion(completer_->currentCompletion());
    }
    else
    QPlainTextEdit::keyPressEvent(e);
}

void SourceWidget::slotCursorChanged()
{
    QTextCursor c = textCursor();

    statusMessage( QString("%1 : %2")
                   .arg(c.blockNumber() + 1)
                   .arg(c.columnNumber() + 1) );

    if (completer_->popup()->isVisible())
        completer_->popup()->hide();
}

void SourceWidget::slotTextChanged()
{
    modified_ = true;

    QTextCursor c = textCursor();
    c.select(QTextCursor::WordUnderCursor);
    QString word = c.selectedText();

    if (!word.isEmpty())
        performCompletion_(word);

    timer_.start();
}

void SourceWidget::slotInsertCompletion(const QString &word)
{
    QTextCursor c = textCursor();
    // characters left to insert from whole word
    int numChars = word.length() - completer_->completionPrefix().length();
    int pos = c.position();
    c.insertText(word.right(numChars));

    c.setPosition(pos);
    c.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);

    setTextCursor(c);
}

void SourceWidget::performCompletion_(const QString &word)
{
    // scan the strings
    completer_->setCompletionPrefix(word);

    if (completer_->completionCount() > 0)
    {
        // break if the only word was fully written by user
        if (completer_->completionCount() == 1 &&
            completer_->currentCompletion() == word)
            return;

        // get the rectangle for the completion popup
        QRect rect = cursorRect();
        rect.setWidth(completer_->popup()->sizeHintForColumn(0)
                    + completer_->popup()->verticalScrollBar()->sizeHint().width() );
        // select first entry
        completer_->popup()->setCurrentIndex(
                    completer_->completionModel()->index(0,0) );
        // show popup
        completer_->complete(rect);
    }
}
