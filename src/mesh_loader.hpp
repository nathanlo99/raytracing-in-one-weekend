
#include "bvh_node.hpp"
#include "material.hpp"
#include <string_view>

std::shared_ptr<bvh_node> load_obj(std::string_view filename, material *mat);
