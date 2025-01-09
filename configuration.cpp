#include "configuration.h"
#include <QDebug>


Configuration::Configuration(QObject *parent):QObject(parent){
}

//常量引用不允許值被修改
const QString Configuration::get(const QString& sectionName,const QString& propertyName,const QString& defaultValue){
    if(sections.contains(sectionName) && sections[sectionName].contains(propertyName)){
        return sections[sectionName][propertyName];
    }
    else{
        if(sectionName!="_setup_" && sectionName != "_default"){
            return get("_default_",propertyName,defaultValue);
        }
        else{
            return defaultValue;
        }       
    }
}


void Configuration::parse(const QString& str){
    //load defaults
    fields.clear();
    sections.clear();
    sections["_setup_"]["port"] = "COM1";
    sections["_setup_"]["baudrate"] = "9600";
    sections["_setup_"]["min"] = "-100";
    sections["_setup_"]["max"] = "100";

    sections["_default_"]["color"] = "black";
    sections["_default_"]["min"] = "-100";
    sections["_default_"]["max"] = "100";
    //parse configuration
    QString sectionName = "_setup_";
    QStringList lines = str.split(QRegExp("[\\x0D\\x0A]+"));
    QStringList::iterator i;
    for(i = lines.begin();i != lines.end(); i++){
        QString line = *i;
        qDebug() << "line" <<line;
        //豆包解釋：
        //这个正则表达式主要用于匹配以方括号[]包裹一个单词（由字母、数字或下划线组成的字符序列）的字符串，
        //并且字符串的开头和结尾可以有零个或多个空白字符。
        QRegExp rx("^\\s*\\[\\s*(\\w+)\\s*\\]\\s*$");
        if(rx.exactMatch(line)){
            sectionName = rx.capturedTexts()[1];
            if(!fields.contains(sectionName) && sectionName != "_setup_" && sectionName != "_default_"){
                fields.append(sectionName);}
            continue;
        }
            rx.setPattern("^\\s*(\\w+)\\s*\\=(.*)$");
            if(rx.exactMatch(line)){
                QString value = rx.capturedTexts()[2];
                value = value.trimmed();
                qDebug() <<"value"<< value;
                sections[sectionName][rx.capturedTexts()[1]] = value;
                continue;
            }
            qDebug() << "sectionName" <<sectionName;
        }

    }



















