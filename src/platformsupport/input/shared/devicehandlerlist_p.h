/****************************************************************************
**
** Copyright (C) 2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com, author Marc Mutz <marc.mutz@kdab.com>
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

#ifndef QTINPUTSUPPORT_DEVICEHANDLERLIST_P_H
#define QTINPUTSUPPORT_DEVICEHANDLERLIST_P_H

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

#include <QString>

#include <vector>
#include <memory>

namespace QtInputSupport {

template <typename Handler>
class DeviceHandlerList {
public:
    struct Device {
        QString deviceNode;
        std::unique_ptr<Handler> handler;
    };

    void add(const QString &deviceNode, std::unique_ptr<Handler> handler)
    {
        v.push_back({deviceNode, std::move(handler)});
    }

    bool remove(const QString &deviceNode)
    {
        const auto deviceNodeMatches = [&] (const Device &d) { return d.deviceNode == deviceNode; };
        const auto it = std::find_if(v.cbegin(), v.cend(), deviceNodeMatches);
        if (it == v.cend())
            return false;
        v.erase(it);
        return true;
    }

    int count() const noexcept { return static_cast<int>(v.size()); }

    typename std::vector<Device>::const_iterator begin() const noexcept { return v.begin(); }
    typename std::vector<Device>::const_iterator end() const  noexcept { return v.end(); }

private:
    std::vector<Device> v;
};

} // QtInputSupport

#endif // QTINPUTSUPPORT_DEVICEHANDLERLIST_P_H
