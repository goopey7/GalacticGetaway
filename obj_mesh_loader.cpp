#include "obj_mesh_loader.h"
#include <graphics/mesh.h>
#include <graphics/primitive.h>
#include <system/platform.h>
#include <graphics/model.h>
#include <assets/png_loader.h>
#include <graphics/texture.h>
#include <graphics/image_data.h>
#include <system/file.h>
#include <system/memory_stream_buffer.h>
#include <graphics/material.h>

#include <cstdio>
#include <cstring>
#include <istream>
#include <cfloat>
#include <algorithm>
#include <cassert>

bool OBJMeshLoader::Load(MeshResource mr, const char* filename, const char* meshmap_key, gef::Platform& platform)
{
	if(mesh_data_map_.contains(mr))
		return true;
	// Get folder name. May be empty if there is no folder the OBJ file is stored in
	std::string folder_name = GetFolderName(filename);

	bool success = true;

	std::vector<gef::Material*> material_list;
	std::vector<gef::Vector4> positions;
	std::vector<gef::Vector4> normals;
	std::vector<gef::Vector2> uvs;
	std::vector<Int32> face_indices;
	std::vector<std::vector<Int32>> primitive_indices;
	std::vector<std::vector<Int32>> texture_indices;
	std::vector<std::string> object_names;
	std::vector<Int32> object_indices;

	std::map<std::string, Int32> materials;

	// Open the OBJ file and load data + Calcualte file size
	std::string obj_filename(filename);
	void* obj_file_data = NULL;
	Int32 file_size = 0;
	gef::File* file = gef::File::Create();

	success = file->Open(obj_filename.c_str());
	if (success)
	{
		success = file->GetSize(file_size);
		if (success)
		{
			obj_file_data = malloc(file_size);
			success = obj_file_data != NULL;
			if (success)
			{
				Int32 bytes_read;
				success = file->Read(obj_file_data, file_size, bytes_read);
				if (success)
					success = bytes_read == file_size;

				file->Close();
				delete file;
				file = NULL;
			}
		}
	}

	// If file loading fails, delete all objects and print error message
	if (!success)
	{
		free(obj_file_data);
		obj_file_data = NULL;

		file->Close();
		delete file;
		file = NULL;
		last_error_ = "Issue with opening and reading OBJ file. Filename: " + std::string(filename);
		return success;
	}

	try {
		// Stream file data to std::istream
		gef::MemoryStreamBuffer buffer((char*)obj_file_data, file_size);
		std::istream stream(&buffer);
	
		// While there is still data to read
		while (!stream.eof())
		{
			std::string line;
			// Extract each line
			stream >> line;

			// If line starts with mtllib - load material file
			if (line.compare("mtllib") == 0)
			{

				std::string material_filename;
				stream >> material_filename;

				if (!folder_name.empty())
				{
					material_filename = folder_name + material_filename;
				}

				LoadMaterials(platform, material_filename.c_str(), folder_name, materials, material_list);
			}

			// If line stats with o - save all mesh data if there is any
			if (line.compare("o") == 0) {

				std::string object_name;
				stream >> object_name;

				object_names.push_back(object_name);

				object_indices.push_back((Int32)face_indices.size());

				primitive_indices.push_back(std::vector<Int32>());
				texture_indices.push_back(std::vector<Int32>());
			}

			// If line stats with v - save data to vector buffer
			else if (line.compare("v") == 0)
			{
				float x, y, z;
				stream >> x;
				stream >> y;
				stream >> z;
				positions.push_back(gef::Vector4(x, y, z));
			}

			// If line stats with vnv - save data to normal buffer
			else if (line.compare("vn") == 0)
			{
				float nx, ny, nz;
				stream >> nx;
				stream >> ny;
				stream >> nz;
				normals.push_back(gef::Vector4(nx, ny, nz));
			}

			// If line stats with vt - save data to texture mapping buffer
			else if (line.compare("vt") == 0)
			{
				float u, v;
				stream >> u;
				stream >> v;
				uvs.push_back(gef::Vector2(u, v));
			}

			// If line stats with vt - save data to texture mapping buffer
			else if (line.compare("usemtl") == 0)
			{

				char material_name[256];
				stream >> material_name;
				std::string mat_name(material_name);
				// any time the material is changed
				// a new primitive is created
				primitive_indices.back().push_back((Int32)face_indices.size() - object_indices.back());

				texture_indices.back().push_back(materials[material_name]);
			}

			// If line stats with f - save data to face buffer
			else if (line.compare("f") == 0)
			{
				Int32 vertexIndex[3], uvIndex[3], normalIndex[3];

				stream >> vertexIndex[0]; stream.ignore(); stream >> uvIndex[0]; stream.ignore(); stream >> normalIndex[0];
				stream >> vertexIndex[1]; stream.ignore(); stream >> uvIndex[1]; stream.ignore(); stream >> normalIndex[1];
				stream >> vertexIndex[2]; stream.ignore(); stream >> uvIndex[2]; stream.ignore(); stream >> normalIndex[2];

				face_indices.push_back(vertexIndex[2]);
				face_indices.push_back(uvIndex[2]);
				face_indices.push_back(normalIndex[2]);

				face_indices.push_back(vertexIndex[1]);
				face_indices.push_back(uvIndex[1]);
				face_indices.push_back(normalIndex[1]);

				face_indices.push_back(vertexIndex[0]);
				face_indices.push_back(uvIndex[0]);
				face_indices.push_back(normalIndex[0]);
			}
		}

		// Iterate through each object, extract the 
		for (int i = 0; i < object_names.size(); i++)
		{
			if(object_names[i] != meshmap_key)
				continue;
			
			int object_start_index = object_indices[i];
			int end = ((i + 1) < object_indices.size()) ? object_indices[(i + 1)] : face_indices.size();

			//std::vector<Int32> face_data(face_indices.begin() + object_start_index, face_indices.begin() + end);

			mesh_data_map_[mr] = new MeshData(platform,face_indices,positions, normals, uvs, material_list, primitive_indices[i], texture_indices[i], true);
		}
	}
	catch (std::exception& exception)
	{
		last_error_ = exception.what();
		//mesh_map.clear();
		return false;
	}

	// don't need the obj file data any more
	free(obj_file_data);
	obj_file_data = NULL;

	return true;
}

