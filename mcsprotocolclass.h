#ifndef MCSCLASS_H
#define MCSCLASS_H

#include <stdint.h>

#include <QObject>
#include <QThread>
#include <QVector>
#include <QTimer>

#include "mcstransportclass.h"


class mcsProtocolClass: public QObject
{
    Q_OBJECT
public:
    mcsProtocolClass();

public:
    // rezults of processing command
    typedef enum
    {
        STATUS_RES_OK,
        STATUS_RES_COMMUNICATION_ERROR,
    }STATUS_RES;

    /****************LIST OF COMMAND*********************/
    bool sendFrame(QVector<uint8_t> frame);

signals:
    //signal to next level
    void signalSendCommand(QVector<uint8_t> command);

    // output to upper level with status of command
    void signalWriteRegisterResp(STATUS_RES status);
    void signalReadRegisterResp(STATUS_RES status, QVector<uint8_t> readReg);

private:

    typedef enum{
        SEND_FRAME      = (uint8_t)0,
    }COMMAND_LIST;


    #pragma pack(push,1)
    typedef struct
    {
        uint8_t commandId;
        uint8_t status;
        uint8_t payload[];
    } rspT;
    #pragma pack(pop)

/*****PRIVATE PROPERTYS DEFINITIONS****/
private:

    uint8_t currentProcessCommand;

//    mcsTransportClass *transport;

    QThread *transportThread;

    QTimer *timerRxResp;

/*****PRIVATE METHODS DEFINITIONS****/
private:
    void sendToTransport(QVector<uint8_t> command);


public slots:
    void slotCommandRxRespTimeout();
    void slotCommandRxResp(bool status, QVector<uint8_t> response);


};

#endif // MCSCLASS_H
