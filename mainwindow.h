#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QPixmap>
#include <QSpinBox>
#include <QTextEdit>
#include <QEvent>
#include "thresholddlg.h"


//#include <opencv/cv.h>
//#include <opencv/highgui.h>
//#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2/core.hpp>
//#include <opencv2/videoio.hpp> // for camera

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>

#include <QMessageBox>

#pragma execution_character_set("utf-8")
using namespace cv;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

//        bool eventFilter(QObject *obj, QEvent *ev);

       Mat globalMat;
       Mat global_ResultMat;
       QImage globalImg;
       int globalMark;
       int showMark;
       Mat outMat;
       QImage  MatToQImage(const cv::Mat& mat);

public:
       ThresholdDlg *TDlg;


signals:
       void sendImage(Mat);


private slots:
       void setTestImg(int);//延时严重
       void setTestImg();

//       void receiveThresholdImg(Mat);

private slots:
    void on_on_openfile_triggered();

    void on_on_restore_triggered();

    void on_ac_clear_triggered();

    void on_ac_save_triggered();

    void on_ac_histogramRGB_triggered();

    void on_ac_test_triggered();



    void on_ac_histogramGray_triggered();

    void on_ac_threshold_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
