#pragma once

// #include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "core/framework/execution_provider.h"
#include "core/graph/graph.h"
#include "core/common/logging/logging.h"

namespace Lotus {

/**
Class for managing lookup of the execution providers in a session.
*/
class ExecutionProviders {
 public:
  ExecutionProviders() = default;

  Common::Status Add(const std::string& provider_id, std::unique_ptr<IExecutionProvider> p_exec_provider) {
    // make sure there are no issues before we change any internal data structures
    if (provider_idx_map_.find(provider_id) != provider_idx_map_.end()) {
      auto status = LOTUS_MAKE_STATUS(LOTUS, FAIL, "Provider ", provider_id, " has already been registered.");
      LOGS_DEFAULT(ERROR) << status.ErrorMessage();
      return status;
    }

    for (const auto& pair : p_exec_provider->GetAllocatorMap()) {
      auto allocator = pair.second;
      if (allocator_idx_map_.find(allocator->Info()) != allocator_idx_map_.end()) {
        auto status = LOTUS_MAKE_STATUS(LOTUS, FAIL, allocator->Info(), " allocator already registered.");
        LOGS_DEFAULT(ERROR) << status.ErrorMessage();
        return status;
      }
    }

    // index that provider will have after insertion
    auto new_provider_idx = exec_providers_.size();

    IGNORE_RETURN_VALUE(provider_idx_map_.insert({provider_id, new_provider_idx}));

    for (const auto& pair : p_exec_provider->GetAllocatorMap()) {
      auto allocator = pair.second;
      IGNORE_RETURN_VALUE(allocator_idx_map_.insert({allocator->Info(), new_provider_idx}));
    }

    exec_providers_.push_back(std::move(p_exec_provider));

    return Status::OK();
  }

  const IExecutionProvider* Get(const LotusIR::Graph& graph, const LotusIR::NodeIndex& idx) const {
    auto node = graph.GetNode(idx);
    LOTUS_ENFORCE(node != nullptr, "Node with id ", idx, " was not found in graph.");

    return Get(*node);
  }

  const IExecutionProvider* Get(const LotusIR::Node& node) const {
    return Get(node.GetExecutionProviderType());
  }

  const IExecutionProvider* Get(LotusIR::ProviderType provider_id) const {
    auto it = provider_idx_map_.find(provider_id);
    if (it == provider_idx_map_.end()) {
      return nullptr;
    }

    return exec_providers_[it->second].get();
  }

  const IExecutionProvider* Get(const AllocatorInfo& allocator_info) const {
    auto it = allocator_idx_map_.find(allocator_info);
    if (it == allocator_idx_map_.end()) {
      return nullptr;
    }

    return exec_providers_[it->second].get();
  }

  bool Empty() const { return exec_providers_.empty(); }

  using const_iterator = typename std::vector<std::unique_ptr<IExecutionProvider>>::const_iterator;
  const_iterator begin() const noexcept { return exec_providers_.cbegin(); }
  const_iterator end() const noexcept { return exec_providers_.cend(); }

 private:
  std::vector<std::unique_ptr<IExecutionProvider>> exec_providers_;

  // maps for fast lookup of an index into exec_providers_
  std::unordered_map<std::string, size_t> provider_idx_map_;
  // using std::map as AllocatorInfo would need a custom hash function to be used with unordered_map,
  // and as this isn't performance critical it's not worth the maintenance overhead of adding one.
  std::map<AllocatorInfo, size_t> allocator_idx_map_;
};
}  // namespace Lotus