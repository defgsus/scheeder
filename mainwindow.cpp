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
#include <QMenu>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "renderwidget.h"
#include "sourcewidget.h"
#include "appsettings.h"
#include "modelfactory.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow     (parent),
    ui_             (new Ui::MainWindow)
{
    ui_->setupUi(this);

    // render window
    renderer_ = new RenderWidget(this);
    auto dw = getDockWidget_("opengl_window", tr("OpenGL window"));
    dw->setWidget(renderer_);
    addDockWidget(Qt::LeftDockWidgetArea, dw);

    // vertex source
    editVert_ = new SourceWidget(this);
    editVert_->setText(appSettings->getValue("vertex_source").toString());
    dw = getDockWidget_("vertex_source", tr("vertex source"));
    dw->setWidget(editVert_);
    addDockWidget(Qt::RightDockWidgetArea, dw);

    // fragment source
    editFrag_ = new SourceWidget(this);
    editFrag_->setText(appSettings->getValue("fragment_source").toString());
    dw = getDockWidget_("fragment_source", tr("fragment source"));
    dw->setWidget(editFrag_);
    addDockWidget(Qt::RightDockWidgetArea, dw);

    createMainMenu_();
}

QDockWidget * MainWindow::getDockWidget_(const QString &obj_id, const QString &title)
{
    auto dw = new QDockWidget(title, this);
    // set an object name in case we want to store the view settings
    dw->setObjectName(obj_id);

    // disable close feature
    dw->setFeatures(QDockWidget::DockWidgetMovable);
    // make it large XXX seems to be ignored :(
    dw->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    return dw;
}

void MainWindow::createMainMenu_()
{
    // --- file menu ---
    QMenu * m = new QMenu(tr("&File"), this);
    QAction * a = new QAction(tr("&Close"), this);
    m->addAction(a);
    connect(a, SIGNAL(triggered()), this, SLOT(close()));

    menuBar()->addMenu(m);

    // --- model menu ---
    m = new QMenu(tr("&Model"), this);
    a = new QAction(tr("Create &Box"), this);
    m->addAction(a);
    connect(a, &QAction::triggered, [=]()
    {
        ModelFactory f;
        Model * m = f.createCube(2);
        renderer_->setModel(m);
    });

    menuBar()->addMenu(m);
}


MainWindow::~MainWindow()
{
    delete ui_;
}
