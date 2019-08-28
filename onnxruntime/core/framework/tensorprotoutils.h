// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <vector>
#include <type_traits>

#include "core/common/common.h"
#include "core/common/status.h"
#include "core/framework/allocator.h"
#include "core/framework/ml_value.h"
#include "core/framework/mem_buffer.h"
#include "core/framework/tensor_external_data_info.h"
#include "core/session/onnxruntime_c_api.h"
#include "core/graph/onnx_protobuf.h"
#include "core/platform/env.h"

namespace ONNX_NAMESPACE {
class TensorProto;
class TensorShapeProto;
}  // namespace ONNX_NAMESPACE

namespace onnxruntime {
class Tensor;
namespace utils {
TensorShape GetTensorShapeFromTensorShapeProto(const ONNX_NAMESPACE::TensorShapeProto& tensor_shape_proto);
/**
 * deserialize a TensorProto into a preallocated memory buffer.
 * \param tensor_proto_path A local file path of where the 'input' was loaded from. Can be NULL if the tensor proto doesn't
 *                        have any external data or it was loaded from current working dir. This path could be either a
 *                        relative path or an absolute path.
 */
common::Status TensorProtoToMLValue(const Env& env, const ORTCHAR_T* tensor_proto_path,
                                    const ONNX_NAMESPACE::TensorProto& input, const MemBuffer& m, OrtValue& value,
                                    OrtCallback& deleter);
// This function doesn't support string tensors
ONNX_NAMESPACE::TensorProto::DataType GetTensorProtoType(const Tensor& tensor);

/** Creates a TensorProto from a Tensor.
    @param[in] tensor the Tensor whose data and shape will be used to create the TensorProto.
    @param[in] tensor_proto_name the name of the TensorProto.
    @param[in] tensor_proto_type the type of the TensorProto.
    @return the TensorProto. 
    
    Note: Method currently requires that data is in little-endian format.
    TODO Once the GetTensorProtoType supports all data types, we can remove the tensor_proto_type parameter and 
    instead get the type from the tensor. */
ONNX_NAMESPACE::TensorProto TensorToTensorProto(const Tensor& tensor, const std::string& tensor_proto_name,
                                                const ONNX_NAMESPACE::TypeProto& tensor_proto_type);

ONNXTensorElementDataType CApiElementTypeFromProtoType(int type);
ONNXTensorElementDataType GetTensorElementType(const ONNX_NAMESPACE::TensorProto& tensor_proto);

// How much memory it will need for putting the content of this tensor into a plain array
// complex64/complex128 tensors are not supported.
// The output value could be zero or -1.
template <size_t alignment>
common::Status GetSizeInBytesFromTensorProto(const ONNX_NAMESPACE::TensorProto& tensor_proto, size_t* out);

template <typename T>
Status UnpackTensor(const ONNX_NAMESPACE::TensorProto& tensor, const void* raw_data, size_t raw_data_len,
                    /*out*/ T* p_data, int64_t expected_size);

}  // namespace utils
}  // namespace onnxruntime
