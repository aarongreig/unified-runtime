<%!
import re
from templates import helper as th
%><%
maybe like
manifests = th.get_manifests(specs, namespace, tags)
??
%>/*
 *
 * Copyright (C) 2025 Intel Corporation
 *
 * Part of the Unified-Runtime Project, under the Apache License v2.0 with LLVM
 * Exceptions.
 * See LICENSE.TXT
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 * @file ${filename}.h
 *
 */

 // Auto-generated file, do not edit.

#pragma once

#include <string>
#include <vector>

#include "ur_util.hpp"
#include <ur_api.h>

namespace ur_loader {
struct ur_adapter_manifest {
  std::string name;
  std::string library;
  ur_adapter_backend_t backend;
  std::vector<ur_device_type_t> device_types;
};

const std::vector<ur_adapter_manifest> ur_adapter_manifests = {
%for manifest in manifests:
{
  {name},
  MAKE_LIBRARY_NAME("ur_adapter_{name}", "0"),
  UR_ADAPTER_BACKEND_{BACKEND},
  {
  %for device_type in manifest.device_types:
    {device_type}
  %endfor
  }
},
%endfor
};
}
