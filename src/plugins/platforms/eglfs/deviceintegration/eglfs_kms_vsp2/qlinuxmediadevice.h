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

#ifndef QLINUXMEDIADEVICE_H
#define QLINUXMEDIADEVICE_H

#include <linux/v4l2-mediabus.h>

#include <QtCore/qglobal.h>
#include <QtCore/QString>

QT_BEGIN_NAMESPACE

class QSize;
class QRect;

class QLinuxMediaDevice
{
public:
    QLinuxMediaDevice(const QString &devicePath);
    ~QLinuxMediaDevice();

    QString model();
    QString deviceName();
    bool resetLinks();
    struct media_link *parseLink(const QString &link);
    struct media_pad *parsePad(const QString &pad);
    bool enableLink(struct media_link *link);
    bool disableLink(struct media_link *link);
    struct media_entity *getEntity(const QString &name);
    int openVideoDevice(const QString &name);

    class CaptureSubDevice
    {
    public:
        CaptureSubDevice(QLinuxMediaDevice *mediaDevice, const QString &name);
        bool setFormat(const QSize &size, uint32_t pixelFormat = V4L2_PIX_FMT_ABGR32); //TODO: fix to match output device
        bool clearBuffers();
        bool requestBuffer();
        bool queueBuffer(int dmabufFd, const QSize &bufferSize);
        bool dequeueBuffer();
        bool streamOn();
        bool streamOff();
    private:
        int m_subdevFd = -1;
    };

    class OutputSubDevice
    {
    public:
        OutputSubDevice(QLinuxMediaDevice *mediaDevice, const QString &name);
        bool setFormat(const QSize &size, uint32_t pixelFormat, uint32_t bytesPerLine);
        bool clearBuffers();
        bool requestBuffer();
        bool queueBuffer(int dmabufFd, uint bytesUsed, uint length);
        bool streamOn();
        bool streamOff();
    private:
        int m_subdevFd = -1;
    };

    static int openVideoDevice(media_pad *pad);

    static bool setSubdevFormat(struct media_pad *pad, const QSize &size, uint32_t mbusFormat = MEDIA_BUS_FMT_ARGB8888_1X32);
    static bool setSubdevAlpha(int subdevFd, qreal alpha);

    static bool setSubdevSelection(struct media_pad *pad, const QRect &geometry, uint target);
    static bool setSubdevCrop(struct media_pad *pad, const QRect &geometry);
    static bool setSubdevCompose(struct media_pad *pad, const QRect &geometry);

private:
    static bool streamOn(int subDeviceFd, v4l2_buf_type bufferType);
    static bool streamOff(int subDeviceFd, v4l2_buf_type bufferType);
    struct media_device *m_mediaDevice = nullptr;
    const struct media_device_info *m_info = nullptr;
};

QT_END_NAMESPACE

#endif // QLINUXMEDIADEVICE_H
