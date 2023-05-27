#pragma once
#include <gef.h>
#include <map>
#include <string>
#include <vector>
#include <maths/vector4.h>
#include <maths/vector2.h>

namespace gef
{
	class Platform;
	class Model;
	class Texture;
	class Mesh;
	class Material;
}

typedef std::map<std::string, gef::Mesh*> MeshMap;

struct MeshData
{
	gef::Platform& platform;
	std::vector<Int32> face_indices;
	std::vector<gef::Vector4> positions;
	std::vector<gef::Vector4> normals;
	std::vector<gef::Vector2> uvs;
	std::vector<gef::Material*> material_list;
	std::vector<Int32> primitive_indices;
	std::vector<Int32> texture_indices;
};

enum MeshResource
{
	Crate,
	Level,
};

class OBJMeshLoader
{
public:
	bool Load(MeshResource mr, const char* filename, const char* meshmap_key, gef::Platform& platform);
	const std::string& GetLastError() { return last_error_; }
	gef::Mesh* GetMesh(MeshResource mr, gef::Vector4& scale);

private:
	const std::string GetFolderName(const char* filename);
	gef::Mesh* CreateMesh(MeshResource mr, gef::Vector4& scale);
	bool LoadMaterials(const gef::Platform& platform, const char* filename, const std::string& folder_name, std::map<std::string, Int32>& materials, std::vector<gef::Material*>& material_list);
	std::map<MeshResource, MeshData*> mesh_data_map_;
private:
	std::string last_error_;
};

