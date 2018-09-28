#ifndef MCSCLASS_H
#define MCSCLASS_H

#include <stdint.h>

#include <QObject>
#include <QThread>
#include <QVector>
#include <QTimer>

#include "sendframetransportlclass.h"
#define START_IND_SUMBOL  "START"

class sendFrameProtocolClass: public QObject
{
    Q_OBJECT
public:
    sendFrameProtocolClass();

public:
    // rezults of processing command
    typedef enum
    {
        STATUS_RES_OK,
        STATUS_RES_COMMUNICATION_ERROR,
    }STATUS_RES;

    /****************LIST OF COMMAND*********************/
    bool sendFrameCommand(QVector<uint8_t> frame, uint32_t frameSize);

signals:
    //signal to next level
    void signalSendCommand(QVector<uint8_t> command);

    // output to upper level with status of command
    void signalWriteRegisterResp(STATUS_RES status);
    void signalReadRegisterResp(STATUS_RES status, QVector<uint8_t> readReg);

private:

    typedef enum{
        SEND_FRAME  = (uint8_t)0,
    }COMMAND_LIST;


    #pragma pack(push,1)
    typedef struct
    {
        uint8_t commandMarker[sizeof(START_IND_SUMBOL) - 1];
        uint32_t size;
        uint8_t payload[];
    } sendFrameCommandT;
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
