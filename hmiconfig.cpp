#include "hmiconfig.h"
#include <QCoreApplication>
#include <QObject>
#include <QThread>
#include <QDebug>
#include <QTimer>


HmiConfig* HmiConfig::m_instance = 0;

/* CRC 高位字节值表 */
const  uint8_t auchCRCHi[] =
{
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

/* CRC低位字节值表*/
const  uint8_t auchCRCLo[] =
{
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
    0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
    0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
    0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
    0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
    0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
    0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
    0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
    0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
    0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
    0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
    0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
    0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
    0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};


/*
* 函数名 :CRC16
* 描述 : 计算CRC16
* 输入 : puchMsg---数据地址,usDataLen---数据长度
* 输出 : 校验值
*/
uint16_t CRC16(uint8_t *puchMsg, uint8_t usDataLen)
{
    uint8_t uchCRCHi = 0xFF ;              // 高CRC字节初始化
    uint8_t uchCRCLo = 0xFF ;              // 低CRC 字节初始化

    uint8_t uIndex ;                      // CRC循环中的索引

    while (usDataLen--)                  // 传输消息缓冲区
    {
        uIndex = uchCRCHi ^ *puchMsg++ ; // 计算CRC
        uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex];
        uchCRCLo = auchCRCLo[uIndex];
    }

    return (uchCRCHi << 8 | uchCRCLo);	// MODBUS 规定高位在前
}

HmiConfig::HmiConfig():m_empty(1),m_full(0)
{
    blclfstart = false;
    m_psetting = NULL;
    m_pserial = NULL;
    enstart = 0;
    hmi_start = 0;
    set_lw_vol_mV = 0;
    set_lw_freq_mHz = 0;
    set_bw_cur_mA = 0;
}




QString toHexadecimal(unsigned char* bf,int nsize)
{
    QString str;
    for(int i = 0; i< nsize; i++){
        QString byteStr = QString::number(static_cast<uchar>(bf[i]), 16);
        if(byteStr.length() == 1) str += "0" + byteStr;
        else str += byteStr;
    }
    return str;
}

int testindex = 0;

