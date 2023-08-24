#include "collection.h"

#include <QDebug>
#include <vector>
#include <string>
#include <sstream>  // 字符串流，在这里是 std::stringstream 所需的头文件
#include <fstream>   // 对文件输入输出
#include <iostream>  //对屏幕上输入输出
#include<QFile> //file文件操作
#include<QFileDialog>

using namespace std;

DataCollection::DataCollection(){

}

//hmimainwindow.cpp文件中使用,如果将这句话注释，将不会再上传和存储数据
//DataCollection::Instance().init();

/*
* 默认使用QT5.15.2版本
* 安装rabbitmq开发库
linux 系统下需要执行如下命令，安装rabbitmq库，目前windows已经进行适配
sudo apt install librabbitmq-dev

备注：arm编译需要安装arm版 rabbitmq-dev环境

若rabbitmq-dev未正确安装，可data_collection.pri 文件中 DEFINES += HAS_RABBITMQ 注释
*/

int DataCollection::init()
{
#ifdef HAS_RABBITMQ
    std::string strIP = "175.154.142.113";
    int iPort = 5670;
//    std::string strIP = "172.30.43.122";
//    int iPort = 5672;
    std::string strUser = "admin";
    std::string strPasswd = "123456";

    //readJSON("E:\\05_git_Repositories\\json.txt");

    setbuf(stdout, NULL);//让printf立即输出
    int iRet = rabbitmq_client_.Connect(strIP, iPort, strUser, strPasswd);
    printf("Rabbitmq Connect Ret: %d\n", iRet);

    //Declare Exchange
    iRet = rabbitmq_client_.ExchangeDeclare(strExchange, "direct");
    printf("Rabbitmq ExchangeDeclare Ret: %d\n", iRet);

    //Declare Queue
    iRet = rabbitmq_client_.QueueDeclare(strQueuename);
    printf("Rabbitmq QueueDeclare Ret: %d\n", iRet);

    //Queue Bind
    iRet = rabbitmq_client_.QueueBind(strQueuename, strExchange, strRoutekey);
    printf("Rabbitmq QueueBind Ret: %d\n", iRet);

    /*输出为0时候才是正常的，如果为-1则出现异常
    Rabbitmq Connect Ret: 0
    ExchangeDeclare : 0
    Rabbitmq ExchangeDeclare Ret: 0
    Rabbitmq QueueDeclare Ret: 0
    Rabbitmq QueueBind Ret: 0
    */

    //connect(HmiConfig::GetInstance(), &HmiConfig::UpdateUISignal, this, &DataCollection::publish_hmi_config_msg);

    //connect(HmiConfig::GetInstance(), &HmiConfig::UpdateUISignal, this, &DataCollection::publish_hmi_config_msg);
    //触发上传，建议关掉，否则上传频率很高

    collect_timer_.setInterval(5000);
    connect(&collect_timer_, &QTimer::timeout,this,&DataCollection::update_hmi_msg);
    collect_timer_.start();
#endif
    return 0;
}

char * DataCollection::serialization_hmi_msg()
{
    HmiConfig* config =  HmiConfig::GetInstance();
    cJSON *root , *head , *body , *arry, *list;

    root=cJSON_CreateObject();
    cJSON_AddItemToObject(root , "header" , head=cJSON_CreateObject());
    cJSON_AddItemToObject(root , "body" ,  body=cJSON_CreateObject());
    QDateTime timeDate = QDateTime::currentDateTime();  // 获取当前时间

    //以时间戳方式存储
    cJSON_AddNumberToObject(head,"timestamp",timeDate .toTime_t());


    //以时间年月日存储
    QString timeDate_string = timeDate .toString("yyyy-MM-dd_hh:mm:ss");//格式化时间;
    const char *c_timeDate_= timeDate_string.toStdString().c_str();
    cJSON_AddStringToObject(head,"timeformat",(char *)c_timeDate_);


    cJSON_AddStringToObject(head,"version", VERSION);

    cJSON_AddNumberToObject(body,"en_ctrl_start",config->enstart);
    cJSON_AddNumberToObject(body,"out_mode",config->recv_start_mode);
    cJSON_AddNumberToObject(body,"dw_vol_mV",config->recv_dw_vol_mV);
    cJSON_AddNumberToObject(body,"dw_freq_mHz",config->recv_dw_freq_mHz);
    cJSON_AddNumberToObject(body,"dw_cur_mA",config->recv_dw_cur_mA);
    cJSON_AddNumberToObject(body,"smb_vol_mV",config->recv_smb_vol_mV);
    cJSON_AddNumberToObject(body,"smb_cur_mA",config->recv_smb_cur_mA);
    cJSON_AddNumberToObject(body,"smb_freq_mHz",config->recv_smb_freq_mHz);
    cJSON_AddNumberToObject(body,"smb_total_soc",config->recv_smb_total_soc);
    cJSON_AddNumberToObject(body,"smb_dw_phase_diff",config->recv_smb_dw_phase_diff);
    cJSON_AddNumberToObject(body,"pf_val_milli",config->recv_pf_val_milli);

    cJSON_AddItemToObject(body , "smb_infos" ,  arry=cJSON_CreateArray());

    for( int i = 0 ; i< 10 ; i++)
    {
        cJSON_AddItemToArray(arry, list = cJSON_CreateObject());
        cJSON_AddNumberToObject(list,"vol_10mV",config->recv_smbinfo[i].vol_10mV);
        cJSON_AddNumberToObject(list,"cur_10mA",config->recv_smbinfo[i].cur_10mA);
        cJSON_AddNumberToObject(list,"soc",config->recv_smbinfo[i].soc);
        cJSON_AddNumberToObject(list,"tmp",config->recv_smbinfo[i].tmp);
    }
    char *out = cJSON_Print(root);   // 将json形式转换成字符串
    //char *out = cJSON_PrintUnformatted(root);   // json

    char *json_data = cJSON_PrintUnformatted(root); //JSON数据结构转换为JSON字符串,json数据将存为一行。
    //free(json_data); //不能用free,否则会乱码
    //cJSON_Delete(root);//清除结构体
    //saveJSON("E:\\05_git_Repositories\\json.txt",json_data);
    saveJSON("json.txt",json_data);


    printf("%s\n",out);

    // 释放内存
    cJSON_Delete(root);


    return out;
}

