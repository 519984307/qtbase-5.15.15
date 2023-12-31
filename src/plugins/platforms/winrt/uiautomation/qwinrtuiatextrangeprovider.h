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

#ifndef QWINRTUIATEXTRANGEPROVIDER_H
#define QWINRTUIATEXTRANGEPROVIDER_H

#include <QtGui/qtguiglobal.h>
#if QT_CONFIG(accessibility)

#include "qwinrtuiabaseprovider.h"

#include <wrl.h>
#include <windows.ui.xaml.h>

QT_BEGIN_NAMESPACE

// Implements the Text Range control pattern provider. Used for text controls.
class QWinRTUiaTextRangeProvider :
        public QWinRTUiaBaseProvider,
        public Microsoft::WRL::RuntimeClass<ABI::Windows::UI::Xaml::Automation::Provider::ITextRangeProvider>
{
    Q_OBJECT
    Q_DISABLE_COPY(QWinRTUiaTextRangeProvider)
    InspectableClass(L"QWinRTUiaTextRangeProvider", BaseTrust);

public:
    explicit QWinRTUiaTextRangeProvider(QAccessible::Id id, int startOffset, int endOffset);
    virtual ~QWinRTUiaTextRangeProvider();

    // ITextRangeProvider
    HRESULT STDMETHODCALLTYPE Clone(ABI::Windows::UI::Xaml::Automation::Provider::ITextRangeProvider **returnValue) override;
    HRESULT STDMETHODCALLTYPE Compare(ABI::Windows::UI::Xaml::Automation::Provider::ITextRangeProvider *textRangeProvider, boolean *returnValue) override;
    HRESULT STDMETHODCALLTYPE CompareEndpoints(ABI::Windows::UI::Xaml::Automation::Text::TextPatternRangeEndpoint endpoint, ABI::Windows::UI::Xaml::Automation::Provider::ITextRangeProvider *textRangeProvider, ABI::Windows::UI::Xaml::Automation::Text::TextPatternRangeEndpoint targetEndpoint, INT32 *returnValue) override;
    HRESULT STDMETHODCALLTYPE ExpandToEnclosingUnit(ABI::Windows::UI::Xaml::Automation::Text::TextUnit unit) override;
    HRESULT STDMETHODCALLTYPE FindAttribute(INT32 attributeId, IInspectable *value, boolean backward, ABI::Windows::UI::Xaml::Automation::Provider::ITextRangeProvider **returnValue) override;
    HRESULT STDMETHODCALLTYPE FindText(HSTRING text, boolean backward, boolean ignoreCase, ABI::Windows::UI::Xaml::Automation::Provider::ITextRangeProvider **returnValue) override;
    HRESULT STDMETHODCALLTYPE GetAttributeValue(INT32 attributeId, IInspectable **returnValue) override;
    HRESULT STDMETHODCALLTYPE GetBoundingRectangles(UINT32 *returnValueSize, DOUBLE **returnValue) override;
    HRESULT STDMETHODCALLTYPE GetEnclosingElement(ABI::Windows::UI::Xaml::Automation::Provider::IIRawElementProviderSimple **returnValue) override;
    HRESULT STDMETHODCALLTYPE GetText(INT32 maxLength, HSTRING *returnValue) override;
    HRESULT STDMETHODCALLTYPE Move(ABI::Windows::UI::Xaml::Automation::Text::TextUnit unit, INT32 count, INT32 *returnValue) override;
    HRESULT STDMETHODCALLTYPE MoveEndpointByUnit(ABI::Windows::UI::Xaml::Automation::Text::TextPatternRangeEndpoint endpoint, ABI::Windows::UI::Xaml::Automation::Text::TextUnit unit, INT32 count, INT32 *returnValue) override;
    HRESULT STDMETHODCALLTYPE MoveEndpointByRange(ABI::Windows::UI::Xaml::Automation::Text::TextPatternRangeEndpoint endpoint, ABI::Windows::UI::Xaml::Automation::Provider::ITextRangeProvider *textRangeProvider, ABI::Windows::UI::Xaml::Automation::Text::TextPatternRangeEndpoint targetEndpoint) override;
    HRESULT STDMETHODCALLTYPE Select() override;
    HRESULT STDMETHODCALLTYPE AddToSelection() override;
    HRESULT STDMETHODCALLTYPE RemoveFromSelection() override;
    HRESULT STDMETHODCALLTYPE ScrollIntoView(boolean alignToTop) override;
    HRESULT STDMETHODCALLTYPE GetChildren(UINT32 *returnValueSize, ABI::Windows::UI::Xaml::Automation::Provider::IIRawElementProviderSimple ***returnValue) override;

private:
    int m_startOffset;
    int m_endOffset;
};

QT_END_NAMESPACE

#endif // QT_CONFIG(accessibility)

#endif // QWINRTUIATEXTRANGEPROVIDER_H
