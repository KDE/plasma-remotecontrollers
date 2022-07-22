/*
    SPDX-FileCopyrightText: 2022 Aditya Mehra <aix.m@outlook.com>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include <keymapmodel.h>
#include <KSharedConfig>
#include <KConfigGroup>
#include <KLocalizedString>

KeyMapModel::KeyMapModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_keyDisplayProvider = new KeyDisplayProvider();
    m_roleNames[DeviceTypeRole] = "deviceType";
    m_roleNames[ButtonDisplayRole] = "buttonDisplay";
    m_roleNames[ButtonTypeRole] = "buttonType";
    m_roleNames[ObjectNameRole] = "objectName";
    m_roleNames[AssignedKeyRole] = "assignedKey";
    m_roleNames[AssignedKeyDisplayRole] = "assignedKeyDisplay";
    m_roleNames[AssignedKeyIconRole] = "assignedKeyIcon";
    load();
}

KeyMapModel::~KeyMapModel()
{
}

QHash<int, QByteArray> KeyMapModel::roleNames() const
{
    return m_roleNames;
}

int KeyMapModel::count() const
{
    return m_keyMap.count();
}

int KeyMapModel::rowCount(const QModelIndex &) const
{
    return m_keyMap.size();
}

void KeyMapModel::load()
{
    static KSharedConfigPtr config = KSharedConfig::openConfig(QLatin1String("plasma-remotecontrollersrc"));
    static KConfigGroup group_cec (config, QLatin1String("General"));
    static KConfigGroup group_gamepad (config, QLatin1String("Gamepad"));

    QVariantMap map;
    // CEC
    map.insert("DeviceTypeRole", "CEC");
    map.insert("ButtonDisplayRole", i18n("Button Left"));
    map.insert("ButtonTypeRole", QStringLiteral("ButtonLeft"));
    map.insert("ObjectNameRole", QStringLiteral("buttonLeftConfigValue"));
    map.insert("AssignedKeyRole", group_cec.readEntry(QStringLiteral("ButtonLeft"), QStringLiteral("")));
    map.insert("AssignedKeyDisplayRole", m_keyDisplayProvider->keyDisplay(map.value("AssignedKeyRole").toInt()));
    map.insert("AssignedKeyIconRole", m_keyDisplayProvider->keyIcon(map.value("AssignedKeyRole").toInt(), QStringLiteral("CEC")));
    m_keyMap.append(map);
    map.clear();

    map.insert("DeviceTypeRole", "CEC");
    map.insert("ButtonDisplayRole", i18n("Button Right"));
    map.insert("ButtonTypeRole", QStringLiteral("ButtonRight"));
    map.insert("ObjectNameRole", QStringLiteral("buttonRightConfigValue"));
    map.insert("AssignedKeyRole", group_cec.readEntry(QStringLiteral("ButtonRight"), QStringLiteral("")));
    map.insert("AssignedKeyDisplayRole", m_keyDisplayProvider->keyDisplay(map.value("AssignedKeyRole").toInt()));
    map.insert("AssignedKeyIconRole", m_keyDisplayProvider->keyIcon(map.value("AssignedKeyRole").toInt(), QStringLiteral("CEC")));
    m_keyMap.append(map);
    map.clear();

    map.insert("DeviceTypeRole", "CEC");
    map.insert("ButtonDisplayRole", i18n("Button Up"));
    map.insert("ButtonTypeRole", QStringLiteral("ButtonUp"));
    map.insert("ObjectNameRole", QStringLiteral("buttonUpConfigValue"));
    map.insert("AssignedKeyRole", group_cec.readEntry(QStringLiteral("ButtonUp"), QStringLiteral("")));
    map.insert("AssignedKeyDisplayRole", m_keyDisplayProvider->keyDisplay(map.value("AssignedKeyRole").toInt()));
    map.insert("AssignedKeyIconRole", m_keyDisplayProvider->keyIcon(map.value("AssignedKeyRole").toInt(), QStringLiteral("CEC")));
    m_keyMap.append(map);
    map.clear();
    
    map.insert("DeviceTypeRole", "CEC");
    map.insert("ButtonDisplayRole", i18n("Button Down"));
    map.insert("ButtonTypeRole", QStringLiteral("ButtonDown"));
    map.insert("ObjectNameRole", QStringLiteral("buttonDownConfigValue"));
    map.insert("AssignedKeyRole", group_cec.readEntry(QStringLiteral("ButtonDown"), QStringLiteral("")));
    map.insert("AssignedKeyDisplayRole", m_keyDisplayProvider->keyDisplay(map.value("AssignedKeyRole").toInt()));
    map.insert("AssignedKeyIconRole", m_keyDisplayProvider->keyIcon(map.value("AssignedKeyRole").toInt(), QStringLiteral("CEC")));
    m_keyMap.append(map);
    map.clear();

    map.insert("DeviceTypeRole", "CEC");
    map.insert("ButtonDisplayRole", i18n("Button Enter"));
    map.insert("ButtonTypeRole", QStringLiteral("ButtonEnter"));
    map.insert("ObjectNameRole", QStringLiteral("buttonEnterConfigValue"));
    map.insert("AssignedKeyRole", group_cec.readEntry(QStringLiteral("ButtonEnter"), QStringLiteral("")));
    map.insert("AssignedKeyDisplayRole", m_keyDisplayProvider->keyDisplay(map.value("AssignedKeyRole").toInt()));
    map.insert("AssignedKeyIconRole", m_keyDisplayProvider->keyIcon(map.value("AssignedKeyRole").toInt(), QStringLiteral("CEC")));
    m_keyMap.append(map);
    map.clear();

    map.insert("DeviceTypeRole", "CEC");
    map.insert("ButtonDisplayRole", i18n("Button Back"));
    map.insert("ButtonTypeRole", QStringLiteral("ButtonBack"));
    map.insert("ObjectNameRole", QStringLiteral("buttonBackConfigValue"));
    map.insert("AssignedKeyRole", group_cec.readEntry(QStringLiteral("ButtonBack"), QStringLiteral("")));
    map.insert("AssignedKeyDisplayRole", m_keyDisplayProvider->keyDisplay(map.value("AssignedKeyRole").toInt()));
    map.insert("AssignedKeyIconRole", m_keyDisplayProvider->keyIcon(map.value("AssignedKeyRole").toInt(), QStringLiteral("CEC")));
    m_keyMap.append(map);
    map.clear();

    map.insert("DeviceTypeRole", "CEC");
    map.insert("ButtonDisplayRole", i18n("Button Home"));
    map.insert("ButtonTypeRole", QStringLiteral("ButtonHomepage"));
    map.insert("ObjectNameRole", QStringLiteral("buttonHomepageConfigValue"));
    map.insert("AssignedKeyRole", group_cec.readEntry(QStringLiteral("ButtonHomepage"), QStringLiteral("")));
    map.insert("AssignedKeyDisplayRole", m_keyDisplayProvider->keyDisplay(map.value("AssignedKeyRole").toInt()));
    map.insert("AssignedKeyIconRole", m_keyDisplayProvider->keyIcon(map.value("AssignedKeyRole").toInt(), QStringLiteral("CEC")));
    m_keyMap.append(map);
    map.clear();

    // Gamepad
    map.insert("DeviceTypeRole", "GAMEPAD");
    map.insert("ButtonDisplayRole", i18n("Button Left"));
    map.insert("ButtonTypeRole", QStringLiteral("ButtonLeft"));
    map.insert("ObjectNameRole", QStringLiteral("buttonLeftConfigValue"));
    map.insert("AssignedKeyRole", group_gamepad.readEntry(QStringLiteral("ButtonLeft"), QStringLiteral("")));
    map.insert("AssignedKeyDisplayRole", m_keyDisplayProvider->keyDisplay(map.value("AssignedKeyRole").toInt()));
    map.insert("AssignedKeyIconRole", m_keyDisplayProvider->keyIcon(map.value("AssignedKeyRole").toInt(), QStringLiteral("Gamepad")));
    m_keyMap.append(map);
    map.clear();

    map.insert("DeviceTypeRole", "GAMEPAD");
    map.insert("ButtonDisplayRole", i18n("Button Right"));
    map.insert("ButtonTypeRole", QStringLiteral("ButtonRight"));
    map.insert("ObjectNameRole", QStringLiteral("buttonRightConfigValue"));
    map.insert("AssignedKeyRole", group_gamepad.readEntry(QStringLiteral("ButtonRight"), QStringLiteral("")));
    map.insert("AssignedKeyDisplayRole", m_keyDisplayProvider->keyDisplay(map.value("AssignedKeyRole").toInt()));
    map.insert("AssignedKeyIconRole", m_keyDisplayProvider->keyIcon(map.value("AssignedKeyRole").toInt(), QStringLiteral("Gamepad")));
    m_keyMap.append(map);
    map.clear();

    map.insert("DeviceTypeRole", "GAMEPAD");
    map.insert("ButtonDisplayRole", i18n("Button Up"));
    map.insert("ButtonTypeRole", QStringLiteral("ButtonUp"));
    map.insert("ObjectNameRole", QStringLiteral("buttonUpConfigValue"));
    map.insert("AssignedKeyRole", group_gamepad.readEntry(QStringLiteral("ButtonUp"), QStringLiteral("")));
    map.insert("AssignedKeyDisplayRole", m_keyDisplayProvider->keyDisplay(map.value("AssignedKeyRole").toInt()));
    map.insert("AssignedKeyIconRole", m_keyDisplayProvider->keyIcon(map.value("AssignedKeyRole").toInt(), QStringLiteral("Gamepad")));
    m_keyMap.append(map);
    map.clear();

    map.insert("DeviceTypeRole", "GAMEPAD");    
    map.insert("ButtonDisplayRole", i18n("Button Down"));
    map.insert("ButtonTypeRole", QStringLiteral("ButtonDown"));
    map.insert("ObjectNameRole", QStringLiteral("buttonDownConfigValue"));
    map.insert("AssignedKeyRole", group_gamepad.readEntry(QStringLiteral("ButtonDown"), QStringLiteral("")));
    map.insert("AssignedKeyDisplayRole", m_keyDisplayProvider->keyDisplay(map.value("AssignedKeyRole").toInt()));
    map.insert("AssignedKeyIconRole", m_keyDisplayProvider->keyIcon(map.value("AssignedKeyRole").toInt(), QStringLiteral("Gamepad")));
    m_keyMap.append(map);
    map.clear();

    map.insert("DeviceTypeRole", "GAMEPAD");
    map.insert("ButtonDisplayRole", i18n("Button Enter"));
    map.insert("ButtonTypeRole", QStringLiteral("ButtonEnter"));
    map.insert("ObjectNameRole", QStringLiteral("buttonEnterConfigValue"));
    map.insert("AssignedKeyRole", group_gamepad.readEntry(QStringLiteral("ButtonEnter"), QStringLiteral("")));
    map.insert("AssignedKeyDisplayRole", m_keyDisplayProvider->keyDisplay(map.value("AssignedKeyRole").toInt()));
    map.insert("AssignedKeyIconRole", m_keyDisplayProvider->keyIcon(map.value("AssignedKeyRole").toInt(), QStringLiteral("Gamepad")));
    m_keyMap.append(map);
    map.clear();

    map.insert("DeviceTypeRole", "GAMEPAD");
    map.insert("ButtonDisplayRole", i18n("Button Back"));
    map.insert("ButtonTypeRole", QStringLiteral("ButtonBack"));
    map.insert("ObjectNameRole", QStringLiteral("buttonBackConfigValue"));
    map.insert("AssignedKeyRole", group_gamepad.readEntry(QStringLiteral("ButtonBack"), QStringLiteral("")));
    map.insert("AssignedKeyDisplayRole", m_keyDisplayProvider->keyDisplay(map.value("AssignedKeyRole").toInt()));
    map.insert("AssignedKeyIconRole", m_keyDisplayProvider->keyIcon(map.value("AssignedKeyRole").toInt(), QStringLiteral("Gamepad")));
    m_keyMap.append(map);
    map.clear();

    map.insert("DeviceTypeRole", "GAMEPAD");
    map.insert("ButtonDisplayRole", i18n("Button Home"));
    map.insert("ButtonTypeRole", QStringLiteral("ButtonHomepage"));
    map.insert("ObjectNameRole", QStringLiteral("buttonHomepageConfigValue"));
    map.insert("AssignedKeyRole", group_gamepad.readEntry(QStringLiteral("ButtonHomepage"), QStringLiteral("")));
    map.insert("AssignedKeyDisplayRole", m_keyDisplayProvider->keyDisplay(map.value("AssignedKeyRole").toInt()));
    map.insert("AssignedKeyIconRole", m_keyDisplayProvider->keyIcon(map.value("AssignedKeyRole").toInt(), QStringLiteral("Gamepad")));
    m_keyMap.append(map);
    map.clear();
}

QVariant KeyMapModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    if (index.row() >= m_keyMap.count()) {
        return QVariant();
    }

    QVariantMap map = m_keyMap[index.row()];
    switch (role) {
        case DeviceTypeRole:
            return map.value("DeviceTypeRole");
        case ButtonDisplayRole:
            return map.value("ButtonDisplayRole");
        case ButtonTypeRole:
            return map.value("ButtonTypeRole");
        case ObjectNameRole:
            return map.value("ObjectNameRole");
        case AssignedKeyRole:
            return map.value("AssignedKeyRole");
        case AssignedKeyDisplayRole:
            return map.value("AssignedKeyDisplayRole");
        case AssignedKeyIconRole:
            return map.value("AssignedKeyIconRole");
        default:
            return QVariant();
    }

    return QVariant();
}

QVariantMap KeyMapModel::get(int index) const
{
    if (index >= m_keyMap.count()) {
        return QVariantMap();
    }
    return m_keyMap.at(index);
}

void KeyMapModel::refresh()
{
    beginResetModel();
    m_keyMap.clear();
    load();
    endResetModel();
}