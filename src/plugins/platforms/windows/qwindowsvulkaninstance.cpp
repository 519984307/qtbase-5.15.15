/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of plugins of the Qt Toolkit.
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

#include "qwindowsvulkaninstance.h"

QT_BEGIN_NAMESPACE

QWindowsVulkanInstance::QWindowsVulkanInstance(QVulkanInstance *instance)
    : m_instance(instance),
      m_getPhysDevPresSupport(nullptr),
      m_createSurface(nullptr)
{
    loadVulkanLibrary(QStringLiteral("vulkan-1"));
}

void QWindowsVulkanInstance::createOrAdoptInstance()
{
    initInstance(m_instance, QByteArrayList() << QByteArrayLiteral("VK_KHR_win32_surface"));

    if (!m_vkInst)
        return;

    m_getPhysDevPresSupport = reinterpret_cast<PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR>(
                m_vkGetInstanceProcAddr(m_vkInst, "vkGetPhysicalDeviceWin32PresentationSupportKHR"));
    if (!m_getPhysDevPresSupport)
        qWarning("Failed to find vkGetPhysicalDeviceWin32PresentationSupportKHR");
}

bool QWindowsVulkanInstance::supportsPresent(VkPhysicalDevice physicalDevice,
                                             uint32_t queueFamilyIndex,
                                             QWindow *window)
{
    if (!m_getPhysDevPresSupport || !m_getPhysDevSurfaceSupport)
        return true;

    bool ok = m_getPhysDevPresSupport(physicalDevice, queueFamilyIndex);

    VkSurfaceKHR surface = QVulkanInstance::surfaceForWindow(window);
    VkBool32 supported = false;
    m_getPhysDevSurfaceSupport(physicalDevice, queueFamilyIndex, surface, &supported);
    ok &= bool(supported);

    return ok;
}

VkSurfaceKHR QWindowsVulkanInstance::createSurface(HWND win)
{
    VkSurfaceKHR surface = VK_NULL_HANDLE;

    if (!m_createSurface) {
        m_createSurface = reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(
                    m_vkGetInstanceProcAddr(m_vkInst, "vkCreateWin32SurfaceKHR"));
    }
    if (!m_createSurface) {
        qWarning("Failed to find vkCreateWin32SurfaceKHR");
        return surface;
    }

    VkWin32SurfaceCreateInfoKHR surfaceInfo;
    memset(&surfaceInfo, 0, sizeof(surfaceInfo));
    surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceInfo.hinstance = GetModuleHandle(nullptr);
    surfaceInfo.hwnd = win;
    VkResult err = m_createSurface(m_vkInst, &surfaceInfo, nullptr, &surface);
    if (err != VK_SUCCESS)
        qWarning("Failed to create Vulkan surface: %d", err);

    return surface;
}

QT_END_NAMESPACE