void HmiConfig::do_work()
{

    m_pserial = new QSerialPort();
    if (my_serial_ == true)
    {
        m_pserial->setPortName(HmiConfig::GetInstance()->GetHimCom().strcom);
        my_serial_ = false;
    }

    qDebug() << "读取到ini文件的端口号:" << HmiConfig::GetInstance()->GetHimCom().strcom;
    qDebug() << "使用端口号:" << m_pserial->portName();
    this->m_pserial->setBaudRate(HmiConfig::GetInstance()->GetHimCom().nbaud);
    m_pserial->setReadBufferSize(500);
    //bool brt = m_pserial->open(QIODevice::ReadWrite);
    brt_serial = m_pserial->open(QIODevice::ReadWrite);
    qDebug() << "是否打开串口" << brt_serial ;



    QByteArray gba;
    gba.clear();
    while (b_running)
    {
        testindex++;
        //qDebug() << "running";
        bool brt = m_full.tryAcquire(1,100);
        QByteArray basend;
        if (brt)
        {
            //this->BasePra(this->m_slaveaddr,this->m_netversion,)
            //控制刷新指令(CTRLRefresh)
            //基本参数配置指令(BasePra)

            if (this->m_cmdaction == BASE_READ)
            {
                 basend = this->BasePra(this->m_slaveaddr,this->m_netversion,0,0,0,++this->nsn);
            }
            else if (this->m_cmdaction == BASE_SET)
            {
                basend = this->BasePra(this->m_slaveaddr,this->m_netversion,3,this->m_basecfg.smb_disCur,m_basecfg.smb_chaCur,++this->nsn);
            }
            else if (this->m_cmdaction == HMI_ACTION)
            {
                basend = this->CTRLRefresh(this->m_slaveaddr,this->m_netversion,enstart,hmi_start,set_lw_vol_mV,set_lw_freq_mHz,set_bw_cur_mA,++this->nsn);

            }
            m_empty.release();
        }
        else
        {
            if (blclfstart)
            {
                //任务计划,blclfstart为两充两放标志位
//              //两充两放的参数
//              //并网充电1

                QTime time = QTime::currentTime();
                unsigned char bwcd = (unsigned char)(recv_start_mode&0x06);
                if (m_cd1begin <= time && time <= m_cd1end)
                {
                    //并网充电1,充电电流：m_cddl1
                    if (((unsigned char)(recv_start_mode&0x01) != (unsigned char)0x01)
                            || bwcd != (unsigned char)0x02)
                    {
                        enstart = 1;
                        hmi_start = (unsigned char)0x01;
                        hmi_start = hmi_start | (unsigned char)0x02;;
                        set_bw_cur_mA = m_cddl1*1000;
                        basend = this->CTRLRefresh(this->m_slaveaddr,this->m_netversion,enstart,hmi_start,set_lw_vol_mV,set_lw_freq_mHz,set_bw_cur_mA,++this->nsn);
                    }
                    else
                    {
                        enstart = 0;
                        basend = this->CTRLRefresh(this->m_slaveaddr,this->m_netversion,enstart,hmi_start,set_lw_vol_mV,set_lw_freq_mHz,set_bw_cur_mA,++this->nsn);
                    }

                }
                else if (m_lwfd1begin <= time && time <= m_lwfd1end)
                {
                    //离网放电1,放电电压m_lwfddy1，放电频率m_lwfdhz1
                    if (((unsigned char)(recv_start_mode&0x01) != (unsigned char)0x01)
                            || bwcd != (unsigned char)0x00)
                    {
                        enstart = 1;
                        hmi_start = (unsigned char)0x01;
                        set_lw_vol_mV = m_lwfddy1*1000;
                        set_lw_freq_mHz = m_lwfdhz1*1000;
                        basend = this->CTRLRefresh(this->m_slaveaddr,this->m_netversion,enstart,hmi_start,set_lw_vol_mV,set_lw_freq_mHz,set_bw_cur_mA,++this->nsn);
                    }
                    else
                    {
                        enstart = 0;
                        basend = this->CTRLRefresh(this->m_slaveaddr,this->m_netversion,enstart,hmi_start,set_lw_vol_mV,set_lw_freq_mHz,set_bw_cur_mA,++this->nsn);
                    }
                }
                else if (m_cd2begin <= time && time <= m_cd2end)
                {
                    //离网放电1,充电电流：m_cddl2
                    if (((unsigned char)(recv_start_mode&0x01) != (unsigned char)0x01)
                            || bwcd != (unsigned char)0x02)
                    {
                        enstart = 1;
                        hmi_start = (unsigned char)0x01;
                        hmi_start = hmi_start | (unsigned char)0x02;;
                        set_bw_cur_mA = m_cddl2*1000;
                        basend = this->CTRLRefresh(this->m_slaveaddr,this->m_netversion,enstart,hmi_start,set_lw_vol_mV,set_lw_freq_mHz,set_bw_cur_mA,++this->nsn);
                    }
                    else
                    {
                        enstart = 0;
                        basend = this->CTRLRefresh(this->m_slaveaddr,this->m_netversion,enstart,hmi_start,set_lw_vol_mV,set_lw_freq_mHz,set_bw_cur_mA,++this->nsn);
                    }
                }
                else if (m_lwfd2begin <= time && time <= m_lwfd2end)
                {
                    //离网放电2,放电电压：m_lwfddy2，放电频率：m_lwfdhz2
                    if (((unsigned char)(recv_start_mode&0x01) != (unsigned char)0x01)
                            || bwcd != (unsigned char)0x00)
                    {
                        enstart = 1;
                        hmi_start = (unsigned char)0x01;
                        set_lw_vol_mV = m_lwfddy2*1000;
                        set_lw_freq_mHz = m_lwfdhz2*1000;
                        basend = this->CTRLRefresh(this->m_slaveaddr,this->m_netversion,enstart,hmi_start,set_lw_vol_mV,set_lw_freq_mHz,set_bw_cur_mA,++this->nsn);
                    }
                    else
                    {
                        enstart = 0;
                        basend = this->CTRLRefresh(this->m_slaveaddr,this->m_netversion,enstart,hmi_start,set_lw_vol_mV,set_lw_freq_mHz,set_bw_cur_mA,++this->nsn);
                    }
                }
                else
                {
                   // qDebug() << "123";
                    if ((unsigned char)(recv_start_mode&0x01) != (unsigned char)0x00)
                    {
                    //    qDebug() << "456";
                        enstart = 1;
                        hmi_start = (unsigned char)0x00;
                        basend = this->CTRLRefresh(this->m_slaveaddr,this->m_netversion,enstart,hmi_start,set_lw_vol_mV,set_lw_freq_mHz,set_bw_cur_mA,++this->nsn);
                    }
                    else
                    {
                   //     qDebug() << "789";
                        enstart = 0;
                        hmi_start = (unsigned char)0x00;
                        basend = this->CTRLRefresh(this->m_slaveaddr,this->m_netversion,enstart,hmi_start,set_lw_vol_mV,set_lw_freq_mHz,set_bw_cur_mA,++this->nsn);

                    }
                }

            }
            else
            {
                enstart = 0;
                basend = this->CTRLRefresh(this->m_slaveaddr,this->m_netversion,enstart,hmi_start,set_lw_vol_mV,set_lw_freq_mHz,set_bw_cur_mA,++this->nsn);
            }
        }
        m_pserial->write(basend);
        m_pserial->flush();

        QEventLoop eventloop ;
        QObject::connect(m_pserial,&QSerialPort::readyRead,&eventloop,&QEventLoop::quit);
        QTimer::singleShot(1000, &eventloop, &QEventLoop::quit);
        eventloop.exec();

        //程序接收串口数据
        while (m_pserial->bytesAvailable() > 0)
        {
            QByteArray ba = m_pserial->readAll();
            gba.append(ba);
        }
        int ndstlen = 0;
        if (gba.size() > 0)
        {
            ndstlen = 4*(uint8_t)gba.data()[2];
        }

        while (gba.size() < ndstlen)
        {
            eventloop.exec();
            if (m_pserial->bytesAvailable() > 0)
            {
                QByteArray ba = m_pserial->readAll();
                gba.append(ba);
            }
            else
            {
                break;
            }
        }


        uint8_t* precbuf = (uint8_t*)gba.data();
        uint16_t ucrc = CRC16(precbuf,gba.size()-2);
        if (((unsigned char)(ucrc&0x00ff)) == (unsigned char)(precbuf[gba.size()-1])
                && (((unsigned char)((ucrc>>8)&0x00ff)) == (unsigned char)precbuf[gba.size()-2]))
        {
            if (precbuf[0] == 0x11 && precbuf[1] == 0xcd)
            {
                recv_frame_len_4B = precbuf[2];
                recv_slave_addr = precbuf[3];
                //当前协议
                recv_NOW_PSVER = *(uint16_t*)(precbuf+4);
                //当前软件版本
                recv_NOW_SOFTVER = *(uint16_t*)(precbuf+8);
                //当前硬件版本
                recv_NOW_HWVER = *(uint16_t*)(precbuf+10);
                recv_start_mode = precbuf[12];
                //电网电压有效值，单位:mV
                recv_dw_vol_mV = (*(uint32_t*)(precbuf+16))&0x000fffff;
 //               qDebug() << "hard version:" << recv_NOW_HWVER;
                //电网电流有效值，单位:mA
                recv_dw_cur_mA = (*(uint32_t*)(precbuf+20))&0x000fffff;
                //电网频率，单位:mHz
                recv_dw_freq_mHz = (*(uint32_t*)(precbuf+24))&0x0003ffff;;
                //SMB电压有效值，单位:mV
                recv_smb_vol_mV = (*(uint32_t*)(precbuf+28))&0x000fffff;;
                //SMB电流有效值，单位:mA
                recv_smb_cur_mA = (*(uint32_t*)(precbuf+32))&0x000fffff;
                //SMB频率，单位:mHz
                recv_smb_freq_mHz = *(uint32_t*)(precbuf+36);
                recv_smb_freq_mHz = recv_smb_freq_mHz&0x0003ffff;

                //SMB总SOC，0~100
                recv_smb_total_soc = precbuf[39];
                //相位差，单位:弧度，-9.99999 ~ 9.99999
                recv_smb_dw_phase_diff = *(float*)(precbuf+40);
                //功率因数(Power factor)，单位:x*0.001
                recv_pf_val_milli = *(uint16_t*)(precbuf+44);
                //SMBx单体
                memcpy(recv_smbinfo,precbuf+48,60);
                //通讯序号，等于收到的com_sn
                recv_com_sn = *(uint16_t*)(precbuf+188);
               // qDebug() << "fasong" << testindex;
                emit this->UpdateUISignal();
                //将主控板插上之后启动程序就一直都能运行到这里,发送signal更新到ui上
                //qDebug() <<precbuf;
            }
            else if (precbuf[0] == 0x21 && precbuf[1] == 0xcd)
            {
                //qDebug() << "配置返回值222222222";
                qDebug() << "配置返回值_基本参数配置指令(BasePra)_最大充电放电电流";
                //配置返回值
                memcpy((char *)&m_basecfg.smb_disCur,precbuf + 12,2);
                memcpy((char *)&m_basecfg.smb_chaCur,precbuf + 14,2);
                emit this->UpdateUISignal2();
                qDebug() << "放电电流:" << m_basecfg.smb_disCur;
                qDebug() << "充电电流:" << m_basecfg.smb_chaCur;
            }
        }

        gba.clear();


    }
    m_pserial->close();
    delete m_pserial;
}

