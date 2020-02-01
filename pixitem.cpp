#include "pixitem.h"
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QPointF>
#include <QGraphicsSceneDragDropEvent>
#include <QDrag>
#include <math.h>
#include <QMessageBox>
#include <QGraphicsObject>
#include "thresholddlg.h"

//构造函数初始化了变量pix
PixItem::PixItem(QPixmap *pixmap)
{
    pix = *pixmap;
    setAcceptDrops(true); //设置可拖拽
    m_scaleValue = 0;//放缩值初始化
    m_isMove = false;//不可移动
    widx=0;
    widy=0;
}

//实现自己的图元边界函数
QRectF PixItem::boundingRect() const
{
    return QRectF(-pix.width()/2, -pix.height()/2,
                  pix.width(), pix.height());//需要对应图元，不然出现残影
}

//只需QPainter的drawPixmap()函数将图元图片绘出即可
void PixItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                    QWidget *)
{
    painter->drawPixmap(-pix.width()/2, -pix.height()/2, pix);//需要对应边界函数
}

//鼠标点击事件
void PixItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    m_startPos = event->pos();
    m_isMove = true;//图元是否可以移动
//////下面测试代码
//    QPointF m_center;
//    m_center=mapToScene(0,0);//坐标映射，图元坐标映射到窗口
//    double a =m_center.x();
//    QString str=QString::number(a,10,0); // 这是你的变量
//    QMessageBox mesg;
//    mesg.about(NULL,QString::fromLocal8Bit("图片大小") , str);
}

void PixItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{

//    if(sceneBoundingRect().width()<widx&&sceneBoundingRect().height()<widy)
//    {
//        m_isMove = false;//小于窗口时候不可以移动
//    }
//    if(sceneBoundingRect().width()>widx-20||sceneBoundingRect().height()>widy-20)
//    {
//        QPointF m_center;
//        m_center=mapToScene(0,0);//图元中心坐标
//        QPointF points = event->pos()- m_startPos;
//        int wx,wy;
//        wx=sceneBoundingRect().width()-widx;
//        wy=sceneBoundingRect().height()-widy;
//        if(pow(1.1,wx/2)>pow(1.1,m_center.x())||pow(1.1,wy/2)>pow(1.1,m_center.y()))
//        {
//            moveBy(points.x(),points.y());
//        }
//        m_isMove = false;
//    }


    if(m_isMove)
    {
        QPointF point =mapToScene(event->pos())-mapToScene(m_startPos);
        moveBy(point.x(),point.y());
        // 鼠标点击后并移动则图元相应移动，进行了图元坐标映射，映射到窗口中
    }
}

void PixItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
    m_isMove = false;
}


//双击复位
void PixItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{

    //    int ag=sceneBoundingRect().width();
    //    int bg=sceneBoundingRect().height();
    //    QString strag=QString::number(ag,10,0); // 这是你的变量
    //    QString strbg=QString::number(bg,10,0); // 这是你的变量
    //    QString  strg = QString::fromLocal8Bit("图片宽度：%1，图片高度：%2").arg(strag).arg(strbg);
    //    QMessageBox mesgg;
    //    mesgg.about(NULL,QString::fromLocal8Bit("图片信息") ,strg);

    setPos(0,0);
    m_scaleValue = 0;
    setScale(1);
    setTransformOriginPoint(0, 0);

    //做了坐标映射，将图片回复到窗口中心
    QPointF m_center;
    m_center=mapToScene(0,0);
    if(m_center.x()!=0||m_center.y()!=0)
    {
        if(m_center.x()>0&&m_center.y()>0) moveBy(-m_center.x(), -m_center.y());
        if(m_center.x()<0&&m_center.y()<0) moveBy(m_center.x(), m_center.y());
        if(m_center.x()>0&&m_center.y()<0) moveBy(-m_center.x(), m_center.y());
        if(m_center.x()<0&&m_center.y()>0) moveBy(m_center.x(), -m_center.y());
    }

}

