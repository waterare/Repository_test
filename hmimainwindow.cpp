#include "hmimainwindow.h"
#include "ui_hmimainwindow.h"
#include <QSerialPortInfo>
#include <QDebug>
#include "hmiconfig.h"
#include "numkeyboard.h"
#include "data_collection/collection.h"


HMIMainWindow::HMIMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HMIMainWindow)
{
    this->setWindowState(Qt::WindowMaximized);
    this->setWindowIcon(QIcon(":/res/icons/hmimainwindow_icon.png"));
    ui->setupUi(this);

    numkeyboard = new NumKeyboard();
    numkeyboard->setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint );
    numkeyboard->setStyleSheet("border:2px solid white;");


    pPlot1 = ui->dlcustomplot;
    QPlot_init(pPlot1);


    QStringList labels;
        labels<<QStringLiteral("序号")<<QStringLiteral("电压(V)")<<QString::fromLocal8Bit("SOC(%)")
             <<QStringLiteral("电流(A)")<<QStringLiteral("温度(℃)");

        //设置行与列
        ui->tableWidget->setColumnCount(5);        //创建表头
        ui->tableWidget->setHorizontalHeaderLabels(labels);
        QFont ft = ui->tableWidget->horizontalHeader()->font();
        ft.setPixelSize(23);
        ui->tableWidget->horizontalHeader()->setFont(ft);

        //设置为只读模式
        ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        ui->tableWidget->horizontalHeader()->setVisible(true);
        //addData();
        QTableWidgetItem *item,*item01,*item1,*item2,*item3;
        ui->tableWidget->setRowCount(10);
        for(int i = 0;i<10;i++)
        {
            item = new QTableWidgetItem();
            item1 = new QTableWidgetItem();
            item01 = new QTableWidgetItem();
            item2 = new QTableWidgetItem();
            item3 = new QTableWidgetItem();
            item->setText(QString::number(i));
            item01->setText(QString::number(0));
            item1->setText(QString::number(0));
            item2->setText(QString::number(0));
            item3->setText(QString::number(0));
            ui->tableWidget->setItem(i,0,item);
            ui->tableWidget->setItem(i,1,item01);
            ui->tableWidget->setItem(i,2,item1);
            ui->tableWidget->setItem(i,3,item2);
            ui->tableWidget->setItem(i,4,item3);
        }




        ui->cbmodeout->addItem("离网放电");
        ui->cbmodeout->addItem("并网充电");
        ui->cbmodeout->addItem("并网放电");
        ui->cbmodeout->addItem("自动");


        ui->lwyb->SetTitle("电网");
        ui->smbyb->SetTitle("SMB");

        ui->lecddl1->setFixedWidth(60);
 ui->lefddy1->setFixedWidth(60);
  ui->lefdpl1->setFixedWidth(60);
   ui->lecddl2->setFixedWidth(60);
    ui->lefddy2->setFixedWidth(60);
    ui->lefdpl2->setFixedWidth(60);

        ui->lecddl1->setValidator(new QRegExpValidator(QRegExp("^(-?\\d+)(\\.\\d+)?$")));
        ui->lefddy1->setValidator(new QRegExpValidator(QRegExp("^(-?\\d+)(\\.\\d+)?$")));
        ui->lefdpl1->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));
        ui->lecddl2->setValidator(new QRegExpValidator(QRegExp("^(-?\\d+)(\\.\\d+)?$")));
        ui->lefddy2->setValidator(new QRegExpValidator(QRegExp("^(-?\\d+)(\\.\\d+)?$")));
        ui->lefdpl2->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));


        HmiConfig::GetInstance()->Init();

        ui->tecdbegin1->setTime(HmiConfig::GetInstance()->m_cd1begin);
        ui->tecdend1->setTime(HmiConfig::GetInstance()->m_cd1end);
        ui->lecddl1->setText(QString::number(HmiConfig::GetInstance()->m_cddl1));

        ui->telwfd1begin->setTime(HmiConfig::GetInstance()->m_lwfd1begin);
        ui->telwfd1end->setTime(HmiConfig::GetInstance()->m_lwfd1end);
        ui->lefddy1->setText(QString::number(HmiConfig::GetInstance()->m_lwfddy1));
        ui->lefdpl1->setText(QString::number(HmiConfig::GetInstance()->m_lwfdhz1));

        ui->tebwcd2begin->setTime(HmiConfig::GetInstance()->m_cd2begin);
        ui->tebwcd2end->setTime(HmiConfig::GetInstance()->m_cd2end);
        ui->lecddl2->setText(QString::number(HmiConfig::GetInstance()->m_cddl2));

        ui->telwfdbegin2->setTime(HmiConfig::GetInstance()->m_lwfd2begin);
        ui->telwfdend2->setTime(HmiConfig::GetInstance()->m_lwfd2end);
        ui->lefddy2->setText(QString::number(HmiConfig::GetInstance()->m_lwfddy2));
        ui->lefdpl2->setText(QString::number(HmiConfig::GetInstance()->m_lwfdhz2));

        ui->lelwv->setText(QString::number(HmiConfig::GetInstance()->set_lw_vol_mV/1000));
        ui->lelwhz->setText(QString::number(HmiConfig::GetInstance()->set_lw_freq_mHz/1000));
        ui->lebwa->setText(QString::number(HmiConfig::GetInstance()->set_bw_cur_mA/1000));
        if (!HmiConfig::GetInstance()->getConfigFile()->value("/CFG/MODEINDEX").isNull())
            ui->cbmodeout->setCurrentIndex(HmiConfig::GetInstance()->getConfigFile()->value("/CFG/MODEINDEX").toInt());
