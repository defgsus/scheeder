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
#include <QLabel>
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

    // status bar
    statusLabel_ = new QLabel(this);
    statusBar()->addWidget(statusLabel_);

    // render window
    QGLFormat glformat;
    glformat.setVersion(3,3);
    glformat.setDepth(true);
    renderer_ = new RenderWidget(this, glformat);
    renderer_->setShader(shader_);
    connect(renderer_, SIGNAL(shaderCompiled()), this, SLOT(slotShaderCompiled()));
    auto dw = getDockWidget_("opengl_window", tr("OpenGL window"));
    rendererDock_ = dw;
    dw->setWidget(renderer_);
    addDockWidget(Qt::LeftDockWidgetArea, dw);

    // vertex source
    editVert_ = new SourceWidget(this);
    editVert_->setText(appSettings->getValue("vertex_source").toString());
    editVert_->setModified(false);
    connect(editVert_, SIGNAL(textChanged()), this, SLOT(slotUpdateSourceTitles()));
    connect(editVert_, SIGNAL(recompile()), this, SLOT(slotSourceChanged()));
    connect(editVert_, SIGNAL(statusMessage(QString)), this, SLOT(slotStatusMessage(QString)));
    dw = editVertDock_ = getDockWidget_("vertex_source", tr("vertex source"));
    dw->setWidget(editVert_);
    addDockWidget(Qt::RightDockWidgetArea, dw);

    // fragment source
    editFrag_ = new SourceWidget(this);
    editFrag_->setText(appSettings->getValue("fragment_source").toString());
    editFrag_->setModified(false);
    connect(editFrag_, SIGNAL(textChanged()), this, SLOT(slotUpdateSourceTitles()));
    connect(editFrag_, SIGNAL(recompile()), this, SLOT(slotSourceChanged()));
    connect(editFrag_, SIGNAL(statusMessage(QString)), this, SLOT(slotStatusMessage(QString)));
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
    QAction * a;

    // --- file menu ---
    QMenu * m = new QMenu(tr("&File"), this);
    menuBar()->addMenu(m);
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
    a = new QAction(tr("&Exit"), this);
    m->addAction(a);
    connect(a, SIGNAL(triggered()), this, SLOT(close()));

    // --- model menu ---
    m = new QMenu(tr("&Model"), this);
    menuBar()->addMenu(m);

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


    // --- shader menu ---

    m = new QMenu(tr("&Shader"), this);
    menuBar()->addMenu(m);
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

    m->addSeparator();
    a = new QAction(tr("Go to &vertex editor"), this);
    a->setShortcut(Qt::ALT + Qt::Key_1);
    m->addAction(a);
    connect(a, SIGNAL(triggered()), editVert_, SLOT(setFocus()));
    a = new QAction(tr("Go to &fragment editor"), this);
    a->setShortcut(Qt::ALT + Qt::Key_2);
    m->addAction(a);
    connect(a, SIGNAL(triggered()), editFrag_, SLOT(setFocus()));

    // ------- textures ----------
    m = new QMenu(tr("&Textures"), this);
    menuBar()->addMenu(m);
    for (int i=0; i<SCH_MAX_TEXTURES; ++i)
    {
        a = new QAction(tr("Select image &%1").arg(i), this);
        m->addAction(a);
        connect(a, &QAction::triggered, [=](){ slotSelectImage(i); });
    }

    // --- options menu ---
    m = new QMenu(tr("&Options"), this);
    menuBar()->addMenu(m);
#ifndef SCH_USE_QT_OPENGLFUNC
    m->addAction(createRenderOptionAction_("doDrawCoords", "draw coordinates"));
#endif
    m->addAction(createRenderOptionAction_("doDepthTest", "depth test"));
    m->addAction(createRenderOptionAction_("doCullFace", "cull faces"));
    m->addAction(createRenderOptionAction_("doFrontFaceCCW", "front is counter-clockwise"));
    /* XXX: NOT WORKING YET
    m->addSeparator();
    a = new QAction(tr("external opengl view"), this);
    a->setShortcut(Qt::Key_F2);
    m->addAction(a);
    connect(a, &QAction::triggered, [=]()
    {
        if (renderer_->parent())
            renderer_->setParent(0);
        else
            renderer_->setParent(rendererDock_);
    });
    */


    // --- animation menu ---
    m = new QMenu(tr("&Animation"), this);
    menuBar()->addMenu(m);
    startAnim_ = a = new QAction(tr("start"), this);
    m->addAction(a);
    a->setShortcut(Qt::Key_F7);
    connect(a, SIGNAL(triggered()), renderer_, SLOT(startAnimation()));
    stopAnim_ = a = new QAction(tr("stop"), this);
    m->addAction(a);
    a->setShortcut(Qt::Key_F8);
    connect(a, SIGNAL(triggered()), renderer_, SLOT(stopAnimation()));

    // --- help menu ---
    m = new QMenu(tr("&Help"), this);
    menuBar()->addMenu(m);
    a = new QAction(tr("Quick &help"), this);
    m->addAction(a);
    connect(a, SIGNAL(triggered()), this, SLOT(slotHelp()));
    m->addSeparator();
    a = new QAction(tr("&About"), this);
    m->addAction(a);
    connect(a, SIGNAL(triggered()), this, SLOT(slotAboutBox()));
    a = new QAction(tr("About &Qt"), this);
    m->addAction(a);
    connect(a, SIGNAL(triggered()), this, SLOT(slotAboutQt()));

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

