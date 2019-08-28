// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "core/providers/cpu/nn/flatten.h"

namespace onnxruntime {
ONNX_CPU_OPERATOR_VERSIONED_KERNEL(
    Flatten,
    1,
    8,
    KernelDefBuilder()
        .Alias(0, 0)
        .TypeConstraint("T", DataTypeImpl::AllTensorTypes()),
    Flatten);

ONNX_CPU_OPERATOR_KERNEL(
    Flatten,
    9,
    KernelDefBuilder()
        .Alias(0, 0)
        .TypeConstraint("T", DataTypeImpl::AllTensorTypes()),
    Flatten);
}
