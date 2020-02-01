#include "thresholddlg.h"
#include "ui_thresholddlg.h"
#include<QDebug>

#include <math.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>
#pragma execution_character_set("utf-8")

using namespace cv;
using namespace std;

ThresholdDlg::ThresholdDlg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ThresholdDlg)
{
    ui->setupUi(this);

    /****************************************************************************************\
    *                                             阈值分割处理器                                             *
    *    注意：此部分处理的图片是由mainWindow传递过来，
    *    请单独加载图片到globMat=imread(Path,1); *
    \****************************************************************************************/
    //设置comboBox值
    ui->comboBox->addItem("0：THRESH_BINARY");            //        0: 二进制阈值
    ui->comboBox->addItem("1：THRESH_BINARY_INV");   //         1: 反二进制阈值
    ui->comboBox->addItem("2：THRESH_TRUNC");            //         2: 截断阈值
    ui->comboBox->addItem("3：THRESH_TOZERO");          //         3: 0阈值
    ui->comboBox->addItem("4：THRESH_TOZERO_INV");  //         4: 反0阈值
    ui->comboBox->addItem("5：THRESH_OTSU");              //        5: 自适应阈值
    //    ui->comboBox->addItem("6：THRESH_TRIANGLE");  //

    
    ui->comboBox_2->addItem("1最外层轮廓：RETR_EXTERNAL");         //  只检测出最外轮廓即c0。
    ui->comboBox_2->addItem("2轮廓list：RETR_LIST ");                         //  检测出所有的轮廓并将他们保存到表(list)中
    ui->comboBox_2->addItem("3轮廓双层：RETR_CCOMP");                  // 检测出所有的轮廓并将他们组织成双层的结构，第一层是外部轮廓边界，第二层边界是孔的边界。
    ui->comboBox_2->addItem("4轮廓网状： RETR_TREE");                     //  检测出所有轮廓并且重新建立网状的轮廓结构。
//    ui->comboBox_2->addItem("5轮廓填充： CV_RETR_FLOODFILL");         //   检测出所有轮廓并填充
       
    ui->comboBox_3->addItem("1：CHAIN_APPROX_NONE ");                   //   存储所以轮廓的点。
    ui->comboBox_3->addItem("2：CHAIN_APPROX_SIMPLE");                  //   压缩水平、垂直和对角线段，只留下它们的端点。
    ui->comboBox_3->addItem("3：CHAIN_APPROX_TC89_L1");               //  使用Teh-Chin链逼近算法中的一个。
    ui->comboBox_3->addItem("4：CHAIN_APPROX_TC89_KCOS");          //   使用Teh-Chin链逼近算法中的一个。
//    ui->comboBox_3->addItem("5：CV_CHAIN_APPROX_TC89_KCOS");    //   使用Teh-Chin链逼近算法中的一个。
//    ui->comboBox_3->addItem("6：CV_LINK_RUNS");                                      //   与上述的算法完全不同，连接所有的水平层次的轮廓。
    
//    findContours(img, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE);
//   findContours(image,contours,hierarchy,RETR_CCOMP,CHAIN_APPROX_NONE,Point());


    QObject::connect(ui->spinBox_1,SIGNAL(valueChanged(int)),
                     ui->horizontalSlider_1,SLOT(setValue(int)));//spinBox值与滑条对应
    QObject::connect(ui->horizontalSlider_1,SIGNAL(valueChanged(int)),
                     ui->spinBox_1,SLOT(setValue(int)));//滑条与spinBox值对应
    ui->horizontalSlider_1->setMaximum(255);//初始化
    ui->horizontalSlider_1->setValue(150);//初始化

    QObject::connect(ui->spinBox_2,SIGNAL(valueChanged(int)),
                     ui->horizontalSlider_2,SLOT(setValue(int)));//spinBox值与滑条对应
    QObject::connect(ui->horizontalSlider_2,SIGNAL(valueChanged(int)),
                     ui->spinBox_2,SLOT(setValue(int)));//滑条与spinBox值对应
    ui->horizontalSlider_2->setMaximum(255);//初始化
    ui->horizontalSlider_2->setValue(150);//初始化

    connect(ui->horizontalSlider_1,SIGNAL(sliderReleased()),this,SLOT(setTestImg()));//滑条释放后刷新处理图
    connect(ui->horizontalSlider_2,SIGNAL(sliderReleased()),this,SLOT(setTestImg()));//滑条释放后刷新处理图
    connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setTestImg(int)));//下拉菜单改变后刷新处理图
    connect(ui->spinBox_1,SIGNAL(valueChanged(int)),this,SLOT(setTestImg(int)));//spinBox值改变后刷新处理图
    connect(ui->spinBox_2,SIGNAL(valueChanged(int)),this,SLOT(setTestImg(int)));//spinBox值改变后刷新处理图
    btnMatk=1;

    areaDlg=new AreaChoose;
    connect(this,SIGNAL(sendImage1(Mat)),areaDlg,SLOT(receiveMainImage1(Mat)));//传递到面积分割窗口

}

ThresholdDlg::~ThresholdDlg()
{
    delete ui;
}

