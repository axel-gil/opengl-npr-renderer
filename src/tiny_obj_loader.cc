#include <iostream>
#include <filesystem>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.hh"

namespace fs = std::filesystem;

bool load_obj(const std::string& path, std::vector<float>& result_buffer)
{
    tinyobj::ObjReaderConfig reader_config;

    const fs::path p = path;
    reader_config.mtl_search_path = p.parent_path(); // Path to material files

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(path, reader_config))
    {
        if (!reader.Error().empty())
            std::cerr << "TinyObjReader error: " << reader.Error();
        return false;
    }

    if (!reader.Warning().empty())
        std::cerr << "TinyObjReader warning: " << reader.Warning();

    const auto& attrib = reader.GetAttrib();
    const auto& shapes = reader.GetShapes();
    const auto& materials = reader.GetMaterials();

    for (const auto& shape : shapes)
    {
        size_t index_offset = 0;

        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
        {
            int fv = shape.mesh.num_face_vertices[f]; // 3 or 4 (or more)

            // // get the material for this face
            int mat_id = shape.mesh.material_ids[f];

            float r = 1.0f, g = 1.0f, b = 1.0f; // default white
            if (mat_id >= 0 && mat_id < (int)materials.size())
            {
                r = materials[mat_id].diffuse[0]; // Kd r
                g = materials[mat_id].diffuse[1]; // Kd g
                b = materials[mat_id].diffuse[2]; // Kd b
            }

            // Fan triangulation: triangle = (0, v, v+1) for v in [1, fv-2]
            for (int v = 1; v <= fv - 2; v++)
            {
                for (int corner : { 0, v, v + 1 })
                {
                    tinyobj::index_t idx =
                        shape.mesh.indices[index_offset + corner];

                    // Position
                    result_buffer.push_back(
                        attrib.vertices[3 * idx.vertex_index + 0]);
                    result_buffer.push_back(
                        attrib.vertices[3 * idx.vertex_index + 1]);
                    result_buffer.push_back(
                        attrib.vertices[3 * idx.vertex_index + 2]);

                    if (idx.normal_index >= 0)
                    {
                        result_buffer.push_back(
                            attrib.normals[3 * idx.normal_index + 0]);
                        result_buffer.push_back(
                            attrib.normals[3 * idx.normal_index + 1]);
                        result_buffer.push_back(
                            attrib.normals[3 * idx.normal_index + 2]);
                    }
                    else
                        return false;

                    if (idx.texcoord_index >= 0)
                    {
                        result_buffer.push_back(
                            attrib.texcoords[2 * idx.texcoord_index + 0]);
                        result_buffer.push_back(
                            attrib.texcoords[2 * idx.texcoord_index + 1]);
                    }
                    else
                        return false;

                    result_buffer.push_back(r);
                    result_buffer.push_back(g);
                    result_buffer.push_back(b);
                }
            }
            index_offset += fv;
        }
    }

    return true;
}
