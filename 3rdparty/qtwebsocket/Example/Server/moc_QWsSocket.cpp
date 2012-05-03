/****************************************************************************
** Meta object code from reading C++ file 'QWsSocket.h'
**
** Created: Wed Apr 25 23:12:38 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../QtWebSocket/QWsSocket.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QWsSocket.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QWsSocket[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      17,   11,   10,   10, 0x05,
      40,   11,   10,   10, 0x05,
      78,   66,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
      99,   92,   10,   10, 0x0a,
     114,   10,   10,   10, 0x2a,
     122,   10,   10,   10, 0x0a,
     129,   10,   10,   10, 0x09,
     144,   10,   10,   10, 0x08,
     168,   10,   10,   10, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QWsSocket[] = {
    "QWsSocket\0\0frame\0frameReceived(QString)\0"
    "frameReceived(QByteArray)\0elapsedTime\0"
    "pong(quint64)\0reason\0close(QString)\0"
    "close()\0ping()\0dataReceived()\0"
    "tcpSocketAboutToClose()\0tcpSocketDisconnected()\0"
};

const QMetaObject QWsSocket::staticMetaObject = {
    { &QAbstractSocket::staticMetaObject, qt_meta_stringdata_QWsSocket,
      qt_meta_data_QWsSocket, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QWsSocket::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QWsSocket::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QWsSocket::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QWsSocket))
        return static_cast<void*>(const_cast< QWsSocket*>(this));
    return QAbstractSocket::qt_metacast(_clname);
}

int QWsSocket::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractSocket::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: frameReceived((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: frameReceived((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 2: pong((*reinterpret_cast< quint64(*)>(_a[1]))); break;
        case 3: close((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: close(); break;
        case 5: ping(); break;
        case 6: dataReceived(); break;
        case 7: tcpSocketAboutToClose(); break;
        case 8: tcpSocketDisconnected(); break;
        default: ;
        }
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void QWsSocket::frameReceived(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QWsSocket::frameReceived(QByteArray _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QWsSocket::pong(quint64 _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
