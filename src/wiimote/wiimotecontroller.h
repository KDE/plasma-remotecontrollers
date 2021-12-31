#ifndef WIIMOTECONTROLLER_H
#define WIIMOTECONTROLLER_H

#include <QDebug>
#include <QThread>
#include <xwiimote.h>
#include "wiimote.h"

class WiimoteController : public QThread {  
public:
    explicit WiimoteController(int fd);
    ~WiimoteController() override;
    
    void run() override;
private:
    int m_fd;
};

#endif // WIIMOTECONTROLLER_H
