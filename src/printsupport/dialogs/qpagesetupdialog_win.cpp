/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qpagesetupdialog.h"

#include <qapplication.h>

#include "../kernel/qprintengine_win_p.h"
#include "qpagesetupdialog_p.h"
#include "qprinter.h"
#include <qpa/qplatformnativeinterface.h>

QT_BEGIN_NAMESPACE

QPageSetupDialog::QPageSetupDialog(QPrinter *printer, QWidget *parent)
    : QDialog(*(new QPageSetupDialogPrivate(printer)), parent)
{
    setWindowTitle(QCoreApplication::translate("QPrintPreviewDialog", "Page Setup"));
    setAttribute(Qt::WA_DontShowOnScreen);
}

QPageSetupDialog::QPageSetupDialog(QWidget *parent)
    : QDialog(*(new QPageSetupDialogPrivate(0)), parent)
{
    setWindowTitle(QCoreApplication::translate("QPrintPreviewDialog", "Page Setup"));
    setAttribute(Qt::WA_DontShowOnScreen);
}

int QPageSetupDialog::exec()
{
    Q_D(QPageSetupDialog);

    if (d->printer->outputFormat() != QPrinter::NativeFormat)
        return Rejected;

    QWin32PrintEngine *engine = static_cast<QWin32PrintEngine*>(d->printer->paintEngine());
    QWin32PrintEnginePrivate *ep = static_cast<QWin32PrintEnginePrivate *>(engine->d_ptr.data());

    PAGESETUPDLG psd;
    memset(&psd, 0, sizeof(PAGESETUPDLG));
    psd.lStructSize = sizeof(PAGESETUPDLG);

    // we need a temp DEVMODE struct if we don't have a global DEVMODE
    HGLOBAL hDevMode = 0;
    int devModeSize = 0;
    if (!engine->globalDevMode()) {
        devModeSize = sizeof(DEVMODE) + ep->devMode->dmDriverExtra;
        hDevMode = GlobalAlloc(GHND, devModeSize);
        if (hDevMode) {
            void *dest = GlobalLock(hDevMode);
            memcpy(dest, ep->devMode, devModeSize);
            GlobalUnlock(hDevMode);
        }
        psd.hDevMode = hDevMode;
    } else {
        psd.hDevMode = engine->globalDevMode();
    }

    HGLOBAL *tempDevNames = engine->createGlobalDevNames();
    psd.hDevNames = tempDevNames;

    QWidget *parent = parentWidget();
    parent = parent ? parent->window() : QApplication::activeWindow();
    Q_ASSERT(!parent ||parent->testAttribute(Qt::WA_WState_Created));

    QWindow *parentWindow = parent ? parent->windowHandle() : 0;
    psd.hwndOwner = parentWindow ? (HWND)QGuiApplication::platformNativeInterface()->nativeResourceForWindow("handle", parentWindow) : 0;

    psd.Flags = PSD_MARGINS;
    QPageLayout layout = d->printer->pageLayout();
    switch (layout.units()) {
    case QPageLayout::Millimeter:
    case QPageLayout::Inch:
        break;
    case QPageLayout::Point:
    case QPageLayout::Pica:
    case QPageLayout::Didot:
    case QPageLayout::Cicero:
        layout.setUnits(QLocale::system().measurementSystem() == QLocale::MetricSystem ? QPageLayout::Millimeter
                                                                                       : QPageLayout::Inch);
        break;
    }
    qreal multiplier = 1.0;
    if (layout.units() == QPageLayout::Millimeter) {
        psd.Flags |= PSD_INHUNDREDTHSOFMILLIMETERS;
        multiplier = 100.0;
    } else { // QPageLayout::Inch)
        psd.Flags |= PSD_INTHOUSANDTHSOFINCHES;
        multiplier = 1000.0;
    }
    psd.rtMargin.left   = layout.margins().left() * multiplier;
    psd.rtMargin.top    = layout.margins().top() * multiplier;
    psd.rtMargin.right  = layout.margins().right() * multiplier;
    psd.rtMargin.bottom = layout.margins().bottom() * multiplier;

    QDialog::setVisible(true);
    bool result = PageSetupDlg(&psd);
    QDialog::setVisible(false);
    if (result) {
        engine->setGlobalDevMode(psd.hDevNames, psd.hDevMode);
        QPageSize pageSize;
        // try to read orientation and paper size ID from the dialog's devmode struct
        if (psd.hDevMode) {
            DEVMODE *rDevmode = reinterpret_cast<DEVMODE*>(GlobalLock(psd.hDevMode));
            if (rDevmode->dmFields & DM_ORIENTATION) {
                layout.setOrientation(rDevmode->dmOrientation == DMORIENT_PORTRAIT
                                      ? QPageLayout::Portrait : QPageLayout::Landscape);
            }
            if (rDevmode->dmFields & DM_PAPERSIZE)
                pageSize = QPageSize::id(rDevmode->dmPaperSize);
            GlobalUnlock(rDevmode);
        }
        // fall back to use our own matching, and assume that paper that's wider than long means landscape
        if (!pageSize.isValid() || pageSize.id() == QPageSize::Custom) {
            QSizeF unitSize(psd.ptPaperSize.x / multiplier, psd.ptPaperSize.y / multiplier);
            if (unitSize.width() > unitSize.height()) {
                layout.setOrientation(QPageLayout::Landscape);
                unitSize.transpose();
            } else {
                layout.setOrientation(QPageLayout::Portrait);
            }
            pageSize = QPageSize(unitSize, layout.units() == QPageLayout::Inch
                                                           ? QPageSize::Inch : QPageSize::Millimeter);
        }
        layout.setPageSize(pageSize);

        const QMarginsF margins(psd.rtMargin.left, psd.rtMargin.top, psd.rtMargin.right, psd.rtMargin.bottom);
        layout.setMargins(margins / multiplier);
        d->printer->setPageLayout(layout);

        // copy from our temp DEVMODE struct
        if (!engine->globalDevMode() && hDevMode) {
            // Make sure memory is allocated
            if (ep->ownsDevMode && ep->devMode)
                free(ep->devMode);
            ep->devMode = (DEVMODE *) malloc(devModeSize);
            ep->ownsDevMode = true;

            // Copy
            void *src = GlobalLock(hDevMode);
            memcpy(ep->devMode, src, devModeSize);
            GlobalUnlock(hDevMode);
        }
    }

    if (!engine->globalDevMode() && hDevMode)
        GlobalFree(hDevMode);
    GlobalFree(tempDevNames);
    done(result);
    return result;
}

void QPageSetupDialog::setVisible(bool visible)
{
    if (!visible)
        return;
    exec();
}

QT_END_NAMESPACE
