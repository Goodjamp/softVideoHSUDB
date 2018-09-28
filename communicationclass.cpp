#include "QVector"

#include "communicationclass.h"
#include "sendframetransportlclass.h"

communicationClass::communicationClass(hidInterface *userInterfaceIn)
{
    userInterface = userInterfaceIn;
}


void communicationClass::run()
{
    mcsTransport = new sendFrameTransportlClass(userInterface);

    connect(this, &communicationClass::signalSend, mcsTransport, &sendFrameTransportlClass::process );
    //retransmit signal
    connect(mcsTransport, &sendFrameTransportlClass::signalReceiveCommand, this, &communicationClass::signalReceive );

    exec();
}


void communicationClass::slotSendCommand(QVector<uint8_t> command)
{
    emit signalSend(command);
}
