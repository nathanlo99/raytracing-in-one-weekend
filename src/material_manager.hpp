
#pragma once

#include <memory>
#include <vector>

#include "material.hpp"

// A global singleton material manager to avoid redundant materials
struct material_manager {
  std::vector<std::unique_ptr<material>> materials;

  static material_manager &instance() {
    static material_manager m_instance;
    return m_instance;
  }

  template <class MaterialClass, class... Args>
  material *create(Args &&...args) {
    materials.push_back(
        std::make_unique<MaterialClass>(std::forward<Args>(args)...));
    return materials.back().get();
  }

private:
  material_manager() = default;

  // Delete copy/move so extra instances can't be created/moved.
  material_manager(const material_manager &) = delete;
  material_manager &operator=(const material_manager &) = delete;
  material_manager(material_manager &&) = delete;
  material_manager &operator=(material_manager &&) = delete;
};
