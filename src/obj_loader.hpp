
#pragma once

#include "bvh.hpp"
#include "material.hpp"
#include <string_view>

std::shared_ptr<bvh> load_obj(const std::string_view &filename,
                              material *default_mat,
                              const bool load_mtls = true);
