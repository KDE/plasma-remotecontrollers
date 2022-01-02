#ifndef WIIMOTECONTROLLER_H
#define WIIMOTECONTROLLER_H

#include "../uinput.h"
#include "../controller.h"
#include "wiimote.h"

#include <QThread>
#include <xwiimote.h>

class WiimoteController : public Controller
{
public:
    explicit WiimoteController(Uinput* uinput);
    ~WiimoteController() override;
    
    void run() override;

private:
    Uinput* m_uinput;
};

#endif // WIIMOTECONTROLLER_H
