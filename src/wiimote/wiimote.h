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
    void run() override;

signals:
    void keyPress(int keyCode, bool pressed);

private:
    char* m_sysPath;
    struct xwii_iface* m_iface;
    struct pollfd m_fds[2];
    int m_fdsNum;
    int m_previousNunchukAxisTime = 0;
    static QHash<int, int> m_keyCodeTranslation;

    void handleKeypress(struct xwii_event *event);
    int handleWatch();
    void handleNunchuk(struct xwii_event *event);
};
