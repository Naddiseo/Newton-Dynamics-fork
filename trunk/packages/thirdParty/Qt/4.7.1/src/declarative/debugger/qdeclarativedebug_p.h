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
#ifndef QDECLARATIVEDEBUG_H
#define QDECLARATIVEDEBUG_H

#include <QtCore/qobject.h>
#include <QtCore/qurl.h>
#include <QtCore/qvariant.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QDeclarativeDebugConnection;
class QDeclarativeDebugWatch;
class QDeclarativeDebugPropertyWatch;
class QDeclarativeDebugObjectExpressionWatch;
class QDeclarativeDebugEnginesQuery;
class QDeclarativeDebugRootContextQuery;
class QDeclarativeDebugObjectQuery;
class QDeclarativeDebugExpressionQuery;
class QDeclarativeDebugPropertyReference;
class QDeclarativeDebugContextReference;
class QDeclarativeDebugObjectReference;
class QDeclarativeDebugFileReference;
class QDeclarativeDebugEngineReference;
class QDeclarativeEngineDebugPrivate;
class Q_DECLARATIVE_EXPORT QDeclarativeEngineDebug : public QObject
{
Q_OBJECT
public:
    enum Status { NotConnected, Unavailable, Enabled };

    explicit QDeclarativeEngineDebug(QDeclarativeDebugConnection *, QObject * = 0);

    Status status() const;

    QDeclarativeDebugPropertyWatch *addWatch(const QDeclarativeDebugPropertyReference &,
                            QObject *parent = 0);
    QDeclarativeDebugWatch *addWatch(const QDeclarativeDebugContextReference &, const QString &,
                            QObject *parent = 0);
    QDeclarativeDebugObjectExpressionWatch *addWatch(const QDeclarativeDebugObjectReference &, const QString &,
                            QObject *parent = 0);
    QDeclarativeDebugWatch *addWatch(const QDeclarativeDebugObjectReference &,
                            QObject *parent = 0);
    QDeclarativeDebugWatch *addWatch(const QDeclarativeDebugFileReference &,
                            QObject *parent = 0);

    void removeWatch(QDeclarativeDebugWatch *watch);

    QDeclarativeDebugEnginesQuery *queryAvailableEngines(QObject *parent = 0);
    QDeclarativeDebugRootContextQuery *queryRootContexts(const QDeclarativeDebugEngineReference &,
                                                QObject *parent = 0);
    QDeclarativeDebugObjectQuery *queryObject(const QDeclarativeDebugObjectReference &, 
                                     QObject *parent = 0);
    QDeclarativeDebugObjectQuery *queryObjectRecursive(const QDeclarativeDebugObjectReference &, 
                                              QObject *parent = 0);
    QDeclarativeDebugExpressionQuery *queryExpressionResult(int objectDebugId, 
                                                   const QString &expr,
                                                   QObject *parent = 0);
    bool setBindingForObject(int objectDebugId, const QString &propertyName,
                             const QVariant &bindingExpression, bool isLiteralValue);
    bool resetBindingForObject(int objectDebugId, const QString &propertyName);
    bool setMethodBody(int objectDebugId, const QString &methodName, const QString &methodBody);

Q_SIGNALS:
    void newObjects();
    void statusChanged(Status status);

private:
    Q_DECLARE_PRIVATE(QDeclarativeEngineDebug)
};

class Q_DECLARATIVE_EXPORT QDeclarativeDebugWatch : public QObject
{
Q_OBJECT
public:
    enum State { Waiting, Active, Inactive, Dead };

    QDeclarativeDebugWatch(QObject *);
    ~QDeclarativeDebugWatch();

    int queryId() const;
    int objectDebugId() const;
    State state() const;

Q_SIGNALS:
    void stateChanged(QDeclarativeDebugWatch::State);
    //void objectChanged(int, const QDeclarativeDebugObjectReference &);
    //void valueChanged(int, const QVariant &);

