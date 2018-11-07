#ifndef MCSCLASS_H
#define MCSCLASS_H

#include <stdint.h>

#include <QObject>
#include <QThread>
#include <QVector>
#include <QTimer>

#include "sendframetransportlclass.h"

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

    typedef enum
    {
        DIRECT = (uint8_t)0,
        FLASH  = (uint8_t)1,
    }fieldTargetT;

    typedef enum
    {
        FLASH_WRITE = (uint8_t)0,
        FLASH_PLAY  = (uint8_t)1,
        FLASH_STOP  = (uint8_t)2,
    }fieldSubTargetT;


    /****************LIST OF COMMAND*********************/
    bool sendFrameCommand(uint16_t frameNumber,  uint16_t frameQuantity, QVector<uint8_t> frame, fieldTargetT target);
    bool sendControlCommand(fieldSubTargetT controllType );

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
        uint8_t target;
        uint8_t subTarget;
        uint16_t frameNumber;
        uint16_t frameQuantity;
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
