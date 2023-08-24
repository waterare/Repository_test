
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <vector>
using namespace  std;
#include "../rabbitmq_client.h"

int read_file(const std::string &filename , string & outbuf) {
    std::ifstream ifs;

    ifs.open(filename,std::ios::in) ;
    if (!ifs.is_open()) {
        std::cout << filename << " 文件打开失败 " << std::endl;
        return -1;
    }
    string buf;
    while (getline(ifs,buf)){
        outbuf.append(buf) ;
        outbuf += "\n";
    }
    std::cout << filename << outbuf << std::endl;
    ifs.close();
    return 0;
}

int main()
{
    CRabbitmqClient objRabbitmq;

    std::string strIP = "175.154.142.113";
    int iPort = 5670;
    std::string strUser = "admin";
    std::string strPasswd = "123456";


    int iRet = objRabbitmq.Connect(strIP, iPort, strUser, strPasswd);
    printf("Rabbitmq Connect Ret: %d\n", iRet);


    std::string strExchange = "ExchageChuneng";
    std::string strRoutekey = "RouteChuneng";
    std::string strQueuename = "chuneng";

    //Declare Exchange
    iRet = objRabbitmq.ExchangeDeclare(strExchange, "direct");
    printf("Rabbitmq ExchangeDeclare Ret: %d\n", iRet);

    //Declare Queue
    iRet = objRabbitmq.QueueDeclare(strQueuename);
    printf("Rabbitmq QueueDeclare Ret: %d\n", iRet);

    //Queue Bind
    iRet = objRabbitmq.QueueBind(strQueuename, strExchange, strRoutekey);
    printf("Rabbitmq QueueBind Ret: %d\n", iRet);


    std::string strSendMsg1;
    std::string strSendMsg2;
    read_file("../hmiMsg.json",strSendMsg1) ;
    read_file("../hmiconfigMsg.json",strSendMsg2) ;

    iRet = objRabbitmq.Publish(strSendMsg1, strExchange, strRoutekey);
    printf("Rabbitmq Publish 1 Ret: %d\n", iRet);
    iRet = objRabbitmq.Publish(strSendMsg2, strExchange, strRoutekey);
    printf("Rabbitmq Publish 2 Ret: %d\n", iRet);

    // Recv Msg
    std::vector<std::string> vecRecvMsg;
    iRet = objRabbitmq.Consumer(strQueuename, vecRecvMsg, 2);
    printf("Rabbitmq Consumer Ret: %d\n", iRet);

    for (size_t i=0; i<vecRecvMsg.size(); ++i) {
        printf("Consumer: %s\n", vecRecvMsg[i].c_str());
    }

    objRabbitmq.Disconnect();
    return 0;
}