void MainWindow::slotStatusMessage(const QString &text)
{
    statusLabel_->setText(text);
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

    slotUpdateSourceTitles();
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
    slotUpdateSourceTitles();
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
    slotUpdateSourceTitles();

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
    slotUpdateSourceTitles();

    return true;
}

void MainWindow::slotUpdateSourceTitles()
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
    slotUpdateSourceTitles();
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
    slotUpdateSourceTitles();
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
    slotUpdateSourceTitles();
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

void MainWindow::slotHelp()
{
    QString attribs = QString(
                "vec4 %1;\t// vertex position<br/>"
                "vec3 %2;\t// vertex normal<br/>"
                "vec4 %3;\t// vertex color (rgba)<br/>"
                "vec2 %4;\t// vertex texture coordinate")
            .arg(appSettings->getValue("ShaderAttributes/position").toString())
            .arg(appSettings->getValue("ShaderAttributes/normal").toString())
            .arg(appSettings->getValue("ShaderAttributes/color").toString())
            .arg(appSettings->getValue("ShaderAttributes/texcoord").toString());

    QString uniforms = QString(
                "mat4 %1;\t// projection matrix<br/>"
                "mat4 %2;\t// transformation/view matrix<br/>"
                "float %3;\t// aspect ratio (width divided by height)<br/>"
                "float %4;\t// animation time in seconds")
            .arg(appSettings->getValue("ShaderUniforms/projection").toString())
            .arg(appSettings->getValue("ShaderUniforms/view").toString())
            .arg(appSettings->getValue("ShaderUniforms/aspect").toString())
            .arg(appSettings->getValue("ShaderUniforms/time").toString());

    QMessageBox::about(this, tr("Short help"),
        tr("<html>This program is basically a live shader editor.<br/>"
           "The important things to say here are the names of the "
           "application specific attributes and uniforms. These are:"
           "<p>vertex attributes:"
           "<pre>%1</pre></p>"
           "<p>uniforms:"
           "<pre>%2</pre></p>"
           "<p>All other uniforms that are defined and used in a shader "
           "are automatically exposed to the user-interface as widgets.<br/>"
           "The types of supported uniforms are currently:<br/>"
           "<b>float, vec2, vec3, vec4</b> and <b>sampler2D</b>.</p>"
           "<p>The Renderwindow listens to some mouse commands to adjust the "
           "transformation matrix, these are: <b>left-drag</b> to rotate and "
           "<b>right-drag</b> to change the distance to the origin.</p>"
           "</html>"
           ).arg(attribs).arg(uniforms)
    );
}

void MainWindow::slotAboutBox()
{
    QMessageBox::about(this,
        tr("About Scheeder"),
        tr("<html>Little demo application for programming OpenGL and GLSL in Qt."
           "<p>(c) 2014 stefan.berke @ modular-audio-graphics.com</p>"
           "<p>This is free software - with free as in freedom -<br/>"
           "licensed under the GNU General Public License 3.0</p>"
           "<p>The source is available at<br/>"
           "<a href=\"https://github.com/defgsus/scheeder\">github.com/defgsus/scheeder</a></p>"
           "<p>built %1 %2</p></html>")
                       .arg(__DATE__)
                       .arg(__TIME__)
        );
}

void MainWindow::slotAboutQt()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::slotSelectImage(uint index)
{
    QString fn =
        QFileDialog::getOpenFileName(this,
            tr("Choose image file for slot %1").arg(index),
            appSettings->getValue("image_path").toString(),
            tr("Images (*.png *.xpm *.jpg *.bmp);"));

    if (!fn.isEmpty())
    {
        // keep permanent link on image
        appSettings->setValue(QString("image%1").arg(index), fn);
        // tell renderwidget
        renderer_->setImage(index, fn);
        // store last directory
        appSettings->setValue("image_path", QDir(fn).absolutePath());
    }
}
