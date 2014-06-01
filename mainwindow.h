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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

// forward declarations
namespace Ui { class MainWindow; }
class QDockWidget;
class RenderWidget;
class SourceWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    /** Creates all the menu actions */
    void createMainMenu_();

    /** Returns a new dock-widget with default settings */
    QDockWidget * getDockWidget_(const QString& obj_id, const QString& title);


    Ui::MainWindow * ui_;

    RenderWidget * renderer_;
    SourceWidget * editVert_, * editFrag_;
};

#endif // MAINWINDOW_H
