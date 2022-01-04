#ifndef WIIMOTE_H
#define WIIMOTE_H

#include <xwiimote.h>
#include <QObject>

class Wiimote : public QObject
{
    Q_OBJECT

public:
    explicit Wiimote(struct xwii_iface* iface);
    ~Wiimote() override;

signals:
    void keyPress(int keyCode, bool pressed);

private:
    int m_previousNunchukAxisTime = 0;
    static QHash<int, int> m_keyCodeTranslation;

    void handleKeypress(struct xwii_event *event);
    void handleNunchuk(struct xwii_event *event);
};

#endif // WIIMOTE_H
