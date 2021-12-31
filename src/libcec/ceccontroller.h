#ifndef CECCONTROLLER_H
#define CECCONTROLLER_H

#include <QDebug>
#include <QThread>

#include <libcec/cec.h>

using namespace CEC;

class CECController : public QThread {  
public:
    explicit CECController(int fd);
    ~CECController() override;
    
    void run() override;
private:
    static int m_fd;
    ICECAdapter* m_cecAdapter = nullptr;
    ICECCallbacks m_cecCallbacks;
    static QHash<int, int> m_keyCodeTranslation;
    
    static void handleCecKeypress(void* param, const cec_keypress* key);
};

#endif // CECCONTROLLER_H