this->resize(1024,560);

        ui->lelwv->installEventFilter(this);
        ui->lelwhz->installEventFilter(this);
        ui->lebwa->installEventFilter(this);

        ui->leMaxDisAm->installEventFilter(this);
        ui->leMaxChaAm->installEventFilter(this);

        ui->lecddl1->installEventFilter(this);
        ui->lefddy1->installEventFilter(this);
        ui->lefdpl1->installEventFilter(this);
        ui->lecddl2->installEventFilter(this);
        ui->lefddy2->installEventFilter(this);
        ui->lefdpl2->installEventFilter(this);

        ui->comboBox_com->installEventFilter(this);


        //串口初始化
        SerialPortInit();

        HmiConfig::GetInstance()->moveToThread(&m_workthread);
        connect(&m_workthread, &QThread::started, HmiConfig::GetInstance(), &HmiConfig::do_work);
        connect(&m_workthread, &QThread::finished, HmiConfig::GetInstance(), &QObject::deleteLater);
        connect(HmiConfig::GetInstance(), &HmiConfig::UpdateUISignal, this, &HMIMainWindow::on_UpdateUI);
        connect(HmiConfig::GetInstance(), &HmiConfig::UpdateUISignal2, this, &HMIMainWindow::on_UpdateUI2);

        m_workthread.start();
        on_pbreadcfg_pressed();
        DataCollection::Instance().init();
        ui->stackedWidget->setCurrentIndex(0);
        QFont ft1 = ui->pushButton->font();
        ft1.setBold(false);
        ft1.setUnderline(false);
        ui->pushButton->setFont(ft1);
        ui->pushButton_2->setFont(ft1);
        ui->pushButton_3->setFont(ft1);

        if (ui->stackedWidget->currentIndex() == 0)
        {
            ft1.setBold(true);
            ft1.setUnderline(true);
            ui->pushButton->setFont(ft1);
        }
        else if (ui->stackedWidget->currentIndex() == 1)
        {
            ft1.setUnderline(true);
            ft1.setBold(true);
            ui->pushButton_2->setFont(ft1);
        }
        else if (ui->stackedWidget->currentIndex() == 2)
        {
            ft1.setUnderline(true);
            ft1.setBold(true);
            ui->pushButton_3->setFont(ft1);
        }

    }
