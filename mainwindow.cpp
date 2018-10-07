#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <opencv2/opencv.hpp>
#include<opencv2/videoio.hpp>
#include<opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include<iostream>
#include<fstream>
#include<string.h>
#include<QtCore>
#include<QObject>
#include<QFileDialog>
#include<QString>

using namespace std;
using namespace cv;

QString VideoName;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->horizontalSlider->setEnabled(false);
    qtimer = new QTimer(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


double MainWindow::getCurrentFrame()
{
    return capWebcam.get(CV_CAP_PROP_POS_FRAMES);
}
double MainWindow::getNumberOfFrames()
{
    return capWebcam.get(CV_CAP_PROP_FRAME_COUNT);
}
double MainWindow::getFrameRate()
{
    return capWebcam.get(CV_CAP_PROP_FPS);
}

QString MainWindow::getFormattedTime(int timeInSeconds)
{
    int seconds = (int) (timeInSeconds) % 60 ;
    int minutes = (int) ((timeInSeconds / 60) % 60);
    int hours   = (int) ((timeInSeconds / (60*60)) % 24);
    QTime t(hours, minutes, seconds);
    if (hours == 0 )
        return t.toString("mm:ss");
    else
        return t.toString("h:mm:ss");
}


void MainWindow::processFrameAndUpdateGUI()
{
    VideoCapture vc;

    capWebcam.read(matOriginal);
    if(matOriginal.empty() == true) return;

    cv::cvtColor(matOriginal, matOriginal, CV_BGR2RGB);

    QImage qimgOriginal((uchar*)matOriginal.data, matOriginal.cols, matOriginal.rows, matOriginal.step, QImage::Format_RGB888);
    ui->label_2->setText( getFormattedTime( (int)getCurrentFrame()/(int)getFrameRate()) );
    ui->horizontalSlider->setValue(getCurrentFrame());
    ui->lblPlay->setPixmap(QPixmap::fromImage(qimgOriginal));

    ui->lblPlay->setScaledContents( true );
    ui->lblPlay->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
}

void MainWindow::on_loadButton_clicked()
{
    VideoName = QFileDialog::getOpenFileName(
                    this,
                    tr("Open Images"),
                    "/home/",
                    tr("mp4 File (*.mp4);; avi File (*.avi)"));
}

void MainWindow::on_playButton_clicked()
{
    std::string file = VideoName.toUtf8().constData();
    capWebcam.open(file);
    if(capWebcam.isOpened() == false) {
        cout <<"error: capWebcam not accessed successfully";
        return;
    }
    else
    {
        ui->horizontalSlider->setEnabled(true);
        ui->horizontalSlider->setMaximum(getNumberOfFrames());
        ui->label_2->setText( getFormattedTime( (int)getCurrentFrame()/(int)getFrameRate()) );
        connect(qtimer, SIGNAL(timeout()), this, SLOT(processFrameAndUpdateGUI()));
        qtimer->start();
    }
}

void MainWindow::on_pushButton_3_clicked()
{
    if(qtimer->isActive() == true)
    {
        qtimer->stop();
        ui->pushButton_3->setText("Resume");
    }
    else
    {
        qtimer->start(0);
        ui->pushButton_3->setText("Pause");
    }
}

void MainWindow::on_horizontalSlider_sliderPressed()
{
    qtimer->stop();
}

void MainWindow::on_horizontalSlider_sliderReleased()
{
    qtimer->start(0);
}

void MainWindow :: setCurrentFrame(int frameNumber)
{
    capWebcam.set(CV_CAP_PROP_POS_FRAMES, frameNumber);
}

void MainWindow::on_horizontalSlider_sliderMoved(int position)
{
    setCurrentFrame(position);
    ui->label_2->setText( getFormattedTime( position/(int)getFrameRate()) );
}
