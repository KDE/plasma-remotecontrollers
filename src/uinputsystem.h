/*
 *   SPDX-FileCopyrightText: 2022 Bart Ribbers <bribbers@disroot.org>
 *   SPDX-FileCopyrightText: 2022 Aleix Pol Gonzalez <aleixpol@kde.org>
 *
 *   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef UINPUTSYSTEM_H
#define UINPUTSYSTEM_H

#include "abstractsystem.h"

class UInputSystem : public AbstractSystem
{
    Q_OBJECT
public:
    UInputSystem();

    bool init() override;
    void emitKey(int key, bool pressed) override;

private:
    void emitEvent(int type, int code, int val);

    int m_fd;
};

#endif // UINPUTSYSTEM_H