char*  DataCollection::serialization_hmi_config_msg()
{
    HmiConfig* config =  HmiConfig::GetInstance();

    cJSON *root , *head , *body , *online1,*online2,*offline1,*offline2;

    root=cJSON_CreateObject();
    cJSON_AddItemToObject(root , "header" , head=cJSON_CreateObject());
    cJSON_AddItemToObject(root , "body" ,  body=cJSON_CreateObject());
    QDateTime timeDate = QDateTime::currentDateTime();  // 获取当前时间

    //以时间戳方式存储
    cJSON_AddNumberToObject(head,"timestamp",timeDate.toTime_t());

    //以时间年月日存储
    QString timeDate_string = timeDate .toString("yyyy-MM-dd_hh:mm:ss");//格式化时间;
    const char *c_timeDate_= timeDate_string.toStdString().c_str();
    cJSON_AddStringToObject(head,"timeformat",(char *)c_timeDate_);

    cJSON_AddStringToObject(head,"version", VERSION );


    cJSON_AddNumberToObject(body,"en_set_smb_chaCur_10mA",config->enstart);
    cJSON_AddNumberToObject(body,"set_smb_disCur_10mA",config->recv_start_mode);
    cJSON_AddNumberToObject(body,"set_smb_chaCur_10mA",config->recv_dw_vol_mV);

    cJSON_AddItemToObject(body , "online1" ,  online1=cJSON_CreateObject());
    cJSON_AddStringToObject(online1,"start_time",config->m_cd1begin.toString("yyyy-MM-dd HH:mm:ss").toStdString().c_str());
    cJSON_AddStringToObject(online1,"end_time",config->m_cd1end.toString("yyyy-MM-dd HH:mm:ss").toStdString().c_str());
    cJSON_AddNumberToObject(online1,"cur_10mA",config->m_cddl1);

    cJSON_AddItemToObject(body , "online2" ,  online2=cJSON_CreateObject());
    cJSON_AddStringToObject(online2,"start_time",config->m_cd2begin.toString("yyyy-MM-dd HH:mm:ss").toStdString().c_str());
    cJSON_AddStringToObject(online2,"end_time",config->m_cd2end.toString("yyyy-MM-dd HH:mm:ss").toStdString().c_str());
    cJSON_AddNumberToObject(online2,"cur_10mA",config->m_cddl2);

    cJSON_AddItemToObject(body , "offline1" ,  offline1=cJSON_CreateObject());
    cJSON_AddStringToObject(offline1,"start_time",config->m_cd1begin.toString("yyyy-MM-dd HH:mm:ss").toStdString().c_str());
    cJSON_AddStringToObject(offline1,"end_time",config->m_lwfd1end.toString("yyyy-MM-dd HH:mm:ss").toStdString().c_str());
    cJSON_AddNumberToObject(offline1,"vol_10mA",config->m_lwfddy1);
    cJSON_AddNumberToObject(offline1,"power",config->m_lwfdhz1);

    cJSON_AddItemToObject(body , "offline2" ,  offline2=cJSON_CreateObject());
    cJSON_AddStringToObject(offline2,"start_time",config->m_cd2begin.toString("yyyy-MM-dd HH:mm:ss").toStdString().c_str());
    cJSON_AddStringToObject(offline2,"end_time",config->m_lwfd2end.toString("yyyy-MM-dd HH:mm:ss").toStdString().c_str());
    cJSON_AddNumberToObject(offline2,"vol_10mA",config->m_lwfddy2);
    cJSON_AddNumberToObject(offline2,"power",config->m_lwfdhz2);
    char *out = cJSON_Print(root);   // 将json形式转换成字符串
    printf("%s\n",out);

    char *json_data = cJSON_PrintUnformatted(root); //JSON数据结构转换为JSON字符串,json数据将存为一行。
    //free(json_data); //不能用free,否则会乱码
    //cJSON_Delete(root);//清除结构体
    //saveJSON("E:\\05_git_Repositories\\json.txt",json_data);
    saveJSON("json.txt",json_data);


    // 释放内存
    cJSON_Delete(root);


    return out;
}