void HMIMainWindow::on_UpdateUI2()
{
    ui->leMaxDisAm->setText(QString::number(HmiConfig::GetInstance()->getBaseCfg()->smb_disCur/100));
    ui->leMaxChaAm->setText(QString::number(HmiConfig::GetInstance()->getBaseCfg()->smb_chaCur/100));
}
void HMIMainWindow::on_UpdateUI()
{
    //功率因子
    ui->lblglyz->setText(QString("%1").arg(HmiConfig::GetInstance()->recv_pf_val_milli*0.001f));
    ui->lblxwc->setText(QString("%1").arg(HmiConfig::GetInstance()->recv_smb_dw_phase_diff));
    ui->lblsoc->setText(QString("%1").arg(HmiConfig::GetInstance()->recv_smb_total_soc));
    //电网电压有效值
    ui->lwyb->setVAHZ(HmiConfig::GetInstance()->recv_dw_vol_mV/1000.0,HmiConfig::GetInstance()->recv_dw_cur_mA/1000.0,HmiConfig::GetInstance()->recv_dw_freq_mHz/1000.0);
    //smb电压有效值
    ui->smbyb->setVAHZ(HmiConfig::GetInstance()->recv_smb_vol_mV/1000.0,HmiConfig::GetInstance()->recv_smb_cur_mA/1000.0,HmiConfig::GetInstance()->recv_smb_freq_mHz/1000.0);


    //customplot控件
    Show_Plot_(pPlot1,pGraph1_2,HmiConfig::GetInstance()->recv_dw_cur_mA/1000.0);
    Show_Plot_(pPlot1,pGraph1_2,HmiConfig::GetInstance()->recv_smb_cur_mA/1000.0);



    for(int i = 0;i<10;i++)
    {
        ui->tableWidget->item(i,1)->setText(QString("%1").arg(HmiConfig::GetInstance()->recv_smbinfo[i].vol_10mV/100.0));
        ui->tableWidget->item(i,2)->setText(QString("%1").arg(HmiConfig::GetInstance()->recv_smbinfo[i].soc));
        ui->tableWidget->item(i,3)->setText(QString("%1").arg(HmiConfig::GetInstance()->recv_smbinfo[i].cur_10mA/100.0));
        ui->tableWidget->item(i,4)->setText(QString("%1").arg(HmiConfig::GetInstance()->recv_smbinfo[i].tmp));
    }

    if (HmiConfig::GetInstance()->blclfstart || (unsigned char)(HmiConfig::GetInstance()->recv_start_mode&0x01) == (unsigned char)0x01)
    {
        //启动
        ui->cbmodeout->setEnabled(false);
        ui->lelwv->setEnabled(false);
        ui->lelwhz->setEnabled(false);
        ui->lebwa->setEnabled(false);

        ui->leMaxDisAm->setEnabled(false);
        ui->leMaxChaAm->setEnabled(false);
        ui->pbreadcfg->setEnabled(false);
        ui->pbmodifycfg->setEnabled(false);
        ui->tecdbegin1->setEnabled(false);
        ui->tecdend1->setEnabled(false);
        ui->telwfd1begin->setEnabled(false);
        ui->telwfd1end->setEnabled(false);
        ui->tebwcd2begin->setEnabled(false);
        ui->tebwcd2end->setEnabled(false);
        ui->telwfdbegin2->setEnabled(false);
        ui->telwfdend2->setEnabled(false);
        ui->pbrefresh->setEnabled(false);
        ui->pbset->setEnabled(false);
        ui->lecddl1->setEnabled(false);
        ui->lefddy1->setEnabled(false);
        ui->lefdpl1->setEnabled(false);
        ui->lecddl2->setEnabled(false);
        ui->lefddy2->setEnabled(false);
        ui->lefdpl2->setEnabled(false);

        ui->tbONOFF->setText("OFF");
        ui->lcd_rx_b_7->setStyleSheet("background-color: rgb(0, 255, 0); color: rgb(255, 255, 255);");
    }
    else
    {
        //开始启动
        ui->cbmodeout->setEnabled(true);
        ui->lelwv->setEnabled(true);
        ui->lelwhz->setEnabled(true);
        ui->lebwa->setEnabled(true);

        ui->leMaxDisAm->setEnabled(true);
        ui->leMaxChaAm->setEnabled(true);
        ui->pbreadcfg->setEnabled(true);
        ui->pbmodifycfg->setEnabled(true);
        ui->tecdbegin1->setEnabled(true);
        ui->tecdend1->setEnabled(true);
        ui->telwfd1begin->setEnabled(true);
        ui->telwfd1end->setEnabled(true);
        ui->tebwcd2begin->setEnabled(true);
        ui->tebwcd2end->setEnabled(true);
        ui->telwfdbegin2->setEnabled(true);
        ui->telwfdend2->setEnabled(true);
        ui->pbrefresh->setEnabled(true);
        ui->pbset->setEnabled(true);
        ui->lecddl1->setEnabled(true);
        ui->lefddy1->setEnabled(true);
        ui->lefdpl1->setEnabled(true);
        ui->lecddl2->setEnabled(true);
        ui->lefddy2->setEnabled(true);
        ui->lefdpl2->setEnabled(true);
        ui->tbONOFF->setText("ON");
        ui->lcd_rx_b_7->setStyleSheet("background-color: rgb(255, 0, 0); color: rgb(255, 255, 255);");
    }

}


HMIMainWindow::~HMIMainWindow()
{
    HmiConfig::GetInstance()->Destory();
    this->m_workthread.requestInterruption();
    this->m_workthread.quit();
    this->m_workthread.wait();

    delete ui;
}

void HMIMainWindow::Read_Data()
{

}

//串口初始化
bool HMIMainWindow::SerialPortInit()
{
    QSettings *m_psetting1;
    QString str_portName;
    bool m_stringcom = false;
    QString hmicfg = QCoreApplication::applicationDirPath() + "/HMIConfig.ini";
    m_psetting1 = new QSettings(hmicfg,QSettings::IniFormat);
    m_psetting1->setIniCodec("UTF8");
    QString m_strcom1  = m_psetting1->value("/COM/Name").toString();
    qDebug() << "读取到的的端口m_strcom1:" << m_strcom1;


    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
    {
        qDebug() << "搜索到可用的端口:" << info.portName();


        if ( 0 == m_strcom1.compare(info.portName())) //判断字符串相同
            {
                m_stringcom = true;
                str_portName = info.portName();
                ui->comboBox_com->currentIndex();//返回当前列表框中选中文本的序号
                qDebug() << "ini文件中有相同端口:" << info.portName();
            }
        else
        {
            ui->comboBox_com->addItem(info.portName());
        }

    }


    if (m_stringcom)
    {
        ui->comboBox_com->addItem(str_portName);
        ui->comboBox_com->setCurrentIndex((ui->comboBox_com->count())-1);//设置combobox当前选中序号
        m_stringcom = false;
    }
    else
    {
        QStandardItemModel *pItemModel = qobject_cast<QStandardItemModel*>(ui->comboBox_com->model());
        int iCount = ui->comboBox_com->count();    //获取总Item数

        QFont font;
        font.setStrikeOut(true);     //中划线
        font.setFamily("Microsoft YaHei UI");  //字体
        font.setPointSize(20);       //文字大小
        ui->comboBox_com->addItem(m_strcom1);
        pItemModel->item(iCount)->setForeground(QColor(255, 0, 0));//修改某项文本颜色
        pItemModel->item(iCount)->setFont(font);//设置某一项字体
        m_stringcom = false;
    }

    return true;
}


void HMIMainWindow::on_pbreadcfg_pressed()
{
    //读取基本配置
    if (!HmiConfig::GetInstance()->getEmptySem()->tryAcquire())
    {
        QMessageBox::information(this,"提示","有指令未执行完,请稍等");
        return;
    }
    //
    *HmiConfig::GetInstance()->getCmdAction() = BASE_READ;

    HmiConfig::GetInstance()->getFullSem()->release();
}


