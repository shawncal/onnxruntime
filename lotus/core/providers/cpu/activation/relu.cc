#include "core/providers/cpu/activation/relu.h"

namespace Lotus {

template class Relu<float>;
REGISTER_KERNEL(KernelDef("Relu")
                    .Domain(LotusIR::kOnnxDomain)
                    .SinceVersion(1, 2)
                    .Provider(LotusIR::kCpuExecutionProvider)
                    .TypeConstraint("T", DataTypeImpl::GetTensorType<float>()),
                Relu<float>);
}  // namespace Lotus