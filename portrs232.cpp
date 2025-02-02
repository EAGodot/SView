#include "mainwindow.h"
#include "portrs232.h"
#include "QDebug"

PortRs232::PortRs232(QObject* parent,Configuration * config):PortBase(parent,config){
    isRunning = false;
    serialPort = 0;
}

void PortRs232::requestToStop(){
    isRunning = false;
}
PortRs232::~PortRs232(){

}

void PortRs232::portSetup(){
    qint32 baudRate = config->get("_setup_","baudrate").toInt();
    serialPort->setBaudRate(baudRate);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setStopBits(QSerialPort::OneStop);
    //设置串口缓冲区大小
    serialPort->setReadBufferSize(1*1024*1024);
}

void PortRs232::run(){
    if(isRunning){ return;}
    qDebug() << config->get("_setup_","port");
    serialPort = new QSerialPort(config->get("_setup_","port"));


    connect(serialPort, &QSerialPort::readyRead,this, [=]() {
        static QByteArray buf;
        if (serialPort->bytesAvailable() > 0){
            emit newData(serialPort->readAll());
        }
     });
    if(serialPort->open(QIODevice::ReadWrite)){
        portSetup();// //Note: on Windows settings must be called after serialPort->open, otherwise we get garbage when device is plugged for the fist time
        isRunning = true;
        readyEmitted = false;
        serialPort->flush();
        serialPort->clear(QSerialPort::AllDirections);
        while((isRunning || sendBytes.length() > 0) && serialPort && serialPort->isOpen()){
            //ready
            if(!readyEmitted && isRunning && mainwindow->isPluginReady()){emit ready(portID);readyEmitted = true;}
            //send, data is queed from the GUI thread so we need to lock this section so both threads do not modify sendBytes at same time
            sendMutex.lock();
                if(sendBytes.length()){
                    serialPort->write(sendBytes);
                    qDebug() << "sent data:" << sendBytes.length();
                    sendBytes.clear();
                }
            sendMutex.unlock();
            qApp->processEvents();
        }
    }else{
       qDebug() << "Port Error!:";
       emit message("Could not open port "+config->get("_setup_","port")+"\nMake sure the port is available and not used by other applications.","critical");
    }
    isRunning = false;
    if(serialPort->isOpen()){ serialPort->close();}
    delete serialPort;
    emit stopped();    
}


void PortRs232::send(const QByteArray & data){
    if(isRunning || forceSend){
        //this runs in the GUI thread, so use mutex to lock access to sendBytes
        sendMutex.lock();
            sendBytes.append(data);
        sendMutex.unlock();
    }
}


