/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtWidgets module of the Qt Toolkit.
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

#ifndef QLABEL_P_H
#define QLABEL_P_H

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

#include <QtWidgets/private/qtwidgetsglobal_p.h>
#include "qlabel.h"

#include "private/qtextdocumentlayout_p.h"
#include "private/qwidgettextcontrol_p.h"
#include "qtextdocumentfragment.h"
#include "qframe_p.h"
#include "qtextdocument.h"
#if QT_CONFIG(movie)
#include "qmovie.h"
#endif
#include "qimage.h"
#include "qbitmap.h"
#include "qpicture.h"
#if QT_CONFIG(menu)
#include "qmenu.h"
#endif

QT_BEGIN_NAMESPACE

class Q_AUTOTEST_EXPORT QLabelPrivate : public QFramePrivate
{
    Q_DECLARE_PUBLIC(QLabel)
public:
    QLabelPrivate();
    ~QLabelPrivate();

    void init();
    void clearContents();
    void updateLabel();
    QSize sizeForWidth(int w) const;

#if QT_CONFIG(movie)
    void _q_movieUpdated(const QRect&);
    void _q_movieResized(const QSize&);
#endif
#ifndef QT_NO_SHORTCUT
    void updateShortcut();
    void _q_buddyDeleted();
#endif
    inline bool needTextControl() const {
        Q_Q(const QLabel);
        return isTextLabel
               && (effectiveTextFormat != Qt::PlainText
                   || (textInteractionFlags & (Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard))
                   || q->focusPolicy() != Qt::NoFocus);
    }

    void ensureTextPopulated() const;
    void ensureTextLayouted() const;
    void ensureTextControl() const;
    void sendControlEvent(QEvent *e);

    void _q_linkHovered(const QString &link);

    QRectF layoutRect() const;
    QRect documentRect() const;
    QPoint layoutPoint(const QPoint& p) const;
    Qt::LayoutDirection textDirection() const;
#ifndef QT_NO_CONTEXTMENU
    QMenu *createStandardContextMenu(const QPoint &pos);
#endif

    mutable QSize sh;
    mutable QSize msh;
    QString text;
    QPixmap  *pixmap;
    QPixmap *scaledpixmap;
    QImage *cachedimage;
#ifndef QT_NO_PICTURE
    QPicture *picture;
#endif
#if QT_CONFIG(movie)
    QPointer<QMovie> movie;
#endif
    mutable QWidgetTextControl *control;
    mutable QTextCursor shortcutCursor;
#ifndef QT_NO_CURSOR
    QCursor cursor;
#endif
#ifndef QT_NO_SHORTCUT
    QPointer<QWidget> buddy;
    int shortcutId;
#endif
    Qt::TextFormat textformat;
    Qt::TextFormat effectiveTextFormat;
    Qt::TextInteractionFlags textInteractionFlags;
    mutable QSizePolicy sizePolicy;
    int margin;
    ushort align;
    short indent;
    mutable uint valid_hints : 1;
    uint scaledcontents : 1;
    mutable uint textLayoutDirty : 1;
    mutable uint textDirty : 1;
    mutable uint isTextLabel : 1;
    mutable uint hasShortcut : 1;
#ifndef QT_NO_CURSOR
    uint validCursor : 1;
    uint onAnchor : 1;
#endif
    uint openExternalLinks : 1;
    // <-- space for more bit field values here

    friend class QMessageBoxPrivate;
};

QT_END_NAMESPACE

#endif // QLABEL_P_H
