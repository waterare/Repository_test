#ifndef COLLECTION_H_
#define COLLECTION_H_

#include <QThread>
#include <QTimer>
#ifdef HAS_RABBITMQ
#include "rabbitmq_client.h"
#endif
//#include "hmiMsg.pb.h"
#include "cJson.h"
#include "../hmiconfig.h"
#include <memory>

#define VERSION "V1.0"

class DataCollection : public QObject{
    Q_OBJECT
public:
    static DataCollection& Instance() {
        static DataCollection instance;
        return instance;
    }

    ~DataCollection(){
        collect_timer_.stop();
    }

    int init();




public slots:
    int update_hmi_msg();
    int publish_hmi_config_msg();

private:
    char* serialization_hmi_msg();

    char* serialization_hmi_config_msg();

    int publish_hmi_msg();

    bool saveJSON(string Save_Address,string JSONtext);
    bool readJSON(string name);

private:
#ifdef HAS_RABBITMQ
    CRabbitmqClient rabbitmq_client_;
#endif
    QTimer collect_timer_;

    std::string strExchange{"ExchageChuneng"};
    std::string strRoutekey{"RouteChuneng"};
    std::string strQueuename{"chuneng"};

private:
    DataCollection();
    DataCollection(const DataCollection&);
    DataCollection& operator=(const DataCollection&);
};



#endif
