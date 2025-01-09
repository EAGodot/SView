#include "portbase.h"
#include "portrs232.h"
#include <QDebug>


PortBase::PortBase(QObject *parent,Configuration *config):QThread(parent){
    this->mainwindow = (MainWindow*)(this->parent());
    this->config = config;
    this->forceSend = false;
    this->portID = 0;
}

PortBase *createPort(QObject *parent,Configuration* config){
    if(0 == config->get("_setup_","port").compare("hid",Qt::CaseInsensitive)){
        //return new PortHid(parent,config);
    }

    qDebug() << "createPort";
    return new PortRs232(parent,config);
}

PortBase::~PortBase(){    
}

//escape 字符轉義
//ascll 轉義
QByteArray stringLiteralUnescape(const QByteArray & data){
    QByteArray result;
    uchar escaped = 0;           //escaped char zone \n \r \t, etc
    uchar escaped_hex = 0;      //escaped hex byte zone  \x20
    uchar escaped_dec = 0;      //escaped dec byte zone  \d062 , \d158
    unsigned char digitsByte;   //byte built from escaped digits (hex, dec, etc)
    //在 C++ 中，tolower函数定义在<cctype>头文件中。它主要用于将大写字母转换为小写字母。
    //例如，如果有一个字符变量c，通过tolower(c)可以将大写字母A - Z转换为对应的小写字母a - z，而对于非大写字母字符（如数字、小写字母、标点符号等），函数通常会直接返回该字符本身。
    for(int i = 0; i<data.size();i++){
        uchar c = data.data()[i];
        if(escaped_hex > 0){
            uchar c_lower = tolower(c);
            if('0'<=c && c<='9') {digitsByte = digitsByte*16 + (c-'0'); escaped_hex--;}
            else if('a'<=c_lower && c_lower<='f') {
                digitsByte = digitsByte*16 + (c-'a'+10); escaped_hex--;}
            else {
                escaped_hex = 0; i--; //invalid digit, process char again on next loop
            }
            if(0==escaped_hex){ result.append(digitsByte);};
        }else if(escaped_dec > 0){
            if('0'<=c && c<='9') {
                digitsByte = digitsByte*10 + (c-'0'); escaped_dec--;}
            else {
                escaped_dec = 0; i--; //process char again, process char again on next loop
            }
            if(0==escaped_dec){ result.append(digitsByte);};
        }else if(escaped){
            if('n'==c) result.append('\n');
            if('r'==c) result.append('\r');
            if('t'==c) result.append('\t');
            if('\\'==c) result.append('\\');
            if('x'==c) {escaped_hex = 2; digitsByte = 0;};    //expect 2 hex chars
            if('d'==c) {escaped_dec = 3;  digitsByte = 0;};   //expect 3 decimal chars
            escaped = 0;
        }else{
            if('\\'==c) escaped = 1;
            else result.append(c);
        }        
    }
    if(0 != escaped_hex)
    {
        result.append(digitsByte);
    }

    return result;
}
















