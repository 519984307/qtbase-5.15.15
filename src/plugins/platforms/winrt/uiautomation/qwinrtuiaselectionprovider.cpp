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

#include <QtGui/qtguiglobal.h>
#if QT_CONFIG(accessibility)

#include "qwinrtuiaselectionprovider.h"
#include "qwinrtuiametadatacache.h"
#include "qwinrtuiamainprovider.h"
#include "qwinrtuiautils.h"

#include <QtGui/QAccessible>
#include <QtGui/QAccessibleInterface>
#include <QtCore/QLoggingCategory>
#include <QtCore/QString>
#include <QtCore/private/qeventdispatcher_winrt_p.h>

#include <memory>

QT_BEGIN_NAMESPACE

using namespace QWinRTUiAutomation;
using namespace ABI::Windows::UI::Xaml::Automation;
using namespace ABI::Windows::UI::Xaml::Automation::Provider;
using namespace ABI::Windows::UI::Xaml::Automation::Peers;

QWinRTUiaSelectionProvider::QWinRTUiaSelectionProvider(QAccessible::Id id) :
    QWinRTUiaBaseProvider(id)
{
    qCDebug(lcQpaUiAutomation) << __FUNCTION__;
}

QWinRTUiaSelectionProvider::~QWinRTUiaSelectionProvider()
{
    qCDebug(lcQpaUiAutomation) << __FUNCTION__;
}

HRESULT STDMETHODCALLTYPE QWinRTUiaSelectionProvider::get_CanSelectMultiple(boolean *value)
{
    qCDebug(lcQpaUiAutomation) << __FUNCTION__;

    if (!value)
        return E_INVALIDARG;
    QSharedPointer<QWinRTUiaControlMetadata> metadata = QWinRTUiaMetadataCache::instance()->metadataForId(id());
    *value = (metadata->state().multiSelectable != 0);
    return S_OK;
}

HRESULT STDMETHODCALLTYPE QWinRTUiaSelectionProvider::get_IsSelectionRequired(boolean *value)
{
    qCDebug(lcQpaUiAutomation) << __FUNCTION__;

    if (!value)
        return E_INVALIDARG;
    *value = false;

    auto accid = id();
    auto selectionRequired = std::make_shared<bool>(false);

    if (!SUCCEEDED(QEventDispatcherWinRT::runOnMainThread([accid, selectionRequired]() {
        // Initially returns false if none are selected. After the first selection, it may be required.
        bool anySelected = false;
        if (QAccessibleInterface *accessible = accessibleForId(accid)) {
            int childCount = accessible->childCount();
            for (int i = 0; i < childCount; ++i) {
                if (QAccessibleInterface *childAcc = accessible->child(i)) {
                     if (childAcc->state().selected) {
                         anySelected = true;
                         break;
                     }
                }
            }
            *selectionRequired = anySelected && !accessible->state().multiSelectable && !accessible->state().extSelectable;
        }
        return S_OK;
    }))) {
        return E_FAIL;
    }

    *value = *selectionRequired;
    return S_OK;
}

// Returns an array of providers with the selected items.
HRESULT STDMETHODCALLTYPE QWinRTUiaSelectionProvider::GetSelection(UINT32 *returnValueSize, IIRawElementProviderSimple ***returnValue)
{
    qCDebug(lcQpaUiAutomation) << __FUNCTION__;

    if (!returnValueSize || !returnValue)
        return E_INVALIDARG;
    *returnValueSize = 0;
    *returnValue = nullptr;

    auto accid = id();
    auto elementIds = std::make_shared<QVarLengthArray<QAccessible::Id>>();

    if (!SUCCEEDED(QEventDispatcherWinRT::runOnMainThread([accid, elementIds]() {
        if (QAccessibleInterface *accessible = accessibleForId(accid)) {
            int childCount = accessible->childCount();
            for (int i = 0; i < childCount; ++i) {
                if (QAccessibleInterface *childAcc = accessible->child(i)) {
                     if (childAcc->state().selected) {
                         QAccessible::Id childId = idForAccessible(childAcc);
                         QWinRTUiaMetadataCache::instance()->load(childId);
                         elementIds->append(childId);
                     }
                }
            }
        }
        return S_OK;
    }))) {
        return E_FAIL;
    }

    return QWinRTUiaMainProvider::rawProviderArrayForAccessibleIdList(*elementIds, returnValueSize, returnValue);
}

QT_END_NAMESPACE

#endif // QT_CONFIG(accessibility)