void HMIMainWindow::on_pbmodifycfg_pressed()
{
    //写入基本配置
    if (!HmiConfig::GetInstance()->getEmptySem()->tryAcquire())
    {
        QMessageBox::information(this,"提示","有指令未执行完,请稍等");
        return;
    }
    *HmiConfig::GetInstance()->getCmdAction() = BASE_SET;
    HmiConfig::GetInstance()->getBaseCfg()->smb_chaCur = ui->leMaxChaAm->text().toInt()*100;
    HmiConfig::GetInstance()->getBaseCfg()->smb_disCur = ui->leMaxDisAm->text().toInt()*100;
    HmiConfig::GetInstance()->getFullSem()->release();
}


void HMIMainWindow::on_tbONOFF_triggered(QAction *arg1)
{

}


bool HMIMainWindow::eventFilter(QObject *watched, QEvent *event)
{
    ui->lelwhz->installEventFilter(this);
    ui->lebwa->installEventFilter(this);

    ui->leMaxDisAm->installEventFilter(this);
    ui->leMaxChaAm->installEventFilter(this);

    ui->lecddl1->installEventFilter(this);
    ui->lefddy1->installEventFilter(this);
    ui->lefdpl1->installEventFilter(this);
    ui->lecddl2->installEventFilter(this);
    ui->lefddy2->installEventFilter(this);
    ui->lefdpl2->installEventFilter(this);


    if (watched == ui->lelwv ||
            watched == ui->lelwhz ||
            watched == ui->lebwa  ||
            watched == ui->leMaxDisAm  ||
            watched == ui->leMaxChaAm  ||
            watched == ui->lecddl1  ||
            watched == ui->lefddy1  ||
            watched == ui->lefdpl1  ||
            watched == ui->lecddl2  ||
            watched == ui->lefddy2  ||
            watched == ui->lefdpl2 )
        {
            if (event->type() == QEvent::MouseButtonPress){
                numkeyboard->setText(((QLineEdit*)watched)->text());  //当前的文本框的内容，this->指向的是SoftKeyLineEdit
                numkeyboard->show((QLineEdit*)watched);
            }
        }

    //2023.07.25 erik.xie
    if (event->type() == QEvent::MouseButtonPress)
    {
        if (watched == ui->comboBox_com)
        {
            ui->comboBox_com->clear();
            SerialPortInit();
        }
    }
        return QWidget::eventFilter(watched,event);
}


void HMIMainWindow::on_tbONOFF_pressed()
{

    if (ui->cbmodeout->currentIndex() == 3)
    {
        //启动任务计划
        if (!HmiConfig::GetInstance()->blclfstart)
        {
            if (QMessageBox::question(this,"警告","是否启动两充两放?") != QMessageBox::Button::Yes)
                return;
            else
            {
                HmiConfig::GetInstance()->blclfstart = true;
      //          QMessageBox::information(this,"提示","两充两放启动成功！");
            }
        }
        else
        {
//            if (QMessageBox::question(this,"警告","是否停止两充两放?") != QMessageBox::Button::Yes)
//                return;
//            else
//            {
                HmiConfig::GetInstance()->blclfstart = false;
       //         QMessageBox::information(this,"提示","两充两放停止成功！");
//            }
        }

        return;
    }

    if (!HmiConfig::GetInstance()->getEmptySem()->tryAcquire())
    {
        QMessageBox::information(this,"提示","有指令未执行完,请稍等重试");
        return;
    }
    //正在启动
    if ((unsigned char)(HmiConfig::GetInstance()->recv_start_mode&0x01) == (unsigned char)0x01)
    {
        //停止启动
        HmiConfig::GetInstance()->hmi_start = 0x00;
    }
    else
    {
        //开始启动
        if (ui->cbmodeout->currentIndex() == 0 )
        {
            if (QMessageBox::question(this,"警告","是否启动离网放电?") != QMessageBox::Button::Yes)
               return;
        }
        else if (ui->cbmodeout->currentIndex() == 1 )
        {
            if (QMessageBox::question(this,"警告","是否启动并网充电?") != QMessageBox::Button::Yes)
               return;
        }
        else if (ui->cbmodeout->currentIndex() == 2 )
        {
            if (QMessageBox::question(this,"警告","是否启动并网放电?") != QMessageBox::Button::Yes)
               return;
        }
        HmiConfig::GetInstance()->hmi_start = 0x01;
    }

    HmiConfig::GetInstance()->hmi_start = HmiConfig::GetInstance()->hmi_start | (unsigned char)ui->cbmodeout->currentIndex()<<1;
    //设定离网电压有效值
    HmiConfig::GetInstance()->set_lw_vol_mV = ui->lelwv->text().toUInt()*1000;
    //设定离网电压频率
    HmiConfig::GetInstance()->set_lw_freq_mHz = ui->lelwhz->text().toUInt()*1000;
    //设定并网电流
    HmiConfig::GetInstance()->set_bw_cur_mA = ui->lebwa->text().toUInt()*1000;

    HmiConfig::GetInstance()->enstart = 1;
    *HmiConfig::GetInstance()->getCmdAction() = HMI_ACTION;
    HmiConfig::GetInstance()->getFullSem()->release();

}


