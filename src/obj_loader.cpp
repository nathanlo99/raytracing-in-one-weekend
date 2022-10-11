
#include "bvh_node.hpp"
#include "hittable_list.hpp"
#include "triangle.hpp"

#include <boost/algorithm/string.hpp>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>

void load_mtl(const std::string_view &filename,
              std::unordered_map<std::string, material *> &materials) {
  std::cout << "Loading MTL file '" << filename << "'" << std::endl;

  std::ifstream ifs(filename);
  if (!ifs.is_open()) {
    std::cerr << "ERROR: Could not open file '" << filename << "'" << std::endl;
    throw std::runtime_error("MTL loading failed: .mtl file unreadable");
  }
}

std::shared_ptr<hittable> load_obj(const std::string_view &filename,
                                   material *default_mat,
                                   const bool load_mtls) {
  hittable_list result;

  std::cout << "Loading OBJ file '" << filename << "'" << std::endl;

  std::string code, line;
  int line_num = 0;
  std::ifstream ifs(filename);
  if (!ifs.is_open()) {
    std::cerr << "ERROR: Could not open file '" << filename << "'" << std::endl;
    throw std::runtime_error("OBJ loading failed: .obj file unreadable");
  }

  std::vector<point3> positions(1); // Since OBJ is 1-indexed
  std::vector<point3> uvs(1);
  std::vector<vec3> normals(1);

  std::unordered_map<std::string, material *> materials;
  materials["default"] = default_mat;
  material *current_material = default_mat;

  std::vector<std::string> skipped_lines; // Just for reference

  while (std::getline(ifs, line)) {
    std::stringstream ss(line);
    line_num++;

    // Read the first token: this is the code
    if (!(ss >> code) || code == "#")
      continue;

    if (code == "v") {
      point3 pt;
      ss >> pt.x >> pt.y >> pt.z;
      positions.push_back(pt);
    } else if (code == "vt") {
      point3 uv;
      ss >> uv.x >> uv.y;
      uvs.push_back(uv);
    } else if (code == "vn") {
      vec3 normal;
      ss >> normal.x >> normal.y >> normal.z;
      normals.push_back(normal);
    } else if (code == "f") {
      std::string vertex_info;
      std::vector<vertex> vertices;

      while (ss >> vertex_info) {
        std::vector<std::string> tokens;
        boost::split(tokens, vertex_info, boost::is_any_of("/"));

        if (tokens.size() == 1) {
          vertices.emplace_back(vertices[std::stoi(tokens[0])]);
        } else if (tokens.size() == 3) {
          const int position_idx = std::stoi(tokens[0]);
          const int uv_idx = std::stoi(tokens[1]);
          const int normal_idx = std::stoi(tokens[2]);
          vertices.emplace_back(positions[position_idx], uvs[uv_idx],
                                normals[normal_idx]);
        }
      }

      const size_t num_points = vertices.size();
      for (size_t idx = 1; idx + 1 < num_points; ++idx) {
        const vertex p0 = vertices[0], p1 = vertices[idx],
                     p2 = vertices[idx + 1];
        result.emplace_back<triangle>(p0, p1, p2, current_material);
      }
    } else if (code == "mtllib") {
      std::string mtl_filename;
      ss >> mtl_filename;
      const std::string mtl_path = std::filesystem::path(filename)
                                       .replace_filename(mtl_filename)
                                       .string();
      load_mtl(mtl_path, materials);
    } else {
      skipped_lines.push_back(std::to_string(line_num) + ": " + line);
    }
  }

  std::cout << "Skipped lines: " << std::endl;
  for (const std::string &line : skipped_lines)
    std::cout << line << std::endl;

  std::cout << "Loaded OBJ file: " << filename << std::endl;
  std::cout << "  Triangles: " << result.size() << std::endl;
  std::cout << "  Positions: " << positions.size() << std::endl;
  std::cout << "  UV Coords: " << uvs.size() << std::endl;
  std::cout << "  Normals  : " << normals.size() << std::endl;

  return bvh_node::from_list(result, 0.0, 1.0);
}
