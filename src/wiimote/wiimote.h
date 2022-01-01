#ifndef WIIMOTE_H
#define WIIMOTE_H

#include <xwiimote.h>
#include <QObject>

class Wiimote : public QObject
{
    Q_OBJECT

public:
    explicit Wiimote(int fd, struct xwii_iface* wiimote);
    ~Wiimote() override;


private:
    static int m_fd;
    int m_previous_nunchuk_axis = 0;
    static QHash<int, int> m_keyCodeTranslation;

    void emit_key(int key, int pressed);
    void emit_event(int type, int code, int val);
    void handle_keypress(struct xwii_event *event);
    void handle_nunchuk(struct xwii_event *event);
};

#endif // WIIMOTE_H
