/*
 *   SPDX-FileCopyrightText: 2022 Bart Ribbers <bribbers@disroot.org>
 *   SPDX-FileCopyrightText: 2022 Aleix Pol Gonzalez <aleixpol@kde.org>
 *
 *   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "kwinfakeinputsystem.h"
#include "qwayland-fake-input.h"
#include <QWaylandClientExtensionTemplate>
#include <KLocalizedString>
#include <QDebug>
#include <linux/input-event-codes.h>

class FakeInput : public QWaylandClientExtensionTemplate<FakeInput>, public QtWayland::org_kde_kwin_fake_input
{
public:
    FakeInput()
        : QWaylandClientExtensionTemplate<FakeInput>(ORG_KDE_KWIN_FAKE_INPUT_KEYBOARD_KEY_SINCE_VERSION)
    {
#if QTWAYLANDCLIENT_VERSION >= QT_VERSION_CHECK(6, 2, 0)
        initialize();
#else
        // QWaylandClientExtensionTemplate invokes this with a QueuedConnection but we want it called immediately
        QMetaObject::invokeMethod(this, "addRegistryListener");
#endif
    }
};

bool KWinFakeInputSystem::init()
{
    m_ext = new FakeInput;
    m_ext->setParent(this);
    if (!m_ext->isActive()) {
        return false;
    }
    m_ext->authenticate({}, {});
    return true;
}

void KWinFakeInputSystem::emitKey(int key, bool pressed)
{
    m_ext->keyboard_key(key, pressed);
}
