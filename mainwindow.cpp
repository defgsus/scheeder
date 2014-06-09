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

#include <QDebug>
#include <QDockWidget>
#include <QMenu>
#include <QTextBrowser>
#include <QCloseEvent>
#include <QLayout>
#include <QFileDialog>
#include <QMessageBox>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "renderwidget.h"
#include "sourcewidget.h"
#include "appsettings.h"
#include "modelfactory.h"
#include "model.h"
#include "glsl.h"
#include "uniformwidgetfactory.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow     (parent),
    ui_             (new Ui::MainWindow),
    shader_         (new Glsl)
{
    setObjectName("MainWindow");

    // uniform factory/controller
    uniFactory_ = new UniformWidgetFactory(this);

    createWidgets_();
    createMainMenu_();

    restoreWidgetsGeometry_();

    if (doAutoCompile_)
        compileShader();
}

MainWindow::~MainWindow()
{
    delete ui_;
}

QDockWidget * MainWindow::getDockWidget_(const QString &obj_id, const QString &title)
{
    auto dw = new QDockWidget(title, this);
    // set an object name because we want to store the layout settings
    dw->setObjectName(obj_id);

    // disable close feature
    dw->setFeatures(QDockWidget::DockWidgetMovable);
    // make it large XXX seems to be ignored :(
    dw->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    return dw;
}

void MainWindow::createWidgets_()
{
    ui_->setupUi(this);

    // render window
    QGLFormat glformat;
    glformat.setVersion(3,3);
    glformat.setDepth(true);
    renderer_ = new RenderWidget(this, glformat);
    renderer_->setShader(shader_);
    connect(renderer_, SIGNAL(shaderCompiled()), this, SLOT(slotShaderCompiled()));
    auto dw = getDockWidget_("opengl_window", tr("OpenGL window"));
    dw->setWidget(renderer_);
    addDockWidget(Qt::LeftDockWidgetArea, dw);

    // vertex source
    editVert_ = new SourceWidget(this);
    editVert_->setText(appSettings->getValue("vertex_source").toString());
    editVert_->setModified(false);
    connect(editVert_, SIGNAL(textChanged()), this, SLOT(slotSourceChanged()));
    dw = editVertDock_ = getDockWidget_("vertex_source", tr("vertex source"));
    dw->setWidget(editVert_);
    addDockWidget(Qt::RightDockWidgetArea, dw);

    // fragment source
    editFrag_ = new SourceWidget(this);
    editFrag_->setText(appSettings->getValue("fragment_source").toString());
    editFrag_->setModified(false);
    connect(editFrag_, SIGNAL(textChanged()), this, SLOT(slotSourceChanged()));
    dw = editFragDock_ = getDockWidget_("fragment_source", tr("fragment source"));
    dw->setWidget(editFrag_);
    addDockWidget(Qt::RightDockWidgetArea, dw);

    // log view
    log_ = new QTextBrowser(this);
    dw = getDockWidget_("log_view", tr("log"));
    dw->setWidget(log_);
    addDockWidget(Qt::LeftDockWidgetArea, dw);

    // uniform edit space
    uniEdit_ = new QWidget(this);
    auto l = new QVBoxLayout(uniEdit_);
    l->setMargin(1);
    dw = getDockWidget_("uni_edit", tr("shader uniforms"));
    dw->setWidget(uniEdit_);
    addDockWidget(Qt::BottomDockWidgetArea, dw);
}

