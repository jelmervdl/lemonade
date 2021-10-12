#pragma once

/// Manages models
class ModelManager {
  using Model = marian::bergamot::TranslationModel;

public:
  ModelManager(size_t maxModels) : maxModels_(maxModels) {}

  bool loadModel(const Model::Config &config) {
    if (models_.size() == maxModels_) {
      return false;
    }
    models_.emplace(models_.end(), std::make_unique<Model>(config));
    return true;
  };

private:
  size_t maxModels_{0};
  std::list<std::shared_ptr<Model>> models_;

  struct Usage {
    const Model *model{nullptr};
    size_t usage{0};
  };
};
