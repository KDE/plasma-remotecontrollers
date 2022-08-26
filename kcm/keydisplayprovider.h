/*
    SPDX-FileCopyrightText: 2022 Aditya Mehra <aix.m@outlook.com>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef KEYDISPLAYPROVIDER_H
#define KEYDISPLAYPROVIDER_H

#include <QObject>

class KeyDisplayProvider : public QObject
{
    Q_OBJECT

public:
    explicit KeyDisplayProvider(QObject *parent = nullptr);
    ~KeyDisplayProvider() override;
    Q_INVOKABLE QString keyDisplay(const int &key) const;
    Q_INVOKABLE QString keyIcon(const int &key, const QString &deviceType) const;

private: 
    QList<QVariantMap> m_displayMap;
};

#endif // KEYDISPLAYPROVIDER_H
