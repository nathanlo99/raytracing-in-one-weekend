
#include "bvh_node.hpp"
#include "hittable_list.hpp"
#include "material.hpp"
#include "material_manager.hpp"
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

  std::string line, code;

  std::string current_name = "";
  obj_material *current_material = nullptr;

  std::unordered_set<std::string> ignored_codes = {"#", "illum", "Tf"};

  while (std::getline(ifs, line)) {
    std::stringstream ss(line);

    // Read the first token: this is the code
    if (!(ss >> code) || ignored_codes.count(code) > 0)
      continue;

    if (code == "newmtl") {
      if (current_material != nullptr)
        materials[current_name] = dynamic_cast<material *>(current_material);
      ss >> current_name;
      current_material = dynamic_cast<obj_material *>(
          material_manager::create<obj_material>());
    } else if (code == "Ns") {
      assert(current_material != nullptr);
      ss >> current_material->specular_exponent;
    } else if (code == "Ka") {
      assert(current_material != nullptr);
      ss >> current_material->ambient_colour.x >>
          current_material->ambient_colour.y >>
          current_material->ambient_colour.z;
    } else if (code == "Kd") {
      assert(current_material != nullptr);
      ss >> current_material->diffuse_colour.x >>
          current_material->diffuse_colour.y >>
          current_material->diffuse_colour.z;
    } else if (code == "Ks") {
      assert(current_material != nullptr);
      ss >> current_material->specular_colour.x >>
          current_material->specular_colour.y >>
          current_material->specular_colour.z;
    } else if (code == "Ke") {
      assert(current_material != nullptr);
      ss >> current_material->emissive_colour.x >>
          current_material->emissive_colour.y >>
          current_material->emissive_colour.z;
    } else if (code == "map_Ka") {
      std::string map_name;
      ss >> map_name;
      const std::string map_filename =
          std::filesystem::path(filename).replace_filename(map_name).string();
      assert(current_material != nullptr);
      current_material->ambient_map =
          std::make_shared<image_texture>(map_filename);
    } else if (code == "map_Kd") {
      std::string map_name;
      ss >> map_name;
      const std::string map_filename =
          std::filesystem::path(filename).replace_filename(map_name).string();
      assert(current_material != nullptr);
      current_material->diffuse_map =
          std::make_shared<image_texture>(map_filename);
    } else if (code == "map_Ks") {
      std::string map_name;
      ss >> map_name;
      const std::string map_filename =
          std::filesystem::path(filename).replace_filename(map_name).string();
      assert(current_material != nullptr);
      current_material->specular_map =
          std::make_shared<image_texture>(map_filename);
    } else if (code == "map_Ke") {
      std::string map_name;
      ss >> map_name;
      const std::string map_filename =
          std::filesystem::path(filename).replace_filename(map_name).string();
      assert(current_material != nullptr);
      current_material->emissive_map =
          std::make_shared<image_texture>(map_filename);
    } else if (code == "map_Bump" || code == "map_bump") {
      std::string map_name;
      ss >> map_name;
      const std::string map_filename =
          std::filesystem::path(filename).replace_filename(map_name).string();
      assert(current_material != nullptr);
      current_material->bump_map =
          std::make_shared<image_texture>(map_filename);
    } else if (code == "Ni") {
      assert(current_material != nullptr);
      ss >> current_material->index_of_refraction;
    } else if (code == "d") {
      real one_minus_transparency;
      ss >> one_minus_transparency;
      assert(current_material != nullptr);
      current_material->transparency = 1.0 - one_minus_transparency;
    } else if (code == "Tr") {
      assert(current_material != nullptr);
      ss >> current_material->transparency;
    } else {
      std::cout << "MTL : Ignored line '" << line << "'" << std::endl;
    }
  }

  // Don't forget to add the last material!
  if (current_material != nullptr)
    materials[current_name] = current_material;
}

std::shared_ptr<hittable> load_obj(const std::string_view &filename,
                                   material *default_mat,
                                   const bool load_mtls) {
  hittable_list result;

  std::cout << "Loading OBJ file '" << filename << "'" << std::endl;

  std::string code, line;
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
  std::unordered_set<std::string> ignored_codes = {"#", "s", "o", "g"};

  while (std::getline(ifs, line)) {
    std::stringstream ss(line);

    // Read the first token: this is the code
    if (!(ss >> code) || ignored_codes.count(code) > 0)
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
          if (tokens[1] == "") {
            const int position_idx = std::stoi(tokens[0]);
            const int normal_idx = std::stoi(tokens[2]);
            vertices.emplace_back(positions[position_idx], std::nullopt,
                                  normals[normal_idx]);
          } else {
            const int position_idx = std::stoi(tokens[0]);
            const int uv_idx = std::stoi(tokens[1]);
            const int normal_idx = std::stoi(tokens[2]);
            vertices.emplace_back(positions[position_idx], uvs[uv_idx],
                                  normals[normal_idx]);
          }
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
    } else if (code == "usemtl") {
      std::string material_name;
      ss >> material_name;
      if (materials.count(material_name) == 0) {
        std::cerr << "Invalid MTL: Could not find material named '"
                  << material_name << "'" << std::endl;
        throw std::runtime_error("Invalid .mtl file");
      }
      current_material = materials[material_name];
    } else {
      std::cout << "OBJ: Ignored line '" << line << "'" << std::endl;
    }
  }

  if (!skipped_lines.empty()) {
    std::cout << "Unsupported lines: " << std::endl;
    for (const std::string &line : skipped_lines)
      std::cout << line << std::endl;
  }

  std::cout << "Loaded OBJ file: " << filename << std::endl;
  std::cout << "  Triangles: " << result.size() << std::endl;
  std::cout << "  Positions: " << positions.size() - 1 << std::endl;
  std::cout << "  UV Coords: " << uvs.size() - 1 << std::endl;
  std::cout << "  Normals  : " << normals.size() - 1 << std::endl;

  return bvh_node::from_list(result, 0.0, 1.0);
}
