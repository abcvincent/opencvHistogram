#ifndef THRESHOLDDLG_H
#define THRESHOLDDLG_H

#include <QWidget>
#include <QImage>
#include "areachoose.h"

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
class ThresholdDlg;
}

class ThresholdDlg : public QWidget
{
    Q_OBJECT

public:
    explicit ThresholdDlg(QWidget *parent = 0);
    ~ThresholdDlg();
    Mat globMat;
    Mat  outGlobalMat;
    QImage globImg;
    QImage  MatToQImage(const cv::Mat& mat);

    AreaChoose  *areaDlg;
    int btnMatk;
    void histogramGray();

    void showImage(Mat);

private:

    PixItem *pixItem;       //自定义的图元类
    QGraphicsScene *m_graphicsScene;  //场景
    QGraphicsView *m_graphicsView;

    PixItem *pixItem2;       //自定义的图元类
    QGraphicsScene *m_graphicsScene2;  //场景
    QGraphicsView *m_graphicsView2;

    PixItem *pixItem3;       //自定义的图元类
    QGraphicsScene *m_graphicsScene3;  //场景
    QGraphicsView *m_graphicsView3;


signals:
    void sendtoMainImage(Mat);

signals:
       void sendImage1(Mat);

private slots:
    void receiveMainImage(Mat);
    void setTestImg();//滑条函数
    void setTestImg(int);//延时严重


    void on_bnt_rect_clicked();
    void on_bnt_circle_clicked();
    void on_bnt_coord_clicked();
    void on_btn_doubleThresh_clicked();
    void on_btnColour_clicked();
    void on_btnThreh_clicked();
    void on_bnt_area_clicked();
    void on_bnt_contour_clicked();

private:
    Ui::ThresholdDlg *ui;
};

#endif // THRESHOLDDLG_H
