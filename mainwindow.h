#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QWidget;
class RenderWindow;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    ///Slot called from the File->Exit menu in the mainwindow.ui file
    void on_fileExit_triggered();

    void on_LandscapeUVSlider_sliderMoved(int position);

    void on_LandscapeUVSlider_valueChanged(int value);

    void on_WireframeToggle_released();

    void on_HouseScene_released();

    void on_WoodsScene_released();

    void on_WorldScene_released();

    void on_DrawMode_released();

    void on_HeightmapScene_released();

    void on_DebugLines_released();

private:
    ///called from the constructor. Initializes different parts of the program.
    void init();

    Ui::MainWindow *ui{nullptr};                //the GUI of the app
    QWidget *mRenderWindowContainer{nullptr};   //Qt container for the RenderWindow
    RenderWindow *mRenderWindow{nullptr};       //The class that actually renders OpenGL

    //Logger class uses private ui pointer from this class
    friend class Logger;
};

#endif // MAINWINDOW_H
