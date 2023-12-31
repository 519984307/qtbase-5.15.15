#!/usr/bin/env python3

#############################################################################
##
## Copyright (C) 2021 The Qt Company Ltd.
## Contact: https://www.qt.io/licensing/
##
## This file is the build configuration utility of the Qt Toolkit.
##
## $QT_BEGIN_LICENSE:COMM$
##
## Commercial License Usage
## Licensees holding valid commercial Qt licenses may use this file in
## accordance with the commercial license agreement provided with the
## Software or, alternatively, in accordance with the terms contained in
## a written agreement between you and The Qt Company. For licensing terms
## and conditions see https://www.qt.io/terms-conditions. For further
## information use the contact form at https://www.qt.io/contact-us.
##
## $QT_END_LICENSE$
##
##
##
##
##
##
##
##
##
##
##
##
##
##
##
##
##
##
##
############################################################################

from __future__ import print_function

import argparse
import json
import subprocess
from distutils.version import StrictVersion

def is_available(object):
    if "isAvailable" in object:
        return object["isAvailable"]   # introduced in Xcode 11
    else:
        return "unavailable" not in object["availability"]

def is_suitable_runtime(runtimes, runtime_name, platform, min_version):
    for runtime in runtimes:
        identifier = runtime["identifier"]
        if (runtime["name"] == runtime_name or identifier == runtime_name) \
            and is_available(runtime) \
            and identifier.startswith("com.apple.CoreSimulator.SimRuntime.{}".format(platform)) \
            and StrictVersion(runtime["version"]) >= min_version:
            return True
    return False

def simctl_runtimes():
    return json.loads(subprocess.check_output(
        ["/usr/bin/xcrun", "simctl", "list", "runtimes", "--json"]))["runtimes"]

def simctl_devices():
    return json.loads(subprocess.check_output(
        ["/usr/bin/xcrun", "simctl", "list", "devices", "--json"]))["devices"]

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--platform', choices=['iOS', 'tvOS', 'watchOS'], required=True)
    parser.add_argument('--minimum-deployment-target', type=StrictVersion, default='0.0')
    parser.add_argument('--state',
        choices=['booted', 'shutdown', 'creating', 'booting', 'shutting-down'], action='append')
    args = parser.parse_args()

    runtimes = simctl_runtimes()
    device_dict = simctl_devices()
    for runtime_name in device_dict:
        if is_suitable_runtime(runtimes, runtime_name, args.platform, args.minimum_deployment_target):
            for device in device_dict[runtime_name]:
                if is_available(device) \
                    and (args.state is None or device["state"].lower() in args.state):
                    print(device["udid"])
