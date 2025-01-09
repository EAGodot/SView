#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include "portrs232.h"
#include <QRegularExpression>

#include "QScrollBar"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    config = new Configuration(this);
    //TODO 增加默认配置文件路径
    //documentFilePath = qApp->applicationFilePath() + "SView.scc";
    documentFilePath = "D:/App/Workspace/Qt/SView/SView.scc";
    qDebug() << documentFilePath;
    loadDocument(documentFilePath);

}

void MainWindow::showEvent(QShowEvent *e)
{
    QMainWindow::showEvent(e);
}
void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
}


void MainWindow::closeEvent(QCloseEvent *e)
{
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionConfiguration_toggled(bool b){
    ui->dockWidgetConfiguration->setVisible(b);
}

void MainWindow::on_configurationText_textChanged(){
}


void MainWindow::on_actionOpen_Config_triggered(){
    //if(!checkDocument()) return;
    QString filePath = QFileDialog::getOpenFileName(this,tr("Open File"),documentFilePath,tr("SerialChart Configuration (*.scc);;All Files (*.*)"));
    qDebug() << filePath;
    if(!filePath.isEmpty()) {
        loadDocument(filePath);
    }
}


bool MainWindow::checkDocument(){
    if(documentIsDirty)
        if(QMessageBox::No == QMessageBox::warning(0,"","Current configuration was changed but not saved.\nAre you sure you want to proceed ?",QMessageBox::Yes,QMessageBox::No|QMessageBox::Default))
            return false;
    return true;
}

bool MainWindow::loadDocument(const QString& filePath){
    bool success = true;
    QFile file(filePath);
    if(file.open(QIODevice::ReadOnly)){
        QTextStream textStream(&file);
        ui->configurationText->setPlainText(textStream.readAll());
        updateDocumentFilePath(filePath);
        success = true;
    }
    if(!success)
        QMessageBox::critical(0,"","Could not save file: " + filePath);
    else
       // ui->statusBar->showMessage("File Saved",2000);

    return success;
}


void MainWindow::updateDocumentFilePath(const QString& filePath){
    documentFilePath = filePath;
    settings.setValue("Last Configuration",filePath);
    this->setWindowTitle("串口示波器" " - " + QFileInfo(filePath).fileName());
    documentIsDirty = false;

}

void MainWindow::on_actionRun_triggered()
{
    config->parse(ui->configurationText->toPlainText());
    QString plugin_url = config->get("_setup_","plugin","about:blank").trimmed();
    QUrl url(plugin_url);
    //port
    port = createPort(this,config); 
    timer = new QTimer(this);

    //设置action行为
    ui->actionRun->setEnabled(false);
    ui->actionStop->setEnabled(true);
    ui->sendButton->setEnabled(true);

    QFile file("example2.txt");
    static char flag = 0;
    if(flag == 0)
    {
        file.open(QIODevice::WriteOnly | QIODevice::Text);
    }
    QTextStream outa(&file);


    // 使用lambda表达式进行类型转换并连接信号和槽
    QObject::connect(dynamic_cast<PortRs232 *>(port), &PortRs232::newData, [this,outa](const QByteArray& data) {
        mutex.lock();
        //if(false == this->text_update){
        if(1){
            this->text_update = true;
            if(ui->checkBoxRHex->isChecked()){
                this->quedata.enqueue(data);

                while(!this->quedata.isEmpty() && 540 > src_data.length()){
                    src_data.append(this->quedata.dequeue());
                }

                if(src_data.length() >= 540){
                    this->strData = this->src_data.toHex(' ')+' ';


                    outa << this->strData;
                    this->src_data.clear();


                    this->src_data.clear();
                    //this->ui->dataText->textCursor().movePosition(QTextCursor::End);
                    this->ui->dataText->insertPlainText(this->strData);
                    this->strData.clear();
                    // 获取垂直滚动条并滚动到最底部
                    QScrollBar *scrollBar = this->ui->dataText->verticalScrollBar();
                    scrollBar->setValue(scrollBar->maximum());
                }


                //this->src_data.append(data);
//                if(1000 <  this->quedata.length())
//                {
//                    //this->ui->dataText->textCursor().movePosition(QTextCursor::End);
//                    this->ui->dataText->insertPlainText(this->quedata.toHex(' '));
//                    this->src_data.clear();
// //                    QFile file("example1.txt");
// //                    static char flag = 0;
// //                    file.open(QIODevice::WriteOnly | QIODevice::Text);
// //                    QTextStream outa(&file);
// //                    outa <<  this->src_data.toHex(' ');
// //                    _sleep(5);
// //                    this->src_data.clear();
// //                    file.close();
//                    //while(1);
//                }


                //\\G表示匹配上一次匹配的结束位置，..表示匹配两个字符
                //然后使用replace函数，将匹配到的每两个字符替换为这两个字符加上一个空格（\\0在替换表达式中表示整个匹配的文本）
                //由于最后会多一个空格，所以使用chop函数去掉最后多余的空格
                //QRegularExpression re("\\G..");
                //newString = this->strData.replace(re,"\\0 ");
                //newString.chop(1);  // 去掉最后多余的空格

            }else {
                this->strData +=  QString::fromLatin1(data);
            }

        }
        mutex.unlock();
    });
    connect(timer, &QTimer::timeout, [this](){

        //mutex.lock();
        if(true == this->text_update){
            this->text_update = false;
            while(!this->quedata.isEmpty()&& 540 > this->src_data.length){
                src_data.append(this->quedata.dequeue());
            }
            this->strData = this->src_data.toHex(' ')+' ';
            QFile file("example2.txt");
            static char flag = 0;
            if(flag == 0)
            {
            file.open(QIODevice::WriteOnly | QIODevice::Text);
            }
            QTextStream outa(&file);
            outa << this->strData;
            this->src_data.clear();

            this->ui->dataText->textCursor().movePosition(QTextCursor::End);
            this->ui->dataText->insertPlainText(this->strData);
            this->strData.clear();

            // 获取垂直滚动条并滚动到最底部
            QScrollBar *scrollBar = this->ui->dataText->verticalScrollBar();
            scrollBar->setValue(scrollBar->maximum());
        }
        //mutex.unlock();
    });
    //timer->start(1000);

    //Startup send
    QString send_run = config->get("_setup_","send_run","").trimmed();
    if("" != send_run){
        port->forceSend = true; //append to send buffer even if port is not running
        sendString(send_run);
        port->forceSend = false;
    }
    //开启串口
    port->start();
}




bool MainWindow::sendString(QString str){
    if(portValid){
        if(ui->checkBoxCR->isChecked()) str.append('\r');
        if(ui->checkBoxLF->isChecked()) str.append('\n');
         QByteArray data;
         //hex 格式
         if(ui->checkBoxSHex->isChecked()){
            QString hexString;
            hexString += "\\x";
            for (int i = 0; i < str.size(); ++i) {
                if(' '==str.at(i)){
                    hexString += "\\x";
                }else{
                    hexString += str.at(i);
                }
            }
            data = stringLiteralUnescape(hexString.toLatin1());
        }
        else{

            data = stringLiteralUnescape(str.toLatin1());
        }
        if(ui->checkBoxEcho->isChecked()) ui->dataText->appendPlainText(data);
        port->send(data);
    }
    return portValid;
}


void MainWindow::on_sendButton_clicked()
{
    sendString( ui->sendText->text());
}


void MainWindow::on_actionStop_triggered(){

    QString send_stop = config->get("_setup_","send_stop","").trimmed();
    if("" != send_stop){
        port->forceSend = true; //append to send buffer even if port is not running
        sendString(send_stop);
        port->forceSend = false;
    }
    //set action
    ui->actionStop->setEnabled(false);
    ui->sendButton->setEnabled(false);
    ui->actionRun->setEnabled(true);
    ui->sendButton->setEnabled(true);

    this->pluginReady = false;  //will be set by mainFrame_loadFinished
    this->pluginLoading = false;

    port->requestToStop();

}

