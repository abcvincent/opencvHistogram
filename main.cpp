#include "mainwindow.h"
#include <QApplication>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;
using namespace std;

///// 全局变量定义及赋值



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);  

//    /**
//     * @主函数
//     */
//    /// 读取一副图片，不改变图片本身的颜色类型（该读取方式为DOS运行模式）
//    src = imread("D:/1.jpg", 1 );
//    /// 将图片转换成灰度图片
//    cvtColor( src, src_gray, CV_RGB2GRAY );
//    /// 创建一个窗口显示图片
//    namedWindow( window_name, CV_WINDOW_AUTOSIZE );
//    /// 创建滑动条来控制阈值,第一个滑条是对应分割类型，第二个对应是分割阈值
//    createTrackbar( trackbar_type,
//                    window_name, &threshold_type,
//                    max_type, Threshold_Demo );
//    createTrackbar( trackbar_value,
//                    window_name, &threshold_value,
//                    max_value, Threshold_Demo );
//    /// 初始化自定义的阈值函数
//    Threshold_Demo( 0, 0 );
//    /// 等待用户按键。如果是ESC健则退出等待过程。
//    while(true)
//    {
//      int c;
//      c = waitKey( 20 );
//      if( (char)c == 27 )
//        { break; }
//     }


//  /////////////////////////////////////////////////

//    Mat imageSource=imread("D:/ImageTest/test2.jpg",0);
//    imshow("Source Image",imageSource);
//    Mat image;
//    blur(imageSource,image,Size(3,3));
//    threshold(image,image,107,255,THRESH_BINARY_INV);
//    imshow("Threshold Image",image);

//    //寻找最外层轮廓
//    vector<vector<Point>> contours;
//    vector<Vec4i> hierarchy;
//    findContours(image,contours,hierarchy,RETR_EXTERNAL,CHAIN_APPROX_NONE,Point());

//    Mat imageContours=Mat::zeros(image.size(),CV_8UC1); //最小外接矩形画布
//    Mat imageContours1=Mat::zeros(image.size(),CV_8UC1); //最小外结圆画布
//    for(int i=0;i<contours.size();i++)
//    {
//        //绘制轮廓
//        drawContours(imageContours,contours,i,Scalar(255),1,8,hierarchy);
//        drawContours(imageContours1,contours,i,Scalar(255),1,8,hierarchy);


//        //绘制轮廓的最小外结矩形
//        RotatedRect rect=minAreaRect(contours[i]);
//        Point2f P[4];
//        rect.points(P);
//        for(int j=0;j<=3;j++)
//        {
//            line(imageContours,P[j],P[(j+1)%4],Scalar(255),2);
//        }

//        //绘制轮廓的最小外结圆
//        Point2f center; float radius;
//        minEnclosingCircle(contours[i],center,radius);
//        circle(imageContours1,center,radius,Scalar(255),2);

//    }
//    imshow("MinAreaRect",imageContours);
//    imshow("MinAreaCircle",imageContours1);
//    waitKey(0);




    MainWindow w;
    w.show();
    return a.exec();
}


