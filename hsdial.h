#ifndef HSDIAL_H
#define HSDIAL_H

#include <QWidget>

class HSDial : public QWidget
{
    Q_OBJECT
public:
    explicit HSDial(QWidget *parent = nullptr);
    void SetTitle(QString tt)
    {
        this->mstrTitle = tt;
    }
    void setVAHZ(float v,float a,float hz)
    {
        mv = v;
        ma =a;
        mhz =hz;
        degRotate = mv-120;
        update();
    }

signals:


    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event);

private:
    float degRotate = -120;//220-120;
    float mv;
    float ma;
    float mhz;
    QString mstrTitle;

private:
private:
    void DrawPoint(QPainter&,int);
    void DrawDigital(QPainter&,int);
    void DrawCircle(QPainter&,int);
    void DrawSmallScale(QPainter&,int);
    void DrawBigScale(QPainter&,int);
    void DrawText(QPainter&,int);
    void DrawPointer(QPainter&,int);
    void drawIndicator(QPainter *painter);



public slots:
    void valueChanged(float);

};

#endif // HSDIAL_H
