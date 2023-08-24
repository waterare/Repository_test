#ifndef HMIMAINWINDOW_H
#define HMIMAINWINDOW_H
#include <QtCharts>
#include <QMainWindow>
#include <QSerialPort>
#include <QSettings>
#include "numkeyboard.h"
#include "widgets/qcustomplot.h"
using namespace QtCharts;

namespace Ui {
class HMIMainWindow;
}

class HMIMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HMIMainWindow(QWidget *parent = 0);
    ~HMIMainWindow();
    bool SerialPortInit();
    void Read_Data();

//2023.07.21增加
signals:
    void Update_serial();



private slots:
    void on_pbreadcfg_pressed();

    void on_pbmodifycfg_pressed();
    void on_UpdateUI();
    void on_UpdateUI2();
    void on_tbONOFF_triggered(QAction *arg1);

    void on_tbONOFF_pressed();

    void on_pbmodifycfg_clicked();

    void on_pbrefresh_clicked();

    void on_pbset_clicked();

    void on_tbONOFF_clicked();

    void on_HMIMainWindow_destroyed();

    void on_tbrun_triggered(QAction *arg1);

    void on_tbsmb_triggered(QAction *arg1);

    void on_tbconfig_triggered(QAction *arg1);

    void on_pushButton_pressed();

    void on_pushButton_2_pressed();

    void on_pushButton_3_pressed();

    void on_pushButton_4_clicked();

    void on_comboBox_com_activated(int index);



private:
    Ui::HMIMainWindow *ui;

    void TimeData_Update(void);
    void Show_Plot(QCustomPlot *customPlot,QCPGraph *graph, double num_x,double num_y);
    void QPlot_init(QCustomPlot *customPlot);

    void TimeData_Update_(void);
    void Show_Plot_(QCustomPlot *customPlot, QCPGraph *graph, double num_y);


private:
QChart *m_chart;
QLineSeries *m_series;
QLineSeries *m_smbseries;

QThread m_workthread,m_workthread_1;

NumKeyboard *numkeyboard;

// 绘图控件的指针
QCustomPlot *pPlot1;
// 绘图控件中曲线的指针
QCPGraph *pGraph1_1;
QCPGraph *pGraph1_2;

QTimer qcustomplot_timer_;

double nowtime_,begintime_;

QDateTime BeginTime, EndTime ,FlagTime ,CursorTime;//更新UI中使用的常量
bool Flag_once = true,time_f_ = true;


// QWidget interface
protected:
void closeEvent(QCloseEvent *event);

// QObject interface
public:
bool eventFilter(QObject *watched, QEvent *event);
};

#endif // HMIMAINWINDOW_H
