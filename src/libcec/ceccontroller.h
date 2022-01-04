#ifndef CECCONTROLLER_H
#define CECCONTROLLER_H

#include <QDebug>
#include <QThread>

#include <libcec/cec.h>

#include "../controller.h"

using namespace CEC;

class CECController : public Controller
{
    Q_OBJECT

public:
    explicit CECController();
    ~CECController() override;
    
    void run() override;

signals:
    void keyPress(int keyCode, bool pressed);

private:
    ICECAdapter* m_cecAdapter = nullptr;
    ICECCallbacks m_cecCallbacks;
    static QHash<int, int> m_keyCodeTranslation;
    
    static void handleCecKeypress(void* param, const cec_keypress* key);
};

#endif // CECCONTROLLER_H