void HmiConfig::Destory()
{
    b_running = false;
}

//基本参数配置指令(BasePra)
//add:从机地址，version：协议版本，en_smb使能设置，smbdiscur：最大放电电流，smb最大充电电流，
QByteArray HmiConfig::BasePra(unsigned char addr,unsigned short version,unsigned char en_smb,
                       unsigned short smbdisCur,unsigned short smbchaCur,unsigned short comsn)
{
    unsigned char CTRLRefresh_HMITX[128] = {0};
    CTRLRefresh_HMITX[0] = 0x20;
    CTRLRefresh_HMITX[1] = 0xcd;
    CTRLRefresh_HMITX[2] = sizeof(CTRLRefresh_HMITX)/4;
    CTRLRefresh_HMITX[3] = addr;
    //当前协议版本(0~999)，1=V0.0.1，123=V1.2.3
    memcpy(CTRLRefresh_HMITX+4,&version,2);
    CTRLRefresh_HMITX[8] = en_smb;
    //设置 SMB最大放电电流，单位:x*10mA，
    //en_set_smb_disCur_10mA=1时有效
    memcpy(CTRLRefresh_HMITX+12,&smbdisCur,2);
    //设置 SMB最大充电电流，单位:x*10mA，
    memcpy(CTRLRefresh_HMITX+14,&smbchaCur,2);
    //通讯序号，每次发送新指令时++
    memcpy(CTRLRefresh_HMITX+124,&comsn,2);
    //CRC校验(CRC-16/MODBUS x16+x15+x2+1)
    unsigned short crcval = CRC16(CTRLRefresh_HMITX,sizeof(CTRLRefresh_HMITX)-2);
    CTRLRefresh_HMITX[127] = (unsigned char)(crcval&0x00ff);
    CTRLRefresh_HMITX[126] = (unsigned char)((crcval>>8)&0x000000ff);
    //memcpy(CTRLRefresh_HMITX+126,&crcval,2);
    return QByteArray((const char *)CTRLRefresh_HMITX,sizeof(CTRLRefresh_HMITX));
}

