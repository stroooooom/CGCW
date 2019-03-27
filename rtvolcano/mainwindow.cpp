#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "rtcanvasqt.h"

#include <QString>
#include <QTimer>
#include <QTime>

#include <iostream>

#define SIZE 300
#define DELAY 34
static int delay = 65;

#define LIGHTPOSLEFT RTVector(0, scene->size/2, scene->size/2)
#define LIGHTPOSUPSTAIRS RTVector(scene->size/2, scene->size, scene->size/2)
#define LIGHTPOSRIGHT RTVector(scene->size, scene->size/2, scene->size/2)

#define SOURCE100 RTPoint(44, 29, 40)
#define SOURCE300 RTPoint(141, 100, 140)

#define TDELTA100 0.01f
#define TDELTA300 0.002f

#define LOWSPEEDRATIO100 0.5f
#define NORMALSPEEDRATIO100 0.75f
#define HIGHTSPEEDRATIO100 1
#define LOWSPEEDRATIO300 0.1f
#define NORMALSPEEDRATIO300 0.2f
#define HIGHTSPEEDRATIO300 0.4f

#define LOWMAGNITUDE100 2
#define NORMALMAGNITUDE100 4
#define HIGHMAGNITUDE100 6
#define LOWMAGNITUDE300 4
#define NORMALMAGNITUDE300 7
#define HIGHMAGNITUDE300 11

static int STEPSPERFRAME = 1;
static int initialized = false;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    setbuf(stdout, nullptr);
    uint size = SIZE;
    paused = true;
    ui->setupUi(this);

    ui->ls_comboBox->addItem("Слева");
    ui->ls_comboBox->addItem("Справа");
    ui->ls_comboBox->addItem("Сверху");

    ui->speed_comboBox->addItem("Низкая");
    ui->speed_comboBox->addItem("Средняя");
    ui->speed_comboBox->addItem("Высокая");

    ui->magnitude_comboBox->addItem("Низкая");
    ui->magnitude_comboBox->addItem("Средняя");
    ui->magnitude_comboBox->addItem("Высокая");

    ui->ls_comboBox->setCurrentIndex(2);
    ui->speed_comboBox->setCurrentIndex(1);
    ui->magnitude_comboBox->setCurrentIndex(1);

    scene = new RTScene(size);
    if (size == 100)
    {
        scene->createMountain(27);
        scene->flow->source = SOURCE100;
        scene->flow->t_delta = TDELTA100;
        scene->flow->t_default = 1;

        scene->flow->speed_ratio = NORMALSPEEDRATIO100;
        scene->flow->magnitude = NORMALMAGNITUDE100;
    }
    if (size == 300)
    {
        scene->createMountain(102);
        scene->flow->source = SOURCE300;
        scene->flow->t_delta = TDELTA300;
        scene->flow->t_default = 1;

        scene->flow->speed_ratio = NORMALSPEEDRATIO300;
        scene->flow->magnitude = NORMALMAGNITUDE300;
    }

    canvas = new RTCanvasQt(this, size);
    if (size == 100) canvas->setGeometry(120, 150, 0, 0);
    if (size == 300) canvas->setGeometry(15, 60, 0, 0);
    canvas->setParent(this);

    render = new RTRender(scene, canvas);
    if (size == 100) render->setRealtimeMode();
    if (size == 300) render->setPrerenderMode();
//    if (size == 300) render->setRealtimeMode();
    render->lightOn = true;

    if (render->getMode() == PRERENDER) ui->frames_number->setNum(100);
    if (render->getMode() == REALTIME) ui->renderButton->setDisabled(true);

    initialized = true;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{

    if (render->getMode() == PRERENDER)
    {
        if (event->key() == 61) { ::delay += 5; }
        else if (event->key() == Qt::Key_Minus) { ::delay -= 5; }
        std::cout << "delay: " << ::delay << " ms\n";

        return;
    }

    float angle = 10;
    int shift = 10;
    int ls_shift = 10;

    if (event->key() == Qt::Key_R) { renderOneFrame(); }
    // MOVING CAMERA
    else if (event->key() == Qt::Key_Left) { render->camera->vertical_axis_angle += angle; }
    else if (event->key() == Qt::Key_Right) { render->camera->vertical_axis_angle -= angle; }
    else if (event->key() == Qt::Key_Up) { render->camera->transverse_axis_angle += angle; }
    else if (event->key() == Qt::Key_Down) { render->camera->transverse_axis_angle -= angle; }
    // MOVING VIEW POSITION
    else if (event->key() == Qt::Key_W) { render->camera->viewPoint.y += shift; }
    else if (event->key() == Qt::Key_S) { render->camera->viewPoint.y -= shift; }
    else if (event->key() == Qt::Key_A) { render->camera->viewPoint.x -= shift; }
    else if (event->key() == Qt::Key_D) { render->camera->viewPoint.x += shift; }
    else if (event->key() == Qt::Key_Q) { render->camera->viewPoint.z -= shift; }
    else if (event->key() == Qt::Key_E) { render->camera->viewPoint.z += shift; }
    // CHANGING LIGHT RATIO VALUE
    else if (event->key() == Qt::Key_Z) { render->light_coeff -= 0.1; }
    else if (event->key() == Qt::Key_X) { render->light_coeff += 0.1; }
    // MOVING LIGHT SOURCE POSITION
    else if (event->key() == Qt::Key_U) { render->lightSource.y += ls_shift; }
    else if (event->key() == Qt::Key_J) { render->lightSource.y -= ls_shift; }
    else if (event->key() == Qt::Key_H) { render->lightSource.x -= ls_shift; }
    else if (event->key() == Qt::Key_K) { render->lightSource.x += ls_shift; }
    else if (event->key() == Qt::Key_Y) { render->lightSource.z -= ls_shift; }
    else if (event->key() == Qt::Key_I) { render->lightSource.z += ls_shift; }
    // LIGHT
    else if (event->key() == Qt::Key_L) { render->lightOn ? render->lightOn = false : render->lightOn = true; }

    render->updateCamera();
    if (render->getMode() == REALTIME)
    {
        render->renderFrame();
        render->paint();
    }
}

