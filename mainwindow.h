#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringListModel>
#include <QSemaphore>

#include "hidinterface.h"
#include "sendframeprotocolclass.h"
#include "communicationclass.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButtonOpenDevice_clicked();

    void on_pushButtonCloseDevice_clicked();

    void on_pushButtonPlay_clicked();

    void on_pushButtonStop_clicked();


    // MCS processing clots description

    void slotReadRegisters(sendFrameProtocolClass::STATUS_RES statusResp, QVector<uint8_t> registers);

    void slotWriteRegisters(sendFrameProtocolClass::STATUS_RES statusResp);

private:
    void messageErrorWindowShow(QString errorString);

    void setDeviseOpenUIState(void);

    void setDeviseCloseUIState(void);

    void sendFrame();

private:

    Ui::MainWindow     *ui;

    hidInterface       *userHID;

    sendFrameProtocolClass   *mcsProtocol;

    communicationClass *mcsTransport;

    //address of first register in READ_REGISTER command
    int  readRegAddress;
};

#endif // MAINWINDOW_H
