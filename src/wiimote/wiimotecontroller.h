#pragma once

#include "wiimote.h"

#include <QThread>
#include <xwiimote.h>

class WiimoteController : public QThread
{
public:
    explicit WiimoteController();
    ~WiimoteController() override;
    
    void run() override;
};
