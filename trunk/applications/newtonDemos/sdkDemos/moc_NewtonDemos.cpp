/****************************************************************************
** Meta object code from reading C++ file 'NewtonDemos.h'
**
** Created: Tue Mar 8 20:44:20 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "toolbox_stdafx.h"
#include "NewtonDemos.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NewtonDemos.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_newtonDemos[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x0a,
      22,   12,   12,   12, 0x0a,
      31,   12,   12,   12, 0x0a,
      40,   12,   12,   12, 0x0a,
      52,   12,   12,   12, 0x0a,
      66,   12,   12,   12, 0x0a,
      85,   12,   12,   12, 0x0a,
     104,   12,   12,   12, 0x0a,
     125,   12,   12,   12, 0x0a,
     147,   12,   12,   12, 0x0a,
     171,   12,   12,   12, 0x0a,
     194,   12,   12,   12, 0x0a,
     218,   12,   12,   12, 0x0a,
     247,   12,   12,   12, 0x0a,
     278,   12,   12,   12, 0x0a,
     297,   12,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_newtonDemos[] = {
    "newtonDemos\0\0OnIdle()\0OnLoad()\0OnSave()\0"
    "OnRunDemo()\0OnAutoSleep()\0OnShowDebugLines()\0"
    "OnShowStatistics()\0OnUseParalleSolver()\0"
    "OnShowContactPoints()\0OnShowPhysicsProfiler()\0"
    "OnShowThreadProfiler()\0OnUseSimdInstructions()\0"
    "OnRunSymulationAsyncronous()\0"
    "OnSelectNumberOfMicroThreads()\0"
    "OnSetCameraSpeed()\0OnNotUsed()\0"
};

const QMetaObject newtonDemos::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_newtonDemos,
      qt_meta_data_newtonDemos, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &newtonDemos::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *newtonDemos::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *newtonDemos::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_newtonDemos))
        return static_cast<void*>(const_cast< newtonDemos*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int newtonDemos::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: OnIdle(); break;
        case 1: OnLoad(); break;
        case 2: OnSave(); break;
        case 3: OnRunDemo(); break;
        case 4: OnAutoSleep(); break;
        case 5: OnShowDebugLines(); break;
        case 6: OnShowStatistics(); break;
        case 7: OnUseParalleSolver(); break;
        case 8: OnShowContactPoints(); break;
        case 9: OnShowPhysicsProfiler(); break;
        case 10: OnShowThreadProfiler(); break;
        case 11: OnUseSimdInstructions(); break;
        case 12: OnRunSymulationAsyncronous(); break;
        case 13: OnSelectNumberOfMicroThreads(); break;
        case 14: OnSetCameraSpeed(); break;
        case 15: OnNotUsed(); break;
        default: ;
        }
        _id -= 16;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
