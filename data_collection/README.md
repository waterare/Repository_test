# &#x20;Ubuntu x86 开发

* 默认使用QT5.15.2版本
* 安装rabbitmq开发库

```
sudo apt install librabbitmq-dev
```
备注：arm编译需要安装arm版 rabbitmq-dev环境

若rabbitmq-dev未正确安装，可data_collection.pri 文件中 DEFINES += HAS_RABBITMQ 注释


- 单元测试


```
#此程序为了测试rabbitmq服务是否正常
#直接读取 hmiMsg.json ,hmiconfigMsg.json上传大数据平台，不依赖主控板上传数据
cd data_collection
cmake .
make
./data_collection_test

```
- 打包程序

1, 将qt 设置build目录下可执行文件 vesc_tool_6.02 和 程序目录下 HMIConfig.ini 拷贝pack目录
2, 在pack目录，执行拷贝依赖库 ./ldd_copy.sh
3, 在pack目录，执行可执行文件 ./vesc_tool_6.02.sh