void HMIMainWindow::on_pbmodifycfg_clicked()
{

}


void HMIMainWindow::on_pbrefresh_clicked()
{
    ui->tecdbegin1->setTime(HmiConfig::GetInstance()->m_cd1begin);
    ui->tecdend1->setTime(HmiConfig::GetInstance()->m_cd1end);
    ui->lecddl1->setText(QString::number(HmiConfig::GetInstance()->m_cddl1));

    ui->telwfd1begin->setTime(HmiConfig::GetInstance()->m_lwfd1begin);
    ui->telwfd1end->setTime(HmiConfig::GetInstance()->m_lwfd1end);
    ui->lefddy1->setText(QString::number(HmiConfig::GetInstance()->m_lwfddy1));
    ui->lefdpl1->setText(QString::number(HmiConfig::GetInstance()->m_lwfdhz1));

    ui->tebwcd2begin->setTime(HmiConfig::GetInstance()->m_cd2begin);
    ui->tebwcd2end->setTime(HmiConfig::GetInstance()->m_cd2end);
    ui->lecddl2->setText(QString::number(HmiConfig::GetInstance()->m_cddl2));

    ui->telwfdbegin2->setTime(HmiConfig::GetInstance()->m_lwfd2begin);
    ui->telwfdend2->setTime(HmiConfig::GetInstance()->m_lwfd2end);
    ui->lefddy2->setText(QString::number(HmiConfig::GetInstance()->m_lwfddy2));
    ui->lefdpl2->setText(QString::number(HmiConfig::GetInstance()->m_lwfdhz2));
}


