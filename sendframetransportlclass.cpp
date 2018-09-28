#include "stdint.h"
#include "string.h"

#include <QVector>
#include <QDebug>

#include "sendframetransportlclass.h"


sendFrameTransportlClass::sendFrameTransportlClass(hidInterface *comInterfaceIn)
{
    comInterface = comInterfaceIn;
}


uint32_t sendFrameTransportlClass::fillPacket(uint8_t *buffer, uint8_t *data, uint16_t quantityPacket, uint16_t packetNumber, uint16_t rest)
{
    uint16_t sendDataLength =  (packetNumber == quantityPacket + 1) ?
                               (rest) :
                               (payloadMaxPacketSize);
     memcpy(buffer, data + (packetNumber - 1) * payloadMaxPacketSize, sendDataLength);
     return sendDataLength;
}

void sendFrameTransportlClass::process(QVector<uint8_t> command)
{
    uint32_t sizeTx;
    uint32_t rezNumTx;
    uint32_t rezNumRx;
    QVector<uint8_t> txBuffer(PACKET_SIZE);
    union
    {
        transportPaketT *packet;
        uint8_t         *buffer;
    }reqCommand =
    {
        .buffer = txBuffer.begin()
    };
    reqCommand.packet->packetNumber = 0;
    reqCommand.packet->quantityPacket = static_cast<uint16_t>(command.size() / payloadMaxPacketSize);
    reqCommand.packet->rest = static_cast<uint16_t>(command.size()) - reqCommand.packet->quantityPacket * payloadMaxPacketSize;
    sizeTx = fillPacket(reqCommand.packet->payload, (uint8_t*)command.begin(), reqCommand.packet->quantityPacket, reqCommand.packet->packetNumber, reqCommand.packet->rest);

    qDebug()<<"---start send data----";
    // send command request
    while(true)
    {   
        rezNumTx = comInterface->write(static_cast<uint8_t*>(command.begin()), sizeTx, TRANSACTION_TIMEOUT);
        if( rezNumTx != sizeTx)
        {
            qDebug()<<"Packet Number = "<< reqCommand.packet->packetNumber<<"Num Tx ="<<rezNumTx;
            return;           
        }
        /*
        rezNumRx = comInterface->read(static_cast<uint8_t*>(command.begin() + txCnt),
                                        sizeTx = (((command.size() - txCnt ) >= PACKET_SIZE) ? (PACKET_SIZE) : ( command.size() - txCnt)),
                                    TRANSACTION_TIMEOUT);
        */
        reqCommand.packet->packetNumber++;
        if(reqCommand.packet->packetNumber >= reqCommand.packet->quantityPacket)
        {
            break;
        }
        sizeTx = fillPacket(reqCommand.packet->payload, (uint8_t*)command.begin(), reqCommand.packet->quantityPacket, reqCommand.packet->packetNumber, reqCommand.packet->rest);
    };
    qDebug()<<"---rxCompleted----";
    // inform apper level about receiving command
    //emit signalReceiveCommand(true, rxCommandBuffer);
}

