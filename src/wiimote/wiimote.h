#pragma once

#include "../device.h"

#include <QObject>

#include <xwiimote.h>
#include <poll.h>

class Wiimote : public Device
{
    Q_OBJECT

public:
    explicit Wiimote(char *sysPath);
    ~Wiimote();

public slots:
    void watchEvents() override;

Q_SIGNALS:
    void keyPress(int keyCode, bool pressed);

private:
    char *m_sysPath;
    struct xwii_iface *m_iface;
    struct pollfd m_fds[2];
    int m_fdsNum;
    char *m_extensionName;
    int m_previousNunchukAxisTime = 0;

    void handleWatch();
    void handleKeypress(struct xwii_event *event);
    void handleNunchuk(struct xwii_event *event);
};