void MainWindow::createMainMenu_()
{
    // --- file menu ---
    QMenu * m = new QMenu(tr("&File"), this);
    QAction * a = new QAction(tr("&Exit"), this);
    m->addAction(a);
    connect(a, SIGNAL(triggered()), this, SLOT(close()));
    m->addSeparator();
    saveAll_ = a = new QAction(tr("&Save all"), this);
    a->setShortcut(Qt::CTRL + Qt::Key_S);
    m->addAction(a);
    connect(a, SIGNAL(triggered()), renderer_, SLOT(stopAnimation()));
    connect(a, SIGNAL(triggered()), this, SLOT(slotSaveShader()));
    a = new QAction(tr("&Save all as ..."), this);
    a->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_S);
    m->addAction(a);
    connect(a, SIGNAL(triggered()), renderer_, SLOT(stopAnimation()));
    connect(a, SIGNAL(triggered()), this, SLOT(slotSaveShaderAs()));
    a = new QAction(tr("Save &vertex source as ..."), this);
    m->addAction(a);
    connect(a, SIGNAL(triggered()), renderer_, SLOT(stopAnimation()));
    connect(a, SIGNAL(triggered()), this, SLOT(slotSaveVertexShaderAs()));
    a = new QAction(tr("Save &fragment source as ..."), this);
    m->addAction(a);
    connect(a, SIGNAL(triggered()), renderer_, SLOT(stopAnimation()));
    connect(a, SIGNAL(triggered()), this, SLOT(slotSaveFragmentShaderAs()));
    m->addSeparator();
    a = new QAction(tr("&Load all"), this);
    a->setShortcut(Qt::CTRL + Qt::Key_L);
    m->addAction(a);
    connect(a, SIGNAL(triggered()), renderer_, SLOT(stopAnimation()));
    connect(a, SIGNAL(triggered()), this, SLOT(slotLoadShader()));
    a = new QAction(tr("Load v&ertex source"), this);
    m->addAction(a);
    connect(a, SIGNAL(triggered()), renderer_, SLOT(stopAnimation()));
    connect(a, SIGNAL(triggered()), this, SLOT(slotLoadVertexShader()));
    a = new QAction(tr("Load f&ragment source"), this);
    m->addAction(a);
    connect(a, SIGNAL(triggered()), renderer_, SLOT(stopAnimation()));
    connect(a, SIGNAL(triggered()), this, SLOT(slotLoadFragmentShader()));

    menuBar()->addMenu(m);

    // --- model menu ---
    m = new QMenu(tr("&Model"), this);

    // create a group to make only one model checkable at a time
    auto group = new QActionGroup(this);
    connect(group, SIGNAL(triggered(QAction*)), this, SLOT(slotCreateModel()));

    modelBox_ = a = new QAction(tr("Create &Box"), this);
    a->setCheckable(true);
    m->addAction(a);
    group->addAction(a);
    modelSphere_ = a = new QAction(tr("Create UV-&Sphere"), this);
    a->setCheckable(true);
    m->addAction(a);
    group->addAction(a);
    modelPot_ = a = new QAction(tr("Create famous &Teapot"), this);
    a->setCheckable(true);
    m->addAction(a);
    group->addAction(a);

    m->addSeparator();
    doGroupVertices_ = a = new QAction(tr("group vertices"), this);
    a->setCheckable(true);
    m->addAction(a);
    connect(a, SIGNAL(triggered()), this, SLOT(slotCreateModel()));

    menuBar()->addMenu(m);


    // --- shader menu ---

    m = new QMenu(tr("&Shader"), this);
    a = new QAction(tr("&Compile"), this);
    a->setShortcut(Qt::ALT + Qt::Key_C);
    m->addAction(a);
    connect(a, SIGNAL(triggered()), this, SLOT(compileShader()));

    a = new QAction(tr("&Automatic compile"), this);
    a->setShortcut(Qt::ALT + Qt::SHIFT + Qt::Key_C);
    doAutoCompile_ = a;
    a->setCheckable(true);
    a->setChecked(appSettings->getValue("auto_compile").toBool());
    m->addAction(a);
    connect(a, &QAction::triggered, [=](bool check)
    {
        appSettings->setValue("auto_compile", check);
    });

    menuBar()->addMenu(m);

    // --- options menu ---
    m = new QMenu(tr("&Options"), this);
    m->addAction(createRenderOptionAction_("doDrawCoords", "draw coordinates"));
    m->addAction(createRenderOptionAction_("doDepthTest", "depth test"));
    m->addAction(createRenderOptionAction_("doCullFace", "cull faces"));
    m->addAction(createRenderOptionAction_("doFrontFaceCCW", "front is counter-clockwise"));

    menuBar()->addMenu(m);

    // --- toolbar ---

    startAnim_ = a = new QAction(tr("start animation"), this);
    ui_->mainToolBar->addAction(a);
    a->setShortcut(Qt::Key_F7);
    connect(a, SIGNAL(triggered()), renderer_, SLOT(startAnimation()));
    stopAnim_ = a = new QAction(tr("stop animation"), this);
    ui_->mainToolBar->addAction(a);
    a->setShortcut(Qt::Key_F8);
    connect(a, SIGNAL(triggered()), renderer_, SLOT(stopAnimation()));

}

QAction * MainWindow::createRenderOptionAction_(const QString& option, const QString& name)
{
    // create a checkable action
    QAction * a = new QAction(name, this);
    a->setCheckable(true);
    // get value from settings
    a->setChecked(appSettings->getValue("RenderSettings/"+option).toBool());
    // on click: change settings and tell renderer to reload settings
    connect(a, &QAction::triggered, [=](bool checked)
    {
        appSettings->setValue("RenderSettings/"+option, checked);
        renderer_->reconfigure();
    });
    // return a nice handy action
    return a;
}



