#include "core/framework/fuse_nodes_funcs.h"
#include "core/platform/env.h"

namespace onnxruntime {
Status FuncManager::AddFuncInfo(const std::string& name, const std::string& dll_path) {
  auto it = fused_funcs_->find(name);
  if (it != fused_funcs_->end())
    return Status(common::ONNXRUNTIME, common::FAIL, "func info for node: " + name + " already exist.");
  (*fused_funcs_)[name] = {dll_path, nullptr, nullptr, nullptr};
  return Status::OK();
}

Status FuncManager::AddFuncInfo(const std::string& name, ComputeFunc compute, CreateFunctionStateFunc create, DestroyFunctionStateFunc release) {
  auto it = fused_funcs_->find(name);
  if (it != fused_funcs_->end())
    return Status(common::ONNXRUNTIME, common::FAIL, "func info for node: " + name + " already exist.");
  if (!compute || !create || !release)
    return Status(common::ONNXRUNTIME, common::FAIL, "Can't use func with null ptr");
  (*fused_funcs_)[name] = {"", compute, create, release};
  return Status::OK();
}

Status FuncManager::GetFuncs(const std::string& name, ComputeFunc* compute, CreateFunctionStateFunc* create, DestroyFunctionStateFunc* release) const {
  auto it = fused_funcs_->find(name);
  if (it == fused_funcs_->end())
    return Status(common::ONNXRUNTIME, common::FAIL, "func info for node: " + name + " not found.");
  if (!it->second.compute_func) {
    //load from path
    void* handle = nullptr;
    ORT_RETURN_IF_ERROR(lib_loader_->LoadExternalLib(it->second.dso_path, &handle));
    void* create_func_symbol_handle = nullptr;
    ORT_RETURN_IF_ERROR(Env::Default().GetSymbolFromLibrary(handle,
                                                            kCreateStateFuncSymbol + name,
                                                            &create_func_symbol_handle));
    void* compute_func_symbol_handle = nullptr;
    ORT_RETURN_IF_ERROR(Env::Default().GetSymbolFromLibrary(handle,
                                                            kComputeFuncSymbol + name,
                                                            &compute_func_symbol_handle));
    void* release_func_symbol_handle = nullptr;
    ORT_RETURN_IF_ERROR(Env::Default().GetSymbolFromLibrary(handle,
                                                            kReleaseStateFuncSymbol + name,
                                                            &release_func_symbol_handle));
    it->second.compute_func = [=](FunctionState state, const OrtCustomOpApi* api, OrtKernelContext* context) {
      return reinterpret_cast<ComputeFuncC>(compute_func_symbol_handle)(state, api, context);
    };

    it->second.create_state_func = [=](ComputeContext* context, FunctionState* state) {
      return reinterpret_cast<CreateFunctionStateC>(create_func_symbol_handle)(context, state);
    };

    it->second.release_state_func = [=](FunctionState state) {
      return reinterpret_cast<DestroyFunctionStateC>(release_func_symbol_handle)(state);
    };
  }
  *compute = it->second.compute_func;
  *create = it->second.create_state_func;
  *release = it->second.release_state_func;
  return Status::OK();
}

}  // namespace onnxruntime
