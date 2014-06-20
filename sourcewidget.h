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

#ifndef SOURCEWIDGET_H
#define SOURCEWIDGET_H

#include <QPlainTextEdit>
#include <QTimer>

class QSyntaxHighlighter;
class QCompleter;

/** A QTextEdit for the needs of GLSL programming.
 */
class SourceWidget : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit SourceWidget(QWidget *parent = 0);

    ~SourceWidget();

    /** Returns the last used filename from load or save, or an empty string */
    const QString& filename() const { return filename_; }

    /** Returns true, if the text has been modified since last load/save. */
    bool modified() const { return modified_; }

    /** Sets the modified flag */
    void setModified(bool modified) { modified_ = modified; }

signals:

    /** Signal is issued with a little delay after text was modified. */
    void recompile();

    /** Signal for updating the MainWindow's status bar message */
    void statusMessage(const QString&);

protected:

    /** Needed to catch Return key for auto-completer */
    void keyPressEvent(QKeyEvent *);


public slots:

    /** Loads the file's contents and sets the filename().
        Returns success. */
    bool loadFile(const QString& filename);

    /** Saves the contents to a text file and sets the filename().
        Returns success. */
    bool saveFile(const QString& filename);

private slots:

    void slotCursorChanged();
    void slotTextChanged();
    void slotInsertCompletion(const QString& word);

private:

    /** Runs the auto-completer popup on the partial word */
    void performCompletion_(const QString& word);

    /** Associated highlighter */
    QSyntaxHighlighter * highlighter_;
    QCompleter * completer_;

    QString filename_;
    bool modified_;

    QTimer timer_;
};

#endif // SOURCEWIDGET_H
