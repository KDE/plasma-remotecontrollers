#pragma once

#include "wiimote.h"
#include "../device.h"

#include <QThread>
#include <xwiimote.h>

class WiimoteController : public QThread
{
    Q_OBJECT

public:
    explicit WiimoteController();
    ~WiimoteController() override;
    
    void run() override;

Q_SIGNALS:
    void newDevice(Device *device);
};
