/*
    SPDX-FileCopyrightText: 2022 Aditya Mehra <aix.m@outlook.com>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef KEYMAPMODEL_H
#define KEYMAPMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include "keydisplayprovider.h"

class KeyMapModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit KeyMapModel(QObject *parent = nullptr);
    ~KeyMapModel();
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    Q_INVOKABLE QVariantMap get(int index) const;
    int count() const;
    void load();
    void refresh();

    enum Roles {
        DeviceTypeRole = Qt::UserRole + 1,
        ButtonDisplayRole,
        ButtonTypeRole,
        ObjectNameRole,
        AssignedKeyRole,
        AssignedKeyDisplayRole,
        AssignedKeyIconRole
    };

    Q_ENUM(Roles)
     
private:
    QHash<int, QByteArray> m_roleNames;
    QVector<QVariantMap>  m_keyMap;
    KeyDisplayProvider *m_keyDisplayProvider;
};

#endif // KEYMAPMODEL_H
