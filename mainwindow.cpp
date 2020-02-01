#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
#include <QTime>
#include <QDir>
#include <QFile>

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


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    globalMat=NULL;
    globalMark=0;
    showMark=0;

    QObject::connect(ui->spinBox,SIGNAL(valueChanged(int)),
                     ui->horizontalSlider,SLOT(setValue(int)));//spinBox值与滑条对应
    QObject::connect(ui->horizontalSlider,SIGNAL(valueChanged(int)),
                     ui->spinBox,SLOT(setValue(int)));//滑条与spinBox值对应

    connect(ui->horizontalSlider,SIGNAL(sliderReleased()),this,SLOT(setTestImg()));
    connect(ui->spinBox,SIGNAL(valueChanged(int)),this,SLOT(setTestImg(int)));

    QObject::connect(ui->spinBox_2,SIGNAL(valueChanged(int)),
                     ui->horizontalSlider_2,SLOT(setValue(int)));//spinBox值与滑条对应
    QObject::connect(ui->horizontalSlider_2,SIGNAL(valueChanged(int)),
                     ui->spinBox_2,SLOT(setValue(int)));//滑条与spinBox值对应
//    ui->horizontalSlider_2->setValue(0);//滑条初始值，范围值在窗口设计器里设置了
    ui->horizontalSlider->setMaximum(255);
    ui->spinBox->setMaximum(255);

