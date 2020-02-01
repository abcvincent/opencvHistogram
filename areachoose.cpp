#include "areachoose.h"
#include "ui_areachoose.h"

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

AreaChoose::AreaChoose(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AreaChoose)
{
    ui->setupUi(this);


    QObject::connect(ui->spinBox_Min,SIGNAL(valueChanged(int)),
                     ui->horizontalSlider_Min,SLOT(setValue(int)));//spinBox值与滑条对应
    QObject::connect(ui->horizontalSlider_Min,SIGNAL(valueChanged(int)),
                     ui->spinBox_Min,SLOT(setValue(int)));//滑条与spinBox值对应
    ui->horizontalSlider_Min->setValue(20);//初始化

    QObject::connect(ui->spinBox_Max,SIGNAL(valueChanged(int)),
                     ui->horizontalSlider_Max,SLOT(setValue(int)));//spinBox值与滑条对应
    QObject::connect(ui->horizontalSlider_Max,SIGNAL(valueChanged(int)),
                     ui->spinBox_Max,SLOT(setValue(int)));//滑条与spinBox值对应
    ui->horizontalSlider_Max->setValue(80);//初始化

    connect(ui->horizontalSlider_Min,SIGNAL(sliderReleased()),this,SLOT(setTestImgA()));//滑条释放后刷新处理图
    connect(ui->horizontalSlider_Max,SIGNAL(sliderReleased()),this,SLOT(setTestImgA()));//滑条释放后刷新处理图
    connect(ui->spinBox_Min,SIGNAL(valueChanged(int)),this,SLOT(setTestImgA()));//滑条释放后刷新处理图
    connect(ui->spinBox_Max,SIGNAL(valueChanged(int)),this,SLOT(setTestImgA()));//滑条释放后刷新处理图


}

AreaChoose::~AreaChoose()
{
    delete ui;
}

void  AreaChoose::receiveMainImage1(Mat  reMat)
{
    QImage  img2;
    img2=MatToQImage(reMat);
    outGlobalMatA=reMat.clone();
//    img2=globImg.scaled(ui->lb_InToImg->width(),ui->lb_InToImg->height(),Qt::KeepAspectRatio);
//    ui->lb_InToImg->setPixmap(QPixmap::fromImage(img2));

    ui->label_T->setPixmap(QPixmap::fromImage(img2));

}


void AreaChoose::setTestImgA()
{

    if(ui->spinBox_Min->value()>=ui->spinBox_Max->value()&&ui->spinBox_Max->value()!=100)
    {
        ui->spinBox_Max->setValue(ui->spinBox_Min->value()+1);
    }
    else if(ui->spinBox_Min->value()==100)
    {
       ui->spinBox_Min->setValue(ui->spinBox_Min->value()-1);
    }

    Mat dst=outGlobalMatA.clone();
    Mat img=outGlobalMatA.clone();

    //寻找最外层轮廓
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
//    findContours(dst,contours,hierarchy,RETR_EXTERNAL,CHAIN_APPROX_NONE,Point());//最外层轮廓
    findContours(dst,contours,hierarchy,RETR_CCOMP, CHAIN_APPROX_SIMPLE,Point());//所有轮廓CV_CHAIN_APPROX_SIMPLE
    //    findContours返回为vector<vector<Point> >的轮廓向量

    Mat dst1 = Mat::zeros(img.size(), CV_8UC3);//初始化dst

//    if( !contours.empty() && !hierarchy.empty() ) //开始处理
//    {
//        //遍历所有顶层轮廓，随机生成颜色值绘制给各连接组成部分
//        for(  int idx = 0; idx >= 0; idx = hierarchy[idx][0] )
//        {
//            Scalar color( (255), (0), (0) );//随机产生颜色 rand()产生随机数
//            //绘制填充轮廓
//            drawContours(
//                        dst1,                    //用来绘制轮廓的图像
//                        contours,         //指向第一个轮廓的指针
//                        idx,                   //
//                        color,               //内层轮廓的颜色
//                        CV_FILLED,     //绘制轮廓的最大等级,绘制轮廓时所使用的线条的粗细度。如果值为负(e.g. =CV_FILLED),绘制内层轮廓。
//                        8,                     //线条的类型
//                        hierarchy );    //按照给出的偏移量移动每一个轮廓点坐标.
//        }
//    }


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


    double  contour_area_g=0;
    double  contour_area_g2=0;
    contour_area_g=(contour_area_max-contour_area_min-1)/100;
    double  area_max=contour_area_g*(ui->spinBox_Max->value()+1);
    double  area_min=contour_area_g*(ui->spinBox_Min->value());

    if( !contours.empty() && !hierarchy.empty() ) //开始处理
    {
        //遍历所有顶层轮廓，随机生成颜色值绘制给各连接组成部分
        for(  int idx = 0; idx >= 0; idx = hierarchy[idx][0] )
        {
            contour_area_g2=fabs(contourArea(contours[idx]));

            if(contour_area_g2>=area_min&&contour_area_g2<area_max)
            {
                Scalar color( (255), (0), (0) );//随机产生颜色 rand()产生随机数
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
            else
            {
                Scalar color( (0), (0), (255) );//随机产生颜色 rand()产生随机数
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

    }





    QImage  imgA= MatToQImage (dst1);
    ui->label_Area->setPixmap(QPixmap::fromImage(imgA));

}


QImage AreaChoose::MatToQImage(const cv::Mat& mat)
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
