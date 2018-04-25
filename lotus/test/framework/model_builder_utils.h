#include "core/graph/model.h"

namespace Lotus {
namespace Test {

namespace ModelBuilder {

// Shape: a wrapper to build a TensorShapeProto
struct Shape {
  TensorShapeProto value;

  // construct a shape with given constant dimensions
  Shape(std::initializer_list<int> dims) {
    for (auto d : dims) {
      auto dim = value.add_dim();
      dim->set_dim_value(d);
    }
  }

  // construct a shape with given symbolic dimensions
  Shape(std::initializer_list<string> dims) {
    for (auto d : dims) {
      auto dim = value.add_dim();
      dim->set_dim_param(d);
    }
  }
};

// Type: a wrapper to build a TypeProto
struct Type {
  TypeProto value;

  // construct a float-tensor-type
  Type() {
    value.mutable_tensor_type()->set_elem_type(TensorProto_DataType_FLOAT);
  }

  // construct a float-tensor-type with given constant dimensions
  Type(std::initializer_list<int> dims) {
    value.mutable_tensor_type()->set_elem_type(TensorProto_DataType_FLOAT);
    auto p_shape = value.mutable_tensor_type()->mutable_shape();
    for (auto d : dims) {
      auto dim = p_shape->add_dim();
      dim->set_dim_value(d);
    }
  }

  // construct a float-tensor-type with given symbolic dimensions
  Type(std::initializer_list<string> symbolic_dims) {
    value.mutable_tensor_type()->set_elem_type(TensorProto_DataType_FLOAT);
    auto p_shape = value.mutable_tensor_type()->mutable_shape();
    for (auto d : symbolic_dims) {
      auto dim = p_shape->add_dim();
      dim->set_dim_param(d);
    }
  }
};

}  // namespace ModelBuilder
}  // namespace Test
}  // namespace Lotus