/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtSCriptTools module of the Qt Toolkit.
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

#ifndef QSCRIPTDEBUGGERCOMMAND_P_H
#define QSCRIPTDEBUGGERCOMMAND_P_H

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

#include <QtCore/qobjectdefs.h>
#include <QtCore/qscopedpointer.h>
#include <QtCore/qhash.h>
#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

class QDataStream;
class QScriptBreakpointData;
class QScriptDebuggerValue;

class QScriptDebuggerCommandPrivate;
class Q_AUTOTEST_EXPORT QScriptDebuggerCommand
{
public:
    friend Q_AUTOTEST_EXPORT QDataStream &operator<<(QDataStream &, const QScriptDebuggerCommand &);
    friend Q_AUTOTEST_EXPORT QDataStream &operator>>(QDataStream &, QScriptDebuggerCommand &);

    enum Type {
        None,

        Interrupt,
        Continue,
        StepInto,
        StepOver,
        StepOut,
        RunToLocation,
        RunToLocationByID,
        ForceReturn,
        Resume,

        SetBreakpoint,
        DeleteBreakpoint,
        DeleteAllBreakpoints,
        GetBreakpoints,
        GetBreakpointData,
        SetBreakpointData,

        GetScripts,
        GetScriptData,
        ScriptsCheckpoint,
        GetScriptsDelta,
        ResolveScript,

        GetBacktrace,
        GetContextCount,
        GetContextInfo,
        GetContextState,
        GetContextID,
        GetThisObject,
        GetActivationObject,
        GetScopeChain,
        ContextsCheckpoint,
        GetPropertyExpressionValue,
        GetCompletions,

        NewScriptObjectSnapshot,
        ScriptObjectSnapshotCapture,
        DeleteScriptObjectSnapshot,

        NewScriptValueIterator,
        GetPropertiesByIterator,
        DeleteScriptValueIterator,

        Evaluate,

        SetScriptValueProperty,
        ScriptValueToString,

        ClearExceptions,

        UserCommand = 1000,
        MaxUserCommand = 32767
    };

    enum Attribute {
        ScriptID,
        FileName,
        LineNumber,
        Program,
        BreakpointID,
        BreakpointData,
        ContextIndex,
        ScriptValue,
        StepCount,
        IteratorID,
        Name,
        SubordinateScriptValue,
        SnapshotID,
        UserAttribute = 1000,
        MaxUserAttribute = 32767
    };

    QScriptDebuggerCommand();
    QScriptDebuggerCommand(Type type);
    QScriptDebuggerCommand(const QScriptDebuggerCommand &other);
    ~QScriptDebuggerCommand();

    Type type() const;

    QVariant attribute(Attribute attribute, const QVariant &defaultValue = QVariant()) const;
    void setAttribute(Attribute attribute, const QVariant &value);
    QHash<Attribute, QVariant> attributes() const;

    QString fileName() const;
    void setFileName(const QString &fileName);

    int lineNumber() const;
    void setLineNumber(int lineNumber);

    qint64 scriptId() const;
    void setScriptId(qint64 id);

    QString program() const;
    void setProgram(const QString &program);

    int breakpointId() const;
    void setBreakpointId(int id);

    QScriptBreakpointData breakpointData() const;
    void setBreakpointData(const QScriptBreakpointData &data);

    QScriptDebuggerValue scriptValue() const;
    void setScriptValue(const QScriptDebuggerValue &value);

    int contextIndex() const;
    void setContextIndex(int index);

    int iteratorId() const;
    void setIteratorId(int id);

    QString name() const;
    void setName(const QString &name);

    QScriptDebuggerValue subordinateScriptValue() const;
    void setSubordinateScriptValue(const QScriptDebuggerValue &value);

    int snapshotId() const;
    void setSnapshotId(int id);

    QScriptDebuggerCommand &operator=(const QScriptDebuggerCommand &other);

    bool operator==(const QScriptDebuggerCommand &other) const;
    bool operator!=(const QScriptDebuggerCommand &other) const;

