#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDebug"
#include "QString"
#include "QFileDialog"
#include "QRegularExpression"
#include "QMessageBox"
#include "QTimer"
#include "stdio.h"

#include "mainParDescriptions.h"

#include "sendframeprotocolclass.h"
//#include "mcstransportclass.h"
#include "communicationclass.h"
#include "hidinterface.h"




MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    /*************INITILISATION USER INTERFACE**************/

    //set fixed size of main window
    setFixedSize(WIN_WEIGH, WIN_HEIGHT);

    this->setWindowTitle(WIN_NAME);

    // button activity
    setDeviseCloseUIState();

    /*************INITILISATION COMMUNICATION INTERFACE**************/

    userHID = new hidInterface();

    /*************INITILISATION COMMUNICATION STACK******************/
    mcsProtocol  = new sendFrameProtocolClass();
    mcsTransport = new communicationClass(userHID);

    connect(mcsProtocol,  &sendFrameProtocolClass::signalSendCommand,       mcsTransport, &communicationClass::slotSendCommand);
    connect(mcsTransport, &communicationClass::signalReceive,         mcsProtocol,  &sendFrameProtocolClass::slotCommandRxResp);
    connect(mcsProtocol,  &sendFrameProtocolClass::signalReadRegisterResp,  this,         &MainWindow::slotReadRegisters);
    connect(mcsProtocol,  &sendFrameProtocolClass::signalWriteRegisterResp, this,         &MainWindow::slotWriteRegisters);

    /*************STACK INITILISATION******************************/
    frameTimer = new QTimer(this);
    frameTimer->setInterval(40);
    connect(frameTimer, &QTimer::timeout, this, &MainWindow::frameSendTimeout);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::frameSendTimeout()
{
    frameTimer->stop();
    if(playerState.state != PLAYER_PLAY)
    {
        return;
    }
    sendFrame();
    frameTimer->start();
}


void MainWindow::setDeviseOpenUIState(void)
{
    ui->pushButtonOpenDevice->setEnabled(false);
    ui->pushButtonCloseDevice->setEnabled(true);
    setDeviseStopUIState();
}


void MainWindow::setDeviseCloseUIState(void)
{
    ui->pushButtonOpenDevice->setEnabled(true);
    ui->pushButtonCloseDevice->setEnabled(false);
    ui->pushButtonStop->setEnabled(false);
    ui->pushButtonPause->setEnabled(false);
    ui->pushButtonPlay->setEnabled(false);
    ui->pushButtonFrameByFrame->setEnabled(false);
}


void MainWindow::setDevisePlayUIState(void)
{
    ui->pushButtonStop->setEnabled(true);
    ui->pushButtonPause->setEnabled(true);
    ui->pushButtonPlay->setEnabled(false);
}


void MainWindow::setDeviseStopUIState(void)
{
    ui->pushButtonStop->setEnabled(false);
    ui->pushButtonPause->setEnabled(false);
    ui->pushButtonPlay->setEnabled(true);
}


void MainWindow::setDevisePauseUIState(void)
{
    ui->pushButtonStop->setEnabled(true);
    ui->pushButtonPause->setEnabled(false);
    ui->pushButtonPlay->setEnabled(true);
}



void MainWindow::messageErrorWindowShow(QString errorString)
{
    QMessageBox messageBox;
    messageBox.critical(0,"Error",errorString);
    messageBox.setFixedSize(500,200);
    messageBox.setModal(true);
    messageBox.show();
}




void MainWindow::slotReadRegisters(sendFrameProtocolClass::STATUS_RES statusResp, QVector<uint8_t> registers)
{

}


void MainWindow::slotWriteRegisters(sendFrameProtocolClass::STATUS_RES statusResp)
{

}


void MainWindow::on_pushButtonOpenDevice_clicked()
{
    userHID->initUSB();
    if( !userHID->openInterface(VID_DEVICE, PID_DEVICE))
    {
        setDeviseCloseUIState();
        messageErrorWindowShow(ERROR_OPEN_DEVICE_NOT_FOUND);
        return;
    }
    setDeviseOpenUIState();
    // run transport protocol
    mcsTransport->start();
}


void MainWindow::on_pushButtonCloseDevice_clicked()
{
    userHID->closeInterface();
    setDeviseCloseUIState();
}


