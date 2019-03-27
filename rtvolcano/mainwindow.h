#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>

#include "rtcanvasqt.h"
#include "rtrender.h"

#define OPTION0 0
#define OPTION1 1
#define OPTION2 2
#define OPTION3 3

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void renderOneFrame();

private slots:
    void keyPressEvent(QKeyEvent *event);

    void drawFrame();

    void on_playButton_clicked();
    void on_renderButton_clicked();
    void on_pauseButton_clicked();
    void on_resetButton_clicked();
    void on_frames_number_slide_valueChanged(int value);
    void on_ls_comboBox_currentIndexChanged(int index);
    void on_speed_comboBox_currentIndexChanged(int index);
    void on_magnitude_comboBox_currentIndexChanged(int index);

private:
    RTCanvasQt *canvas;
    RTRender *render;
    RTScene *scene;

    Ui::MainWindow *ui;

    int lightSourcePosition;
    int speed;
    int magnitude;

    bool paused;
    int frames_number;
    int current_frame;
};

#endif // MAINWINDOW_H
