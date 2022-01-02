#ifndef UINPUT_H
#define UINPUT_H

#include <QObject>

class Uinput : public QObject
{
    Q_OBJECT

public:
    explicit Uinput();
    ~Uinput() override;

public slots:
    void emitKey(int key, bool pressed);

private:
    static int m_fd;

    void emitEvent(int type, int code, int val);
};

#endif // UINPUT_H
