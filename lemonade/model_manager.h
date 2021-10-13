#pragma once
#include "translator/translation_model.h"
#include <list>
#include <memory>

/// Manages models
class ModelManager {
  using Model = marian::bergamot::TranslationModel;

public:
  ModelManager(size_t maxModels) : maxModels_(maxModels) {}

  void shareModel(std::shared_ptr<Model> model) {
    models_.emplace(models_.end(), model);
  }

private:
  size_t maxModels_{0};
  std::list<std::shared_ptr<Model>> models_;

  struct Usage {
    const Model *model{nullptr};
    size_t usage{0};
  };
};
