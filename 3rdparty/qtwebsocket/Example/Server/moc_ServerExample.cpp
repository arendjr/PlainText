/****************************************************************************
** Meta object code from reading C++ file 'ServerExample.h'
**
** Created: Wed Apr 25 23:12:39 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "ServerExample.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ServerExample.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ServerExample[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x0a,
      41,   36,   14,   14, 0x0a,
      77,   65,   14,   14, 0x0a,
      93,   14,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ServerExample[] = {
    "ServerExample\0\0onClientConnection()\0"
    "data\0onDataReceived(QString)\0elapsedTime\0"
    "onPong(quint64)\0onClientDisconnection()\0"
};

const QMetaObject ServerExample::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ServerExample,
      qt_meta_data_ServerExample, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ServerExample::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ServerExample::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ServerExample::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ServerExample))
        return static_cast<void*>(const_cast< ServerExample*>(this));
    return QObject::qt_metacast(_clname);
}

int ServerExample::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: onClientConnection(); break;
        case 1: onDataReceived((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: onPong((*reinterpret_cast< quint64(*)>(_a[1]))); break;
        case 3: onClientDisconnection(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
