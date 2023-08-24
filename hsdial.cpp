#include "hsdial.h"

#include <QPainter>
#include <QPainterPath>
#include <QtMath>

HSDial::HSDial(QWidget *parent)
    : QWidget{parent}
{
    mv = 0;
    ma = 0;
    mhz = 0;
}


void HSDial::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    int width=this->width();
    int height=this->height();
    int radius=((width>height)?height:width)/2;
    //移动画笔到中下方
    painter.translate(width/2,height*0.6);
    //启用反锯齿
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);
    //设置画刷颜色
    painter.setBrush(QColor(138,43,226));
    DrawPoint(painter,radius);
    DrawDigital(painter,radius-10);
    DrawCircle(painter,radius-35);
   // DrawSmallScale(painter,radius-40);
    DrawBigScale(painter,radius-55);
    DrawText(painter,radius/2);
    DrawPointer(painter,radius-100);
}


void HSDial::DrawPoint(QPainter& painter,int radius)
{
    //组装点的路径图
    QPainterPath pointPath;
    pointPath.moveTo(-2,-2);
    pointPath.lineTo(2,-2);
    pointPath.lineTo(2,2);
    pointPath.lineTo(0,4);
    pointPath.lineTo(-2,2);
    //绘制13个小点
    for(int i=0;i<13;++i){
        QPointF point(0,0);
        painter.save();
        painter.setBrush(QColor(255,127,80));
        //计算并移动绘图对象中心点
        point.setX(radius*qCos(((210-i*20)*M_PI)/180));
        point.setY(radius*qSin(((210-i*20)*M_PI)/180));
        //计算并移动绘图对象的中心点
        painter.translate(point.x(),-point.y());
        //计算并选择绘图对象坐标
        painter.rotate(-120+i*20);
        //绘制路径
        painter.drawPath(pointPath);
        painter.restore();
    }
}
void HSDial::DrawDigital(QPainter& painter,int radius)
{
    //设置画笔，画笔默认NOPEN
        painter.setPen(QColor(218,112,214));
        QFont font;
        font.setFamily("Cambria");
        font.setPointSize(9);
        painter.setFont(font);
        for(int i=0;i<13;++i){
            QPointF point(0,0);
            painter.save();
            point.setX(radius*qCos(((210-i*20)*M_PI)/180));
            point.setY(radius*qSin(((210-i*20)*M_PI)/180));
            painter.translate(point.x(),-point.y());
            painter.rotate(-120+i*20);
            painter.drawText(-15, -5, 40, 30,Qt::AlignCenter,QString::number(i*20));
            painter.restore();
        }
        //还原画笔
        painter.setPen(Qt::NoPen);
}
void HSDial::DrawCircle(QPainter& painter,int radius)
{
    //保存绘图对象
        painter.save();
        //计算大小圆路径
        QPainterPath outRing;
        QPainterPath inRing;
        outRing.moveTo(0,0);
        inRing.moveTo(0,0);
        outRing.arcTo(-radius,-radius, 2*radius,2*radius,-31,242);
        inRing.addEllipse(-radius+10,-radius+10,2*(radius-10),2*(radius-10));
        outRing.closeSubpath();
        //设置渐变色k
        QRadialGradient radialGradient(0,0,radius,0,0);
        radialGradient.setColorAt(0.93,QColor(43,120,255));
        radialGradient.setColorAt(1,QColor(0,0,0));
        //设置画刷
        painter.setBrush(radialGradient);
        //大圆减小圆
        painter.drawPath(outRing.subtracted(inRing));
        //painter.drawPath(outRing);
        //painter.drawPath(inRing);
        painter.restore();
}
void HSDial::DrawSmallScale(QPainter& painter,int radius)
{
    //组装点的路径图
        QPainterPath pointPath;
        pointPath.moveTo(-2,-2);
        pointPath.lineTo(-1,-4);
        pointPath.lineTo(1,-4);
        pointPath.lineTo(2,-2);
        pointPath.lineTo(1,8);
        pointPath.lineTo(-1,8);
        //绘制121个小点
        for(int i=0;i<121;++i){
            QPointF point(0,0);
            painter.save();
            point.setX(radius*qCos(((210-i*2)*M_PI)/180));
            point.setY(radius*qSin(((210-i*2)*M_PI)/180));
            painter.translate(point.x(),-point.y());
            painter.rotate(-120+i*2);
            if(i>=90) painter.setBrush(QColor(250,0,0));
            painter.drawPath(pointPath);
            painter.restore();
        }
}
void HSDial::DrawBigScale(QPainter& painter,int radius)
{
    //组装点的路径图
        QPainterPath pointPath1;
        pointPath1.moveTo(-2,-2);
        pointPath1.lineTo(-1,-4);
        pointPath1.lineTo(1,-4);
        pointPath1.lineTo(2,-2);
        pointPath1.lineTo(1,8);
        pointPath1.lineTo(-1,8);
        QPainterPath pointPath2;
        pointPath2.moveTo(-2,-2);
        pointPath2.lineTo(-1,-4);
        pointPath2.lineTo(1,-4);
        pointPath2.lineTo(2,-2);
        pointPath2.lineTo(1,15);
        pointPath2.lineTo(-1,15);
        //绘制25个刻度
        for(int i=0;i<25;++i){
            QPointF point(0,0);
            painter.save();
            point.setX(radius*qCos(((210-i*10)*M_PI)/180));
            point.setY(radius*qSin(((210-i*10)*M_PI)/180));
            painter.translate(point.x(),-point.y());
            painter.rotate(-120+i*10);
            if(i>=18) painter.setBrush(QColor(250,0,0));
            if(i%2){
                painter.drawPath(pointPath1);
            }
            else{
                painter.drawPath(pointPath2);
            }
            painter.restore();
        }
}

