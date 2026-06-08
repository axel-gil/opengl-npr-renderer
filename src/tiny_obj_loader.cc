#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.hh"

#include <iostream>
#include <filesystem>
#include <map>

bool load_obj(const std::string& path, std::vector<Mesh>& meshes)
{
    tinyobj::ObjReaderConfig reader_config;

    const std::filesystem::path p = path;
    reader_config.mtl_search_path = p.parent_path().string();

    tinyobj::ObjReader reader;
    if (!reader.ParseFromFile(path, reader_config))
    {
        if (!reader.Error().empty())
            std::cerr << "TinyObjReader error: " << reader.Error();
        return false;
    }

    const auto& attrib = reader.GetAttrib();
    const auto& shapes = reader.GetShapes();
    const auto& materials = reader.GetMaterials();
    const std::filesystem::path base = p.parent_path();

    // un Mesh par material_id (-1 = bucket "sans matériau")
    std::map<int, size_t> mat_to_mesh;
    auto get_mesh = [&](int mat_id) -> Mesh& {
        auto it = mat_to_mesh.find(mat_id);
        if (it != mat_to_mesh.end())
            return meshes[it->second];
        meshes.push_back(Mesh{});
        Mesh& m = meshes.back();
        if (mat_id >= 0 && mat_id < (int)materials.size()
            && !materials[mat_id].diffuse_texname.empty())
            m.texture_path = materials[mat_id].diffuse_texname;
        mat_to_mesh[mat_id] = meshes.size() - 1;
        return m;
    };

    for (const auto& shape : shapes)
    {
        size_t index_offset = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
        {
            int fv = shape.mesh.num_face_vertices[f];
            int mat_id = shape.mesh.material_ids[f];

            float r = 1, g = 1, b = 1;
            if (mat_id >= 0 && mat_id < (int)materials.size())
            {
                r = materials[mat_id].diffuse[0];
                g = materials[mat_id].diffuse[1];
                b = materials[mat_id].diffuse[2];
            }

            Mesh& mesh = get_mesh(mat_id);

            for (int v = 1; v <= fv - 2; v++)
                for (int corner : { 0, v, v + 1 })
                {
                    tinyobj::index_t idx =
                        shape.mesh.indices[index_offset + corner];

                    mesh.buffer.push_back(
                        attrib.vertices[3 * idx.vertex_index + 0]);
                    mesh.buffer.push_back(
                        attrib.vertices[3 * idx.vertex_index + 1]);
                    mesh.buffer.push_back(
                        attrib.vertices[3 * idx.vertex_index + 2]);

                    // normale : défaut (0,1,0) si absente plutôt que d'échouer
                    if (idx.normal_index >= 0)
                    {
                        mesh.buffer.push_back(
                            attrib.normals[3 * idx.normal_index + 0]);
                        mesh.buffer.push_back(
                            attrib.normals[3 * idx.normal_index + 1]);
                        mesh.buffer.push_back(
                            attrib.normals[3 * idx.normal_index + 2]);
                    }
                    else
                    {
                        mesh.buffer.push_back(0);
                        mesh.buffer.push_back(1);
                        mesh.buffer.push_back(0);
                    }

                    // uv : défaut (0,0) si absent
                    if (idx.texcoord_index >= 0)
                    {
                        mesh.buffer.push_back(
                            attrib.texcoords[2 * idx.texcoord_index + 0]);
                        mesh.buffer.push_back(
                            attrib.texcoords[2 * idx.texcoord_index + 1]);
                    }
                    else
                    {
                        mesh.buffer.push_back(0);
                        mesh.buffer.push_back(0);
                    }

                    mesh.buffer.push_back(r);
                    mesh.buffer.push_back(g);
                    mesh.buffer.push_back(b);
                }
            index_offset += fv;
        }
    }
    return true;
}