    // Server sends value as string if it is a user-type variant
    void valueChanged(const QByteArray &name, const QVariant &value);

private:
    friend class QDeclarativeEngineDebug;
    friend class QDeclarativeEngineDebugPrivate;
    void setState(State);
    State m_state;
    int m_queryId;
    QDeclarativeEngineDebug *m_client;
    int m_objectDebugId;
};

class Q_DECLARATIVE_EXPORT QDeclarativeDebugPropertyWatch : public QDeclarativeDebugWatch
{
    Q_OBJECT
public:
    QDeclarativeDebugPropertyWatch(QObject *parent);

    QString name() const;

private:
    friend class QDeclarativeEngineDebug;
    QString m_name;
};

class Q_DECLARATIVE_EXPORT QDeclarativeDebugObjectExpressionWatch : public QDeclarativeDebugWatch
{
    Q_OBJECT
public:
    QDeclarativeDebugObjectExpressionWatch(QObject *parent);

    QString expression() const;

private:
    friend class QDeclarativeEngineDebug;
    QString m_expr;
    int m_debugId;
};


class Q_DECLARATIVE_EXPORT QDeclarativeDebugQuery : public QObject
{
Q_OBJECT
public:
    enum State { Waiting, Error, Completed };

    State state() const;
    bool isWaiting() const;

//    bool waitUntilCompleted();

Q_SIGNALS:
    void stateChanged(QDeclarativeDebugQuery::State);

protected:
    QDeclarativeDebugQuery(QObject *);

private:
    friend class QDeclarativeEngineDebug;
    friend class QDeclarativeEngineDebugPrivate;
    void setState(State);
    State m_state;
};

class Q_DECLARATIVE_EXPORT QDeclarativeDebugFileReference 
{
public:
    QDeclarativeDebugFileReference();
    QDeclarativeDebugFileReference(const QDeclarativeDebugFileReference &);
    QDeclarativeDebugFileReference &operator=(const QDeclarativeDebugFileReference &);

    QUrl url() const;
    void setUrl(const QUrl &);
    int lineNumber() const;
    void setLineNumber(int);
    int columnNumber() const;
    void setColumnNumber(int);

private:
    friend class QDeclarativeEngineDebugPrivate;
    QUrl m_url;
    int m_lineNumber;
    int m_columnNumber;
};

class Q_DECLARATIVE_EXPORT QDeclarativeDebugEngineReference
{
public:
    QDeclarativeDebugEngineReference();
    QDeclarativeDebugEngineReference(int);
    QDeclarativeDebugEngineReference(const QDeclarativeDebugEngineReference &);
    QDeclarativeDebugEngineReference &operator=(const QDeclarativeDebugEngineReference &);

    int debugId() const;
    QString name() const;

private:
    friend class QDeclarativeEngineDebugPrivate;
    int m_debugId;
    QString m_name;
};

class Q_DECLARATIVE_EXPORT QDeclarativeDebugObjectReference
{
public:
    QDeclarativeDebugObjectReference();
    QDeclarativeDebugObjectReference(int);
    QDeclarativeDebugObjectReference(const QDeclarativeDebugObjectReference &);
    QDeclarativeDebugObjectReference &operator=(const QDeclarativeDebugObjectReference &);

    int debugId() const;
    QString className() const;
    QString idString() const;
    QString name() const;

    QDeclarativeDebugFileReference source() const;
    int contextDebugId() const;

    QList<QDeclarativeDebugPropertyReference> properties() const;
    QList<QDeclarativeDebugObjectReference> children() const;

private:
    friend class QDeclarativeEngineDebugPrivate;
    int m_debugId;
    QString m_class;
    QString m_idString;
    QString m_name;
    QDeclarativeDebugFileReference m_source;
    int m_contextDebugId;
    QList<QDeclarativeDebugPropertyReference> m_properties;
    QList<QDeclarativeDebugObjectReference> m_children;
};

class Q_DECLARATIVE_EXPORT QDeclarativeDebugContextReference
{
public:
    QDeclarativeDebugContextReference();
    QDeclarativeDebugContextReference(const QDeclarativeDebugContextReference &);
    QDeclarativeDebugContextReference &operator=(const QDeclarativeDebugContextReference &);

