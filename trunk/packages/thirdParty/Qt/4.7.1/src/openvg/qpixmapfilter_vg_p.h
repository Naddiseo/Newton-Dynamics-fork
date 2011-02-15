/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenVG module of the Qt Toolkit.
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

#ifndef QPIXMAPFILTER_VG_P_H
#define QPIXMAPFILTER_VG_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qpixmapdata_vg_p.h"
#include <QtGui/private/qpixmapfilter_p.h>
#include <QtCore/qvarlengtharray.h>

QT_BEGIN_NAMESPACE

#if !defined(QT_SHIVAVG)

class QVGPixmapConvolutionFilter : public QPixmapConvolutionFilter
{
    Q_OBJECT
public:
    QVGPixmapConvolutionFilter();
    ~QVGPixmapConvolutionFilter();

    void draw(QPainter *painter, const QPointF &dest, const QPixmap &src, const QRectF &srcRect) const;
};

class QVGPixmapColorizeFilter : public QPixmapColorizeFilter
{
    Q_OBJECT
public:
    QVGPixmapColorizeFilter();
    ~QVGPixmapColorizeFilter();

    void draw(QPainter *painter, const QPointF &dest, const QPixmap &src, const QRectF &srcRect) const;
};

class QVGPixmapDropShadowFilter : public QPixmapDropShadowFilter
{
    Q_OBJECT
public:
    QVGPixmapDropShadowFilter();
    ~QVGPixmapDropShadowFilter();

    void draw(QPainter *p, const QPointF &pos, const QPixmap &px, const QRectF &src) const;
};

class QVGPixmapBlurFilter : public QPixmapBlurFilter
{
    Q_OBJECT
public:
    QVGPixmapBlurFilter(QObject *parent = 0);
    ~QVGPixmapBlurFilter();

    void draw(QPainter *painter, const QPointF &dest, const QPixmap &src, const QRectF &srcRect = QRectF()) const;
};

#endif

QT_END_NAMESPACE

#endif