//    this->ui->label_4->installEventFilter(this);
    this->ui->spinBox->installEventFilter(this);

    TDlg=new ThresholdDlg;
    connect(this,SIGNAL(sendImage(Mat)),TDlg,SLOT(receiveMainImage(Mat)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_on_openfile_triggered()
{

    ui->originalImg_label->clear();
    ui->resultImg_label->clear();
    QString fileName = QFileDialog::getOpenFileName(
                this, tr("open image file"),
                "./", tr("Image files(*.bmp *.jpg *.pbm *.pgm *.png *.ppm *.xbm *.xpm);;All files (*.*)"));

    if(fileName.isEmpty())
    {
        QMessageBox mesg;
        mesg.warning(this,"警告","打开图片失败!");//.about(NULL,"图片大小", strg);
        return;
    }
    else
    {
        QString  str=fileName;//文件名
        std::string path = str.toLocal8Bit().toStdString(); //opencv加载中文问题关键是这个
        Mat pImgs=imread(path);//cv函数创建Mat

        emit sendImage(pImgs);   
        globalMat=pImgs;
        globalImg=MatToQImage(pImgs);


        //        Mat bw = 50 < 128 ? (globalMat < 100) : (globalMat < 10);
//        Mat bw = globalMat<100;
//        globalImg=MatToQImage(bw);



        QImage img;
        ui->originalImg_label->clear();
        img=globalImg.scaled(ui->originalImg_label->width(),ui->originalImg_label->height(),Qt::KeepAspectRatio);
        ui->originalImg_label->setPixmap(QPixmap::fromImage(img));
        globalMark=1;
        showMark=0;

        if(img.isNull())
        {
            QMessageBox::information(this,"信息","打开文件失败");
        }
    }
}


QImage MainWindow::MatToQImage(const cv::Mat& mat)
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

void MainWindow::on_on_restore_triggered()
{
    ui->resultImg_label->clear();
}

void MainWindow::on_ac_clear_triggered()
{
    ui->originalImg_label->clear();
    ui->resultImg_label->clear();
    globalMat=NULL;
    globalMark=0;
    showMark=0;
}

void MainWindow::on_ac_save_triggered()
{
    if( globalMark==1)
    {
        QString dirPath;
        QString filePath;
        QString saveName="原始图";
        QString saveName2="处理结果图";

        QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
        QString str = time.toString("yyyyMMdd_hhmmss"); //设置显示格式
        filePath = QFileDialog::getExistingDirectory(this,"");
        dirPath=QString("%1/Opencv develop temp%2").arg(filePath).arg(str);
        QDir *temp = new QDir;//声明文件对象
        temp->mkdir(dirPath);//创建文件夹

        QImage images;
        images=MatToQImage( globalMat);///////////////////////////////////////
        QString savePath=QString("%1//%2.jpg").arg(dirPath).arg(saveName);
        images.save(savePath);

        QImage images2;
        images2=MatToQImage( global_ResultMat);///////////////////////////////////////
        QString savePath2=QString("%1//%2.jpg").arg(dirPath).arg(saveName2);

        if(images2.isNull())
        {
              QMessageBox::information(this,"信息","处理结果为空未保存");
        }
        else
       {
             images2.save(savePath2);
        }

        QMessageBox::information(this,"信息","保存成功");
    }
    else
    {
        QMessageBox::information(this,"信息","保存失败");
    }
}

void MainWindow::on_ac_histogramRGB_triggered()
{
    Mat src;
    src=globalMat;
    if( !src.data )
    {
        QMessageBox::information(this,"信息","没有加载图像");
    }
    else
    {
        /// 分割成3个单通道图像 ( R, G 和 B )
        vector<Mat> rgb_planes;
        split( src, rgb_planes );
        /// 设定bin数目
        int histSize = 255;
        /// 设定取值范围 ( R,G,B) )
        float range[] = { 0, 255 } ;
        const float* histRange = { range };
        bool uniform = true; bool accumulate = false;
        Mat r_hist, g_hist, b_hist;
        /// 计算直方图:
        calcHist( &rgb_planes[0], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate );
        calcHist( &rgb_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate );
        calcHist( &rgb_planes[2], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate );
        // 创建直方图画布
        int hist_w = 400; int hist_h = 400;
        int bin_w = cvRound( (double) hist_w/histSize );
        Mat histImage( hist_w, hist_h, CV_8UC3, Scalar( 0,0,0) );
        /// 将直方图归一化到范围 [ 0, histImage.rows ]
        normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
        normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
        normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
        /// 在直方图画布上画出直方图
        for( int i = 1; i < histSize; i++ )
        {
            line( histImage, Point( bin_w*(i-1), hist_h - cvRound(r_hist.at<float>(i-1)) ) ,
                  Point( bin_w*(i), hist_h - cvRound(r_hist.at<float>(i)) ),
                  Scalar( 0, 0, 255), 2, 8, 0  );
            line( histImage, Point( bin_w*(i-1), hist_h - cvRound(g_hist.at<float>(i-1)) ) ,
                  Point( bin_w*(i), hist_h - cvRound(g_hist.at<float>(i)) ),
                  Scalar( 0, 255, 0), 2, 8, 0  );
            line( histImage, Point( bin_w*(i-1), hist_h - cvRound(b_hist.at<float>(i-1)) ) ,
                  Point( bin_w*(i), hist_h - cvRound(b_hist.at<float>(i)) ),
                  Scalar( 255, 0, 0), 2, 8, 0  );
        }

//        /// 显示直方图
//        namedWindow("calcHist Demo", CV_WINDOW_AUTOSIZE );
//        imshow("calcHist Demo", histImage );
//        waitKey(0);

        QImage imgshow=MatToQImage(histImage);
        this-> ui->resultImg_label->clear();
        imgshow=imgshow.scaled( this->ui->resultImg_label->width(), this->ui->resultImg_label->height(),Qt::KeepAspectRatio);
        this-> ui->resultImg_label->setPixmap(QPixmap::fromImage(imgshow));
    }
}


void MainWindow::on_ac_test_triggered()
{
    showMark=4;

    this->ui->spinBox->setValue(160);
    int threshval=this->ui->spinBox->value();
    Mat img=globalMat;

    //进行，灰度处理
    cv::cvtColor(img,img,COLOR_RGB2GRAY);

    Mat bw = threshval < 128 ? (img < threshval) : (img > threshval);
    vector<vector<Point> > contours;  //定义点和向量
    vector<Vec4i> hierarchy;//定义点和向量
    findContours( bw, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );  //查找轮廓
    Mat dst = Mat::zeros(img.size(), CV_8UC3);//初始化dst
    if( !contours.empty() && !hierarchy.empty() ) //开始处理
    {
        //遍历所有顶层轮廓，随机生成颜色值绘制给各连接组成部分
        for(  int idx = 0; idx >= 0; idx = hierarchy[idx][0] )
        {
            Scalar color( (rand()&255), (rand()&255), (rand()&255) );
            //绘制填充轮廓
            drawContours(
                        dst,                    //用来绘制轮廓的图像
                        contours,         //指向第一个轮廓的指针
                        idx,                   //
                        color,               //内层轮廓的颜色
                        CV_FILLED,     //绘制轮廓的最大等级,绘制轮廓时所使用的线条的粗细度。如果值为负(e.g. =CV_FILLED),绘制内层轮廓。
                        8,                     //线条的类型
                        hierarchy );    //按照给出的偏移量移动每一个轮廓点坐标.
        }
    }

//    imshow( "Connected Components", dst );//显示窗口

    QImage imgshow=MatToQImage(dst);
    this-> ui->resultImg_label->clear();
    imgshow=imgshow.scaled( this->ui->resultImg_label->width(), this->ui->resultImg_label->height(),Qt::KeepAspectRatio);
    this-> ui->resultImg_label->setPixmap(QPixmap::fromImage(imgshow));

}

//这个延时比较严重
void MainWindow::setTestImg(int value_m)
{
    Mat img=globalMat;


    if(showMark==4)
    {
    //进行，灰度处理
    cv::cvtColor(img,img,COLOR_RGB2GRAY);

    Mat bw = value_m< 128 ? (img < value_m) : (img > value_m);
    vector<vector<Point> > contours;  //定义点和向量
    vector<Vec4i> hierarchy;//定义点和向量
    findContours( bw, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );  //查找轮廓
    Mat dst = Mat::zeros(img.size(), CV_8UC3);//初始化dst
    if( !contours.empty() && !hierarchy.empty() ) //开始处理
    {
        //遍历所有顶层轮廓，随机生成颜色值绘制给各连接组成部分
        int idx = 0;
        for( ; idx >= 0; idx = hierarchy[idx][0] )
        {
            Scalar color( (rand()&255), (rand()&255), (rand()&255) );
            //绘制填充轮廓
            drawContours( dst, contours, idx, color, CV_FILLED, 8, hierarchy );
        }
    }

//    imshow( "Connected Components", dst );//显示窗口

    QImage imgshow=MatToQImage(dst);
    this-> ui->resultImg_label->clear();
    imgshow=imgshow.scaled( this->ui->resultImg_label->width(), this->ui->resultImg_label->height(),Qt::KeepAspectRatio);
    this-> ui->resultImg_label->setPixmap(QPixmap::fromImage(imgshow));
     global_ResultMat=dst;
    }

}

void MainWindow::setTestImg()
{

    Mat img=globalMat;

    if(showMark==4)
    {
        int value_m=this->ui->spinBox->value();

        //进行，灰度处理
        cv::cvtColor(img,img,COLOR_RGB2GRAY);

        Mat bw = value_m< 128 ? (img < value_m) : (img > value_m);
        vector<vector<Point> > contours;  //定义点和向量
        vector<Vec4i> hierarchy;//定义点和向量
        findContours( bw, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );  //查找轮廓
        Mat dst = Mat::zeros(img.size(), CV_8UC3);//初始化dst
        if( !contours.empty() && !hierarchy.empty() ) //开始处理
        {
        //遍历所有顶层轮廓，随机生成颜色值绘制给各连接组成部分
        int idx = 0;
        for( ; idx >= 0; idx = hierarchy[idx][0] )
        {
            Scalar color( (rand()&255), (rand()&255), (rand()&255) );
            //绘制填充轮廓
            drawContours( dst, contours, idx, color, CV_FILLED, 8, hierarchy );
        }
    }

//    imshow( "Connected Components", dst );//显示窗口

    QImage imgshow=MatToQImage(dst);
    this-> ui->resultImg_label->clear();
    imgshow=imgshow.scaled( this->ui->resultImg_label->width(), this->ui->resultImg_label->height(),Qt::KeepAspectRatio);
    this-> ui->resultImg_label->setPixmap(QPixmap::fromImage(imgshow));
    global_ResultMat=dst;
    }

}


//bool MainWindow::eventFilter(QObject *obj, QEvent *event)
//{
//    if (obj ==this->ui->spinBox) {
//        if (event->type() ==QEvent::MouseButtonPress)
//        {
//            qDebug() << "Ate key press" ;
////            setTestImg();
////            ui->spinBox->setValue(ui->horizontalSlider->value());
//            return true;
//        }
//        else
//        {
//            return false;
//        }
//    }
//    else
//    {
//        // pass the event on to the parent class
//        return QMainWindow::eventFilter(obj, event);
//    }
//}

void MainWindow::on_ac_histogramGray_triggered()
{

#define cvQueryHistValue_1D( hist, idx0 )\
    ((float)cvGetReal1D( (hist)->bins, (idx0)))

    //进行，灰度处理
    //matOri输入图像 由对话框来选
    //dstImage输出图像，最后希望显示的图像必须添加
    //例如：cv::cvtColor(matOri,dstImage,COLOR_RGB2GRAY);

    //灰度直方图处理测试

    IplImage *src= &IplImage(globalMat);
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
    QImage imgshow=MatToQImage(outMat);
    this-> ui->resultImg_label->clear();
    imgshow=imgshow.scaled( this->ui->resultImg_label->width(), this->ui->resultImg_label->height(),Qt::KeepAspectRatio);
    this-> ui->resultImg_label->setPixmap(QPixmap::fromImage(imgshow));

    global_ResultMat=outMat;
}

void MainWindow::on_ac_threshold_triggered()
{

    if(!globalMat.empty())
    {
        TDlg->show();
    }
    else {

    }
}