//下面函数是接收mainwindow信号，传递过来的Mat
void  ThresholdDlg::receiveMainImage(Mat reMat)
{
    QImage  img2;
    globMat=reMat;
    ui->lb_InToImg->clear();
    globImg=MatToQImage(reMat);
    img2=globImg.scaled(ui->lb_InToImg->width(),ui->lb_InToImg->height(),Qt::KeepAspectRatio);;
    ui->lb_InToImg->setPixmap(QPixmap::fromImage(img2));
    histogramGray();//显示灰度直方图
    setTestImg();//显示处理图片

//    m_graphicsScene->removeItem(pixItem);   //将上一个图元从场景中移除,重新添加新的图元
    QPixmap pixmapCV =QPixmap::fromImage(globImg.scaled(size(), Qt::KeepAspectRatio));
    pixItem = new PixItem(new QPixmap(pixmapCV));
    //将该图元对象添加到场景中，并设置此图元在场景中的位置为中心（0，0）
    m_graphicsScene = new QGraphicsScene;  //new 一个新的场景对象
    ui->graphicsView->setSceneRect(-10/2,-10/2,10,10);
    //设置开启滚动条范围，不然即使隐藏了滚动条后还是有滚动效果
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//隐藏横向滚动条
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//隐藏纵向滚动条
    ui->graphicsView->setScene(m_graphicsScene);//添加场景
    m_graphicsScene->addItem(pixItem);//将图元添加到场景中
    pixItem->setPos(0,0);//中心位置

}

//////////////////////////////////////
//////////////////////////////////
/// 图元函数

////双击恢复图像
//void ThresholdDlg::mouseDoubleClickEvent(QMouseEvent *e)
//{
//    ////////***********图片复原代码**************/////
//    pixItem->setPos(0,0);
//    pixItem->setScaleValue(1);
//    pixItem->setZoomState(NO_STATE);
//}

////滚轮滚动图片缩小放大
//void ThresholdDlg::wheelEvent(QWheelEvent *event)
//{
//    pixItem->setZoomState(NO_STATE);//枚举，对应pixitem类中
//    int scaleValue=pixItem->getScaleValue();//获取ScaleValue值（放大缩小值）
//    if(event->delta() > 0)  //delta（）为正，滚轮向上滚
//    {
//        scaleValue++;
//    }
//    else
//    {
//        scaleValue--;
//    }
//    pixItem->setScaleValue(scaleValue);//重新设置ScaleValue值（放大缩小值）
//}

////复原函数
//void ThresholdDlg::resetzoom()
//{
//    pixItem->setPos(0,0);//复原位置
//    pixItem->setScaleValue(0);//复原比例
//    pixItem->setZoomState(NO_STATE);
//}
//////////////////////
///////////////