/*
void HSDial::DrawText(QPainter& painter,int radius)
{
    painter.save();
    painter.setPen(QColor(255,255,255));

    //自己添加,2023.07.17
    QFont labelFont_hsdial_1("Microsoft YaHei", 10);
    labelFont_hsdial_1.setBold(true);
    QFont labelFont_hsdial_2("Microsoft YaHei", 14);
    labelFont_hsdial_2.setBold(true);

    //QFont font;
    //font.setFamily("Cambria");
    //font.setBold(true);
    //font.setPointSize(12);
    //设置仪表盘内文字,xy是以圆心为准
    painter.setFont(labelFont_hsdial_1);//表盘标题的字体设置
    //painter.drawText(0,0,this->width(),this->height(),Qt::AlignCenter,"123ABC一二三");
    //painter.drawText(0, 50, 80, 30,Qt::AlignLeft,QString("频率:%1Hz").arg(QString::number(mhz,'f',1)));
    //painter.drawText(x, y, 文字框长度(60), 文字框高度(2),Qt::AlignLeft,QString("电压:%1V").arg(QString::number(mv,'f',1)));
    painter.setPen(QColor(64,64,68));//表盘标题的字体颜色
    //自己改的部分
    //painter.drawText(-145, -35, this->width(), this->height(),Qt::AlignLeft,QString("电压:%1V").arg(QString::number(mv,'f',1)));
    //painter.drawText(-32, -35, this->width(), this->height(),Qt::AlignLeft,QString("电流:%1A").arg(QString::number(ma,'f',1)));
    //painter.drawText(63, -35, this->width(), this->height(),Qt::AlignLeft,QString("频率:%1Hz").arg(QString::number(mhz,'f',1)));

    painter.drawText(-40, -radius+10, 80, 30,Qt::AlignCenter,QString("电压:%1V").arg(QString::number(mv,'f',1)));
    painter.drawText(-40, radius-40, 80, 30,Qt::AlignCenter,QString("电流:%1A").arg(QString::number(ma,'f',1)));
    painter.drawText(-radius+10, -30, 89, 30,Qt::AlignCenter,QString("频率:%1Hz").arg(QString::number(mhz,'f',1)));

    //painter.drawText(-40, -radius+10, 80, 30,Qt::AlignCenter,QString("电压:%1V").arg(QString::number(mv,'f',1)));
    //painter.drawText(-40, radius-40, 80, 30,Qt::AlignCenter,QString("电流:%1A").arg(QString::number(ma,'f',1)));
    //painter.drawText(-radius+10, -30, 80, 30,Qt::AlignCenter,QString("频率:%1Hz").arg(QString::number(mhz,'f',1)));

    //painter.drawText(-40, -radius+10, 80, 30,Qt::AlignCenter,QString("电压:%1V").arg(QString::number(mv,'f',1)));
    //painter.drawText(-40, radius-40, 80, 30,Qt::AlignCenter,QString("电流:%1A").arg(QString::number(ma,'f',1)));
    //painter.drawText(-radius+10, -30, 80, 30,Qt::AlignCenter,QString("频率:%1Hz").arg(QString::number(mhz,'f',1)));
    //painter.drawText(0, -30, 80, 30,Qt::AlignCenter,QString("W:0.98"));
    //painter.drawText(-35, radius-30, 90, 30,Qt::AlignCenter,QString("φ:0.0125"));
    painter.restore();
    painter.setPen(QColor(255,0,0));//表盘标题的字体颜色
    //font.setPointSize(20);

    painter.setFont(labelFont_hsdial_2);//表盘标题的字体设置
    painter.drawText(-radius+20, radius/2+10, 80, 40,Qt::AlignCenter,this->mstrTitle);
    //painter.setFont(font);
    //painter.drawText(-radius+20, radius/2+10, 80, 40,Qt::AlignCenter,this->mstrTitle);
    //自己改的部分
    //painter.drawText(-315, -140, this->width(), this->height(),Qt::AlignCenter,this->mstrTitle);
    //painter.drawText(-radius+20, radius/2+10, 80, 40,Qt::AlignCenter,this->mstrTitle);
    //painter.drawText(-35, radius-30, 90, 30,Qt::AlignCenter,QString("φ:0.0125"));
    painter.restore();
}
*/