//int DataCollection::serialization_hmi_msg(const std::shared_ptr<string> &buf)
//{
//    HmiConfig* config =  HmiConfig::GetInstance();
//    HmiMsg hmi_msg{};
//    QDateTime timeDate = QDateTime::currentDateTime();  // 获取当前时间
//    hmi_msg.mutable_header()->set_timestamp(timeDate .toTime_t());
//    hmi_msg.set_en_ctrl_start(config->enstart);
//    hmi_msg.set_out_mode(config->recv_start_mode);
//    hmi_msg.set_dw_vol_mv(config->recv_dw_vol_mV);
//    hmi_msg.set_dw_freq_mhz(config->recv_dw_freq_mHz);
//    hmi_msg.set_dw_cur_ma(config->recv_dw_cur_mA);
//    hmi_msg.set_smb_vol_mv(config->recv_smb_vol_mV);
//    hmi_msg.set_smb_cur_ma(config->recv_smb_cur_mA);
//    hmi_msg.set_smb_freq_mhz(config->recv_smb_freq_mHz);
//    hmi_msg.set_smb_total_soc(config->recv_smb_total_soc);
//    hmi_msg.set_smb_dw_phase_diff(config->recv_smb_dw_phase_diff);
//    hmi_msg.SerializeToString( buf.get());
//    printf("hmi msg : %s\n", hmi_msg.DebugString().c_str());
//    return 0;
//}


//int DataCollection::serialization_hmi_config_msg(const std::shared_ptr<string> &buf){
//    HmiConfig* config =  HmiConfig::GetInstance();
//    HmiConfigMsg hmi_config_msg{};
//    QDateTime timeDate = QDateTime::currentDateTime();  // 获取当前时间
//    hmi_config_msg.mutable_header()->set_timestamp(timeDate .toTime_t());
//    hmi_config_msg.set_en_set_smb_chacur_10ma(config->enstart);
//    hmi_config_msg.set_set_smb_discur_10ma(config->recv_start_mode);
//    hmi_config_msg.set_set_smb_chacur_10ma(config->recv_dw_vol_mV);

//    hmi_config_msg.mutable_online1()->set_start_time(config->m_cd1begin.toString("yyyy-MM-dd HH:mm:ss").toStdString());
//    hmi_config_msg.mutable_online1()->set_end_time(config->m_cd1end.toString("yyyy-MM-dd HH:mm:ss").toStdString());
//    hmi_config_msg.mutable_online1()->set_cur_10ma(config->m_cddl1);

//    hmi_config_msg.mutable_online2()->set_start_time(config->m_cd2begin.toString("yyyy-MM-dd HH:mm:ss").toStdString());
//    hmi_config_msg.mutable_online2()->set_end_time(config->m_cd2end.toString("yyyy-MM-dd HH:mm:ss").toStdString());
//    hmi_config_msg.mutable_online2()->set_cur_10ma(config->m_cddl2);

//    hmi_config_msg.mutable_offline1()->set_start_time(config->m_lwfd1begin.toString("yyyy-MM-dd HH:mm:ss").toStdString());
//    hmi_config_msg.mutable_offline1()->set_end_time(config->m_lwfd1end.toString("yyyy-MM-dd HH:mm:ss").toStdString());
//    hmi_config_msg.mutable_offline1()->set_vol_10ma(config->m_lwfddy1);
//    hmi_config_msg.mutable_offline1()->set_power(config->m_lwfdhz1);

//    hmi_config_msg.mutable_offline2()->set_start_time(config->m_lwfd2begin.toString("yyyy-MM-dd HH:mm:ss").toStdString());
//    hmi_config_msg.mutable_offline2()->set_end_time(config->m_lwfd2end.toString("yyyy-MM-dd HH:mm:ss").toStdString());
//    hmi_config_msg.mutable_offline2()->set_vol_10ma(config->m_lwfddy2);
//    hmi_config_msg.mutable_offline2()->set_power(config->m_lwfdhz2);

