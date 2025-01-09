#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCoreApplication>
#include <QSettings>
#include <QDebug>
#include "configuration.h"
#include "portbase.h"
#include <QMutex>
#include "QTimer"
#include <QFile>
#include <QTextStream>
#include "QQueue"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool isPluginReady(){return pluginReady;};

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *e);
    void showEvent(QShowEvent *e);


private:
    Ui::MainWindow *ui;
    QTimer *timer;
    QTextStream out;
    bool text_update = false;
    QSettings settings;
    QString documentFilePath;

    QString strData;
    QByteArray src_data;
    QQueue<QByteArray> quedata;


    void updateDocumentFilePath(const QString& filePath);
    bool saveDocument(const QString& filePath);
    bool loadDocument(const QString& filePath);
    bool checkDocument();
    bool documentIsDirty;
    Configuration* config;

    PortBase* port;
    QMutex mutex;
    bool portValid;
    bool sendString(QString str);

    bool pluginReady = false;
    bool pluginLoading = false;
private slots:
    void on_actionConfiguration_toggled(bool );
    void on_configurationText_textChanged();
    void on_actionOpen_Config_triggered();
    void on_actionRun_triggered();
    void on_actionStop_triggered();


    void on_sendButton_clicked();
};
#endif // MAINWINDOW_H