QImage ThresholdDlg::MatToQImage(const cv::Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if (mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        image.setColorCount(256);
        for (int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = mat.data;
        for (int row = 0; row < mat.rows; row++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if (mat.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if (mat.type() == CV_8UC4)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else
    {
        return QImage();
    }
}

void  ThresholdDlg::setTestImg()
{
    Mat img=globMat.clone();
    Mat dst;
    int value_m=this->ui->spinBox_1->value();
    int value_m2=this->ui->spinBox_2->value();
    int threshold_type=cv::THRESH_BINARY;

    if(ui->comboBox->currentText()=="0：THRESH_BINARY"){threshold_type=cv::THRESH_BINARY;}
    if(ui->comboBox->currentText()=="1：THRESH_BINARY_INV"){threshold_type=cv::THRESH_BINARY_INV;}
    if(ui->comboBox->currentText()=="2：THRESH_TRUNC"){threshold_type=cv::THRESH_TRUNC;}
    if(ui->comboBox->currentText()=="3：THRESH_TOZERO"){threshold_type=cv::THRESH_TOZERO;}
    if(ui->comboBox->currentText()=="4：THRESH_TOZERO_INV"){threshold_type=cv::THRESH_TOZERO_INV;}
    if(ui->comboBox->currentText()=="5：THRESH_OTSU"){threshold_type=cv::THRESH_OTSU;}

    if(btnMatk==1)
    {
        cv::cvtColor(img,img,COLOR_RGB2GRAY);//进行，灰度处理
        //阈值分割
        threshold( img,                 //输入图像,原始数组 (单通道 , 8-bit of 32-bit 浮点数).
                   dst,                          //输出图像,输出数组，必须与 src 的类型一致，或者为 8-bit.
                   value_m,                 //分割值
                   255,                         // 使用 CV_THRESH_BINARY 和 CV_THRESH_BINARY_INV 的最大值.
                   threshold_type ); //阈值类型
        QImage imgshow=MatToQImage(dst);
        this-> ui->lb_OutToImg->clear();
        imgshow=imgshow.scaled( this->ui->lb_OutToImg->width(), this->ui->lb_OutToImg->height(),Qt::KeepAspectRatio);
        this-> ui->lb_OutToImg->setPixmap(QPixmap::fromImage(imgshow));
        outGlobalMat=dst.clone();

        showImage(dst);
//        QImage imgshow2=MatToQImage(dst);
//        //        m_graphicsScene->removeItem(pixItem2);   //将上一个图元从场景中移除,重新添加新的图元
//        QPixmap pixmapCV =QPixmap::fromImage(imgshow2.scaled(size(), Qt::KeepAspectRatio));
//        pixItem2 = new PixItem(new QPixmap(pixmapCV));
//        //将该图元对象添加到场景中，并设置此图元在场景中的位置为中心（0，0）
//        m_graphicsScene2 = new QGraphicsScene;  //new 一个新的场景对象
//        ui->graphicsView_2->setSceneRect(-10/2,-10/2,10,10);
//        //设置开启滚动条范围，不然即使隐藏了滚动条后还是有滚动效果
//        ui->graphicsView_2->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//隐藏横向滚动条
//        ui->graphicsView_2->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//隐藏纵向滚动条
//        ui->graphicsView_2->setScene(m_graphicsScene2);//添加场景
//        m_graphicsScene2->addItem(pixItem2);//将图元添加到场景中
//        pixItem2->setPos(0,0);//中心位置

    }
    else
    {
        //    IplImage* cvCreateImage( CvSize size, int depth, int channels );
        //    size
        //    图像宽、高.
        //    depth
        //    图像元素的位深度，可以是下面的其中之一：
        //    IPL_DEPTH_8U - 无符号8位整型
        //    IPL_DEPTH_8S - 有符号8位整型
        //    IPL_DEPTH_16U - 无符号16位整型
        //    IPL_DEPTH_16S - 有符号16位整型
        //    IPL_DEPTH_32S - 有符号32位整型
        //    IPL_DEPTH_32F - 单精度浮点数
        //    IPL_DEPTH_64F - 双精度浮点数
        //    channels
        //    每个元素（像素）的颜色通道数量.可以是 1, 2, 3 或 4.通道是交叉存取的，例如通常的彩色图像数据排列是：
        //    b0 g0 r0 b1 g1 r1 ...
        /////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////

        Mat img2=globMat.clone();
        cv::cvtColor(img2,img2,COLOR_RGB2GRAY);//进行，灰度处理
        //阈值分割
        threshold( img2,                 //输入图像,原始数组 (单通道 , 8-bit of 32-bit 浮点数).
                   img2,                          //输出图像,输出数组，必须与 src 的类型一致，或者为 8-bit.
                   value_m,                    //分割值
                   0,                                 // 使用 CV_THRESH_BINARY 和 CV_THRESH_BINARY_INV 的最大值.
                   cv::THRESH_TOZERO); //阈值类型

        threshold(img2,                 //输入图像,原始数组 (单通道 , 8-bit of 32-bit 浮点数).
                  dst,                             //输出图像,输出数组，必须与 src 的类型一致，或者为 8-bit.
                  value_m2,                  //分割值
                  0,                                 // 使用 CV_THRESH_BINARY 和 CV_THRESH_BINARY_INV 的最大值.
                  cv::THRESH_TOZERO_INV  ); //阈值类型

        threshold( dst,                 //输入图像,原始数组 (单通道 , 8-bit of 32-bit 浮点数).
                   dst,                          //输出图像,输出数组，必须与 src 的类型一致，或者为 8-bit.
                   1,                             //分割值
                   255,                         // 使用 CV_THRESH_BINARY 和 CV_THRESH_BINARY_INV 的最大值.
                   cv::THRESH_BINARY); //阈值类型

        QImage imgshow=MatToQImage(dst);
        this-> ui->lb_OutToImg->clear();
        imgshow=imgshow.scaled( this->ui->lb_OutToImg->width(), this->ui->lb_OutToImg->height(),Qt::KeepAspectRatio);
        this-> ui->lb_OutToImg->setPixmap(QPixmap::fromImage(imgshow));
        outGlobalMat=dst.clone();


        showImage(dst);
//        QImage imgshow2=MatToQImage(dst);
//        //        m_graphicsScene->removeItem(pixItem2);   //将上一个图元从场景中移除,重新添加新的图元
//        QPixmap pixmapCV =QPixmap::fromImage(imgshow2.scaled(size(), Qt::KeepAspectRatio));
//        pixItem2 = new PixItem(new QPixmap(pixmapCV));
//        //将该图元对象添加到场景中，并设置此图元在场景中的位置为中心（0，0）
//        m_graphicsScene2 = new QGraphicsScene;  //new 一个新的场景对象
//        ui->graphicsView_2->setSceneRect(-10/2,-10/2,10,10);
//        //设置开启滚动条范围，不然即使隐藏了滚动条后还是有滚动效果
//        ui->graphicsView_2->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//隐藏横向滚动条
//        ui->graphicsView_2->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//隐藏纵向滚动条
//        ui->graphicsView_2->setScene(m_graphicsScene2);//添加场景
//        m_graphicsScene2->addItem(pixItem2);//将图元添加到场景中
//        pixItem2->setPos(0,0);//中心位置

    }
}


void ThresholdDlg::histogramGray()
{

#define cvQueryHistValue_1D( hist, idx0 )\
    ((float)cvGetReal1D( (hist)->bins, (idx0)))

    //进行，灰度处理
    //matOri输入图像 由对话框来选
    //dstImage输出图像，最后希望显示的图像必须添加
    //例如：cv::cvtColor(matOri,dstImage,COLOR_RGB2GRAY);

    //灰度直方图处理测试

    IplImage *src= &IplImage(globMat);
    IplImage* gray_plane = cvCreateImage(cvGetSize(src),8,1);
    cvCvtColor(src,gray_plane,CV_RGB2GRAY);
    //  cv::cvtColor(src,gray_plane,COLOR_RGB2GRAY);


    int hist_size = 256;    //直方图尺寸
    int hist_height = 256;
    float range[] = {0,255};  //灰度级的范围
    float* ranges[]={range};
    //创建一维直方图，统计图像在[0 255]像素的均匀分布
    CvHistogram* gray_hist = cvCreateHist(1,&hist_size,CV_HIST_ARRAY,ranges,1);
    //计算灰度图像的一维直方图
    cvCalcHist(&gray_plane,gray_hist,0,0);
    //归一化直方图
    cvNormalizeHist(gray_hist,1.0);

    int scale = 2;
    //创建一张一维直方图的“图”，横坐标为灰度级，纵坐标为像素个数（*scale）
    IplImage* hist_image = cvCreateImage(cvSize(hist_size*scale,hist_height),8,3);
    cvZero(hist_image);
    //统计直方图中的最大直方块
    float max_value = 0;
    cvGetMinMaxHistValue(gray_hist, 0,&max_value,0,0);

    //分别将每个直方块的值绘制到图中
    for(int i=0;i<hist_size;i++)
    {
        float bin_val = cvQueryHistValue_1D(gray_hist,i); //像素i的概率
        int intensity = cvRound(bin_val*hist_height/max_value);  //要绘制的高度
        cvRectangle(hist_image,
                    cvPoint(i*scale,hist_height-1),
                    cvPoint((i+1)*scale - 1, hist_height - intensity),
                    CV_RGB(255,255,255));
    }

    Mat outMat=cvarrToMat(hist_image);
    IplImage* transIplimage = cvCloneImage(&(IplImage) outMat);
    cvLine(transIplimage,cvPoint(1*2*(ui->horizontalSlider_1->value()),0),cvPoint(1*2*(ui->horizontalSlider_1->value()),255),CV_RGB(0,255,0),2);
    if(btnMatk==2)
    {
        if(ui->horizontalSlider_1->value()>=ui->horizontalSlider_2->value()&&ui->horizontalSlider_2->value()!=255)
        {
            ui->horizontalSlider_2->setValue(ui->horizontalSlider_1->value()+1);
        }

            cvLine(transIplimage,cvPoint(1*2*(ui->horizontalSlider_2->value()),0),cvPoint(1*2*(ui->horizontalSlider_2->value()),255),CV_RGB(255,0,0),2);
    }

    //    cvLine(transIplimage,cvPoint(255*2,0),cvPoint(255*2,250),CV_RGB(0,0,255),2);
    outMat=cvarrToMat(transIplimage);

    QImage imgshow=MatToQImage(outMat);
    ui->lb_HistogramImg->clear();
    imgshow=imgshow.scaled( this->ui->lb_HistogramImg->width(), this->ui->lb_HistogramImg->height(),Qt::KeepAspectRatio);
    this-> ui->lb_HistogramImg->setPixmap(QPixmap::fromImage(imgshow));



    QImage imgshow2=MatToQImage(outMat);
    //        m_graphicsScene->removeItem(pixItem2);   //将上一个图元从场景中移除,重新添加新的图元
//    QPixmap pixmapCV =QPixmap::fromImage(imgshow2.scaled(size(), Qt::KeepAspectRatio));
     QPixmap pixmapCV =QPixmap::fromImage(imgshow2.scaled(this->ui->graphicsView_3->width(),this->ui->graphicsView_3->height(), Qt::KeepAspectRatio));

    pixItem3 = new PixItem(new QPixmap(pixmapCV));
    //将该图元对象添加到场景中，并设置此图元在场景中的位置为中心（0，0）
    m_graphicsScene3 = new QGraphicsScene;  //new 一个新的场景对象
    ui->graphicsView_3->setSceneRect(-10/2,-10/2,10,10);
    //设置开启滚动条范围，不然即使隐藏了滚动条后还是有滚动效果
    ui->graphicsView_3->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//隐藏横向滚动条
    ui->graphicsView_3->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//隐藏纵向滚动条
    ui->graphicsView_3->setScene(m_graphicsScene3);//添加场景
    m_graphicsScene3->addItem(pixItem3);//将图元添加到场景中
    pixItem3->setPos(0,0);//中心位置


}

void  ThresholdDlg::setTestImg(int setValue)
{
    histogramGray();
    setTestImg();

    sendImage1(outGlobalMat);//发送到面积分割
}


 void ThresholdDlg::on_bnt_rect_clicked()
 {

//     /////////////////////////FindContours外接图//////
//     //     Mat img=globMat;//直接等于会复制mat的头及地址会修改
//     Mat img=globMat.clone();
//     cv::cvtColor(img,img,COLOR_RGB2GRAY);//进行，灰度处理
//     int value_m=this->ui->spinBox_1->value();
//     Mat imageSource=img;
//     Mat image;
//     blur(imageSource,image,Size(3,3));
//     threshold(image,image, value_m,255,THRESH_BINARY_INV);

     Mat image=outGlobalMat.clone();

     //寻找所有轮廓
     vector<vector<Point>> contours;
     vector<Vec4i> hierarchy;
     findContours(image,contours,hierarchy,RETR_CCOMP,CHAIN_APPROX_NONE,Point());

     Mat imageContours=Mat::zeros(image.size(),CV_8UC1); //最小外接矩形画布
     Mat imageContours1=Mat::zeros(image.size(),CV_8UC1); //最小外结圆画布
     for(int i=0;i<contours.size();i++)
     {

//           CvScalar color = CV_RGB( rand()%255, rand()%255, rand()%255 );
         //绘制轮廓
         drawContours(imageContours,contours,i,Scalar(255),1,8,hierarchy);
         drawContours(imageContours1,contours,i,Scalar(255),1,8,hierarchy);
         //绘制轮廓的最小外结矩形
         RotatedRect rect=minAreaRect(contours[i]);
         Point2f P[4];
         rect.points(P);
         for(int j=0;j<=3;j++)
         {
             line(imageContours,P[j],P[(j+1)%4],Scalar(255),2);
         }
         //绘制轮廓的最小外结圆
         Point2f center; float radius;
         minEnclosingCircle(contours[i],center,radius);
         circle(imageContours1,center,radius,Scalar(255),2);
     }

     QImage imgshow=MatToQImage(imageContours);
     this-> ui->lb_OutToImg->clear();
     imgshow=imgshow.scaled( this->ui->lb_OutToImg->width(), this->ui->lb_OutToImg->height(),Qt::KeepAspectRatio);
     this-> ui->lb_OutToImg->setPixmap(QPixmap::fromImage(imgshow));

      showImage(imageContours);
 }

void ThresholdDlg::on_bnt_circle_clicked()
{

//    /////////////////////////FindContours外接图//////
//    Mat img=globMat.clone();
//    cv::cvtColor(img,img,COLOR_RGB2GRAY);//进行，灰度处理
//    int value_m=this->ui->spinBox_1->value();
//    Mat imageSource=img;
//    Mat image;
//    blur(imageSource,image,Size(3,3));
//    threshold(image,image, value_m,255,THRESH_BINARY_INV);


    Mat image=outGlobalMat.clone();

    //寻找所有轮廓
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(image,contours,hierarchy,RETR_CCOMP,CHAIN_APPROX_NONE,Point());

    Mat imageContours=Mat::zeros(image.size(),CV_8UC1); //最小外接矩形画布
    Mat imageContours1=Mat::zeros(image.size(),CV_8UC1); //最小外结圆画布

    for(int i=0;i<contours.size();i++)
    {
        //绘制轮廓
        drawContours(imageContours,contours,i,Scalar(255),1,8,hierarchy);
        drawContours(imageContours1,contours,i,Scalar(255,0,0),1,8,hierarchy);
        //绘制轮廓的最小外结矩形
        RotatedRect rect=minAreaRect(contours[i]);
        Point2f P[4];
        rect.points(P);
        for(int j=0;j<=3;j++)
        {
            line(imageContours,P[j],P[(j+1)%4],Scalar(255),2);
        }
        //绘制轮廓的最小外结圆
        Point2f center; float radius;
        minEnclosingCircle(contours[i],center,radius);
        circle(imageContours1,center,radius,Scalar(255),2);
    }

    QImage imgshow=MatToQImage(imageContours1);
    this-> ui->lb_OutToImg->clear();
    imgshow=imgshow.scaled( this->ui->lb_OutToImg->width(), this->ui->lb_OutToImg->height(),Qt::KeepAspectRatio);
    this-> ui->lb_OutToImg->setPixmap(QPixmap::fromImage(imgshow));

    showImage(imageContours1);
}

void ThresholdDlg::on_bnt_coord_clicked()
{

    //    /////////////////////////FindContours外接图//////
    //    Mat img=globMat;
    //    Mat img2=globMat.clone();
    //    cv::cvtColor(img,img,COLOR_RGB2GRAY);//进行，灰度处理
    ////    GaussianBlur(img,img,Size(5,5),0,0);//高斯滤波，滤去噪声
    //    int value_m=this->ui->spinBox_1->value();
    //    Mat imageSource=img;
    //    Mat image;
    //    blur(imageSource,image,Size(3,3));
    //    threshold(image,image, value_m,255,THRESH_BINARY_INV);

    Mat image=outGlobalMat.clone();
     Mat img2=globMat.clone();

    //寻找最外层轮廓
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
//    findContours(image,contours,hierarchy,RETR_CCOMP, CHAIN_APPROX_SIMPLE,Point());
//     findContours(dst,contours,hierarchy,RETR_CCOMP, CHAIN_APPROX_SIMPLE,Point());//所有轮廓CV_CHAIN_APPROX_SIMPLE
//    if(ui->comboBox_2->currentIndex()==0){mode=RETR_EXTERNAL;}//只检测外轮廓。忽略轮廓内部的洞。
    findContours(image,contours,hierarchy,RETR_EXTERNAL, CHAIN_APPROX_SIMPLE,Point());

    drawContours(img2,contours,-1,Scalar(0,0,255),2,8);


    for(int i=0;i<contours.size();i++)
    {
        //        //绘制轮廓
        //        drawContours(imageContours,contours,i,Scalar(255),1,8,hierarchy);
        //        drawContours(img2,contours,i,Scalar(255),1,8,hierarchy);
        //绘制轮廓的最小外结矩形
        RotatedRect rect=minAreaRect(contours[i]);
        Point2f P[4];
        rect.points(P);
        for(int j=0;j<=3;j++)
        {
            line(img2,P[j],P[(j+1)%4],Scalar(255),2);
        }

        //绘制轮廓的最小外结圆
        Point2f center; float radius;
        minEnclosingCircle(contours[i],center,radius);
        circle(img2,center,radius,Scalar(255),2);

    }

    IplImage *dst= &IplImage(img2);//mat转IplImage
    for(int i=0;i<contours.size();i++)
    {
        //绘制轮廓的最小外结圆
        Point2f center; float radius;
        minEnclosingCircle(contours[i],center,radius);
        RotatedRect rect2=minAreaRect(contours[i]);

        Point p1(center.x,center.y-10);
        Point p2(center.x,center.y+10);
        Point p3(center.x-10,center.y);
        Point p4(center.x+10,center.y);

        line(img2,p1,p2,CV_RGB(255,255,255),2);
        line(img2,p3,p4,CV_RGB(255,255,255),2);


        QString strg=QString::number(rect2.angle,10,1);
        //            CvFont font;
        //            cvInitFont(&font,CV_FONT_HERSHEY_COMPLEX,0.5,0.5,0,0.5,8);
        //            cvPutText(dst,srg,center,&font,cvScalar(255,0,0));

        QString strx=QString::number(center.x,10,1);
        QString stry=QString::number(center.y,10,1);
        QString centerXY=QString("(%1,%2),%3").arg(strx).arg(stry).arg(strg);

        QByteArray cStr = centerXY.toLocal8Bit(); // 注意，这个QByteArray 对象一定要建立
        char *p = cStr.data();

        CvFont font2;
        cvInitFont(&font2,CV_FONT_VECTOR0,0.5,0.5,0,1,8);
        //在图像中显示文本字符串
        cvPutText(dst,p,center,&font2,CV_RGB(255,0,0));
    }
    Mat img3=cvarrToMat(dst);//IplImage转Mat
    QImage imgshow=MatToQImage(img3);
    this-> ui->lb_OutToImg->clear();
    imgshow=imgshow.scaled( this->ui->lb_OutToImg->width(), this->ui->lb_OutToImg->height(),Qt::KeepAspectRatio);
    this-> ui->lb_OutToImg->setPixmap(QPixmap::fromImage(imgshow));

      showImage(img3);
}

void ThresholdDlg::on_btn_doubleThresh_clicked()
{
    btnMatk=2;
    histogramGray();
    setTestImg();
}

void ThresholdDlg::on_btnColour_clicked()
{
    sendImage1(outGlobalMat);//发送到面积分割

    Mat dst=outGlobalMat.clone();
    Mat img=globMat.clone();

    //寻找最外层轮廓
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
//    findContours(dst,contours,hierarchy,RETR_EXTERNAL,CHAIN_APPROX_NONE,Point());//最外层轮廓
    findContours(dst,contours,hierarchy,RETR_CCOMP, CHAIN_APPROX_SIMPLE,Point());//所有轮廓CV_CHAIN_APPROX_SIMPLE
    //    findContours返回为vector<vector<Point> >的轮廓向量
    Mat dst1 = Mat::zeros(img.size(), CV_8UC3);//初始化dst
    if( !contours.empty() && !hierarchy.empty() ) //开始处理
    {
        //遍历所有顶层轮廓，随机生成颜色值绘制给各连接组成部分
        for(  int idx = 0; idx >= 0; idx = hierarchy[idx][0] )
        {
            Scalar color( (rand()&255), (rand()&255), (rand()&255) );//随机产生颜色 rand()产生随机数
            //绘制填充轮廓
            drawContours(
                        dst1,                    //用来绘制轮廓的图像
                        contours,         //指向第一个轮廓的指针
                        idx,                   //
                        color,               //内层轮廓的颜色
                        CV_FILLED,     //绘制轮廓的最大等级,绘制轮廓时所使用的线条的粗细度。如果值为负(e.g. =CV_FILLED),绘制内层轮廓。
                        8,                     //线条的类型
                        hierarchy );    //按照给出的偏移量移动每一个轮廓点坐标.
        }
    }

    QImage imgshow=MatToQImage(dst1);
    this-> ui->lb_OutToImg->clear();
    imgshow=imgshow.scaled( this->ui->lb_OutToImg->width(), this->ui->lb_OutToImg->height(),Qt::KeepAspectRatio);
    this-> ui->lb_OutToImg->setPixmap(QPixmap::fromImage(imgshow));

    showImage(dst1);


    //统计面积
    double  contour_area_tmp=0;
    double  contour_area_max=0;
    double  contour_area_min=0;
    double  contour_area_sum=0;
    for (int i = 0; i<contours.size(); i++)
    {
        contour_area_tmp=fabs(contourArea(contours[i]));
        if( contour_area_tmp > contour_area_max )
        {
            contour_area_max = contour_area_tmp; //找到面积最大的轮廓
        }
        contour_area_sum += contour_area_tmp; //求所有轮廓的面积和
    }

    contour_area_min=contour_area_max;
    for (int i = 0; i<contours.size(); i++)
    {
        contour_area_tmp=fabs(contourArea(contours[i]));
        if( contour_area_tmp< contour_area_min )
        {
            contour_area_min = contour_area_tmp; //找到面积最小的轮廓
        }
    }
    double  contour_area_ave = contour_area_sum/ (contours.size()+1); //求出所有轮廓的平均值

    qDebug()<<"面积最大值："<<contour_area_max;//输出计时
    qDebug()<<"面积最小值："<<contour_area_min;//输出计时
    qDebug()<<"对象数量："<<contours.size()+1;//输出计时
    qDebug()<<"面积总和："<<contour_area_sum;//输出计时
    qDebug()<<"面积平均值："<<contour_area_ave;//输出计时

    for (int i = 0; i<contours.size(); i++)
    {
        contour_area_tmp=fabs(contourArea(contours[i]));
        qDebug()<<"所有面积值："<<i<<":"<<QString::number(contour_area_tmp,10,2);//输出计时
    }

 qDebug()<<"///////////////////////";//输出

    vector<double>  contoursGram;
    for (int i = 0; i<contours.size(); i++)
    {
        contoursGram.push_back(fabs(contourArea(contours[i])));
//        qDebug()<<"所有面积值："<<i<<":"<<QString::number( contoursGram[i],10,2);//输出计时
    }


    sort( contoursGram.begin(), contoursGram.end());//sort函数排序

    for (int i = 0; i< contoursGram.size(); i++)
    {
        qDebug()<<"所有面积值："<<i<<":"<<QString::number( contoursGram[i],10,2);//输出计时
    }




    //        //cvFindContours方法提取
    //        cv::cvtColor(img,img,COLOR_RGB2GRAY);//进行，灰度处理
    //        //         GaussianBlur(img,img,Size(5,5),0,0);//高斯滤波，滤去噪声
    //        IplImage *dst_gray= &IplImage(img);//mat转IplImage
    //        IplImage *dst_bw1 = 0; //创建一个图
    //        dst_bw1 = cvCreateImage( cvGetSize(dst_gray), dst_gray->depth, dst_gray->nChannels);//根据灰度图创建
    //        cvThreshold( dst_gray, dst_bw1 ,value_m, 255, threshold_type ); //分割

    //        //分割出图像，并随机染色
    //        IplImage* dst1 = cvCreateImage( cvGetSize(dst_gray), 8, 3 );
    //        CvMemStorage* storage = cvCreateMemStorage(0);
    //        CvSeq* contour = 0;
    //        //函数cvFindContours从二值图像中检索轮廓，
    //        //并返回检测到的轮廓的个数。first_contour的值由函数填充返回，
    //        //它的值将为第一个外轮廓的指针，当没有轮廓被检测到时为NULL。
    //        //其它轮廓可以使用h_next和v_next连接，从first_contour到达。
    //        cvFindContours( dst_bw1,//输入图像，为二值图
    //                        storage,             //得到的轮廓的存储容器 CvMemStorage,
    //                        &contour,              //输出参数：包含第一个输出轮廓的指针
    //                        sizeof(CvContour),  //如果 method=CV_CHAIN_CODE，则序列头的大小 >=sizeof(CvChain)，否则 >=sizeof(CvContour)
    //                        CV_RETR_CCOMP,//提取模式.
    //                        CV_CHAIN_APPROX_SIMPLE );//逼近方法
    //        cvZero( dst1 );
    //        for( ; contour != 0; contour = contour->h_next )
    //        {
    //            CvScalar color = CV_RGB( rand()%255, rand()%255, rand()%255 );//rand()随机数函数
    //            //        CvScalar color = CV_RGB( rand()&255, rand()&255, rand()&255 );//rand()随机数函数，可以用%或者&表示随机数最大范围
    //            /* 用1替代 CV_FILLED  所指示的轮廓外形 */
    //            cvDrawContours( dst1, contour, color, color, -1, CV_FILLED, 8 );
    //        }

    //        Mat showMat=cvarrToMat(dst1);//IplImage转Mat
    //        QImage imgshow=MatToQImage( showMat);
    //        this-> ui->lb_OutToImg->clear();
    //        imgshow=imgshow.scaled( this->ui->lb_OutToImg->width(), this->ui->lb_OutToImg->height(),Qt::KeepAspectRatio);
    //        this-> ui->lb_OutToImg->setPixmap(QPixmap::fromImage(imgshow));
    //        }

    //            void drawContours//绘制轮廓，用于绘制找到的图像轮廓
    //            (
    //             InputOutputArray image,//要绘制轮廓的图像
    //             InputArrayOfArrays contours,//所有输入的轮廓，每个轮廓被保存成一个point向量
    //             int contourIdx,//指定要绘制轮廓的编号，如果是负数，则绘制所有的轮廓
    //             const Scalar& color,//绘制轮廓所用的颜色
    //             int thickness = 1, //绘制轮廓的线的粗细，如果是负数，则轮廓内部被填充
    //             int lineType = 8, /绘制轮廓的线的连通性
    //             InputArray hierarchy = noArray(),//关于层级的可选参数，只有绘制部分轮廓时才会用到
    //             int maxLevel = INT_MAX,//绘制轮廓的最高级别，这个参数只有hierarchy有效的时候才有效
    //                                                      //maxLevel=0，绘制与输入轮廓属于同一等级的所有轮廓即输入轮廓和与其相邻的轮廓
    //                                                      //maxLevel=1, 绘制与输入轮廓同一等级的所有轮廓与其子节点。
    //                                                      //maxLevel=2，绘制与输入轮廓同一等级的所有轮廓与其子节点以及子节点的子节点
    //             Point offset = Point()
    //            )
    //            cvDrawContours（）是旧的，之前的C-API。
    //            drawContours来自当前的c ++ api，它与cv :: Mat一起使用，来自cv :: namespace的函数。

    //            CV_EXPORTS_W void drawContours(
    //                        InputOutputArray image,
    //                        InputArrayOfArrays contours,
    //                        int contourIdx,
    //                        const Scalar& color,
    //                        int thickness = 1,
    //                        int lineType = LINE_8,
    //                        InputArray hierarchy = noArray(),
    //                        int maxLevel = INT_MAX,
    //                        Point offset = Point() );
    //            CVAPI(void)  cvDrawContours(
    //                        CvArr *img,
    //                        CvSeq* contour,
    //                        CvScalar external_color,
    //                        CvScalar hole_color,
    //                        int max_level,
    //                        int thickness CV_DEFAULT(1),
    //                        int line_type CV_DEFAULT(8),
    //                        CvPoint offset CV_DEFAULT(cvPoint(0,0)));

}

void ThresholdDlg::on_btnThreh_clicked()
{
     btnMatk=1;
    setTestImg();
    histogramGray();
}

void ThresholdDlg::on_bnt_area_clicked()
{
    areaDlg->show();
//    sendImage1(globMat);
}

void ThresholdDlg::on_bnt_contour_clicked()
{
    Mat image=outGlobalMat.clone();

//    GaussianBlur(image, image, Size(3, 3), 3, 3);//高斯滤波

    int mode=RETR_EXTERNAL;
    int method=CHAIN_APPROX_NONE;
    if(ui->comboBox_2->currentIndex()==0){mode=RETR_EXTERNAL;}//只检测外轮廓。忽略轮廓内部的洞。
    if(ui->comboBox_2->currentIndex()==1){mode=RETR_LIST;}           //检测所有轮廓，但不建立继承(包含)关系
    if(ui->comboBox_2->currentIndex()==2){mode=RETR_CCOMP;}
    // RETR_CCOMP：检测所有轮廓，但是仅仅建立两层包含关系。外轮廓放到顶层，
    //  外轮廓包含的第一层内轮廓放到底层，如果内轮廓还包含轮廓，那就把这些内轮廓放到顶层去。
    if(ui->comboBox_2->currentIndex()==3){mode=RETR_TREE;}       //检测所有轮廓，并且建立所有的继承(包含)关系。

    //RETR_EXTERNAL和RETR_LIST方法的时候hierarchy这个变量是没用的，
    //因为前者没有包含关系，找到的都是外轮廓，后者仅仅是找到所哟的轮廓但并不把包含关系区分。
    //用TREE这种检测方法的时候我们的hierarchy这个参数才是有意义的。
    //事实上，应用前两种方法的时候，我们就用findContours这个函数的第二种声明了，如下：
    //    findContours(
    //                InputOutputArray image,
    //                OutputArrayOfArrays contours,
    //                int mode,
    //                int method,
    //                Point offset=Point());


    if(ui->comboBox_3->currentIndex()==0){method=CHAIN_APPROX_NONE;}
    if(ui->comboBox_3->currentIndex()==1){method=CHAIN_APPROX_SIMPLE;}
    if(ui->comboBox_3->currentIndex()==2){method=CHAIN_APPROX_TC89_L1;}
    if(ui->comboBox_3->currentIndex()==3){method=CHAIN_APPROX_TC89_KCOS;}

    //    method：表示一条轮廓的方法。
    //     CHAIN_APPROX_NONE：把轮廓上所有的点存储。
    //     CHAIN_APPROX_SIMPLE：只存储水平，垂直，对角直线的起始点。对drawContours函数来说，这两种方法没有区别。

    //寻找所有轮廓
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(image,contours,hierarchy,mode,method,Point());// 函数将白色区域当作前景物体。所以找轮廓找到的是白色区域的轮廓。
    //hierarchy[i][0],hierarchy[i][1],hierarchy[i][2],hierarchy[i][3],
    //分别表示的是第i条轮廓(contours[i])的下一条，前一条，第一条轮廓和包含他的轮廓(父轮廓)。

    Mat resultImage =Mat::zeros(image.size(), CV_8UC3);//初始化dst
    drawContours(
                resultImage,     // 用于绘制的输入图像
                contours,           // 点的vectors的vector
                -1,                        // 需要绘制的轮廓的指数 (-1 表示 "all")
                Scalar(0,0,255),// 轮廓的颜色
                1,                        // 轮廓线的宽度
                8,                        //  轮廓线的邻域模式('4'邻域 或 '8'邻域)
                hierarchy);        // 可选 (从 findContours得到)

    //    void cv::drawContours(
    //        cv::InputOutputArray image, // 用于绘制的输入图像
    //        cv::InputArrayOfArrays contours, // 点的vectors的vector
    //        int contourIdx, // 需要绘制的轮廓的指数 (-1 表示 "all")
    //        const cv::Scalar& color, // 轮廓的颜色
    //        int thickness = 1, // 轮廓线的宽度
    //        int lineType = 8, //  轮廓线的邻域模式('4'邻域 或 '8'邻域)
    //        cv::InputArray hierarchy = noArray(), // 可选 (从 findContours得到)
    //        int maxLevel = INT_MAX, // 轮廓中的最大下降
    //        cv::Point offset = cv::Point() // (可选) 所有点的偏移
    //    )

    QImage imgshow=MatToQImage(resultImage);
    this-> ui->lb_OutToImg->clear();
    imgshow=imgshow.scaled( this->ui->lb_OutToImg->width(), this->ui->lb_OutToImg->height(),Qt::KeepAspectRatio);
    this-> ui->lb_OutToImg->setPixmap(QPixmap::fromImage(imgshow));

    showImage(resultImage);

}

  void ThresholdDlg::showImage(Mat showMat)
  {
      QImage imgshow=MatToQImage(showMat);
      //        m_graphicsScene->removeItem(pixItem2);   //将上一个图元从场景中移除,重新添加新的图元
      QPixmap pixmapCV =QPixmap::fromImage(imgshow.scaled(size(), Qt::KeepAspectRatio));
      pixItem2 = new PixItem(new QPixmap(pixmapCV));
      //将该图元对象添加到场景中，并设置此图元在场景中的位置为中心（0，0）
      m_graphicsScene2 = new QGraphicsScene;  //new 一个新的场景对象
      ui->graphicsView_2->setSceneRect(-10/2,-10/2,10,10);
      //设置开启滚动条范围，不然即使隐藏了滚动条后还是有滚动效果
      ui->graphicsView_2->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//隐藏横向滚动条
      ui->graphicsView_2->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//隐藏纵向滚动条
      ui->graphicsView_2->setScene(m_graphicsScene2);//添加场景
      m_graphicsScene2->addItem(pixItem2);//将图元添加到场景中
      pixItem2->setPos(0,0);//中心位置
  }