void MainWindow::renderOneFrame()
{
    scene->moveLava();
    render->renderFrame();
    render->paint();
}

void MainWindow::on_playButton_clicked()
{
    if (!paused) return;
    paused = false;

    if ((render->getMode() == PRERENDER && render->state == READY)
            || render->getMode() == REALTIME)
    {
        current_frame = 0;
        drawFrame();
    }
}

void MainWindow::on_renderButton_clicked()
{
    QTime t;
    t.start();
    if (render->getMode() == PRERENDER && render->state == READY)
    {
        auto n = ui->frames_number->text().split(" ")[0].toInt();
        ui->render_progressBar->setMaximum(n);
        ui->renderButton->setDisabled(true);
        ui->playButton->setDisabled(true);
        ui->pauseButton->setDisabled(true);
        ui->status->setText("Генерация");
        ui->status->repaint();
        qApp->processEvents();

        render->setPrerenderMode();
        render->setFramesNumber(n);
        render->state = RENDERING;
        int t_start = t.elapsed();
        for (int i = 0; i < n; i++)
        {
            int t0 = t.elapsed();
            for (int k = 0; k < STEPSPERFRAME; k++) scene->moveLava();
            render->renderFrame();

            if (i % 4 == 0)
            {
                auto remaning_time = (t.elapsed() - t0) * (n - i) / 1000;
                auto remaining_min = (int) remaning_time / 60;
                auto remaining_sec = (int) remaning_time % 60;

                ui->remtime_min->setNum((int) remaining_min);
                ui->remtime_sec->setNum((int) remaining_sec);
            }
            ui->render_progressBar->setValue(i+1);
            qApp->processEvents();
        }

        std::cout << "\nОбщее время: " << round(t.elapsed() - t_start) / 60000 << " минут" << std::endl;

        frames_number = n;
        render->state = READY;

        ui->renderButton->setDisabled(false);
        ui->playButton->setDisabled(false);
        ui->pauseButton->setDisabled(false);
        ui->remtime_min->setText("");
        ui->remtime_sec->setText("");
        ui->status->setText("Готово");

        ui->renderButton->repaint();
        ui->playButton->repaint();
        ui->pauseButton->repaint();
        ui->remtime_min->repaint();
        ui->remtime_sec->repaint();
        ui->status->repaint();

        qApp->processEvents();
    }
}

void MainWindow::drawFrame()
{
    if (paused) return;

    if (render->getMode() == REALTIME)
    {
        scene->moveLava();
        render->renderFrame();
        render->paint();

        QTimer::singleShot(delay, this, SLOT(drawFrame()));
        return;
    }

    if (current_frame >= frames_number)
    {
        paused = true;
        return;
    };

    render->paint(current_frame++);
    QTimer::singleShot(delay, this, SLOT(drawFrame()));
}

void MainWindow::on_pauseButton_clicked() { paused = true; }

void MainWindow::on_resetButton_clicked() { render->resetScene(); }

void MainWindow::on_frames_number_slide_valueChanged(int value) { ui->frames_number->setNum(value*100); }

void MainWindow::on_ls_comboBox_currentIndexChanged(int index)
{
    if (!initialized || render->state == RENDERING) return;
    if (index == OPTION0) render->lightSource = LIGHTPOSLEFT;
    else if (index == OPTION1) render->lightSource = LIGHTPOSRIGHT;
    else if (index == OPTION2) render->lightSource = LIGHTPOSUPSTAIRS;
}

void MainWindow::on_speed_comboBox_currentIndexChanged(int index)
{
    if (!initialized || render->state == RENDERING) return;
    if (scene->size == 100)
    {
        if (index == OPTION0) scene->flow->speed_ratio = LOWSPEEDRATIO100;
        else if (index == OPTION1) scene->flow->speed_ratio = NORMALSPEEDRATIO100;
        else if (index == OPTION2) scene->flow->speed_ratio = HIGHTSPEEDRATIO100;
    }
    else if (scene->size == 300)
    {
        if (index == OPTION0) scene->flow->speed_ratio = LOWSPEEDRATIO300;
        else if (index == OPTION1) scene->flow->speed_ratio = NORMALSPEEDRATIO300;
        else if (index == OPTION2) scene->flow->speed_ratio = HIGHTSPEEDRATIO300;
    }
}

void MainWindow::on_magnitude_comboBox_currentIndexChanged(int index)
{
    if (!initialized || render->state == RENDERING) return;
    if (scene->size == 100)
    {
        if (index == OPTION0) scene->flow->magnitude = LOWMAGNITUDE100;
        else if (index == OPTION1) scene->flow->magnitude = NORMALMAGNITUDE100;
        else if (index == OPTION2) scene->flow->magnitude = HIGHMAGNITUDE100;
    }
    else if (scene->size == 300)
    {
        if (index == OPTION0) scene->flow->magnitude = LOWMAGNITUDE300;
        else if (index == OPTION1) scene->flow->magnitude = NORMALMAGNITUDE300;
        else if (index == OPTION2) scene->flow->magnitude = HIGHMAGNITUDE300;
    }
}


MainWindow::~MainWindow()
{
    delete ui;
}