    int debugId() const;
    QString name() const;

    QList<QDeclarativeDebugObjectReference> objects() const;
    QList<QDeclarativeDebugContextReference> contexts() const;

private:
    friend class QDeclarativeEngineDebugPrivate;
    int m_debugId;
    QString m_name;
    QList<QDeclarativeDebugObjectReference> m_objects;
    QList<QDeclarativeDebugContextReference> m_contexts;
};

class Q_DECLARATIVE_EXPORT QDeclarativeDebugPropertyReference
{
public:
    QDeclarativeDebugPropertyReference();
    QDeclarativeDebugPropertyReference(const QDeclarativeDebugPropertyReference &);
    QDeclarativeDebugPropertyReference &operator=(const QDeclarativeDebugPropertyReference &);

    int objectDebugId() const;
    QString name() const;
    QVariant value() const;
    QString valueTypeName() const;
    QString binding() const;
    bool hasNotifySignal() const;

private:
    friend class QDeclarativeEngineDebugPrivate;
    int m_objectDebugId;
    QString m_name;
    QVariant m_value;
    QString m_valueTypeName;
    QString m_binding;
    bool m_hasNotifySignal;
};


class Q_DECLARATIVE_EXPORT QDeclarativeDebugEnginesQuery : public QDeclarativeDebugQuery
{
Q_OBJECT
public:
    virtual ~QDeclarativeDebugEnginesQuery();
    QList<QDeclarativeDebugEngineReference> engines() const;
private:
    friend class QDeclarativeEngineDebug;
    friend class QDeclarativeEngineDebugPrivate;
    QDeclarativeDebugEnginesQuery(QObject *);
    QDeclarativeEngineDebug *m_client;
    int m_queryId;
    QList<QDeclarativeDebugEngineReference> m_engines;
};

class Q_DECLARATIVE_EXPORT QDeclarativeDebugRootContextQuery : public QDeclarativeDebugQuery
{
Q_OBJECT
public:
    virtual ~QDeclarativeDebugRootContextQuery();
    QDeclarativeDebugContextReference rootContext() const;
private:
    friend class QDeclarativeEngineDebug;
    friend class QDeclarativeEngineDebugPrivate;
    QDeclarativeDebugRootContextQuery(QObject *);
    QDeclarativeEngineDebug *m_client;
    int m_queryId;
    QDeclarativeDebugContextReference m_context;
};

class Q_DECLARATIVE_EXPORT QDeclarativeDebugObjectQuery : public QDeclarativeDebugQuery
{
Q_OBJECT
public:
    virtual ~QDeclarativeDebugObjectQuery();
    QDeclarativeDebugObjectReference object() const;
private:
    friend class QDeclarativeEngineDebug;
    friend class QDeclarativeEngineDebugPrivate;
    QDeclarativeDebugObjectQuery(QObject *);
    QDeclarativeEngineDebug *m_client;
    int m_queryId;
    QDeclarativeDebugObjectReference m_object;

};

class Q_DECLARATIVE_EXPORT QDeclarativeDebugExpressionQuery : public QDeclarativeDebugQuery
{
Q_OBJECT
public:
    virtual ~QDeclarativeDebugExpressionQuery();
    QVariant expression() const;
    QVariant result() const;
private:
    friend class QDeclarativeEngineDebug;
    friend class QDeclarativeEngineDebugPrivate;
    QDeclarativeDebugExpressionQuery(QObject *);
    QDeclarativeEngineDebug *m_client;
    int m_queryId;
    QVariant m_expr;
    QVariant m_result;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QDeclarativeDebugEngineReference)
Q_DECLARE_METATYPE(QDeclarativeDebugObjectReference)
Q_DECLARE_METATYPE(QDeclarativeDebugContextReference)
Q_DECLARE_METATYPE(QDeclarativeDebugPropertyReference)

QT_END_HEADER

#endif // QDECLARATIVEDEBUG_H
