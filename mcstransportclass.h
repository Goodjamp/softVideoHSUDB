#ifndef MCSTRANSPORTCLASS_H
#define MCSTRANSPORTCLASS_H


#include <stdint.h>

#include <QObject>
#include <QVector>

#include "hidInterface.h"

class mcsTransportClass: public QObject
{
    Q_OBJECT
public:
    mcsTransportClass(hidInterface *comInterfaceIn);

signals:
    void signalReceiveCommand(bool status, QVector<uint8_t> command);

public slots:
    void process(QVector<uint8_t> command);

private:
    hidInterface *comInterface;
};

#endif // MCSTRANSPORTCLASS_H
