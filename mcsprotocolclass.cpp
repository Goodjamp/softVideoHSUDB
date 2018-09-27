#include <stdint.h>

#include <QObject>
#include <QTimer>
#include <QVector>
#include <QDebug>

#include "mcsprotocolclass.h"
#include "mcstransportclass.h"

mcsProtocolClass::mcsProtocolClass()
{
    timerRxResp = new QTimer(this);
    timerRxResp->setInterval(3000);
    connect(timerRxResp, &QTimer::timeout, this, &mcsProtocolClass::slotCommandRxRespTimeout);
}


void mcsProtocolClass::slotCommandRxRespTimeout()
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


void mcsProtocolClass::slotCommandRxResp(bool status, QVector<uint8_t> response)
{

}


bool mcsProtocolClass::writeRegister(uint16_t i2cDeviceAddressIn, uint16_t registerAddresIn, uint16_t numberOfRegisterIn, QVector<uint8_t> listOfRegisters )
{

    if(timerRxResp->isActive())
    {
        return false;
    }  
    QVector<uint8_t> mcsReq(sizeof(reqWriteRegistersCommandT) + listOfRegisters.size());
    union
    {
        reqReadRegistersCommandT *request;
        uint8_t                  *buff;
    }reqCommand;

    reqCommand.buff = mcsReq.begin();
    reqCommand.request->commandId  = WRITE_REGISTER;
    reqCommand.request->i2cAddress = i2cDeviceAddressIn;
    reqCommand.request->numReg     = numberOfRegisterIn;
    reqCommand.request->addressReg = registerAddresIn;

    currentProcessCommand = WRITE_REGISTER;
    sendToTransport(mcsReq);

    return true;
}


bool mcsProtocolClass::sendFrame(QVector<uint8_t> frame)
{

    if(timerRxResp->isActive())
    {
        return false;
    }
    QVector<uint8_t> mcsReq(sizeof(reqReadRegistersCommandT));
    union
    {
        reqReadRegistersCommandT *request;
        uint8_t                  *buff;
    }reqCommand;

    reqCommand.buff = mcsReq.begin();
    reqCommand.request->commandId  = READ_REGISTER;
    reqCommand.request->i2cAddress = i2cDeviceAddressIn;
    reqCommand.request->numReg     = numberOfRegisterIn;
    reqCommand.request->addressReg = registerAddresIn;

    currentProcessCommand = READ_REGISTER;

    sendToTransport(mcsReq);

    return true;
}


void mcsProtocolClass::sendToTransport(QVector<uint8_t> command)
{
    emit signalSendCommand(command);
    // start timer for timeout receive responce
    timerRxResp->start();
}






