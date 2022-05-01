#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSurfaceFormat>
#include <QDebug>

#include "renderwindow.h"
#include "Core/Logger.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow)
{
    //this sets up what's in the mainwindow.ui - the GUI
    ui->setupUi(this);
    //feed in MainWindow to the logger - have to be done, else logger will crash program
    Logger::getInstance()->setMainWindow(this);
    LOG_HIGHLIGHT("Logger started");
    init(); //initializes parts of the program
}

MainWindow::~MainWindow()
{
    delete mRenderWindow;
    delete ui;
}

void MainWindow::init()
{
    //This will contain the setup of the OpenGL surface we will render into
    QSurfaceFormat format;

    //OpenGL v 4.1 - (Ole Flatens Mac does not support higher than this - sorry!)
    //you can try other versions, but then have to update RenderWindow and Shader
    //to inherit from other than QOpenGLFunctions_4_1_Core
    //(The 3DProgramming course does not use anything from higher than 4.1, so pleas don't change it)
    format.setVersion(4, 5);
    //Using the main profile for OpenGL - no legacy code permitted
    format.setProfile(QSurfaceFormat::CoreProfile);
    //A QSurface can be other types than OpenGL
    format.setRenderableType(QSurfaceFormat::OpenGL);

    //This should activate OpenGL debug Context used in RenderWindow::startOpenGLDebugger().
    //This line (and the startOpenGLDebugger() and checkForGLerrors() in RenderWindow class)
    //can be deleted, but it is nice to have some OpenGL debug info!
    format.setOption(QSurfaceFormat::DebugContext);

    // The surface will need a depth buffer - (not requiered to set in glfw-tutorials)
    format.setDepthBufferSize(24);

    // Multisample buffer for AA
    format.setSamples(4);

    // Disable v-sync
    //format.setSwapBehavior(QSurfaceFormat::SwapBehavior::SingleBuffer);
    //format.setSwapInterval(0);

    //Just prints out what OpenGL format we try to get
    // - this can be deleted
    qDebug() << "Requesting surface format: " << format;

    //We have a surface format for the OpenGL window, so let's make it:
    mRenderWindow = new RenderWindow(format, this);

    //Check if renderwindow did initialize, else prints error and quit
    if (!mRenderWindow->context()) {
        qDebug() << "Failed to create context. Can not continue. Quits application!";
        delete mRenderWindow;
        return;
    }

    //The OpenGL RenderWindow got made, so continuing the setup:
    //We put the RenderWindow inside a QWidget so we can put in into a
    //layout that is made in the .ui-file
    // (Qt has a QOpenGLWidget but that forces us to use more Qt specific OpenGL-stuff
    //  So we make a renderwindow manually to get more freedom)
    mRenderWindowContainer = QWidget::createWindowContainer(mRenderWindow);
    //OpenGLLayout is made in the mainwindow.ui-file!
    ui->OpenGLLayout->addWidget(mRenderWindowContainer);

    //sets the keyboard input focus to the RenderWindow when program starts
    // - can be deleted, but then you have to click inside the renderwindow to get the focus
    mRenderWindowContainer->setFocus();
}

//File menu Exit closes the program
void MainWindow::on_fileExit_triggered()
{
    close();       //Shuts down the whole program
}


void MainWindow::on_LandscapeUVSlider_sliderMoved(int position)
{
    //mRenderWindow->GUI_LandscapeUVSlider(position);
}


void MainWindow::on_LandscapeUVSlider_valueChanged(int value)
{
    mRenderWindow->GUI_LandscapeUVSlider(value);
}


void MainWindow::on_WireframeToggle_released()
{
    mRenderWindow->GUI_WireframeToggele();
}


void MainWindow::on_HouseScene_released()
{
    mRenderWindow->GUI_HouseScene();
}


void MainWindow::on_WoodsScene_released()
{
    mRenderWindow->GUI_WoodsScene();
}


void MainWindow::on_WorldScene_released()
{
    mRenderWindow->GUI_WorldScene();
}


void MainWindow::on_DrawMode_released()
{
    mRenderWindow->GUI_DrawMode();
}


void MainWindow::on_HeightmapScene_released()
{
    mRenderWindow->GUI_HeightmapScene();
}


void MainWindow::on_DebugLines_released()
{
    mRenderWindow->GUI_DebugLines();
}