    static QScriptDebuggerCommand interruptCommand();
    static QScriptDebuggerCommand continueCommand();
    static QScriptDebuggerCommand stepIntoCommand(int count = 1);
    static QScriptDebuggerCommand stepOverCommand(int count = 1);
    static QScriptDebuggerCommand stepOutCommand();
    static QScriptDebuggerCommand runToLocationCommand(const QString &fileName, int lineNumber);
    static QScriptDebuggerCommand runToLocationCommand(qint64 scriptId, int lineNumber);
    static QScriptDebuggerCommand forceReturnCommand(int contextIndex, const QScriptDebuggerValue &value);
    static QScriptDebuggerCommand resumeCommand();

    static QScriptDebuggerCommand setBreakpointCommand(const QString &fileName, int lineNumber);
    static QScriptDebuggerCommand setBreakpointCommand(const QScriptBreakpointData &data);
    static QScriptDebuggerCommand deleteBreakpointCommand(int id);
    static QScriptDebuggerCommand deleteAllBreakpointsCommand();
    static QScriptDebuggerCommand getBreakpointsCommand();
    static QScriptDebuggerCommand getBreakpointDataCommand(int id);
    static QScriptDebuggerCommand setBreakpointDataCommand(int id, const QScriptBreakpointData &data);

    static QScriptDebuggerCommand getScriptsCommand();
    static QScriptDebuggerCommand getScriptDataCommand(qint64 id);
    static QScriptDebuggerCommand scriptsCheckpointCommand();
    static QScriptDebuggerCommand getScriptsDeltaCommand();
    static QScriptDebuggerCommand resolveScriptCommand(const QString &fileName);

    static QScriptDebuggerCommand getBacktraceCommand();
    static QScriptDebuggerCommand getContextCountCommand();
    static QScriptDebuggerCommand getContextStateCommand(int contextIndex);
    static QScriptDebuggerCommand getContextInfoCommand(int contextIndex);
    static QScriptDebuggerCommand getContextIdCommand(int contextIndex);
    static QScriptDebuggerCommand getThisObjectCommand(int contextIndex);
    static QScriptDebuggerCommand getActivationObjectCommand(int contextIndex);
    static QScriptDebuggerCommand getScopeChainCommand(int contextIndex);
    static QScriptDebuggerCommand contextsCheckpoint();
    static QScriptDebuggerCommand getPropertyExpressionValue(int contextIndex, int lineNumber,
                                                             const QStringList &path);
    static QScriptDebuggerCommand getCompletions(int contextIndex, const QStringList &path);

    static QScriptDebuggerCommand newScriptObjectSnapshotCommand();
    static QScriptDebuggerCommand scriptObjectSnapshotCaptureCommand(int id, const QScriptDebuggerValue &object);
    static QScriptDebuggerCommand deleteScriptObjectSnapshotCommand(int id);

    static QScriptDebuggerCommand newScriptValueIteratorCommand(const QScriptDebuggerValue &object);
    static QScriptDebuggerCommand getPropertiesByIteratorCommand(int id, int count);
    static QScriptDebuggerCommand deleteScriptValueIteratorCommand(int id);

    static QScriptDebuggerCommand evaluateCommand(int contextIndex, const QString &program,
                                                  const QString &fileName = QString(),
                                                  int lineNumber = 1);

    static QScriptDebuggerCommand setScriptValuePropertyCommand(const QScriptDebuggerValue &object,
                                                                const QString &name,
                                                                const QScriptDebuggerValue &value);
    static QScriptDebuggerCommand scriptValueToStringCommand(const QScriptDebuggerValue &value);

    static QScriptDebuggerCommand clearExceptionsCommand();

private:
    QScopedPointer<QScriptDebuggerCommandPrivate> d_ptr;

    Q_DECLARE_PRIVATE(QScriptDebuggerCommand)
};

Q_AUTOTEST_EXPORT QDataStream &operator<<(QDataStream &, const QScriptDebuggerCommand &);
Q_AUTOTEST_EXPORT QDataStream &operator>>(QDataStream &, QScriptDebuggerCommand &);

QT_END_NAMESPACE

#endif