//    hmi_config_msg.SerializeToString( buf.get());
//    printf("hmi config msg : %s\n", hmi_config_msg.DebugString().c_str());
//    return 0;
//}

//对数据json数据写入和读取
//char *json_data = cJSON_PrintUnformatted(root); //JSON数据结构转换为JSON字符串,json数据将存为一行。
//free(json_data); //不能用free,否则会乱码
//cJSON_Delete(root);//清除结构体
//saveJSON("E:\\05_git_Repositories\\json.txt",json_data);

//需要引入如下头文件
//#include <QDebug>
//#include <vector>
//#include <string>
//#include <sstream>  // 字符串流，在这里是 std::stringstream 所需的头文件
//#include <fstream>   // 对文件输入输出
//#include <iostream>  //对屏幕上输入输出

//#include<QFile> //file文件操作
//#include<QFileDialog>

//using namespace std;

bool DataCollection::saveJSON(string Save_Address,string JSONtext)
{
    //ifstream默认以输入方式打开文件，而ofstream默认以输出方式打开文件
    qDebug() << "saveJSON方法中";
    std::ofstream fout_json(Save_Address.c_str(),std::ios::app);//app是在文档尾部添加
    qDebug() << Save_Address.c_str();


   if (!fout_json.is_open())
   {
       qDebug() << "没有正确创建文件";
       printf( "没有正确创建文件");
       return false;
   }
   qDebug() << "josn文件开始存储";
   fout_json <<JSONtext<< endl;
   qDebug() << "save文件存储完成" ;
   //fout_json.close();
   return true;
}

//readJSON("E:\\05_git_Repositories\\json.txt");
bool DataCollection::readJSON(string name)
{
   //用std::ifstream 进行文件读取，注意while的条件。（用法不正确时，会出现文本最后一行重复读取两遍）
   qDebug() << "read方法中";
   std::ifstream fout2(name.c_str());//读取文件的名字，可以相对或绝对
   if (!fout2.is_open())
   {
       printf("无法正确读取文件");
       qDebug() << "无法正确读取文件";
       return false;
   }
   else
   {
       qDebug() << "文件打开成功";
   }
   qDebug() << "开始读取文件";
   //函数eof()用来判断文件是否读到尾部,没读到尾部返回false，否则返回true。若尾部有回车，那么最后一条记录会读取两次。若尾部没有回车，那么最后一条记录只会读取一次
   //while (!fout2.eof())
   while (fout2.peek()!=EOF) //避免最后一行读取两遍
   {
       //getline按行读取，字符串方式读取并打印输出需要添加头文件：#include<string>
       string line;
       getline(fout2, line);
       //std::stringstream data(line);
       //qDebug() << "读取文件中"<<endl;
       cout<< "read file..."<<endl;
       cout<<line<<endl;
       cout<<endl;
       cout<<endl;




       //string a, b, c;
       //char a[20],b[20],c[20];
       //fout2>>a>>b>>c;//读取的时候遇见空格才会跳跃。
       //cout<<a<<"	"<<b<<"  "<<c<<"  "<<endl;


       //按照行来读取
       //char str[50] = { 0 };
       //fout2.getline(str, 50);
       //cout << str << endl;


       //string s, a1, b1;
       //data >> s >> a1 >> b1;
       //qDebug() << s << a1 << b1;
       //setbuf(stdout, NULL);//让printf立即输出
       //printf("%s %s %s\n", s, a1, b1);
   }
   fout2.close();
   return true;
}


int DataCollection::publish_hmi_config_msg(){
    char *hmi_config_msg_str = serialization_hmi_config_msg(  );
#ifdef HAS_RABBITMQ
    rabbitmq_client_.Publish(hmi_config_msg_str, strExchange, strRoutekey);
#endif
    free(hmi_config_msg_str);
    return 0;
}

int DataCollection::publish_hmi_msg(){
    char *hmi_msg_str = serialization_hmi_msg(  );
#ifdef HAS_RABBITMQ
    rabbitmq_client_.Publish(hmi_msg_str, strExchange, strRoutekey);
#endif
    free(hmi_msg_str);
    return 0;
}

int DataCollection::update_hmi_msg()
{
    publish_hmi_msg();
    publish_hmi_config_msg();
    // test recv
//    std::vector<std::string> vecRecvMsg;
//    int iRet = rabbitmq_client_.Consumer(strQueuename, vecRecvMsg, 2);
//    printf("Rabbitmq Consumer Ret: %d\n", iRet);

//    for (size_t i=0; i<vecRecvMsg.size(); ++i) {
//        printf("Consumer: %s\n", vecRecvMsg[i].c_str());
//    }

    return 0;
}


