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

#include <QDockWidget>
#include <QTextEdit>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "renderwidget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow     (parent),
    ui_             (new Ui::MainWindow)
{
    ui_->setupUi(this);

    // render window
    auto dw = getDockWidget_("opengl_window", tr("OpenGL window"));
    renderer_ = new RenderWidget(dw);
    dw->setWidget(renderer_);
    addDockWidget(Qt::LeftDockWidgetArea, dw);

    // vertex source
    dw = getDockWidget_("vertex_source", tr("vertex source"));
    editVert_ = new QTextEdit(this);
    dw->setWidget(editVert_);
    addDockWidget(Qt::RightDockWidgetArea, dw);

    // fragment source
    dw = getDockWidget_("fragment_source", tr("fragment source"));
    editFrag_ = new QTextEdit(this);
    dw->setWidget(editFrag_);
    addDockWidget(Qt::RightDockWidgetArea, dw);

}

QDockWidget * MainWindow::getDockWidget_(const QString &obj_id, const QString &title)
{
    auto dw = new QDockWidget(title, this);
    dw->setObjectName(obj_id);
    dw->setFeatures(QDockWidget::DockWidgetMovable);
    dw->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    return dw;
}

MainWindow::~MainWindow()
{
    delete ui_;
}
