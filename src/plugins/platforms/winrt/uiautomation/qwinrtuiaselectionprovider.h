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

#ifndef QWINRTUIASELECTIONPROVIDER_H
#define QWINRTUIASELECTIONPROVIDER_H

#include <QtGui/qtguiglobal.h>
#if QT_CONFIG(accessibility)

#include "qwinrtuiabaseprovider.h"

#include <wrl.h>
#include <windows.ui.xaml.h>

QT_BEGIN_NAMESPACE

// Implements the Selection control pattern provider. Used for Lists.
class QWinRTUiaSelectionProvider :
        public QWinRTUiaBaseProvider,
        public Microsoft::WRL::RuntimeClass<ABI::Windows::UI::Xaml::Automation::Provider::ISelectionProvider>
{
    Q_OBJECT
    Q_DISABLE_COPY(QWinRTUiaSelectionProvider)
    InspectableClass(L"QWinRTUiaSelectionProvider", BaseTrust);

public:
    explicit QWinRTUiaSelectionProvider(QAccessible::Id id);
    virtual ~QWinRTUiaSelectionProvider();

    // ISelectionProvider
    HRESULT STDMETHODCALLTYPE get_CanSelectMultiple(boolean *value) override;
    HRESULT STDMETHODCALLTYPE get_IsSelectionRequired(boolean *value) override;
    HRESULT STDMETHODCALLTYPE GetSelection(UINT32 *returnValueSize, ABI::Windows::UI::Xaml::Automation::Provider::IIRawElementProviderSimple ***returnValue) override;
};

QT_END_NAMESPACE

#endif // QT_CONFIG(accessibility)

#endif // QWINRTUIASELECTIONPROVIDER_H
