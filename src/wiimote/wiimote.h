#ifndef WIIMOTE_H
#define WIIMOTE_H

#include <xwiimote.h>
#include <QObject>

class Wiimote : public QObject
{
    Q_OBJECT

public:
    explicit Wiimote(int fd, struct xwii_iface* iface);
    ~Wiimote() override;


private:
    static int m_fd;
    int m_previousNunchukAxisTime = 0;
    static QHash<int, int> m_keyCodeTranslation;

    void emitKey(int key, int pressed);
    void emitEvent(int type, int code, int val);
    void handleKeypress(struct xwii_event *event);
    void handleNunchuk(struct xwii_event *event);
};

#endif // WIIMOTE_H