void HMIMainWindow::on_pbset_clicked()
{
    //设置任务计划
    if (!(ui->tecdbegin1->time() <  ui->tecdend1->time()))
    {
        QMessageBox::information(this,"提示","并网充电1起止时间不合法！");
        return;
    }
    if (!(ui->lecddl1->text().toInt() > 0))
    {
        QMessageBox::information(this,"提示","并网充电1充电电流不合法！");
        return;
    }
    if (!(ui->telwfd1begin->time() <  ui->telwfd1end->time()))
    {
        QMessageBox::information(this,"提示","离网放电1起止时间不合法！");
        return;
    }
    if (ui->telwfd1begin->time() >= ui->tecdbegin1->time() && ui->telwfd1begin->time() <=  ui->tecdend1->time())
    {
        QMessageBox::information(this,"提示","离网放电1起止时间不合法,状态1！");
        return;
    }
    if (ui->telwfd1end->time() >= ui->tecdbegin1->time() && ui->telwfd1end->time() <=  ui->tecdend1->time())
    {
        QMessageBox::information(this,"提示","离网放电1起止时间不合法,状态2！");
        return;
    }

    if (ui->tecdbegin1->time() >= ui->telwfd1begin->time() && ui->tecdbegin1->time() <=  ui->telwfd1end->time())
    {
        QMessageBox::information(this,"提示","离网放电1起止时间不合法,状态3！");
        return;
    }
    if (ui->tecdend1->time() >= ui->telwfd1begin->time() && ui->tecdend1->time() <=  ui->telwfd1end->time())
    {
        QMessageBox::information(this,"提示","离网放电1起止时间不合法,状态4！");
        return;
    }

    if (!(ui->tebwcd2begin->time() <  ui->tebwcd2end->time()))
    {
        QMessageBox::information(this,"提示","并网充电2起止时间不合法！");
        return;
    }

    if (ui->tebwcd2begin->time() >= ui->tecdbegin1->time() && ui->tebwcd2begin->time() <=  ui->tecdend1->time())
    {
        QMessageBox::information(this,"提示","并网充电2起止时间不合法,状态1！");
        return;
    }
    if (ui->tebwcd2end->time() >= ui->tecdbegin1->time() && ui->tebwcd2end->time() <=  ui->tecdend1->time())
    {
        QMessageBox::information(this,"提示","并网充电2起止时间不合法,状态2！");
        return;
    }

    if (ui->tecdbegin1->time() >= ui->tebwcd2begin->time() && ui->tecdbegin1->time() <=  ui->tebwcd2end->time())
    {
        QMessageBox::information(this,"提示","并网充电2起止时间不合法,状态3！");
        return;
    }
    if (ui->tecdend1->time() >= ui->tebwcd2begin->time() && ui->tecdend1->time() <=  ui->tebwcd2end->time())
    {
        QMessageBox::information(this,"提示","并网充电2起止时间不合法,状态4！");
        return;
    }

    if (ui->tebwcd2begin->time() >= ui->telwfd1begin->time() && ui->tebwcd2begin->time() <=  ui->telwfd1end->time())
    {
        QMessageBox::information(this,"提示","并网充电2起止时间不合法,状态5！");
        return;
    }
    if (ui->tebwcd2end->time() >= ui->telwfd1begin->time() && ui->tebwcd2end->time() <=  ui->telwfd1end->time())
    {
        QMessageBox::information(this,"提示","并网充电2起止时间不合法,状态6！");
        return;
    }

    if (ui->telwfd1begin->time() >= ui->tebwcd2begin->time() && ui->telwfd1begin->time() <=  ui->tebwcd2end->time())
    {
        QMessageBox::information(this,"提示","并网充电2起止时间不合法,状态7！");
        return;
    }
    if (ui->telwfd1end->time() >= ui->tebwcd2begin->time() && ui->telwfd1end->time() <=  ui->tebwcd2end->time())
    {
        QMessageBox::information(this,"提示","并网充电2起止时间不合法,状态8！");
        return;
    }


    if (!(ui->telwfdbegin2->time() <  ui->telwfdend2->time()))
    {
        QMessageBox::information(this,"提示","离网放电2起止时间不合法！");
        return;
    }

    if (ui->telwfdbegin2->time() >= ui->tecdbegin1->time() && ui->telwfdbegin2->time() <=  ui->tecdend1->time())
    {
        QMessageBox::information(this,"提示","离网放电2起止时间不合法,状态1！");
        return;
    }
    if (ui->telwfdend2->time() >= ui->tecdbegin1->time() && ui->telwfdend2->time() <=  ui->tecdend1->time())
    {
        QMessageBox::information(this,"提示","离网放电2起止时间不合法,状态2！");
        return;
    }

    if (ui->tecdbegin1->time() >= ui->telwfdbegin2->time() && ui->tecdbegin1->time() <=  ui->telwfdend2->time())
    {
        QMessageBox::information(this,"提示","离网放电2起止时间不合法,状态3！");
        return;
    }
    if (ui->tecdend1->time() >= ui->telwfdbegin2->time() && ui->tecdend1->time() <=  ui->telwfdend2->time())
    {
        QMessageBox::information(this,"提示","离网放电2起止时间不合法,状态4！");
        return;
    }

    if (ui->telwfdbegin2->time() >= ui->telwfd1begin->time() && ui->telwfdbegin2->time() <=  ui->telwfd1end->time())
    {
        QMessageBox::information(this,"提示","离网放电2起止时间不合法,状态5！");
        return;
    }
    if (ui->telwfdend2->time() >= ui->telwfd1begin->time() && ui->telwfdend2->time() <=  ui->telwfd1end->time())
    {
        QMessageBox::information(this,"提示","离网放电2起止时间不合法,状态6！");
        return;
    }

    if (ui->telwfd1begin->time() >= ui->telwfdbegin2->time() && ui->telwfd1begin->time() <=  ui->telwfdend2->time())
    {
        QMessageBox::information(this,"提示","离网放电2起止时间不合法,状态7！");
        return;
    }
    if (ui->telwfd1end->time() >= ui->telwfdbegin2->time() && ui->telwfd1end->time() <=  ui->telwfdend2->time())
    {
        QMessageBox::information(this,"提示","离网放电2起止时间不合法,状态8！");
        return;
    }

    if (ui->telwfdbegin2->time() >= ui->tebwcd2begin->time() && ui->telwfdbegin2->time() <=  ui->tebwcd2end->time())
    {
        QMessageBox::information(this,"提示","离网放电2起止时间不合法,状态9！");
        return;
    }
    if (ui->telwfdend2->time() >= ui->tebwcd2begin->time() && ui->telwfdend2->time() <=  ui->tebwcd2end->time())
    {
        QMessageBox::information(this,"提示","离网放电2起止时间不合法,状态10！");
        return;
    }

    if (ui->tebwcd2begin->time() >= ui->telwfdbegin2->time() && ui->tebwcd2begin->time() <=  ui->telwfdend2->time())
    {
        QMessageBox::information(this,"提示","离网放电2起止时间不合法,状态11！");
        return;
    }
    if (ui->tebwcd2end->time() >= ui->telwfdbegin2->time() && ui->tebwcd2end->time() <=  ui->telwfdend2->time())
    {
        QMessageBox::information(this,"提示","离网放电2起止时间不合法,状态12！");
        return;
    }


    if (!(ui->lecddl1->text().toInt() > 0))
    {
        QMessageBox::information(this,"提示","并网充电1充电电流不合法！");
        return;
    }

    if (!(ui->lefddy1->text().toInt() > 0))
    {
        QMessageBox::information(this,"提示","离网放电1放电电流不合法！");
        return;
    }

    if (!(ui->lefdpl1->text().toInt() > 0))
    {
        QMessageBox::information(this,"提示","离网放电1放电频率不合法！");
        return;
    }

    if (!(ui->lecddl2->text().toInt() > 0))
    {
        QMessageBox::information(this,"提示","并网充电2充电电流不合法！");
        return;
    }

    if (!(ui->lefddy2->text().toInt() > 0))
    {
        QMessageBox::information(this,"提示","离网放电2放电电流不合法！");
        return;
    }

    if (!(ui->lefdpl2->text().toInt() > 0))
    {
        QMessageBox::information(this,"提示","离网放电2放电频率不合法！");
        return;
    }

    HmiConfig::GetInstance()->m_cd1begin = ui->tecdbegin1->time();
    HmiConfig::GetInstance()->m_cd1end = ui->tecdend1->time();
    HmiConfig::GetInstance()->m_cddl1 = ui->lecddl1->text().toUInt();

    HmiConfig::GetInstance()->m_lwfd1begin = ui->telwfd1begin->time();
    HmiConfig::GetInstance()->m_lwfd1end = ui->telwfd1end->time();
    HmiConfig::GetInstance()->m_lwfddy1 = ui->lefddy1->text().toUInt();
    HmiConfig::GetInstance()->m_lwfdhz1 = ui->lefdpl1->text().toInt();

    HmiConfig::GetInstance()->m_cd2begin = ui->tebwcd2begin->time();
    HmiConfig::GetInstance()->m_cd2end = ui->tebwcd2end->time();
    HmiConfig::GetInstance()->m_cddl2 = ui->lecddl2->text().toUInt();

    HmiConfig::GetInstance()->m_lwfd2begin = ui->telwfdbegin2->time();
    HmiConfig::GetInstance()->m_lwfd2end = ui->telwfdend2->time();
    HmiConfig::GetInstance()->m_lwfddy2 = ui->lefddy2->text().toUInt();
    HmiConfig::GetInstance()->m_lwfdhz2 = ui->lefdpl2->text().toUInt();

    //写入配置
    //并网充电1
    HmiConfig::GetInstance()->getConfigFile()->setValue("/BWCD1/BEGIN",HmiConfig::GetInstance()->m_cd1begin.toString());
    HmiConfig::GetInstance()->getConfigFile()->setValue("/BWCD1/END",HmiConfig::GetInstance()->m_cd1end.toString());
    HmiConfig::GetInstance()->getConfigFile()->setValue("/BWCD1/DL",HmiConfig::GetInstance()->m_cddl1);

    //离网放电1
    HmiConfig::GetInstance()->getConfigFile()->setValue("/LWFD1/BEGIN",HmiConfig::GetInstance()->m_lwfd1begin.toString());
    HmiConfig::GetInstance()->getConfigFile()->setValue("/LWFD1/END",HmiConfig::GetInstance()->m_lwfd1end.toString());
    HmiConfig::GetInstance()->getConfigFile()->setValue("/LWFD1/DY",HmiConfig::GetInstance()->m_lwfddy1);
    HmiConfig::GetInstance()->getConfigFile()->setValue("/LWFD1/HZ",HmiConfig::GetInstance()->m_lwfdhz1);

    //并网充电2
    HmiConfig::GetInstance()->getConfigFile()->setValue("/BWCD2/BEGIN",HmiConfig::GetInstance()->m_cd2begin.toString());
    HmiConfig::GetInstance()->getConfigFile()->setValue("/BWCD2/END",HmiConfig::GetInstance()->m_cd2end.toString());
    HmiConfig::GetInstance()->getConfigFile()->setValue("/BWCD2/DL",HmiConfig::GetInstance()->m_cddl2);

    //离网放电2
    HmiConfig::GetInstance()->getConfigFile()->setValue("/LWFD2/BEGIN",HmiConfig::GetInstance()->m_lwfd2begin.toString());
    HmiConfig::GetInstance()->getConfigFile()->setValue("/LWFD2/END",HmiConfig::GetInstance()->m_lwfd2end.toString());
    HmiConfig::GetInstance()->getConfigFile()->setValue("/LWFD2/DY",HmiConfig::GetInstance()->m_lwfddy2);
    HmiConfig::GetInstance()->getConfigFile()->setValue("/LWFD2/HZ",HmiConfig::GetInstance()->m_lwfdhz2);


    QMessageBox::information(this,"提示","两充两放保存成功！");

}


