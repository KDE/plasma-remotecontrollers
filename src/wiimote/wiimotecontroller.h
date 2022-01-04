#ifndef WIIMOTECONTROLLER_H
#define WIIMOTECONTROLLER_H

#include "../controller.h"
#include "wiimote.h"

#include <QThread>
#include <xwiimote.h>

class WiimoteController : public Controller
{
public:
    explicit WiimoteController();
    ~WiimoteController() override;
    
    void run() override;
};

#endif // WIIMOTECONTROLLER_H
