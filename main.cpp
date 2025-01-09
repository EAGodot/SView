#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextBlock>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    a.setWindowIcon(QIcon(":/images/logo.ico"));
    //报错兼容处理(注册 QTextCharFormat 和 QTextCursor)
    qRegisterMetaType<QTextCharFormat>("QTextCharFormat");
    qRegisterMetaType<QTextCursor>("QTextCursor");
    //使用qRegisterMetaType注册QTextBlock为元类型
    qRegisterMetaType<QTextBlock>("QTextBlock");
    w.show();
    return a.exec();
}
