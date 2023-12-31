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

#include "qxcbwmsupport.h"
#include "qxcbscreen.h"

#include <qdebug.h>

QT_BEGIN_NAMESPACE

QXcbWMSupport::QXcbWMSupport(QXcbConnection *c)
    : QXcbObject(c)
{
    updateNetWMAtoms();
    updateVirtualRoots();
}

bool QXcbWMSupport::isSupportedByWM(xcb_atom_t atom) const
{
    return net_wm_atoms.contains(atom);
}



void QXcbWMSupport::updateNetWMAtoms()
{
    net_wm_atoms.clear();

    xcb_window_t root = connection()->primaryScreen()->root();
    int offset = 0;
    int remaining = 0;
    do {
        auto reply = Q_XCB_REPLY(xcb_get_property, xcb_connection(), false, root, atom(QXcbAtom::_NET_SUPPORTED), XCB_ATOM_ATOM, offset, 1024);
        if (!reply)
            break;

        remaining = 0;

        if (reply->type == XCB_ATOM_ATOM && reply->format == 32) {
            int len = xcb_get_property_value_length(reply.get())/sizeof(xcb_atom_t);
            xcb_atom_t *atoms = (xcb_atom_t *)xcb_get_property_value(reply.get());
            int s = net_wm_atoms.size();
            net_wm_atoms.resize(s + len);
            memcpy(net_wm_atoms.data() + s, atoms, len*sizeof(xcb_atom_t));

            remaining = reply->bytes_after;
            offset += len;
        }
    } while (remaining > 0);
}

// update the virtual roots array
void QXcbWMSupport::updateVirtualRoots()
{
    net_virtual_roots.clear();

    if (!isSupportedByWM(atom(QXcbAtom::_NET_VIRTUAL_ROOTS)))
        return;

    xcb_window_t root = connection()->primaryScreen()->root();
    int offset = 0;
    int remaining = 0;
    do {
        auto reply = Q_XCB_REPLY(xcb_get_property, xcb_connection(),
                                 false, root, atom(QXcbAtom::_NET_VIRTUAL_ROOTS), XCB_ATOM_WINDOW, offset, 1024);
        if (!reply)
            break;

        remaining = 0;

        if (reply->type == XCB_ATOM_WINDOW && reply->format == 32) {
            int len = xcb_get_property_value_length(reply.get())/sizeof(xcb_window_t);
            xcb_window_t *roots = (xcb_window_t *)xcb_get_property_value(reply.get());
            int s = net_virtual_roots.size();
            net_virtual_roots.resize(s + len);
            memcpy(net_virtual_roots.data() + s, roots, len*sizeof(xcb_window_t));

            remaining = reply->bytes_after;
            offset += len;
        }

    } while (remaining > 0);

//#define VIRTUAL_ROOTS_DEBUG
#ifdef VIRTUAL_ROOTS_DEBUG
    qDebug("======== updateVirtualRoots");
    for (int i = 0; i < net_virtual_roots.size(); ++i)
        qDebug() << connection()->atomName(net_virtual_roots.at(i));
    qDebug("======== updateVirtualRoots");
#endif
}

QT_END_NAMESPACE
