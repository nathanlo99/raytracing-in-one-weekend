
#include "bvh_node.hpp"
#include "hittable_list.hpp"
#include "triangle.hpp"
#include "vec3.hpp"
#include <boost/algorithm/string.hpp>
#include <fstream>

std::shared_ptr<hittable> load_obj(std::string_view filename, material *mat) {
  hittable_list result;

  std::cout << "Loading OBJ file @ " << filename << std::endl;

  std::string code, line;
  std::ifstream ifs(filename);

  std::vector<point3> vertices(1); // Since OBJ is 1-indexed

  while (std::getline(ifs, line)) {
    std::stringstream ss(line);

    // Read the first token: this is the code
    if (!(ss >> code) || code == "#")
      continue;

    if (code == "v") {
      point3 pt;
      ss >> pt.e[0] >> pt.e[1] >> pt.e[2];
      vertices.push_back(pt);
    } else if (code == "f") {
      // TODO: Remove assumption that a face has 3 points
      std::string point_info;
      std::vector<point3> points;

      while (ss >> point_info) {
        std::vector<std::string> tokens;
        boost::split(tokens, point_info, boost::is_any_of("/"));

        if (tokens.size() == 1) {
          points.emplace_back(vertices[std::stoi(tokens[0])]);
        } else if (tokens.size() == 3) {
          points.emplace_back(vertices[std::stoi(tokens[0])]);
          // TODO: Parse uv's and normals
        }
      }

      const size_t num_points = points.size();
      // std::cout << "Found face with " << num_points << " points" <<
      // std::endl;
      for (size_t idx = 1; idx + 1 < num_points; ++idx) {
        const point3 p1 = points[0], p2 = points[idx], p3 = points[idx + 1];
        result.add(std::make_shared<triangle>(p1, p2, p3, mat));
      }
    }
  }

  std::cout << "Loaded " << result.size() << " triangles" << std::endl;

  return bvh_node::from_list(result, 0.0, 0.0);
}
