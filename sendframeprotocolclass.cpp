#include <stdint.h>

#include <QObject>
#include <QTimer>
#include <QVector>
#include <QDebug>
#include <QString>

#include "sendframeprotocolclass.h"
#include "sendframetransportlclass.h"

sendFrameProtocolClass::sendFrameProtocolClass()
{
    timerRxResp = new QTimer(this);
    timerRxResp->setInterval(20);
    connect(timerRxResp, &QTimer::timeout, this, &sendFrameProtocolClass::slotCommandRxRespTimeout);
}


void sendFrameProtocolClass::slotCommandRxRespTimeout()
{
    timerRxResp->stop();
    switch( currentProcessCommand )
    {
        case SEND_FRAME:
            emit signalWriteRegisterResp(STATUS_RES_COMMUNICATION_ERROR);
            break;
        default:
            break;
    }
}


void sendFrameProtocolClass::slotCommandRxResp(bool status, QVector<uint8_t> response)
{
    timerRxResp->stop();
    switch( currentProcessCommand )
    {
        case SEND_FRAME:
            emit signalWriteRegisterResp(STATUS_RES_OK);
            break;
        default:
            break;
    }

}

bool sendFrameProtocolClass::sendControlCommand(fieldSubTargetT controllType )
{

}


bool sendFrameProtocolClass::sendFrameCommand(uint16_t frameNumber,  uint16_t frameQuantity, QVector<uint8_t> frame, fieldTargetT target)
{
    if(timerRxResp->isActive())
    {
        return false;
    }
    QVector<uint8_t> videoFrame(frame.size() + sizeof(sendFrameCommandT));
    union
    {
        sendFrameCommandT *packet;
        uint8_t           *buffer;
    }reqCommand =
    {
        .buffer = static_cast<uint8_t*>(videoFrame.begin())
    };
    reqCommand.packet->target        = target;
    reqCommand.packet->subTarget     = sendFrameProtocolClass::FLASH_WRITE;
    reqCommand.packet->frameNumber   = frameNumber;
    reqCommand.packet->frameQuantity = frameQuantity;
    reqCommand.packet->size          = frame.size();
    memcpy(reqCommand.packet->payload, (uint8_t*)frame.begin(), frame.size());

    currentProcessCommand = SEND_FRAME;

    sendToTransport(videoFrame);

    return true;
}


void sendFrameProtocolClass::sendToTransport(QVector<uint8_t> command)
{
    emit signalSendCommand(command);
    // start timer for timeout receive responce
    timerRxResp->start();
}






