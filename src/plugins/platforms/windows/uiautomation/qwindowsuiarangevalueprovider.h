/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the plugins of the Qt Toolkit.
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

#ifndef QWINDOWSUIARANGEVALUEPROVIDER_H
#define QWINDOWSUIARANGEVALUEPROVIDER_H

#include <QtGui/qtguiglobal.h>
#if QT_CONFIG(accessibility)

#include "qwindowsuiabaseprovider.h"

QT_BEGIN_NAMESPACE

// Implements the Range Value control pattern provider.
class QWindowsUiaRangeValueProvider : public QWindowsUiaBaseProvider,
                                      public QWindowsComBase<IRangeValueProvider>
{
    Q_DISABLE_COPY_MOVE(QWindowsUiaRangeValueProvider)
public:
    explicit QWindowsUiaRangeValueProvider(QAccessible::Id id);
    virtual ~QWindowsUiaRangeValueProvider();

    // IRangeValueProvider
    HRESULT STDMETHODCALLTYPE SetValue(double val) override;
    HRESULT STDMETHODCALLTYPE get_Value(double *pRetVal) override;
    HRESULT STDMETHODCALLTYPE get_IsReadOnly(BOOL *pRetVal) override;
    HRESULT STDMETHODCALLTYPE get_Maximum(double *pRetVal) override;
    HRESULT STDMETHODCALLTYPE get_Minimum(double *pRetVal) override;
    HRESULT STDMETHODCALLTYPE get_LargeChange(double *pRetVal) override;
    HRESULT STDMETHODCALLTYPE get_SmallChange(double *pRetVal) override;
};

QT_END_NAMESPACE

#endif // QT_CONFIG(accessibility)

#endif // QWINDOWSUIARANGEVALUEPROVIDER_H
