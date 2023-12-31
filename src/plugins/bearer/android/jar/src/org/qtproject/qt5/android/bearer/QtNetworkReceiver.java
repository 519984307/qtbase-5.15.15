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

package org.qtproject.qt5.android.bearer;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.util.Log;
import android.net.ConnectivityManager;

public class QtNetworkReceiver
{
    private static final String LOG_TAG = "QtNetworkReceiver";
    private static native void activeNetworkInfoChanged();
    private static BroadcastReceiverPrivate m_broadcastReceiver = null;
    private static final Object m_lock = new Object();

    private static class BroadcastReceiverPrivate extends BroadcastReceiver
    {
        @Override
        public void onReceive(Context context, Intent intent)
        {
            activeNetworkInfoChanged();
        }
    }

    private QtNetworkReceiver() {}

    public static void registerReceiver(final Context context)
    {
        synchronized (m_lock) {
            if (m_broadcastReceiver == null) {
                m_broadcastReceiver = new BroadcastReceiverPrivate();
                IntentFilter intentFilter = new IntentFilter(ConnectivityManager.CONNECTIVITY_ACTION);
                context.registerReceiver(m_broadcastReceiver, intentFilter);
            }
        }
    }

    public static void unregisterReceiver(final Context context)
    {
        synchronized (m_lock) {
            if (m_broadcastReceiver == null)
                return;

            context.unregisterReceiver(m_broadcastReceiver);
        }
    }

    public static ConnectivityManager getConnectivityManager(final Context context)
    {
        return (ConnectivityManager)context.getSystemService(Context.CONNECTIVITY_SERVICE);
    }
}
