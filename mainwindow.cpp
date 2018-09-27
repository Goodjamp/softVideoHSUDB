#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDebug"
#include "QString"
#include "QFileDialog"
#include "QRegularExpression"
#include "QMessageBox"

#include "mainParDescriptions.h"

#include "mcsprotocolclass.h"
//#include "mcstransportclass.h"
#include "communicationclass.h"
#include "hidinterface.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    /*************INITILISATION USER INTERFACE**************/

    QStringList lisdDeviceAddress = LIST_OF_DEVICE_ADDRESS;


    //set fixed size of main window
    setFixedSize(WIN_WEIGH, WIN_HEIGHT);

    this->setWindowTitle(WIN_NAME);

    // button activity
    setDeviseCloseUIState();

    /*************INITILISATION COMMUNICATION INTERFACE**************/

    userHID = new hidInterface();

    /*************INITILISATION COMMUNICATION STACK******************/
    mcsProtocol  = new mcsProtocolClass();
    mcsTransport = new communicationClass(userHID);

    connect(mcsProtocol,  &mcsProtocolClass::signalSendCommand,       mcsTransport, &communicationClass::slotSendCommand);
    connect(mcsTransport, &communicationClass::signalReceive,         mcsProtocol,  &mcsProtocolClass::slotCommandRxResp);
    connect(mcsProtocol,  &mcsProtocolClass::signalReadRegisterResp,  this,         &MainWindow::slotReadRegisters);
    connect(mcsProtocol,  &mcsProtocolClass::signalWriteRegisterResp, this,         &MainWindow::slotWriteRegisters);

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
    ui->pushButtonSendCommand->setEnabled(true);
}


void MainWindow::setDeviseCloseUIState(void)
{
    ui->pushButtonOpenDevice->setEnabled(true);
    ui->pushButtonCloseDevice->setDisabled(true);
    ui->pushButtonSendCommand->setDisabled(true);
}


void MainWindow::messageErrorWindowShow(QString errorString)
{
    QMessageBox messageBox;
    messageBox.critical(0,"Error",errorString);
    messageBox.setFixedSize(500,200);
    messageBox.setModal(true);
    messageBox.show();
}




void MainWindow::slotReadRegisters(mcsProtocolClass::STATUS_RES statusResp, QVector<uint8_t> registers)
{

}


void MainWindow::slotWriteRegisters(mcsProtocolClass::STATUS_RES statusResp)
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


/*
uint8_t writeBuff[32 + 1] = {0,21,31,41,51,61};
uint8_t readBuff[32 + 1];
uint32_t numRead = 0;


void MainWindow::on_pushButtonSend_clicked()
{
     userHID->write(writeBuff, 60 + 1, 1);
}


void MainWindow::on_pushButtonReceive_clicked()
{
    numRead = userHID->read(writeBuff, 61, 1);
    qDebug()<<"Reaz Read "<< numRead<<"\n";
    qDebug()<<"data: "<< writeBuff[0]<<" "<< writeBuff[1]<<" "<< writeBuff[2]<<" "<< writeBuff[3]<<" "<<"\n";
}
*/

void MainWindow::on_pushButtonCloseDevice_clicked()
{
    userHID->closeInterface();
    setDeviseCloseUIState();
}


void MainWindow::sendFrame()
{
    // read next frame from video and send to protocol class
}


void MainWindow::on_pushButtonPlay_clicked()
{
    /*
    #define SIZE_RX_BUFFER   (512 * 32)
    QVector<uint8_t> testVetor(SIZE_RX_BUFFER);
    for(uint32_t cnt = 0; cnt < SIZE_RX_BUFFER; cnt++)
    {
    testVetor[cnt] = cnt;
    }
    //mcsProtocol->sendToTransport(testVetor);
*/
    // TODO run timer !!
    sendFrame();
}

void MainWindow::on_pushButtonStop_clicked()
{

}
