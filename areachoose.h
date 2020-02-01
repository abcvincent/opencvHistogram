#ifndef AREACHOOSE_H
#define AREACHOOSE_H

#include <QWidget>

#include"pixitem.h"
#include <QGraphicsView>    //视图类
#include <QGraphicsScene>   //场景类
#include <QGraphicsItem>    //图元类
#include <math.h>
#include <QWheelEvent>

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
class AreaChoose;
}

class AreaChoose : public QWidget
{
    Q_OBJECT

public:
    explicit AreaChoose(QWidget *parent = 0);
    ~AreaChoose();
    QImage  MatToQImage(const cv::Mat& mat);


    Mat outGlobalMatA;

private slots:
    void receiveMainImage1(Mat);
    void setTestImgA();




private:
    Ui::AreaChoose *ui;
};

#endif // AREACHOOSE_H
