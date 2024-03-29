/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEVME_P_H
#define QDECLARATIVEVME_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qdeclarativeerror.h"
#include "private/qbitfield_p.h"

#include <QtCore/QString>
#include <QtCore/QStack>

QT_BEGIN_NAMESPACE

class QObject;
class QDeclarativeInstruction;
class QDeclarativeCompiledData;
class QDeclarativeCompiledData;
class QDeclarativeContextData;

template<typename T, int N = 128>
class QDeclarativeVMEStack {
public:
    QDeclarativeVMEStack() : index(-1), maxSize(N), data((T *)fixedData) {}
    ~QDeclarativeVMEStack() { if (data != (T *)fixedData) qFree(data); }

    bool isEmpty() const { return index == -1; }
    const T &top() const { return data[index]; }
    void push(const T &i) { ++index; if (index == maxSize) realloc(); data[index] = i; }
    const T &pop() { --index; return data[index + 1]; }
    int count() const { return index + 1; }
    const T &at(int idx) { return data[idx]; }

private:
    void realloc() {
        maxSize += N;
        if (data != (T *)fixedData) {
            data = (T*)qRealloc(data, maxSize * sizeof(T));
        } else {
            data = (T*)qMalloc(maxSize * sizeof(T));
        }
    }
    int index;
    int maxSize;
    T *data;
    char fixedData[N * sizeof(T)];
};

class QDeclarativeVME
{
public:
    QDeclarativeVME();

    QObject *run(QDeclarativeContextData *, QDeclarativeCompiledData *, 
                 int start = -1, int count = -1, 
                 const QBitField & = QBitField());
    void runDeferred(QObject *);

    bool isError() const;
    QList<QDeclarativeError> errors() const;

private:
    QObject *run(QDeclarativeVMEStack<QObject *> &, 
                 QDeclarativeContextData *, QDeclarativeCompiledData *, 
                 int start, int count, 
                 const QBitField &);
    QList<QDeclarativeError> vmeErrors;
};

QT_END_NAMESPACE

#endif // QDECLARATIVEVME_P_H
