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
    timerRxResp->setInterval(200);
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


bool sendFrameProtocolClass::sendFrameCommand(QVector<uint8_t> frame, uint32_t frameSize)
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
        .buffer = videoFrame.begin()
    };

    reqCommand.packet->size = frameSize;
    memcpy(reqCommand.packet->commandMarker, START_IND_SUMBOL, strlen(START_IND_SUMBOL));
    memcpy(reqCommand.packet->payload, (uint8_t*)frame.begin(), frameSize);

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