void HSDial::DrawText(QPainter& painter,int radius)
{
    painter.save();
    painter.setPen(QColor(255,255,255));
    QFont font;
    font.setFamily("Cambria");
    //font.setBold(true);
    font.setPointSize(18);
//    painter.drawText(-40, -radius+10, 80, 30,Qt::AlignCenter,QString("电压:%1V").arg(QString::number(mv,'f',1)));
//    painter.drawText(-40, radius-40, 80, 30,Qt::AlignCenter,QString("电流:%1A").arg(QString::number(ma,'f',1)));
//    painter.drawText(-radius+10, -30, 80, 30,Qt::AlignCenter,QString("频率:%1Hz").arg(QString::number(mhz,'f',1)));


    painter.drawText(-40, -radius+10, 80, 30,Qt::AlignCenter,QString("电流:%1A").arg(QString::number(ma,'f',1)));
    painter.drawText(-radius+10, -30, 80, 30,Qt::AlignCenter,QString("频率:%1Hz").arg(QString::number(mhz,'f',1)));






//    painter.drawText(0, -30, 80, 30,Qt::AlignCenter,QString("W:0.98"));
//    painter.drawText(-35, radius-30, 90, 30,Qt::AlignCenter,QString("φ:0.0125"));
    painter.restore();
    painter.setPen(QColor(255,0,0));//设置仪表盘标题的颜色
    font.setPointSize(20);
    painter.setFont(font);

    painter.drawText(-40, radius-40, 80, 30,Qt::AlignCenter,QString("%1V").arg(QString::number(mv,'f',1)));

//    painter.drawText(-radius+20, radius/2+10, 80, 40,Qt::AlignLeft,this->mstrTitle);
    painter.drawText(-40,radius, 80, 40,Qt::AlignCenter,this->mstrTitle);
//    painter.drawText(-35, radius-30, 90, 30,Qt::AlignCenter,QString("φ:0.0125"));
    painter.restore();
}


void HSDial::DrawPointer(QPainter& painter,int radius)
{
    //组装点的路径图
        QPainterPath pointPath;
        pointPath.moveTo(3,0);
        pointPath.lineTo(1,-radius-25);
        pointPath.lineTo(-1,-radius-25);
        pointPath.lineTo(-3,0);
        pointPath.arcTo(-3,0,6,6,180,180);
        QPainterPath inRing;
        inRing.addEllipse(-2,-2,4,4);
        painter.save();
        QRadialGradient radialGradient(0,0,radius,0,0);
        radialGradient.setColorAt(0,QColor(255,252,27));
        radialGradient.setColorAt(1,QColor(255,252,27));
        //计算并选择绘图对象坐标
        painter.rotate(degRotate);
        painter.setBrush(radialGradient);
        painter.drawPath(pointPath.subtracted(inRing));
        painter.restore();
}
void HSDial::drawIndicator(QPainter* painter)
{

}

void HSDial::valueChanged(float val)
{
    this->degRotate = val;
        update();
}
