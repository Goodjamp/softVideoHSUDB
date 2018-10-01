#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDebug"
#include "QString"
#include "QFileDialog"
#include "QRegularExpression"
#include "QMessageBox"
#include "stdio.h"

#include "mainParDescriptions.h"

#include "sendframeprotocolclass.h"
//#include "mcstransportclass.h"
#include "communicationclass.h"
#include "hidinterface.h"

/*
system("VideoRead.py C:\\");
*/


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
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::setDeviseOpenUIState(void)
{
    ui->pushButtonOpenDevice->setDisabled(true);
    ui->pushButtonCloseDevice->setEnabled(true);
    //ui->pushButtonStop->setEnabled(true);
    //ui->pushButtonPlay->setEnabled(true);
}


void MainWindow::setDeviseCloseUIState(void)
{
    ui->pushButtonOpenDevice->setEnabled(true);
    ui->pushButtonCloseDevice->setDisabled(true);
    //ui->pushButtonStop->setDisabled(true);
    //ui->pushButtonPlay->setDisabled(true);
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
    QString framePath = playState.path + QString::number(playState.orderNumber) + FRAME_EXTENTION;
    qDebug()<<"Current frame path:"<< framePath;
    FILE *frameFile = fopen(framePath.toUtf8(),"rb");
    if(frameFile == NULL)
    {
        qDebug()<<"Error Or last video frame was reading";
        ui->pushButtonPlay->setEnabled(true);
        ui->pushButtonStop->setEnabled(false);
        return;
    }
    // get size of file
    fseek(frameFile, 0, SEEK_END);
    fileSize = ftell(frameFile);
    fseek(frameFile, 0, SEEK_SET);
    qDebug()<<"frame open close OK, file size = "<<fileSize;
    QVector<uint8_t> videoFrame(fileSize);
    fread((uint8_t*)videoFrame.begin(), sizeof(uint8_t), fileSize, frameFile);
    fclose(frameFile);
    mcsProtocol->sendFrameCommand(videoFrame, fileSize);
}


void MainWindow::on_pushButtonPlay_clicked()
{
    playState.orderNumber = 0;
    // TODO run timer !!

    playState.path = QDir::currentPath() + FRAME_FILE_NAME;
    qDebug()<<playState.path;
    sendFrame();
}


void MainWindow::on_pushButtonStop_clicked()
{

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