//控制刷新指令(CTRLRefresh)
QByteArray HmiConfig::CTRLRefresh(unsigned char addr,unsigned short version,unsigned char en_ctrl_start,
                                  unsigned char ctrl_start,unsigned int set_lw_vol_mV,
                                  unsigned int set_lw_freq_mHz,unsigned set_bw_cur_mA,unsigned short comsn)
{

    unsigned char CTRLRefresh_HMITX[128] = {0};
    CTRLRefresh_HMITX[0] = 0x10;
    CTRLRefresh_HMITX[1] = 0xcd;
    //协议4字节长度(含CRC)
    CTRLRefresh_HMITX[2] = sizeof(CTRLRefresh_HMITX)/4;
    //从机地址，0~249。254为绝对地址，
    //即不管从机地址为多少，都回复;255为广播地址，
    //无需回复
    CTRLRefresh_HMITX[3] = addr;
    //当前协议版本(0~999)，1=V0.0.1，123=V1.2.3
    memcpy(CTRLRefresh_HMITX+4,&version,2);
    //使能位:使能 ctrl_start
    CTRLRefresh_HMITX[8] = en_ctrl_start;
    //启停控制位，1=启动，0=
    //停止;en_ctrl_start==1时有效
    CTRLRefresh_HMITX[12] = ctrl_start;
    //设定离网电压有效值，单位:mV
    memcpy(CTRLRefresh_HMITX+16,&set_lw_vol_mV,4);
    //设定离网频率，单位:mHz
    memcpy(CTRLRefresh_HMITX+20,&set_lw_freq_mHz,4);
    //设定并网电流，单位:mA
    memcpy(CTRLRefresh_HMITX+24,&set_bw_cur_mA,4);
    //通讯序号，每次发送新指令时++
    memcpy(CTRLRefresh_HMITX+124,&comsn,2);

    //CRC校验(CRC-16/MODBUS x16+x15+x2+1)
    unsigned short crcval = CRC16(CTRLRefresh_HMITX,sizeof(CTRLRefresh_HMITX)-2);
    CTRLRefresh_HMITX[127] = (unsigned char)(crcval&0x00ff);
    CTRLRefresh_HMITX[126] = (unsigned char)((crcval>>8)&0x000000ff);
 //   memcpy(CTRLRefresh_HMITX+126,&crcval,2);
    return QByteArray((const char *)CTRLRefresh_HMITX,sizeof(CTRLRefresh_HMITX));
}

