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

#include "qwindowsuiaselectionprovider.h"
#include "qwindowsuiamainprovider.h"
#include "qwindowsuiautils.h"
#include "qwindowscontext.h"

#include <QtGui/qaccessible.h>
#include <QtCore/qloggingcategory.h>
#include <QtCore/qstring.h>
#include <QtCore/qlist.h>
#include <QtCore/qvector.h>

QT_BEGIN_NAMESPACE

using namespace QWindowsUiAutomation;


QWindowsUiaSelectionProvider::QWindowsUiaSelectionProvider(QAccessible::Id id) :
    QWindowsUiaBaseProvider(id)
{
}

QWindowsUiaSelectionProvider::~QWindowsUiaSelectionProvider()
{
}

// Returns an array of providers with the selected items.
HRESULT STDMETHODCALLTYPE QWindowsUiaSelectionProvider::GetSelection(SAFEARRAY **pRetVal)
{
    qCDebug(lcQpaUiAutomation) << __FUNCTION__;

    if (!pRetVal)
        return E_INVALIDARG;
    *pRetVal = nullptr;

    QAccessibleInterface *accessible = accessibleInterface();
    if (!accessible)
        return UIA_E_ELEMENTNOTAVAILABLE;

    // First put selected items in a list, then build a safe array with the right size.
    QVector<QAccessibleInterface *> selectedList;
    for (int i = 0; i < accessible->childCount(); ++i) {
        if (QAccessibleInterface *child = accessible->child(i)) {
            if (child->state().selected) {
                selectedList.append(child);
            }
        }
    }

    if ((*pRetVal = SafeArrayCreateVector(VT_UNKNOWN, 0, selectedList.size()))) {
        for (LONG i = 0; i < selectedList.size(); ++i) {
            if (QWindowsUiaMainProvider *childProvider = QWindowsUiaMainProvider::providerForAccessible(selectedList.at(i))) {
                SafeArrayPutElement(*pRetVal, &i, static_cast<IRawElementProviderSimple *>(childProvider));
                childProvider->Release();
            }
        }
    }
    return S_OK;
}

HRESULT STDMETHODCALLTYPE QWindowsUiaSelectionProvider::get_CanSelectMultiple(BOOL *pRetVal)
{
    qCDebug(lcQpaUiAutomation) << __FUNCTION__;

    if (!pRetVal)
        return E_INVALIDARG;
    *pRetVal = FALSE;

    QAccessibleInterface *accessible = accessibleInterface();
    if (!accessible)
        return UIA_E_ELEMENTNOTAVAILABLE;

    *pRetVal = accessible->state().multiSelectable;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE QWindowsUiaSelectionProvider::get_IsSelectionRequired(BOOL *pRetVal)
{
    qCDebug(lcQpaUiAutomation) << __FUNCTION__;

    if (!pRetVal)
        return E_INVALIDARG;
    *pRetVal = FALSE;

    QAccessibleInterface *accessible = accessibleInterface();
    if (!accessible)
        return UIA_E_ELEMENTNOTAVAILABLE;

    // Initially returns false if none are selected. After the first selection, it may be required.
    bool anySelected = false;
    for (int i = 0; i < accessible->childCount(); ++i) {
        if (QAccessibleInterface *child = accessible->child(i)) {
            if (child->state().selected) {
                anySelected = true;
                break;
            }
        }
    }

    *pRetVal = anySelected && !accessible->state().multiSelectable && !accessible->state().extSelectable;
    return S_OK;
}

QT_END_NAMESPACE

#endif // QT_CONFIG(accessibility)
