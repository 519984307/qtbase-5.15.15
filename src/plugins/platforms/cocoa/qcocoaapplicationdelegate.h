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


/****************************************************************************
 **
 ** Copyright (c) 2007-2008, Apple, Inc.
 **
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are met:
 **
 **   * Redistributions of source code must retain the above copyright notice,
 **     this list of conditions and the following disclaimer.
 **
 **   * Redistributions in binary form must reproduce the above copyright notice,
 **     this list of conditions and the following disclaimer in the documentation
 **     and/or other materials provided with the distribution.
 **
 **   * Neither the name of Apple, Inc. nor the names of its contributors
 **     may be used to endorse or promote products derived from this software
 **     without specific prior written permission.
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 ** CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 ** EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 ** PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 ** PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 ** LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 ** NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 ** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **
 ****************************************************************************/

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of qapplication_*.cpp, qwidget*.cpp, qcolor_x11.cpp, qfiledialog.cpp
// and many other.  This header file may change from version to version
// without notice, or even be removed.
//
// We mean it.
//


#import <AppKit/AppKit.h>

#include <qglobal.h>
#include <private/qcore_mac_p.h>
#include "qcocoansmenu.h"

@interface QT_MANGLE_NAMESPACE(QCocoaApplicationDelegate) : NSObject <NSApplicationDelegate>
@property (nonatomic, retain) NSMenu *dockMenu;
+ (instancetype)sharedDelegate;
- (void)setReflectionDelegate:(NSObject<NSApplicationDelegate> *)oldDelegate;
- (void)removeAppleEventHandlers;
- (bool)inLaunch;
@end

QT_NAMESPACE_ALIAS_OBJC_CLASS(QCocoaApplicationDelegate);

@interface QCocoaApplicationDelegate (MenuAPI)
- (void)qt_itemFired:(QCocoaNSMenuItem *)item;
@end

