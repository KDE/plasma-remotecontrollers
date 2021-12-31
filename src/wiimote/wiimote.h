#ifndef WIIMOTE_H
#define WIIMOTE_H

#include <xwiimote.h>
#include <QObject>

class wiimote : public QObject
{
    Q_OBJECT

public:
    explicit wiimote(int fd, struct xwii_iface* wiimote);
    ~wiimote() override;


private:
    int m_fd;
    int m_previous_nunchuk_axis = 0;
    
    
    void emit_key(int key);
    void emit_event(int type, int code, int val);
    void handle_keypress(struct xwii_event *event);
    void handle_nunchuk(struct xwii_event *event);
};

#endif // WIIMOTE_H
