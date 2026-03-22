/****************************************************************************
** Meta object code from reading C++ file 'dashboardwindow.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../dashboardwindow.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dashboardwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.10.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN15DashboardWindowE_t {};
} // unnamed namespace

template <> constexpr inline auto DashboardWindow::qt_create_metaobjectdata<qt_meta_tag_ZN15DashboardWindowE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "DashboardWindow",
        "lotsLoaded",
        "",
        "QList<SectionInfo>",
        "sections",
        "lotsLoadError",
        "error",
        "raiseResult",
        "nodeId",
        "ok",
        "msg",
        "raiseAllFinished",
        "chatsLoaded",
        "QList<ChatContact>",
        "contacts",
        "chatsLoadError",
        "chatHistoryLoaded",
        "FunPayAPI::ChatNodeInfo",
        "info",
        "lastMsgId",
        "QList<ChatHistoryMsg>",
        "messages",
        "chatHistoryLoadError",
        "onStartStopClicked",
        "onBotLog",
        "level",
        "onBotNewMessage",
        "ChatMessage",
        "onBotNewOrder",
        "OrderInfo",
        "order",
        "onBotStatusChanged",
        "status",
        "onBotStopped",
        "onRefreshLots",
        "onRaiseNow",
        "onLotsLoaded",
        "onLotsLoadError",
        "onRaiseResult",
        "onRaiseAllFinished",
        "onAddRule",
        "onDeleteRule",
        "onSaveRules",
        "onAddDelivery",
        "onDeleteDelivery",
        "onSaveDelivery",
        "onSaveSettings",
        "onRefreshChats",
        "onChatContactSelected",
        "QListWidgetItem*",
        "item",
        "onSendChatMessage",
        "onChatsLoaded",
        "onChatsLoadError",
        "onChatHistoryLoaded",
        "onChatHistoryLoadError"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'lotsLoaded'
        QtMocHelpers::SignalData<void(QList<SectionInfo>)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'lotsLoadError'
        QtMocHelpers::SignalData<void(QString)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 },
        }}),
        // Signal 'raiseResult'
        QtMocHelpers::SignalData<void(QString, bool, QString)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 8 }, { QMetaType::Bool, 9 }, { QMetaType::QString, 10 },
        }}),
        // Signal 'raiseAllFinished'
        QtMocHelpers::SignalData<void()>(11, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'chatsLoaded'
        QtMocHelpers::SignalData<void(QList<ChatContact>)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 13, 14 },
        }}),
        // Signal 'chatsLoadError'
        QtMocHelpers::SignalData<void(QString)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 },
        }}),
        // Signal 'chatHistoryLoaded'
        QtMocHelpers::SignalData<void(QString, FunPayAPI::ChatNodeInfo, qint64, QList<ChatHistoryMsg>)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 8 }, { 0x80000000 | 17, 18 }, { QMetaType::LongLong, 19 }, { 0x80000000 | 20, 21 },
        }}),
        // Signal 'chatHistoryLoadError'
        QtMocHelpers::SignalData<void(QString, QString)>(22, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 8 }, { QMetaType::QString, 6 },
        }}),
        // Slot 'onStartStopClicked'
        QtMocHelpers::SlotData<void()>(23, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onBotLog'
        QtMocHelpers::SlotData<void(int, const QString &)>(24, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 25 }, { QMetaType::QString, 10 },
        }}),
        // Slot 'onBotNewMessage'
        QtMocHelpers::SlotData<void(const ChatMessage &)>(26, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 27, 10 },
        }}),
        // Slot 'onBotNewOrder'
        QtMocHelpers::SlotData<void(const OrderInfo &)>(28, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 29, 30 },
        }}),
        // Slot 'onBotStatusChanged'
        QtMocHelpers::SlotData<void(const QString &)>(31, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 32 },
        }}),
        // Slot 'onBotStopped'
        QtMocHelpers::SlotData<void()>(33, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRefreshLots'
        QtMocHelpers::SlotData<void()>(34, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRaiseNow'
        QtMocHelpers::SlotData<void()>(35, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onLotsLoaded'
        QtMocHelpers::SlotData<void(QList<SectionInfo>)>(36, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Slot 'onLotsLoadError'
        QtMocHelpers::SlotData<void(QString)>(37, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 6 },
        }}),
        // Slot 'onRaiseResult'
        QtMocHelpers::SlotData<void(QString, bool, QString)>(38, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 8 }, { QMetaType::Bool, 9 }, { QMetaType::QString, 10 },
        }}),
        // Slot 'onRaiseAllFinished'
        QtMocHelpers::SlotData<void()>(39, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAddRule'
        QtMocHelpers::SlotData<void()>(40, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDeleteRule'
        QtMocHelpers::SlotData<void()>(41, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSaveRules'
        QtMocHelpers::SlotData<void()>(42, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAddDelivery'
        QtMocHelpers::SlotData<void()>(43, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDeleteDelivery'
        QtMocHelpers::SlotData<void()>(44, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSaveDelivery'
        QtMocHelpers::SlotData<void()>(45, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSaveSettings'
        QtMocHelpers::SlotData<void()>(46, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRefreshChats'
        QtMocHelpers::SlotData<void()>(47, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onChatContactSelected'
        QtMocHelpers::SlotData<void(QListWidgetItem *)>(48, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 49, 50 },
        }}),
        // Slot 'onSendChatMessage'
        QtMocHelpers::SlotData<void()>(51, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onChatsLoaded'
        QtMocHelpers::SlotData<void(QList<ChatContact>)>(52, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 13, 14 },
        }}),
        // Slot 'onChatsLoadError'
        QtMocHelpers::SlotData<void(QString)>(53, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 6 },
        }}),
        // Slot 'onChatHistoryLoaded'
        QtMocHelpers::SlotData<void(QString, FunPayAPI::ChatNodeInfo, qint64, QList<ChatHistoryMsg>)>(54, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 8 }, { 0x80000000 | 17, 18 }, { QMetaType::LongLong, 19 }, { 0x80000000 | 20, 21 },
        }}),
        // Slot 'onChatHistoryLoadError'
        QtMocHelpers::SlotData<void(QString, QString)>(55, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 8 }, { QMetaType::QString, 6 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<DashboardWindow, qt_meta_tag_ZN15DashboardWindowE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject DashboardWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15DashboardWindowE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15DashboardWindowE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN15DashboardWindowE_t>.metaTypes,
    nullptr
} };

void DashboardWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<DashboardWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->lotsLoaded((*reinterpret_cast<std::add_pointer_t<QList<SectionInfo>>>(_a[1]))); break;
        case 1: _t->lotsLoadError((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->raiseResult((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 3: _t->raiseAllFinished(); break;
        case 4: _t->chatsLoaded((*reinterpret_cast<std::add_pointer_t<QList<ChatContact>>>(_a[1]))); break;
        case 5: _t->chatsLoadError((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->chatHistoryLoaded((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<FunPayAPI::ChatNodeInfo>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<qint64>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QList<ChatHistoryMsg>>>(_a[4]))); break;
        case 7: _t->chatHistoryLoadError((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 8: _t->onStartStopClicked(); break;
        case 9: _t->onBotLog((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 10: _t->onBotNewMessage((*reinterpret_cast<std::add_pointer_t<ChatMessage>>(_a[1]))); break;
        case 11: _t->onBotNewOrder((*reinterpret_cast<std::add_pointer_t<OrderInfo>>(_a[1]))); break;
        case 12: _t->onBotStatusChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 13: _t->onBotStopped(); break;
        case 14: _t->onRefreshLots(); break;
        case 15: _t->onRaiseNow(); break;
        case 16: _t->onLotsLoaded((*reinterpret_cast<std::add_pointer_t<QList<SectionInfo>>>(_a[1]))); break;
        case 17: _t->onLotsLoadError((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 18: _t->onRaiseResult((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 19: _t->onRaiseAllFinished(); break;
        case 20: _t->onAddRule(); break;
        case 21: _t->onDeleteRule(); break;
        case 22: _t->onSaveRules(); break;
        case 23: _t->onAddDelivery(); break;
        case 24: _t->onDeleteDelivery(); break;
        case 25: _t->onSaveDelivery(); break;
        case 26: _t->onSaveSettings(); break;
        case 27: _t->onRefreshChats(); break;
        case 28: _t->onChatContactSelected((*reinterpret_cast<std::add_pointer_t<QListWidgetItem*>>(_a[1]))); break;
        case 29: _t->onSendChatMessage(); break;
        case 30: _t->onChatsLoaded((*reinterpret_cast<std::add_pointer_t<QList<ChatContact>>>(_a[1]))); break;
        case 31: _t->onChatsLoadError((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 32: _t->onChatHistoryLoaded((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<FunPayAPI::ChatNodeInfo>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<qint64>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QList<ChatHistoryMsg>>>(_a[4]))); break;
        case 33: _t->onChatHistoryLoadError((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (DashboardWindow::*)(QList<SectionInfo> )>(_a, &DashboardWindow::lotsLoaded, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (DashboardWindow::*)(QString )>(_a, &DashboardWindow::lotsLoadError, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (DashboardWindow::*)(QString , bool , QString )>(_a, &DashboardWindow::raiseResult, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (DashboardWindow::*)()>(_a, &DashboardWindow::raiseAllFinished, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (DashboardWindow::*)(QList<ChatContact> )>(_a, &DashboardWindow::chatsLoaded, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (DashboardWindow::*)(QString )>(_a, &DashboardWindow::chatsLoadError, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (DashboardWindow::*)(QString , FunPayAPI::ChatNodeInfo , qint64 , QList<ChatHistoryMsg> )>(_a, &DashboardWindow::chatHistoryLoaded, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (DashboardWindow::*)(QString , QString )>(_a, &DashboardWindow::chatHistoryLoadError, 7))
            return;
    }
}

const QMetaObject *DashboardWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DashboardWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15DashboardWindowE_t>.strings))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int DashboardWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 34)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 34;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 34)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 34;
    }
    return _id;
}

// SIGNAL 0
void DashboardWindow::lotsLoaded(QList<SectionInfo> _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void DashboardWindow::lotsLoadError(QString _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void DashboardWindow::raiseResult(QString _t1, bool _t2, QString _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2, _t3);
}

// SIGNAL 3
void DashboardWindow::raiseAllFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void DashboardWindow::chatsLoaded(QList<ChatContact> _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void DashboardWindow::chatsLoadError(QString _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void DashboardWindow::chatHistoryLoaded(QString _t1, FunPayAPI::ChatNodeInfo _t2, qint64 _t3, QList<ChatHistoryMsg> _t4)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1, _t2, _t3, _t4);
}

// SIGNAL 7
void DashboardWindow::chatHistoryLoadError(QString _t1, QString _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1, _t2);
}
QT_WARNING_POP
