#include "stdint.h"
#include "string.h"

#include <QVector>
#include <QDebug>

#include "mcstransportclass.h"

mcsTransportClass::mcsTransportClass(hidInterface *comInterfaceIn)
{
    comInterface = comInterfaceIn;
}


void mcsTransportClass::process(QVector<uint8_t> command)
{

    #define PACKET_SIZE              512
    #define TRANSACTION_TIMEOUT      1000

    int32_t txCnt = 0;
    int32_t sizeTx;
    int32_t rezNumTx;
    int32_t rezNumRx;
    qDebug()<<"---start send data----";
    // send command request
    do
    {   
        // send request

        rezNumTx = comInterface->write(static_cast<uint8_t*>(command.begin() + txCnt),
                                        sizeTx = (((command.size() - txCnt ) >= PACKET_SIZE) ? (PACKET_SIZE) : ( command.size() - txCnt)),
                                        TRANSACTION_TIMEOUT);
        if( rezNumTx == 0)
        {
            qDebug()<<"---tx error----";
            return;           
        }
        rezNumRx = comInterface->read(static_cast<uint8_t*>(command.begin() + txCnt),
                                        sizeTx = (((command.size() - txCnt ) >= PACKET_SIZE) ? (PACKET_SIZE) : ( command.size() - txCnt)),
                                        TRANSACTION_TIMEOUT);
        txCnt += sizeTx;
    }while( txCnt < command.size() );


    qDebug()<<"---rxCompleted----";
    // inform apper level about receiving ommand
    //emit signalReceiveCommand(true, rxCommandBuffer);
}