gef::Mesh* OBJMeshLoader::GetMesh(MeshResource mr, gef::Vector4& scale)
{
	return CreateMesh(mr, scale);
}

OBJMeshLoader::~OBJMeshLoader()
{
	for(auto& mesh_data : mesh_data_map_)
	{
		delete mesh_data.second;
		mesh_data.second = NULL;
	}
}

const std::string OBJMeshLoader::GetFolderName(const char* filename)
{
	// Convert to string
	std::string folder_name(filename);

	// Reverse so we can find the last position of \\ or /
	std::reverse(folder_name.begin(), folder_name.end());

	// Find \\ or /
	auto delim_pos = folder_name.find("\\");
	if (delim_pos == std::string::npos)
		delim_pos = folder_name.find("/");

	// If we find it, save the folder string and reverse the folder name so its in the right order
	if (delim_pos != std::string::npos) {
		folder_name = folder_name.substr(delim_pos, folder_name.size());
		std::reverse(folder_name.begin(), folder_name.end());
	}
	else
		folder_name.clear();

	return folder_name;
}

gef::Mesh* OBJMeshLoader::CreateMesh(MeshResource mr, gef::Vector4& scale)
{
	MeshData& md = *mesh_data_map_[mr];
	
	if (md.face_indices.empty())
		return nullptr;

	gef::Mesh* mesh = new gef::Mesh(md.platform);

	// start building the mesh
	Int32 num_faces = (Int32)md.face_indices.size() / 9;
	Int32 num_vertices = num_faces * 3;

	// create vertex buffer
	gef::Mesh::Vertex* vertices = new gef::Mesh::Vertex[num_vertices];

	// need to record min and max position values for mesh bounds
	gef::Vector4 pos_min(FLT_MAX, FLT_MAX, FLT_MAX), pos_max(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for (Int32 vertex_num = 0; vertex_num < num_vertices; ++vertex_num)
	{
		gef::Mesh::Vertex* vertex = &vertices[vertex_num];
		gef::Vector4 position = md.positions[md.face_indices[vertex_num * 3] - 1];

		position.set_x(position.x() * scale.x());
		position.set_y(position.y() * scale.y());
		position.set_z(position.z() * scale.z());

		gef::Vector2 uv = md.uvs[md.face_indices[vertex_num * 3 + 1] - 1];
		gef::Vector4 normal = md.normals[md.face_indices[vertex_num * 3 + 2] - 1];

		vertex->px = position.x();
		vertex->py = position.y();
		vertex->pz = position.z();
		vertex->nx = normal.x();
		vertex->ny = normal.y();
		vertex->nz = normal.z();
		vertex->u = uv.x;
		vertex->v = -uv.y;

		// update min and max positions for bounds
		if (position.x() < pos_min.x())
			pos_min.set_x(position.x());
		if (position.y() < pos_min.y())
			pos_min.set_y(position.y());
		if (position.z() < pos_min.z())
			pos_min.set_z(position.z());
		if (position.x() > pos_max.x())
			pos_max.set_x(position.x());
		if (position.y() > pos_max.y())
			pos_max.set_y(position.y());
		if (position.z() > pos_max.z())
			pos_max.set_z(position.z());
	}


	// set bounds
	gef::Aabb aabb(pos_min, pos_max);
	gef::Sphere sphere(aabb);
	mesh->set_aabb(aabb);
	mesh->set_bounding_sphere(sphere);


	mesh->InitVertexBuffer(md.platform, vertices, num_vertices, sizeof(gef::Mesh::Vertex));

	// create primitives
	mesh->AllocatePrimitives((UInt32)md.primitive_indices.size());

	std::vector<UInt32*> indices;
	indices.resize(md.primitive_indices.size());
	for (UInt32 primitive_num = 0; primitive_num < md.primitive_indices.size(); ++primitive_num)
	{
		Int32 index_count = 0;

		if (primitive_num == md.primitive_indices.size() - 1)
			index_count = (Int32)md.face_indices.size() - md.primitive_indices[primitive_num];
		else
			index_count = md.primitive_indices[primitive_num + 1] - md.primitive_indices[primitive_num];

		// 9 indices per triangle, index count is the number of vertices in this primitive
		index_count /= 3;

		indices[primitive_num] = new UInt32[index_count];

		for (Int32 index = 0; index < index_count; ++index)
			indices[primitive_num][index] = md.primitive_indices[primitive_num] + index;

		mesh->GetPrimitive(primitive_num)->set_type(gef::TRIANGLE_LIST);
		mesh->GetPrimitive(primitive_num)->InitIndexBuffer(md.platform, indices[primitive_num], index_count, sizeof(UInt32));
		//			mesh->GetPrimitive(primitive_num)->InitIndexBuffer(platform, indices[primitive_num], 3, sizeof(UInt32));


		Int32 texture_index = md.texture_indices[primitive_num];
		if (texture_index == -1)
			mesh->GetPrimitive(primitive_num)->set_material(NULL);
		else
			mesh->GetPrimitive(primitive_num)->set_material(md.material_list[texture_index]);
		//mesh->GetPrimitive(primitive_num)->set_texture(textures_[0]);
	}

	// mesh construction complete
	// clean up
	DeleteArrayNull(vertices);
	for (UInt32 primitive_num = 0; primitive_num < indices.size(); ++primitive_num)
		DeleteArrayNull(indices[primitive_num]);

	return mesh;
}

bool StringEndsWith(std::string const& full_string, std::string const& ending)
{
	if (full_string.size() >= ending.size())
	{
		return (0 == full_string.compare(full_string.size() - ending.size(), ending.size(), ending));
	}

	return false;
}

bool OBJMeshLoader::LoadMaterials(const gef::Platform& platform, const char* filename, const std::string& folder_name, std::map<std::string, Int32>& materials, std::vector<gef::Material*>& material_list)
{
	gef::PNGLoader png_loader;
	std::vector<std::pair<gef::Texture*, gef::Colour>> textures;

	bool success = true;

	std::string mtl_filename(filename);
	void* mtl_file_data = NULL;
	Int32 file_size = 0;
	gef::File* file = gef::File::Create();
	success = file->Open(mtl_filename.c_str());
	if (success)
	{
		success = file->GetSize(file_size);
		if (success)
		{
			mtl_file_data = malloc(file_size);
			success = mtl_file_data != NULL;
			if (success)
			{
				Int32 bytes_read;
				success = file->Read(mtl_file_data, file_size, bytes_read);
				if (success)
					success = bytes_read == file_size;
			}
		}
	}

	if (!success)
	{
		free(mtl_file_data);
		mtl_file_data = NULL;
		last_error_ = "Issue with opening and reading MTL file. Filename: " + std::string(filename);
		delete file;
		file = NULL;
		return false;
	}
	gef::MemoryStreamBuffer buffer((char*)mtl_file_data, file_size);
	std::istream stream(&buffer);


	{
		std::map<std::string, std::pair<std::string, gef::Colour>> material_name_mappings;
		char material_name[256];
		while (!stream.eof())
		{
			char line[128];
			stream >> line;

			if (strcmp(line, "newmtl") == 0)
			{
				stream >> material_name;
				material_name_mappings[material_name] = std::make_pair("", gef::Colour());
			}
			else if (strcmp(line, "map_Kd") == 0)
			{
				char texture_name[256];
				stream >> texture_name;
				std::string full_texture_name(texture_name);
				if (!folder_name.empty())
					full_texture_name = folder_name + full_texture_name;
				material_name_mappings[material_name].first = full_texture_name;
			}
			else if (strcmp(line, "Kd") == 0)
			{
				// Write diffuse colour to material
				double r, g, b;
				stream >> r; stream.ignore(); stream >> g; stream.ignore(); stream >> b;

				material_name_mappings[material_name].second = gef::Colour(r, g, b);
			}
		}

		free(mtl_file_data);
		mtl_file_data = NULL;
		delete file;
		file = NULL;

		for (auto iter = material_name_mappings.begin(); iter != material_name_mappings.end(); ++iter)
		{
			if (iter->second.first.compare("") != 0)
			{
				if (!StringEndsWith(iter->second.first, ".png")) {
					std::string message = "Attempted to load an image that was not a PNG. Texture name: " + iter->second.first;
					throw std::exception(message.c_str());
				}
					
				gef::ImageData image_data;
				png_loader.Load(iter->second.first.c_str(), platform, image_data);
				if (image_data.image() != NULL) {
					gef::Texture* texture = gef::Texture::Create(platform, image_data);
					textures.push_back(std::make_pair(texture, iter->second.second));
					materials[iter->first] = (Int32)textures.size() - 1;
				} 
				else
				{
					std::string message = "Cannot load image. Image filename: " + iter->second.first;
					throw std::exception(message.c_str());
				}
			}
			else
			{
				textures.push_back(std::pair<gef::Texture*, gef::Colour>(NULL, iter->second.second));
				materials[iter->first] = (Int32)textures.size() - 1;
			}
		}
	}

	// create materials for each texture
	for (auto texture = textures.begin(); texture != textures.end(); ++texture)
	{
		gef::Material* material = new gef::Material();
		if (texture->first)
		{
			material->set_texture(texture->first);
		}
		material->set_colour(texture->second.GetABGR());
		material_list.push_back(material);
	}

	return success;
}