//使用滚轮整体缩放
void PixItem::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    setZoomState(NO_STATE);
    int scaleValue = m_scaleValue;
    if(event->delta() > 0)  //delta（）为正，滚轮向上滚
    {
        scaleValue++;
    }
    else
    {
        scaleValue--;
    }

    /////测试代码以下
    //    if(scaleValue<=0)//小于窗口时候自动回中央
    //    {
    //        setPos(0,0);
    //        QPointF m_center;
    //        m_center=mapToScene(0,0);
    //        if(m_center.x()!=0||m_center.y()!=0)
    //        {
    //            if(m_center.x()>0&&m_center.y()>0) moveBy(-m_center.x(), -m_center.y());
    //            if(m_center.x()<0&&m_center.y()<0) moveBy(m_center.x(), m_center.y());
    //            if(m_center.x()>0&&m_center.y()<0) moveBy(-m_center.x(), m_center.y());
    //            if(m_center.x()<0&&m_center.y()>0) moveBy(m_center.x(), -m_center.y());
    //        }
    //    }
    //////////////*************以上

    if (scaleValue > ZOOM_IN_TIMES || scaleValue < ZOOM_OUT_TIMES)
        return;

    m_scaleValue = scaleValue;
    qreal s;
    if(m_scaleValue > 0)
    {
        s = pow(1.1, m_scaleValue);        //放大 计算x的y方次 参数都是double类型
    }
    else
    {
        s = pow(1 / 1.1, -m_scaleValue);      //缩小
    }
    setScale(s);//setScale设置比例放缩，内置的处理图像放缩的方法
    if(sceneBoundingRect().width()>=widx||sceneBoundingRect().height()>=widy)
    {
        //  m_isMove = false;
        setTransformOriginPoint(event->pos());//基于图元坐标内鼠标指针变换中心
    }

    //    QPointF m_center;
    //    m_center=mapToScene(event->pos().x(),event->pos().y());//转换至窗口后指针变换中心
    //    setTransformOriginPoint(m_center);
  }

//从widget获取的缩放值，用于同步滚轮和按键
void PixItem::setScaleValue(const int &scaleValue)
{
    if (scaleValue > ZOOM_IN_TIMES || scaleValue < ZOOM_OUT_TIMES)
        return;

    m_scaleValue = scaleValue;

////////*******测试代码以下
//    if(scaleValue<=0)//小于窗口时候自动回中央
//    {
//        setPos(0,0);
//        QPointF m_center;
//        m_center=mapToScene(0,0);
//        if(m_center.x()!=0||m_center.y()!=0)
//        {
//            if(m_center.x()>0&&m_center.y()>0) moveBy(-m_center.x(), -m_center.y());
//            if(m_center.x()<0&&m_center.y()<0) moveBy(m_center.x(), m_center.y());
//            if(m_center.x()>0&&m_center.y()<0) moveBy(-m_center.x(), m_center.y());
//            if(m_center.x()<0&&m_center.y()>0) moveBy(m_center.x(), -m_center.y());
//        }
//    }

    qreal s;
    if(m_scaleValue > 0)
    {
        s = pow(1.1, m_scaleValue);     //放大 计算x的y方次 参数都是double类型，去除正负数
    }
    else
    {
        s = pow(1 / 1.1, -m_scaleValue); //缩小
    }
    setScale(s);
}

//复原图像
void PixItem::setZoomState(const int &zoomState)
{
    m_zoomState = zoomState;
    if (m_zoomState == RESET)
    {
        m_scaleValue = 0;
        setScale(1);
        setTransformOriginPoint(0, 0);
    }
}

//获取放缩值
int PixItem::getScaleValue() const
{
    return m_scaleValue;
}

//返回窗口值
void  PixItem::setValue(const QPointF &pointxy)
{
    QPointF p;
    p=pointxy;
    widx=p.x();
    widy=p.y();
}
