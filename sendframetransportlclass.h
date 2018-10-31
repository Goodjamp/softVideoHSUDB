#ifndef MCSTRANSPORTCLASS_H
#define MCSTRANSPORTCLASS_H


#include <stdint.h>

#include <QObject>
#include <QVector>

#include "hidInterface.h"

#define PACKET_SIZE              1024U
#define TRANSACTION_TIMEOUT      2

class sendFrameTransportlClass: public QObject
{
    Q_OBJECT
public:
    sendFrameTransportlClass(hidInterface *comInterfaceIn);

signals:
    void signalReceiveCommand(bool status, QVector<uint8_t> command);

public slots:
    void process(QVector<uint8_t> command);

private:
    inline uint32_t fillPacket(uint8_t *buffer, uint8_t *data, uint16_t quantityPacket, uint16_t packetNumber, uint16_t rest);
private:
#pragma pack(push,1)
    typedef struct
    {
        uint16_t quantityPacket;
        uint16_t packetNumber;
        uint16_t rest;
        uint8_t  payload[];
    }transportPaketT;
#pragma pack(pop)
    hidInterface *comInterface;
    const uint16_t payloadMaxPacketSize = static_cast<uint16_t>(PACKET_SIZE) - static_cast<uint16_t>(sizeof(transportPaketT));
    const uint16_t transportHeadSize = static_cast<uint16_t>(sizeof(transportPaketT));
};

#endif // MCSTRANSPORTCLASS_H
