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
    qtimer = new QTimer(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::processFrameAndUpdateGUI()
{
    VideoCapture vc;

    capWebcam.read(matOriginal);
    if(matOriginal.empty() == true) return;

    cv::cvtColor(matOriginal, matOriginal, CV_BGR2RGB);

    QImage qimgOriginal((uchar*)matOriginal.data, matOriginal.cols, matOriginal.rows, matOriginal.step, QImage::Format_RGB888);


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
    connect(qtimer, SIGNAL(timeout()), this, SLOT(processFrameAndUpdateGUI()));
    qtimer->start();
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
