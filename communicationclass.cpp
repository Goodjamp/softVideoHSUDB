#include "QVector"

#include "communicationclass.h"
#include "mcstransportclass.h"

communicationClass::communicationClass(hidInterface *userInterfaceIn)
{
    userInterface = userInterfaceIn;

    //start();
}


void communicationClass::run()
{
    mcsTransport = new mcsTransportClass(userInterface);

    connect(this, &communicationClass::signalSend, mcsTransport, &mcsTransportClass::process );
    //retransmit signal
    connect(mcsTransport, &mcsTransportClass::signalReceiveCommand, this, &communicationClass::signalReceive );

    exec();
}


void communicationClass::slotSendCommand(QVector<uint8_t> command)
{
    emit signalSend(command);
}
