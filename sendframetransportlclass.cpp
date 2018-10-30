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
    uint16_t sendDataLength =  ((packetNumber + 1) == quantityPacket) ?
                               (rest) :
                               (payloadMaxPacketSize);
     memcpy(buffer, data + packetNumber * payloadMaxPacketSize, sendDataLength);
     return sendDataLength + transportHeadSize;
}


void sendFrameTransportlClass::process(QVector<uint8_t> command)
{
    uint32_t sizeTx;
    uint32_t rezNumTx;

    static QVector<uint8_t> txBuffer(PACKET_SIZE);
    union
    {
        transportPaketT *packet;
        uint8_t         *buffer;
    }reqCommand =
    {
        .buffer = txBuffer.begin()
    };
    reqCommand.packet->packetNumber   = 0;
    reqCommand.packet->rest           = static_cast<uint16_t>(command.size() % payloadMaxPacketSize);
    reqCommand.packet->quantityPacket = (reqCommand.packet->rest == 0) ? (static_cast<uint16_t>(command.size() / payloadMaxPacketSize)):
                                                                         (static_cast<uint16_t>(command.size() / payloadMaxPacketSize) + 1);
    sizeTx = fillPacket(reqCommand.packet->payload,
                        (uint8_t*)command.data(),
                        reqCommand.packet->quantityPacket,
                        reqCommand.packet->packetNumber,
                        reqCommand.packet->rest);

    qDebug()<<"---start send data----";
    // send command request
    while(true)
    {   
        rezNumTx = comInterface->write(reqCommand.buffer, PACKET_SIZE, TRANSACTION_TIMEOUT);
        //qDebug()<<"Packet Number = "<< reqCommand.packet->packetNumber<<"Num Tx ="<<rezNumTx;
        if( rezNumTx < sizeTx)
        {
           //qDebug()<<"Packet Number = "<< reqCommand.packet->packetNumber<<"Num Tx ="<<rezNumTx;
            return;           
        }        
        /*
        rezNumRx = comInterface->read(static_cast<uint8_t*>(command.begin() + txCnt),
                                        sizeTx = (((command.size() - txCnt ) >= PACKET_SIZE) ? (PACKET_SIZE) : ( command.size() - txCnt)),
                                    TRANSACTION_TIMEOUT);
        */
        reqCommand.packet->packetNumber++;
        if(reqCommand.packet->packetNumber  >= reqCommand.packet->quantityPacket)
        {
            break;
        }
        sizeTx = fillPacket(reqCommand.packet->payload,
                            (uint8_t*)command.data(),
                            reqCommand.packet->quantityPacket,
                            reqCommand.packet->packetNumber,
                            reqCommand.packet->rest);
    };
    qDebug()<<"---rxCompleted----";
    // inform apper level about receiving command
    //emit signalReceiveCommand(true, rxCommandBuffer);
}



/*



    QString framePath_ = "C:\\Private\\Programing\\QT\\softVideoHSUDB\\build-softVideoHSUDB-Desktop_Qt_5_8_0_MinGW_32bit2-Debug\\data\\rawFrame565_0_.bin";
    qDebug()<<"Current frame path:"<< framePath_;
    FILE *frameFile_ = fopen(framePath_.toUtf8(),"wb");
    for(uint16_t i = 0; i < 128; i++)
    {
        for(uint16_t j = 0; j < 128; j++)
        {
            uint16_t buffCollor = 0;
            if((i == 3*j) || (i == 127 - j))
            {
                buffCollor = 0xffff;
            }
            if((j == 32) || (j == 64)|| (j == 125))
            {
                buffCollor = 0xffff;
            }
            if((i == 32) || (i == 64)|| (i == 125))
            {
                buffCollor = 0xffff;
            }
            fwrite(&buffCollor, 2, 1, frameFile_);
        }
    }
    fclose(frameFile_);


    FILE *frameFile = fopen(framePath_.toUtf8(),"rb");
   // QVector<uint8_t> videoFrame(32768);
    fread((uint8_t*)command.data(), sizeof(uint8_t), 32768, frameFile);
    fclose(frameFile);

    QString framePath = "C:\\Private\\Programing\\QT\\softVideoHSUDB\\build-softVideoHSUDB-Desktop_Qt_5_8_0_MinGW_32bit2-Debug\\data\\rawFrame565_0.bin";
    frameFile = fopen(framePath.toUtf8(),"rb");

    static uint8_t data_[32768];

    fread(data_, sizeof(uint8_t), 32768, frameFile);
    fclose(frameFile);

*/
