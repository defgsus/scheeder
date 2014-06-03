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
#include <QTextBrowser>
#include <QCloseEvent>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "renderwidget.h"
#include "sourcewidget.h"
#include "appsettings.h"
#include "modelfactory.h"
#include "glsl.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow     (parent),
    ui_             (new Ui::MainWindow),
    shader_         (new Glsl)
{
    setObjectName("MainWindow");

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

    // log view
    log_ = new QTextBrowser(this);
    dw = getDockWidget_("log_view", tr("log"));
    dw->setWidget(log_);
    addDockWidget(Qt::LeftDockWidgetArea, dw);

    createMainMenu_();

    restoreWidgetsGeometry_();
}

MainWindow::~MainWindow()
{
    delete shader_;
    delete ui_;
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

    // --- shader menu ---
    m = new QMenu(tr("&Shader"), this);
    a = new QAction(tr("&Compile"), this);
    m->addAction(a);
    connect(a, SIGNAL(triggered()), this, SLOT(compileShader()));

    menuBar()->addMenu(m);

}

void MainWindow::saveWidgetsGeometry_()
{
    appSettings->beginGroup("Geometry");
    appSettings->setLayout(this);
    appSettings->setValue("windowState", saveState());
    appSettings->endGroup();
}

void MainWindow::restoreWidgetsGeometry_()
{
    appSettings->beginGroup("Geometry");
    appSettings->getLayout(this);
    if (appSettings->contains("windowState"))
        restoreState(appSettings->getValue("windowState").toByteArray());
    appSettings->endGroup();
}


// ---------------- ACTION / EXECUTION -------------------------


void MainWindow::closeEvent(QCloseEvent * e)
{
    // store the source texts
    appSettings->setValue("vertex_source", editVert_->toPlainText());
    appSettings->setValue("fragment_source", editFrag_->toPlainText());

    saveWidgetsGeometry_();

    e->accept();
}




void MainWindow::compileShader()
{
    shader_->setVertexSource(editVert_->toPlainText());
    shader_->setFragmentSource(editFrag_->toPlainText());

    shader_->compile();
    log_->setText(shader_->log());
}
