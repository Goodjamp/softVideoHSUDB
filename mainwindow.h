#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringListModel>
#include <QSemaphore>
#include <QTimer>

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

private:

    typedef  enum
    {
        PLAYER_STOP,
        PLAYER_PAUSE,
        PLAYER_PLAY,
        PLAYER_FbyF,
        PLAYER_WRITE_FLASH,
    }playerStateT;

    Ui::MainWindow     *ui;

    hidInterface       *userHID;

    sendFrameProtocolClass   *mcsProtocol;

    QTimer *frameTimer;

    communicationClass *mcsTransport;

    struct{
        uint16_t     totalFrameNumber;
        QString      path;
        uint16_t     frameNumber;
        playerStateT state;
    }playerState;

    //address of first register in READ_REGISTER command
    int  readRegAddress;

private slots:
    void on_pushButtonOpenDevice_clicked();

    void on_pushButtonCloseDevice_clicked();

    void on_pushButtonPlay_clicked();

    void on_pushButtonStop_clicked();


    // MCS processing clots description

    void slotReadRegisters(sendFrameProtocolClass::STATUS_RES statusResp, QVector<uint8_t> registers);

    void slotWriteRegisters(sendFrameProtocolClass::STATUS_RES statusResp);

    void on_pushButton_clicked();

    void on_pushButtonFrameByFrame_clicked();

    void on_pushButtonPause_clicked();

    void on_pushButtonWriteFlash_clicked();

    void on_checkBoxWriteFlash_stateChanged(int arg1);

private:
    void messageErrorWindowShow(QString errorString);

    void setDeviseOpenUIState(void);

    void setDeviseCloseUIState(void);

    void setDevisePlayUIState(void);

    void setDeviseStopUIState(void);

    void setDevisePauseUIState(void);

    void sendFrame(sendFrameProtocolClass::fieldTargetT target);

    void sendControll(sendFrameProtocolClass::fieldTargetT target);

    void frameSendTimeout();

    void playerProcessing(playerStateT newState);

    void playerPlay(void);

    void playerStop(void);

    void playerPause(void);

    void playerPlayFrame(void);

    void calcFrameQuantity(QString videoPath);

    void writeFlash(void);

};

#endif // MAINWINDOW_H
