DEFINES += HAS_RABBITMQ
DEFINES += HAVE_SELECT


HEADERS += \
    $$PWD/cJson.h \
    $$PWD/collection.h \
    #$$PWD/rabbitmq/include/amqp.h \
    #$$PWD/rabbitmq/include/amqp_framing.h \
    #$$PWD/rabbitmq/include/amqp_tcp_socket.h \
    #$$PWD/rabbitmq/include/rabbitmq-c/amqp.h \
    #$$PWD/rabbitmq/include/rabbitmq-c/export.h \
    #$$PWD/rabbitmq/include/rabbitmq-c/framing.h \
    #$$PWD/rabbitmq/include/rabbitmq-c/tcp_socket.h \
    #$$PWD/rabbitmq/librabbitmq/amqp_private.h \
    #$$PWD/rabbitmq/librabbitmq/amqp_socket.h \
    #$$PWD/rabbitmq/librabbitmq/amqp_table.h \
    #$$PWD/rabbitmq/librabbitmq/amqp_time.h \
    $$PWD/rabbitmq_client.h

SOURCES += \
    $$PWD/cJson.c \
    $$PWD/collection.cpp \
    #$$PWD/rabbitmq/librabbitmq/amqp_api.c \
    #$$PWD/rabbitmq/librabbitmq/amqp_connection.c \
    #$$PWD/rabbitmq/librabbitmq/amqp_consumer.c \
    #$$PWD/rabbitmq/librabbitmq/amqp_framing.c \
    #$$PWD/rabbitmq/librabbitmq/amqp_mem.c \
    #$$PWD/rabbitmq/librabbitmq/amqp_socket.c \
    #$$PWD/rabbitmq/librabbitmq/amqp_table.c \
    #$$PWD/rabbitmq/librabbitmq/amqp_tcp_socket.c \
    #$$PWD/rabbitmq/librabbitmq/amqp_time.c \
    #$$PWD/rabbitmq/librabbitmq/amqp_url.c \
    $$PWD/rabbitmq_client.cpp

DISTFILES += \
    $$PWD/README.md \
    $$PWD/hmiMsg.json \
    $$PWD/hmiMsg.proto \
    $$PWD/hmiconfigMsg.json \
    $$PWD/rabbitmq/librabbitmq/CMakeLists.txt \
    $$PWD/rabbitmq/librabbitmq/codegen.py \
    $$PWD/unit_test/main.cpp \
    $$PWD/unit_test/CMakeLists.txt \
    $$PWD/unit_test/rabbitmqProducer_j3.py

#INCLUDEPATH += $$PWD/rabbitmq/include
#LIBS +=-lws2_32

INCLUDEPATH +=
LIBS +=-lrabbitmq