bool HmiConfig::Init()
{
qDebug() << "HmiConfig::中_InitHmiConfig_Init_INI文件读取";
    nsn = 0;
    b_running = true;
    if (m_psetting == NULL)
    {
        qDebug() << "HmiConfig::中_m_psetting = NULL";
        QString hmicfg = QCoreApplication::applicationDirPath() + "/HMIConfig.ini";
        m_psetting = new QSettings(hmicfg,QSettings::IniFormat);
        m_psetting->setIniCodec("UTF8");
        m_himcom.strcom  = m_psetting->value("/COM/Name").toString();
        qDebug() << "ini文件strcom值:" << m_himcom.strcom;
        m_himcom.nbaud = m_psetting->value("/COM/Baud").toInt();
        m_slaveaddr = m_psetting->value("/PRJ/SLAVEADDR").toUInt();
        m_netversion = m_psetting->value("/PRJ/VER").toUInt();

        //并网充电1
        if (!m_psetting->value("/BWCD1/BEGIN").isNull())
        {
            this->m_cd1begin = QTime::fromString(m_psetting->value("/BWCD1/BEGIN").toString());
        }
        if (!m_psetting->value("/BWCD1/END").isNull())
        {
            this->m_cd1end = QTime::fromString(m_psetting->value("/BWCD1/END").toString());
        }
        if (!m_psetting->value("/BWCD1/DL").isNull())
        {
            this->m_cddl1 = m_psetting->value("/BWCD1/DL").toInt();
        }
        else
            this->m_cddl1 = 0;
        //离网放电1
        if (!m_psetting->value("/LWFD1/BEGIN").isNull())
        {
            this->m_lwfd1begin = QTime::fromString(m_psetting->value("/LWFD1/BEGIN").toString());
        }
        if (!m_psetting->value("/LWFD1/END").isNull())
        {
            this->m_lwfd1end = QTime::fromString(m_psetting->value("/LWFD1/END").toString());
        }
        if (!m_psetting->value("/LWFD1/DY").isNull())
        {
            this->m_lwfddy1 = m_psetting->value("/LWFD1/DY").toInt();
        }
        else
            this->m_lwfddy1 = 220;
        if (!m_psetting->value("/LWFD1/HZ").isNull())
        {
            this->m_lwfdhz1 = m_psetting->value("/LWFD1/HZ").toInt();
        }
        else
            this->m_lwfdhz1 = 50;

        //并网充电2
        if (!m_psetting->value("/BWCD2/BEGIN").isNull())
        {
            this->m_cd2begin = QTime::fromString(m_psetting->value("/BWCD2/BEGIN").toString());
        }
        if (!m_psetting->value("/BWCD2/END").isNull())
        {
            this->m_cd2end = QTime::fromString(m_psetting->value("/BWCD2/END").toString());
        }
        if (!m_psetting->value("/BWCD2/DL").isNull())
        {
            this->m_cddl2 = m_psetting->value("/BWCD2/DL").toInt();
        }
        else
            this->m_cddl2 = 0;
        //离网放电1
        if (!m_psetting->value("/LWFD2/BEGIN").isNull())
        {
            this->m_lwfd2begin = QTime::fromString(m_psetting->value("/LWFD2/BEGIN").toString());
        }
        if (!m_psetting->value("/LWFD2/END").isNull())
        {
            this->m_lwfd2end = QTime::fromString(m_psetting->value("/LWFD2/END").toString());
        }
        if (!m_psetting->value("/LWFD2/DY").isNull())
        {
            this->m_lwfddy2 = m_psetting->value("/LWFD2/DY").toInt();
        }
        else
            m_lwfddy2 = 220;
        if (!m_psetting->value("/LWFD2/HZ").isNull())
        {
            this->m_lwfdhz2 = m_psetting->value("/LWFD2/HZ").toInt();
        }
        else
            m_lwfdhz2 = 50;


        //离网电压
        if (!m_psetting->value("/CFG/LWDY").isNull())
        {
            this->set_lw_vol_mV = m_psetting->value("/CFG/LWDY").toUInt();
        }
        else
            set_lw_vol_mV = 220000;

        //离网电压频率
        if (!m_psetting->value("/CFG/LWDYHZ").isNull())
        {
            this->set_lw_freq_mHz = m_psetting->value("/CFG/LWDYHZ").toUInt();
        }
        else
            set_lw_freq_mHz =50000;
        //并网电流
        if (!m_psetting->value("/CFG/BWDL").isNull())
        {
            this->set_bw_cur_mA = m_psetting->value("/CFG/BWDL").toUInt();
        }
        else
            set_bw_cur_mA =10000;

        return true;

    }
    return false;
}

HmiConfig* HmiConfig::GetInstance()
{
    if (m_instance == 0)
        m_instance = new HmiConfig;
    return m_instance;
}