void HMIMainWindow::on_tbONOFF_clicked()
{

}

void HMIMainWindow::closeEvent(QCloseEvent *event)
{

    //离网电压
    HmiConfig::GetInstance()->getConfigFile()->setValue("/CFG/LWDY",HmiConfig::GetInstance()->set_lw_vol_mV);
    //离网电压频率
    HmiConfig::GetInstance()->getConfigFile()->setValue("/CFG/LWDYHZ",HmiConfig::GetInstance()->set_lw_freq_mHz);
    //并网电流
    HmiConfig::GetInstance()->getConfigFile()->setValue("/CFG/BWDL",HmiConfig::GetInstance()->set_bw_cur_mA);

    HmiConfig::GetInstance()->getConfigFile()->setValue("/CFG/MODEINDEX",ui->cbmodeout->currentIndex());
}

void HMIMainWindow::on_HMIMainWindow_destroyed()
{


}

void HMIMainWindow::on_tbrun_triggered(QAction *arg1)
{

    ui->stackedWidget->setCurrentIndex(0);
}

void HMIMainWindow::on_tbsmb_triggered(QAction *arg1)
{
    ui->stackedWidget->setCurrentIndex(1);
}

void HMIMainWindow::on_tbconfig_triggered(QAction *arg1)
{
    ui->stackedWidget->setCurrentIndex(2);
}

void HMIMainWindow::on_pushButton_pressed()
{
    ui->stackedWidget->setCurrentIndex(0);
    QFont ft1 = ui->pushButton->font();
    ft1.setBold(false);
    ft1.setUnderline(false);
    ui->pushButton->setFont(ft1);
    ui->pushButton_2->setFont(ft1);
    ui->pushButton_3->setFont(ft1);

    if (ui->stackedWidget->currentIndex() == 0)
    {
        ft1.setBold(true);
        ft1.setUnderline(true);
        ui->pushButton->setFont(ft1);
    }
    else if (ui->stackedWidget->currentIndex() == 1)
    {
        ft1.setUnderline(true);
        ft1.setBold(true);
        ui->pushButton_2->setFont(ft1);
    }
    else if (ui->stackedWidget->currentIndex() == 2)
    {
        ft1.setUnderline(true);
        ft1.setBold(true);
        ui->pushButton_3->setFont(ft1);
    }
}