// ---------------- ACTION / EXECUTION -------------------------

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


void MainWindow::closeEvent(QCloseEvent * e)
{
    // store the source texts
    appSettings->setValue("vertex_source", editVert_->toPlainText());
    appSettings->setValue("fragment_source", editFrag_->toPlainText());

    saveWidgetsGeometry_();

    e->accept();
}


void MainWindow::updateUniformWidgets_()
{
    // delete the previous uniform widgets
    deleteUniformWidgets_();

    if (shader_->numUniforms() <= 0)
        return;

    // create the uniform widgets

    for (size_t i=0; i<shader_->numUniforms(); ++i)
    {
        // get the uniform struct
        Uniform * u = shader_->getUniform(i);
        // see if a widget can be created
        if (!uniFactory_->isSupported(u->type()))
            continue;
        // create a widget for the uniform
        QWidget * w = uniFactory_->getWidget(u, uniEdit_);
        // add to layout
        uniEdit_->layout()->addWidget(w);
    }

    // add a 'stretch' at the bottom to push the widgets together
    // NOTE: base class QLayout unfortunately does not have addStretch()
    // but we know it's a QVBoxLayout
    auto l = qobject_cast<QVBoxLayout*>(uniEdit_->layout());
    if (l) l->addStretch(2);
}

void MainWindow::deleteUniformWidgets_()
{
    // delete layout items
    QLayoutItem *child;
    while ((child = uniEdit_->layout()->takeAt(0)) != 0)
        delete child;

    // delete widgets
    QObjectList childs = uniEdit_->children();
    for (auto i = childs.begin(); i!=childs.end(); ++i)
    {
        if (QWidget * w = qobject_cast<QWidget*>(*i))
            w->deleteLater();
    }
}


void MainWindow::slotSourceChanged()
{
    updateSourceTitles_();
    if (doAutoCompile_->isChecked())
        compileShader();
}

void MainWindow::compileShader()
{
    // disconnect uniform updates
    disconnect(uniFactory_, SIGNAL(uniformChanged(Uniform*)),
                    this, SLOT(slotUniformChanged(Uniform*)));
    // gray-out uniform editor
    uniEdit_->setEnabled(false);

    shader_->setVertexSource(editVert_->toPlainText());
    shader_->setFragmentSource(editFrag_->toPlainText());

    // tell renderer to compile the shader
    renderer_->requestCompileShader();
}

void MainWindow::slotShaderCompiled()
{
    log_->setText(shader_->log());

    // connect uniform updates
    if (shader_->ready())
    {
        updateUniformWidgets_();
        connect(uniFactory_, SIGNAL(uniformChanged(Uniform*)),
                        this, SLOT(slotUniformChanged(Uniform*)));
        uniEdit_->setEnabled(true);
    }
}

void MainWindow::slotUniformChanged(Uniform * )
{
    //qDebug() << "changed uniform" << u->name() << u->floats[0] << u->floats[1] << u->floats[2];
    renderer_->update();
}



// -------------------- SOURCE LOAD/SAVE ----------------------------

void MainWindow::slotSaveShader()
{
    if (editVert_->modified())
    {
        if (editVert_->filename().isEmpty())
        {
            if (!slotSaveVertexShaderAs())
                return;
        }
        else
            editVert_->saveFile(editVert_->filename());
    }

    if (editFrag_->modified())
    {
        if (editFrag_->filename().isEmpty())
        {
            if (!slotSaveFragmentShaderAs())
                return;
        }
        else
            editFrag_->saveFile(editFrag_->filename());
    }

    updateSourceTitles_();
}

void MainWindow::slotSaveShaderAs()
{
choose_again:
    QString fn =
        QFileDialog::getSaveFileName(this,
            tr("Save shader source (.vert & .frag)"),
            appSettings->getValue("source_path").toString()
            );

    // aborted?
    if (fn.isEmpty())
        return;

    QString vname = fn + ".vert",
            fname = fn + ".frag";

    // see if existing

    bool vexists = QFile::exists(vname),
         fexists = QFile::exists(fname);

    if (vexists || fexists)
    {
        QString text;
        if (vexists && fexists)
            text = tr("%1 and %2 already exist.\nDo you want to overwrite them?")
                    .arg(vname).arg(fname);
        else
            text = tr("%1 already exists.\nDo you want to overwrite it?")
                    .arg(vexists? vname : fname);
        // ask user what to do
        QMessageBox::StandardButton ret =
            QMessageBox::question(this, tr("confirm overwrite"), text,
                    QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                    QMessageBox::No);

        if (ret == QMessageBox::Cancel)
            return;
        if (ret == QMessageBox::No)
            goto choose_again;
    }

    // save both
    editVert_->saveFile(vname);
    editFrag_->saveFile(fname);

    // store current file path
    appSettings->setValue("source_path", QDir(fn).absolutePath());
    // update titles
    updateSourceTitles_();
}

