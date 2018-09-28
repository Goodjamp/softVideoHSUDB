#ifndef COMMUNICATIONCLASS_H
#define COMMUNICATIONCLASS_H

#include <QObject>
#include <QThread>
#include <QVector>

#include "sendframetransportlclass.h"
#include "hidInterface.h"


class communicationClass: public QThread
{
    Q_OBJECT
public:
    communicationClass(hidInterface *userInterfaceIn);

signals:
    void signalReceive(bool status, QVector<uint8_t> command);

public slots:
    void slotSendCommand(QVector<uint8_t> command);

private:

    void run();
    hidInterface             *userInterface;
    sendFrameTransportlClass *mcsTransport;

signals:
    void signalSend(QVector<uint8_t> command);


};

Q_DECLARE_METATYPE(QVector<uint8_t>)

#endif // COMMUNICATIONCLASS_H
