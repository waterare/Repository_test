#ifndef HMICONFIG_H
#define HMICONFIG_H
#include "qdatetime.h"
#include <QSettings>
#include <QSerialPort>
#include <QSemaphore>

struct HmiCom
{
    QString strcom;
    int nbaud;
};

enum HMI_CMD
{
    BASE_READ,
    BASE_SET,
    HMI_ACTION
};

//基本信息
struct BaseConfig
{
    //最大放电
    unsigned short smb_disCur;
    //最大充电
    unsigned short smb_chaCur;
};

typedef struct
{
uint16_t vol_10mV; // SMBx 单体电压，单位:x*10mV
int16_t cur_10mA; // SMBx 单体电流，单位:x*10mA
uint8_t soc; // SMBx 单体SOC，0~100
int8_t tmp; // SMBx 单体温度，单位:℃
} SMBx_Info__;



class HmiConfig: public QObject
{
    Q_OBJECT
signals:
    void UpdateUISignal();
    void UpdateUISignal2();

public:
    static HmiConfig* GetInstance();
    bool Init();
    void Destory();

    //static QSerialPort *m_pserial;//将私有的修改为公有的,增加static修饰,20230721
    QSerialPort *m_pserial;//将私有的修改为公有的,20230721
    bool brt_serial , my_serial_=true;


    HmiCom GetHimCom() { return m_himcom;}
    //控制刷新指令
    //addr:从机地址，0~249。254为绝对地址，255为广播地址，
    //version:当前协议版本(0~999)，1=V0.0.1，123=V1.2.3
    //en_ctrl_start:使能位:使能 ctrl_start
    //ctrl_start:启停控制位，1=启动，0=停止;en_ctrl_start==1时有效
    //set_lw_vol_mV:设定离网电压有效值，单位:mV
    //set_lw_freq_mHz:设定离网频率，单位:mHz
    //set_bw_cur_mA:设定并网电流，单位:mA
    //comsn:通讯序号，每次发送新指令时++
    QByteArray CTRLRefresh(unsigned char addr,unsigned short version,unsigned char en_ctrl_start,
                           unsigned char ctrl_start,unsigned int set_lw_vol_mV,unsigned int set_lw_freq_mHz,
                           unsigned set_bw_cur_mA,unsigned short comsn);

    //add:从机地址，version：协议版本，en_smb使能设置，smbdiscur：最大放电电流，smb最大充电电流，
    QByteArray BasePra(unsigned char addr,unsigned short version,unsigned char en_smb,
                           unsigned short smbdisCur,unsigned short smbchaCur,unsigned short comsn);
    QSemaphore *getFullSem()
    {
        return &this->m_full;
    }

    QSemaphore *getEmptySem()
    {
        return &this->m_empty;
    }

    HMI_CMD *getCmdAction()
    {
        return &m_cmdaction;
    }

    BaseConfig *getBaseCfg()
    {
        return &this->m_basecfg;
    }

    QSettings *getConfigFile()
    {
        return m_psetting;
    }

    //两充两放的参数
    //并网充电1
    QTime m_cd1begin;
    QTime m_cd1end;
    unsigned int m_cddl1;
    //离网放电1
    QTime m_lwfd1begin;
    QTime m_lwfd1end;
    unsigned int m_lwfddy1;
    unsigned int m_lwfdhz1;
    //并网充电2
    QTime m_cd2begin;
    QTime m_cd2end;
    unsigned int m_cddl2;
    //离网放电1
    QTime m_lwfd2begin;
    QTime m_lwfd2end;
    unsigned int m_lwfddy2;
    unsigned int m_lwfdhz2;

public slots:
    void do_work();


private:
    static HmiConfig* m_instance;

    HmiConfig();
    QSettings *m_psetting;

    //QSerialPort *m_pserial;
    //从机地址
    unsigned char m_slaveaddr;
    //协议版本
    unsigned short m_netversion;
    bool b_running;
    unsigned short nsn;
    QSemaphore m_empty;
    QSemaphore m_full;
    HMI_CMD m_cmdaction;






    //基本参数
    BaseConfig m_basecfg;
public:
    //istart
    uint8_t recv_frame_len_4B ;
    uint8_t recv_slave_addr;
    //当前协议
    uint16_t recv_NOW_PSVER ;
    //当前软件版本
    uint16_t recv_NOW_SOFTVER;
    //当前硬件版本
    uint16_t recv_NOW_HWVER;
    uint8_t recv_start_mode;
    //电网电压有效值，单位:mV
    uint32_t recv_dw_vol_mV;
    //电网电流有效值，单位:mA
    uint32_t recv_dw_cur_mA;
    //电网频率，单位:mHz
    uint32_t recv_dw_freq_mHz;
    //SMB电压有效值，单位:mV
    uint32_t recv_smb_vol_mV;
    //SMB电流有效值，单位:mA
    uint32_t recv_smb_cur_mA;
    //SMB频率，单位:mHz
    uint32_t recv_smb_freq_mHz;
    //SMB总SOC，0~100
    uint8_t recv_smb_total_soc;
    //相位差，单位:弧度，-9.99999 ~ 9.99999
    float recv_smb_dw_phase_diff;
    //功率因数(Power factor)，单位:x*0.001
    uint16_t recv_pf_val_milli;
    uint16_t recv_com_sn;
    SMBx_Info__ recv_smbinfo[10];

    //hmiaction
    //en_ctrl_start:使能位:使能 ctrl_start
    //ctrl_start:启停控制位，1=启动，0=停止;en_ctrl_start==1时有效
    //set_lw_vol_mV:设定离网电压有效值，单位:mV
    //set_lw_freq_mHz:设定离网频率，单位:mHz
    //set_bw_cur_mA:设定并网电流，单位:mA
    uint8_t enstart;
    uint8_t hmi_start;
    unsigned int set_lw_vol_mV;
    unsigned int set_lw_freq_mHz;
    unsigned set_bw_cur_mA;


    bool blclfstart;
    HmiCom m_himcom;//2023.07.24修改，将私有修改为公有



};

#endif // HMICONFIG_H
