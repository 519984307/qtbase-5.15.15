/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtOpenGL module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

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

#ifndef QGL2PEXVERTEXARRAY_P_H
#define QGL2PEXVERTEXARRAY_P_H

#include <QRectF>

#include <private/qdatabuffer_p.h>
#include <private/qvectorpath_p.h>
#include <private/qgl_p.h>

QT_BEGIN_NAMESPACE

class QGLPoint
{
public:
    QGLPoint(GLfloat new_x, GLfloat new_y) :
        x(new_x), y(new_y) {};

    QGLPoint(const QPointF &p) :
        x(p.x()), y(p.y()) {};

    QGLPoint(const QPointF* p) :
        x(p->x()), y(p->y()) {};

    GLfloat x;
    GLfloat y;

    operator QPointF() {return QPointF(x,y);}
    operator QPointF() const {return QPointF(x,y);}
};

struct QGLRect
{
    QGLRect(const QRectF &r)
        :  left(r.left()), top(r.top()), right(r.right()), bottom(r.bottom()) {}

    QGLRect(GLfloat l, GLfloat t, GLfloat r, GLfloat b)
        : left(l), top(t), right(r), bottom(b) {}

    GLfloat left;
    GLfloat top;
    GLfloat right;
    GLfloat bottom;

    operator QRectF() const {return QRectF(left, top, right-left, bottom-top);}
};

class QGL2PEXVertexArray
{
public:
    QGL2PEXVertexArray() :
        vertexArray(0), vertexArrayStops(0),
        maxX(-2e10), maxY(-2e10), minX(2e10), minY(2e10),
        boundingRectDirty(true)
    { }

    inline void addRect(const QRectF &rect)
    {
        qreal top = rect.top();
        qreal left = rect.left();
        qreal bottom = rect.bottom();
        qreal right = rect.right();

        vertexArray << QGLPoint(left, top)
                    << QGLPoint(right, top)
                    << QGLPoint(right, bottom)
                    << QGLPoint(right, bottom)
                    << QGLPoint(left, bottom)
                    << QGLPoint(left, top);
    }

    inline void addQuad(const QRectF &rect)
    {
        qreal top = rect.top();
        qreal left = rect.left();
        qreal bottom = rect.bottom();
        qreal right = rect.right();

        vertexArray << QGLPoint(left, top)
                    << QGLPoint(right, top)
                    << QGLPoint(left, bottom)
                    << QGLPoint(right, bottom);

    }

    inline void addVertex(const GLfloat x, const GLfloat y)
    {
        vertexArray.add(QGLPoint(x, y));
    }

    void addPath(const QVectorPath &path, GLfloat curveInverseScale, bool outline = true);
    void clear();

    QGLPoint*        data() {return vertexArray.data();}
    int *stops() const { return vertexArrayStops.data(); }
    int stopCount() const { return vertexArrayStops.size(); }
    QGLRect         boundingRect() const;

    int vertexCount() const { return vertexArray.size(); }

    void lineToArray(const GLfloat x, const GLfloat y);

private:
    QDataBuffer<QGLPoint> vertexArray;
    QDataBuffer<int>      vertexArrayStops;

    GLfloat     maxX;
    GLfloat     maxY;
    GLfloat     minX;
    GLfloat     minY;
    bool        boundingRectDirty;
    void addClosingLine(int index);
    void addCentroid(const QVectorPath &path, int subPathIndex);
};

QT_END_NAMESPACE

#endif