void HMIMainWindow::on_pushButton_2_pressed()
{
    ui->stackedWidget->setCurrentIndex(1);
    QFont ft1 = ui->pushButton->font();
    ft1.setBold(false);
    ft1.setUnderline(false);
    ui->pushButton->setFont(ft1);
    ui->pushButton_2->setFont(ft1);
    ui->pushButton_3->setFont(ft1);

    if (ui->stackedWidget->currentIndex() == 0)
    {
        ft1.setBold(true);
        ft1.setUnderline(true);
        ui->pushButton->setFont(ft1);
    }
    else if (ui->stackedWidget->currentIndex() == 1)
    {
        ft1.setUnderline(true);
        ft1.setBold(true);
        ui->pushButton_2->setFont(ft1);
    }
    else if (ui->stackedWidget->currentIndex() == 2)
    {
        ft1.setUnderline(true);
        ft1.setBold(true);
        ui->pushButton_3->setFont(ft1);
    }
}

void HMIMainWindow::on_pushButton_3_pressed()
{
    ui->stackedWidget->setCurrentIndex(2);
    QFont ft1 = ui->pushButton->font();
    ft1.setBold(false);
    ft1.setUnderline(false);
    ui->pushButton->setFont(ft1);
    ui->pushButton_2->setFont(ft1);
    ui->pushButton_3->setFont(ft1);

    if (ui->stackedWidget->currentIndex() == 0)
    {
        ft1.setBold(true);
        ft1.setUnderline(true);
        ui->pushButton->setFont(ft1);
    }
    else if (ui->stackedWidget->currentIndex() == 1)
    {
        ft1.setUnderline(true);
        ft1.setBold(true);
        ui->pushButton_2->setFont(ft1);
    }
    else if (ui->stackedWidget->currentIndex() == 2)
    {
        ft1.setUnderline(true);
        ft1.setBold(true);
        ui->pushButton_3->setFont(ft1);
    }
}

int i = 0;
void HMIMainWindow::on_pushButton_4_clicked()
{
    //test
}


void HMIMainWindow::on_comboBox_com_activated(int index)
{
    qDebug() << "combobox值修改";

    HmiConfig* config =  HmiConfig::GetInstance();

    config->m_pserial->close();

    HmiConfig::GetInstance()->m_himcom.strcom = ui->comboBox_com->currentText();
    config->m_pserial->setPortName(ui->comboBox_com->currentText());

    config->brt_serial = config->m_pserial->open(QIODevice::ReadWrite);
    qDebug() << "打开串口成功与否:"<< config->brt_serial;
    HmiConfig::GetInstance()->getConfigFile()->setValue("/COM/Name",ui->comboBox_com->currentText());
}

void HMIMainWindow::QPlot_init(QCustomPlot *customPlot)
{

    pGraph1_1 = customPlot->addGraph();
    pGraph1_2 = customPlot->addGraph();

    pGraph1_1->setPen(QPen(Qt::red));
    pGraph1_2->setPen(QPen(Qt::green));

    customPlot->graph(0)->setBrush(QBrush(QColor(255,0,0,15)));
    customPlot->graph(1)->setBrush(QBrush(QColor(0,255,0,15)));

    customPlot->yAxis->setLabel("电流(A)");
    customPlot->yAxis->setRange(-2,10);

    customPlot->legend->setVisible(true);
    customPlot->legend->setBrush(QColor(255,255,255,0));//设置图例背景
    customPlot->legend->setBorderPen(Qt::NoPen);//图例边框
    QFont font;
    font.setPixelSize(15);//文字像素大小
    font.setFamily("微软雅黑");//字体
    customPlot->legend->setFont(font);

    pGraph1_1->setName("电网");
    pGraph1_2->setName("SMB");

    customPlot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom| QCP::iSelectAxes |
                                      QCP::iSelectLegend | QCP::iSelectPlottables);

    customPlot->setBackground(Qt::transparent);
    customPlot->setStyleSheet("background: transparent;");
    customPlot->graph(0)->rescaleAxes();
    customPlot->graph(1)->rescaleAxes(true);

    QSharedPointer<QCPAxisTickerDateTime> timeTicker(new QCPAxisTickerDateTime);
    timeTicker->setDateTimeFormat("mm:ss");
    customPlot->xAxis->setTicker(timeTicker);
    customPlot->xAxis->setTickLabels(true);
    customPlot->axisRect()->setupFullAxesBox();
    customPlot->setOpenGl(true);//opengl绘图

}


void HMIMainWindow::Show_Plot_(QCustomPlot *customPlot,QCPGraph *graph,double num_y)
{

    nowtime_ = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
    if (time_f_ == true)
    {
        begintime_ = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
        time_f_ = false;
    }

    graph->addData(nowtime_, num_y);
    customPlot->xAxis->setRange(((nowtime_-begintime_)>15)?(nowtime_)-15:begintime_,nowtime_);
    customPlot->graph(0)->data()->removeBefore(nowtime_-20);
    customPlot->graph(1)->data()->removeBefore(nowtime_-20);
    customPlot->replot(QCustomPlot::rpQueuedReplot);

}