void MainWindow::sendFrame()
{
    size_t fileSize = 0;
    // read next frame from video and send to protocol class
    QString framePath = playerState.path + QString::number(playerState.frameNumber) + FRAME_EXTENTION;
    qDebug()<<"Current frame path:"<< framePath;
    FILE *frameFile = fopen(framePath.toUtf8(),"rb");
    if(frameFile == NULL)
    {
        qDebug()<<"Error Or last video frame was reading";
        playerProcessing(PLAYER_STOP);
        return;
    }
    // get size of file
    //fseek(frameFile, 0, SEEK_END);
    //fileSize = ftell(frameFile);
    //fseek(frameFile, 0, SEEK_SET);
    //qDebug()<<"frame open close OK, file size = "<<fileSize;
    static QVector<uint8_t> videoFrame(32768);
    fread((uint8_t*)videoFrame.data(), sizeof(uint8_t), 32768, frameFile);
    fclose(frameFile);
    mcsProtocol->sendFrameCommand(videoFrame, 32768);
    playerState.frameNumber++;
}


void MainWindow::on_pushButton_clicked()
{
    QFileDialog openDialog;
    openDialog.setModal(true);
    QString filePath = openDialog.getOpenFileName();
    QString runPythonStr = "VideoRead.py " + filePath;
    qDebug()<<runPythonStr;
    system(runPythonStr.toLocal8Bit());
}


void MainWindow::on_pushButtonPlay_clicked()
{
    playerProcessing(PLAYER_PLAY);
}


void MainWindow::on_pushButtonStop_clicked()
{
     playerProcessing(PLAYER_STOP);
}


void MainWindow::on_pushButtonPause_clicked()
{
    playerProcessing(PLAYER_PAUSE);
}


void MainWindow::on_pushButtonFrameByFrame_clicked()
{
    playerProcessing(PLAYER_FbyF);
}


void MainWindow::playerPlay(void)
{
    playerState.state = PLAYER_PLAY;
    if(playerState.frameNumber == 0)
    {
        playerState.path = QDir::currentPath() + FRAME_FILE_NAME;
    }
    frameTimer->start();
    sendFrame();
    setDevisePlayUIState();
}

void MainWindow::playerStop(void)
{
    playerState.state = PLAYER_STOP;
    playerState.frameNumber = 0;
    frameTimer->stop();
    setDeviseStopUIState();
}

void MainWindow::playerPause(void)
{
    playerState.state = PLAYER_PAUSE;
    frameTimer->stop();
    setDeviseStopUIState();
}


void MainWindow::playerPlayFrame(void)
{
    playerState.state = PLAYER_FbyF;
    if(playerState.frameNumber == 0)
    {
        playerState.path = QDir::currentPath() + FRAME_FILE_NAME;
    }
    frameTimer->stop();
    sendFrame();
    setDevisePlayUIState();
}


void MainWindow::playerProcessing(playerStateT newState)
{
     switch(playerState.state)
     {
     case PLAYER_STOP:
         switch(newState)
         {
         case PLAYER_STOP: break;
         case PLAYER_PAUSE: break;
         case PLAYER_PLAY:
             playerPlay();
             break;
         case PLAYER_FbyF:
             playerPlayFrame();
             break;
         }

         break;
     case PLAYER_PAUSE:
         switch(newState)
         {
         case PLAYER_STOP:
             playerStop();
             break;
         case PLAYER_PAUSE: break;
         case PLAYER_PLAY:
             playerPlay();
             break;
         case PLAYER_FbyF:
             playerPlayFrame();
             break;
         }
         break;
     case PLAYER_PLAY:
         switch(newState)
         {
         case PLAYER_STOP:
             playerStop();
             break;
         case PLAYER_PAUSE:
             playerPause();
             break;
         case PLAYER_PLAY: break;
         case PLAYER_FbyF:
             playerPause();
             break;
         }
         break;
     case PLAYER_FbyF:
         switch(newState)
         {
         case PLAYER_STOP: break;
             playerStop();
         case PLAYER_PAUSE: break;
         case PLAYER_PLAY:
             playerPlay();
             break;
         case PLAYER_FbyF:
             playerPlayFrame();
             break;
         }
         break;
     }
}

