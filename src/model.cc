#include "opennmt/model.h"

#include "opennmt/transformer.h"
#include "opennmt/utils.h"

namespace opennmt {

  StorageView Model::load_data(const Shape& shape, size_t data_width, void* data) {
    if (data_width == 4) {
      return StorageView(shape, reinterpret_cast<float*>(data));
    } else if (data_width == 2) {
      StorageView s_data(shape, reinterpret_cast<int16_t*>(data));
      if (support_avx2())
        return s_data;
      else {
        // int16 GEMM is not optimized prior AVX2 so fallback to float.
        static const ops::Unquantize unquantize_op(1000);
        StorageView s_data_cast;
        unquantize_op(s_data, s_data_cast);
        return s_data_cast;
      }
    }

    throw std::runtime_error("unsupported data type");
  }

  std::shared_ptr<Model> ModelFactory::load(const std::string& type, const std::string& path) {
    if (type == "transformer")
      return load(ModelType::Transformer, path);
    return nullptr;
  }

  std::shared_ptr<Model> ModelFactory::load(ModelType type, const std::string& path) {
    Model* model = nullptr;

    switch (type) {
    case ModelType::Transformer:
      model = new TransformerModel(path);
      break;
    }

    return std::shared_ptr<Model>(model);
  }

}