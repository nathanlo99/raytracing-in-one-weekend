
#include "bvh_node.hpp"
#include "material.hpp"
#include <string_view>

std::shared_ptr<bvh_node> load_obj(const std::string &filename, material *mat);
