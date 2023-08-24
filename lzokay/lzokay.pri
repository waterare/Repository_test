HEADERS += \
    $$PWD/lzokay.hpp

SOURCES += \
    $$PWD/lzokay.cpp

#lzokay源文件中linux系统下编译std::begin和std::end无法call,简单粗暴的方式是将其注释掉
