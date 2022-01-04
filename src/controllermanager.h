#ifndef CONTROLLERMANAGER_H
#define CONTROLLERMANAGER_H

#include <QObject>

class ControllerManager : public QObject
{
    Q_OBJECT

public:
    explicit ControllerManager(QObject *parent = nullptr);
    virtual ~ControllerManager();
    static ControllerManager &instance();

public slots:
    void emitKey(int key, bool pressed);

private:
    static int m_fd;

    void emitEvent(int type, int code, int val);
};

#endif // CONTROLLERMANAGER_H
