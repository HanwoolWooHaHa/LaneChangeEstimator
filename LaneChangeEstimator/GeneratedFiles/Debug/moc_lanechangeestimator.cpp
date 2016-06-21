/****************************************************************************
** Meta object code from reading C++ file 'lanechangeestimator.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../lanechangeestimator.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'lanechangeestimator.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_LaneChangeEstimator_t {
    QByteArrayData data[9];
    char stringdata0[137];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_LaneChangeEstimator_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_LaneChangeEstimator_t qt_meta_stringdata_LaneChangeEstimator = {
    {
QT_MOC_LITERAL(0, 0, 19), // "LaneChangeEstimator"
QT_MOC_LITERAL(1, 20, 17), // "handleModeChanged"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 5), // "index"
QT_MOC_LITERAL(4, 45, 18), // "handleModeSelected"
QT_MOC_LITERAL(5, 64, 17), // "handleTypeChanged"
QT_MOC_LITERAL(6, 82, 18), // "handleTypeSelected"
QT_MOC_LITERAL(7, 101, 19), // "handleMethodChanged"
QT_MOC_LITERAL(8, 121, 15) // "openMenuClicked"

    },
    "LaneChangeEstimator\0handleModeChanged\0"
    "\0index\0handleModeSelected\0handleTypeChanged\0"
    "handleTypeSelected\0handleMethodChanged\0"
    "openMenuClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LaneChangeEstimator[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x08 /* Private */,
       4,    0,   47,    2, 0x08 /* Private */,
       5,    1,   48,    2, 0x08 /* Private */,
       6,    0,   51,    2, 0x08 /* Private */,
       7,    1,   52,    2, 0x08 /* Private */,
       8,    0,   55,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,

       0        // eod
};

void LaneChangeEstimator::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        LaneChangeEstimator *_t = static_cast<LaneChangeEstimator *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->handleModeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->handleModeSelected(); break;
        case 2: _t->handleTypeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->handleTypeSelected(); break;
        case 4: _t->handleMethodChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->openMenuClicked(); break;
        default: ;
        }
    }
}

const QMetaObject LaneChangeEstimator::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_LaneChangeEstimator.data,
      qt_meta_data_LaneChangeEstimator,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *LaneChangeEstimator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LaneChangeEstimator::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_LaneChangeEstimator.stringdata0))
        return static_cast<void*>(const_cast< LaneChangeEstimator*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int LaneChangeEstimator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