bool MainWindow::slotSaveVertexShaderAs()
{
    QString fn =
        QFileDialog::getSaveFileName(this,
            tr("Save vertex shader source"),
            // choose stored path or previous filename
            editVert_->filename().isEmpty() ?
                    appSettings->getValue("source_path").toString()
                :   editVert_->filename()
            );

    // aborted?
    if (fn.isEmpty())
        return false;

    if (!editVert_->saveFile(fn))
        return true;

    // store current file path
    appSettings->setValue("source_path", QDir(fn).absolutePath());
    // update titles
    updateSourceTitles_();

    return true;
}

bool MainWindow::slotSaveFragmentShaderAs()
{
    QString fn =
        QFileDialog::getSaveFileName(this,
            tr("Save fragment shader source"),
            editFrag_->filename().isEmpty() ?
                    appSettings->getValue("source_path").toString()
                :   editFrag_->filename()
            );

    // aborted?
    if (fn.isEmpty())
        return false;

    if (!editFrag_->saveFile(fn))
        return true;

    // store current file path
    appSettings->setValue("source_path", QDir(fn).absolutePath());
    // update titles
    updateSourceTitles_();

    return true;
}

void MainWindow::updateSourceTitles_()
{
    // enable save all only when source is modified
    saveAll_->setEnabled(editVert_->modified() || editFrag_->modified());

    QString title = tr("vertex source");
    if (editVert_->modified())
        title += " *";
    if (!editVert_->filename().isEmpty())
        title += " [" + editVert_->filename() + "]";
    editVertDock_->setWindowTitle(title);

    title = tr("fragment source");
    if (editFrag_->modified())
        title += " *";
    if (!editFrag_->filename().isEmpty())
        title += " [" + editFrag_->filename() + "]";
    editFragDock_->setWindowTitle(title);
}

void MainWindow::slotLoadShader()
{
    QStringList fn =
        QFileDialog::getOpenFileNames(this,
            tr("Load vertex & fragment shader source"),
            appSettings->getValue("source_path").toString()
            );

    // aborted?
    if (fn.isEmpty())
        return;

    for (int i=0; i<2; ++i)
    if (i < fn.size())
    {
        if (fn[i].contains(".vert"))
            editVert_->loadFile(fn[i]);
        else if (fn[i].contains(".frag"))
            editFrag_->loadFile(fn[i]);
    }

    // store current file path
    appSettings->setValue("source_path", QDir(fn[0]).absolutePath());
    // update titles
    updateSourceTitles_();
}

void MainWindow::slotLoadVertexShader()
{
    QString fn =
        QFileDialog::getOpenFileName(this,
            tr("Load vertex shader source"),
            editVert_->filename().isEmpty() ?
                    appSettings->getValue("source_path").toString()
                :   editVert_->filename()
            );

    // aborted?
    if (fn.isEmpty())
        return;

    editVert_->loadFile(fn);

    // store current file path
    appSettings->setValue("source_path", QDir(fn).absolutePath());
    // update titles
    updateSourceTitles_();
}

void MainWindow::slotLoadFragmentShader()
{
    QString fn =
        QFileDialog::getOpenFileName(this,
            tr("Load fragment shader source"),
            editFrag_->filename().isEmpty() ?
                    appSettings->getValue("source_path").toString()
                :   editFrag_->filename()
            );

    // aborted?
    if (fn.isEmpty())
        return;

    editFrag_->loadFile(fn);

    // store current file path
    appSettings->setValue("source_path", QDir(fn).absolutePath());
    // update titles
    updateSourceTitles_();
}

void MainWindow::slotCreateModel()
{
    float scale = 5.f;

    ModelFactory f;
    Model * m;

    if (modelBox_->isChecked())
        m = f.createCube(scale);
    else if (modelSphere_->isChecked())
        m = f.createUVSphere(scale, 20, 20);
    else
        m = f.createTeapot(scale/5);

    if (!doGroupVertices_->isChecked())
        m->unGroupVertices();

    renderer_->setModel(m);
}
