#ifndef WIIMOTECONTROLLER_H
#define WIIMOTECONTROLLER_H

#include <QThread>
#include <xwiimote.h>
#include "wiimote.h"
#include "../controller.h"

class WiimoteController : public Controller
{
public:
    explicit WiimoteController(int fd);
    ~WiimoteController() override;
    
    void run() override;

private:
    int m_fd;
};

#endif // WIIMOTECONTROLLER_H
