#include <wipch.h>

#include "Model.h"
#include <fstream>
#include <string>

#include <glew.h>

#include <Wiwa/core/Application.h>
#include <Wiwa/ecs/EntityManager.h>
#include <Wiwa/scene/SceneManager.h>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Exporter.hpp>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <Wiwa/utilities/filesystem/FileSystem.h>
#include <Wiwa/utilities/render/Animator.h>
#include <Wiwa/core/Resources.h>

// For converting between ASSIMP and glm
static inline glm::vec3 vec3_cast(const aiVector3D& v) { return glm::vec3(v.x, v.y, v.z); }
static inline glm::vec2 vec2_cast(const aiVector3D& v) { return glm::vec2(v.x, v.y); } // it's aiVector3D because assimp's texture coordinates use that
static inline glm::quat quat_cast(const aiQuaternion& q) { return glm::quat(q.w, q.x, q.y, q.z); }
static inline glm::mat4 mat4_cast(const aiMatrix4x4& m) { return glm::transpose(glm::make_mat4(&m.a1)); }
static inline glm::mat4 mat4_cast(const aiMatrix3x3& m) { return glm::transpose(glm::make_mat3(&m.a1)); }

namespace Wiwa {
	bool Model::getMeshFromFile(const char* file, ModelSettings* settings, bool gen_buffers)
	{
		unsigned int flags = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals;//aiProcessPreset_TargetRealtime_Quality | aiProcess_FlipUVs;//aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_FlipUVs;
		
		if (settings->preTranslatedVertices) {
			//flags |= aiProcess_PreTransformVertices;
		}

		sbyte* file_data = NULL;
		size_t file_buf_size = FileSystem::ReadAll(file, &file_data);

		WI_INFO("Loaded with bytes: {}", file_buf_size);

		const aiScene* scene = aiImportFileFromMemory(file_data, file_buf_size, flags, NULL);
		
		delete[] file_data;

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			WI_ERROR("Couldn't load mesh file: {0}", file);
			return false;
		}

		WI_CORE_INFO("Loading mesh file at: {0} ...", file);
		
		is_root = true;


		m_ModelPath = file;

		model_name = scene->mRootNode->mName.C_Str();

		std::filesystem::path path = file;
		path.remove_filename();

		std::filesystem::path curr_path = std::filesystem::current_path();

		// Process materials
		if (scene->HasMaterials()) {
			for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
				aiMaterial* mat = scene->mMaterials[i];

				aiString name;
				aiGetMaterialString(mat, AI_MATKEY_NAME, &name);

				aiString texture_diffuse;
				aiGetMaterialString(mat, AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), &texture_diffuse);

				aiColor4D diffuse(1.0f, 1.0f, 1.0f, 1.0f);
				aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &diffuse);

				aiColor4D specular(1.0f, 1.0f, 1.0f, 1.0f);
				aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &specular);

				float shininess = 0.1f;
				aiGetMaterialFloat(mat, AI_MATKEY_SHININESS, &shininess);
				
				std::filesystem::path mat_path = path;
				mat_path += name.C_Str();
				mat_path += ".wimaterial";

				ResourceId matID = Resources::Load<Material>(mat_path.string().c_str());

				if (matID == -1)
				{
					Material material; // Default settings
					size_t id;

					if (texture_diffuse.length > 0)
					{

						std::filesystem::current_path(path);

						std::filesystem::path texture_path = texture_diffuse.C_Str();
						texture_path = std::filesystem::absolute(texture_path);

						std::filesystem::current_path(curr_path);

						texture_path = std::filesystem::relative(texture_path);

						const char* default_shader = "resources/shaders/light/toon_textured";

						id = Resources::Load<Shader>(default_shader);
						material.setShader(Resources::GetResourceById<Shader>(id), default_shader);
						bool imported = Resources::Import<Image>(texture_path.string().c_str());

						if (imported) {
							ResourceId imgId = Resources::Load<Image>(texture_path.string().c_str());
							Image* img = Resources::GetResourceById<Image>(imgId);
							Uniform::SamplerData data;
							data.tex_id = img->GetTextureId();
							data.resource_id = imgId;
							memcpy(data.tex_path, &texture_path.string()[0], texture_path.string().size() + 1);
							material.SetUniformData("u_Texture", data);
						}
						material.SetUniformData("u_ToonLevels", 4);
					}
					else
					{
						id = Resources::Load<Shader>("resources/shaders/light/toon_color");
						material.setShader(Resources::GetResourceById<Shader>(id), "resources/shaders/light/toon_color");

						material.SetUniformData("u_Color", glm::vec4(diffuse.r, diffuse.g, diffuse.b, diffuse.a));
						material.SetUniformData("u_ToonLevels", 4);
					}


					Material::SaveMaterial(mat_path.string().c_str(), &material);
				}
				materials.push_back(mat_path.string());
			}
		}

		// Load mesh list
		if (scene->HasMeshes())
		{
			int totalVertices = 0;
			int totalIndices = 0;
			int totalBones = 0;
			//resize the vector that will contain the number of base index
			meshes.resize(scene->mNumMeshes);

			for (unsigned int i = 0; i < scene->mNumMeshes; i++) {

				const aiMesh* pMesh = scene->mMeshes[i];

				int numVertices = pMesh->mNumVertices;
				int numIndices = pMesh->mNumFaces * 3;
				int numBones = pMesh->mNumBones;
				meshes[i] = totalVertices;
				totalVertices += numVertices;
				totalIndices += numIndices;
				totalBones += numBones;

				Model* model = loadmesh(pMesh);
				model->parent = this;
				//Load bones
				if (pMesh->HasBones())
				{
					model->has_bones = true;
					model->bone_data.resize(totalVertices);
					model->LoadMeshBones(i, pMesh);
					m_BoneCounter = model->m_BoneCounter;
				}
				if (gen_buffers) {
					model->generateBuffers();
				}
				models.push_back(model);
			}
		}

		model_hierarchy = loadModelHierarchy(scene->mRootNode);
		globalInverseTransform = mat4_cast(scene->mRootNode->mTransformation);
		globalInverseTransform = glm::inverse(globalInverseTransform);

		std::filesystem::path p = file;
		model_hierarchy->name = p.stem().string();

		/*if (scene->HasAnimations())
		{
			Animator* animator = new Animator();	
			WI_INFO("Model {0} has {1} animations", model_name, scene->mNumAnimations);

			for (unsigned int i = 0; i < scene->mNumAnimations; i++)
			{
				std::shared_ptr<Animation> anim = std::make_shared<Animation>(scene->mAnimations[i], this);
				anim->SaveWiAnimation(anim.get(), file);
				animator->m_Animations.push_back(anim);
			}

			Animator::SaveWiAnimator(*animator,file);
			delete animator;
		}*/


		aiReleaseImport(scene);
		return true;
	}

	void Model::getWiMeshFromFile(const char* file, bool genBuffers)
	{
		is_root = true;

		File f = FileSystem::OpenIB(file);

		if (f.IsOpen()) {
			m_ModelPath = file;

			size_t model_size;

			// Model size
			f.Read(&model_size, sizeof(size_t));

			models.reserve(model_size);

			//load hierarchy
			model_hierarchy = LoadModelHierarchy(f);

			//load globbal inverse matrix
			f.Read(&globalInverseTransform, sizeof(glm::mat4));

			//load map name to index bone
			size_t bone_index_size;
			f.Read(&bone_index_size, sizeof(size_t));
			for (size_t i = 0; i < bone_index_size; i++)
			{
				std::pair<std::string, BoneInfo> item;
				size_t name_size;
				f.Read(&name_size, sizeof(size_t));
				item.first.resize(name_size);
				f.Read(&item.first[0], name_size);
				f.Read(&item.second.id, sizeof(unsigned int));
				f.Read(&item.second.offsetMatrix, sizeof(glm::mat4));
				f.Read(&item.second.globalTransformation, sizeof(glm::mat4));

				m_BoneInfoMap.insert(item);
			}

			// Material size
			size_t mat_size;

			f.Read(&mat_size, sizeof(size_t));

			materials.resize(mat_size);

			for (size_t i = 0; i < mat_size; i++) {
				size_t name_size;

				f.Read(&name_size, sizeof(size_t));
				materials[i].resize(name_size);

				f.Read(&materials[i][0], name_size);
			}

			// Children models
			for (size_t i = 0; i < model_size; i++) {
				Model* model = new Model(NULL);

				// Read material index
				f.Read(&model->model_mat, sizeof(size_t));

				// Read vbo
				size_t vbo_size;
				f.Read(&vbo_size, sizeof(size_t));
				model->vbo_data.resize(vbo_size);
				f.Read(&model->vbo_data[0], vbo_size * sizeof(float));

				// Read ebo
				size_t ebo_size;
				f.Read(&ebo_size, sizeof(size_t));
				model->ebo_data.resize(ebo_size);
				f.Read(&model->ebo_data[0], ebo_size * sizeof(int));

				// Read Bones
				size_t bones_size;
				f.Read(&bones_size, sizeof(size_t));
				model->bone_data.resize(bones_size);
				
				if (bones_size != 0)
				f.Read(&model->bone_data[0], bones_size * sizeof(VertexBoneData));

			
				//hold reference to root
				model->parent = this;
				
				if (genBuffers) {
					model->generateBuffers();
				}

				models.push_back(model);
			}
		}
		f.Close();
	}

	int Model::GetBoneId(const aiBone* pBone)
	{
		size_t boneid = 0;
		std::string bone_name(pBone->mName.C_Str());
		if (m_BoneInfoMap.find(bone_name) == m_BoneInfoMap.end())
		{
			//allocate an index for a new bone
			boneid = m_BoneInfoMap.size();
			m_BoneInfoMap[bone_name].id = boneid;
		}
		else {
			boneid = m_BoneInfoMap[bone_name].id;
		}
		return boneid;
	}

	Model* Model::loadmesh(const aiMesh* mesh)
	{
		Model* model = new Model(NULL);

		model->model_mat = mesh->mMaterialIndex;

		model->model_name = mesh->mName.C_Str();
		for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
			// Vertices
			model->vbo_data.push_back(mesh->mVertices[j].x);
			model->vbo_data.push_back(mesh->mVertices[j].y);
			model->vbo_data.push_back(mesh->mVertices[j].z);
			// Normals
			model->vbo_data.push_back(mesh->mNormals[j].x);
			model->vbo_data.push_back(mesh->mNormals[j].y);
			model->vbo_data.push_back(mesh->mNormals[j].z);
			// Texture coordinates
			if (mesh->mTextureCoords[0])
			{
				model->vbo_data.push_back(mesh->mTextureCoords[0][j].x);
				model->vbo_data.push_back(mesh->mTextureCoords[0][j].y);
			}
			else {
				model->vbo_data.push_back(0.0f);
				model->vbo_data.push_back(0.0f);
			}
		}
		if (model->vbo_data.empty())
		{
			WI_CORE_ERROR("Error while loading mesh vertex buffer");
		}
		else
		{
			WI_CORE_INFO("Vertex buffer generated correctly");
		}
		// Indices
		for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
			aiFace& face = mesh->mFaces[j];
			for (unsigned int k = 0; k < face.mNumIndices; k++) {
				model->ebo_data.push_back(face.mIndices[k]);
			}
		}
		if (model->ebo_data.empty())
		{
			WI_CORE_ERROR("Error while loading mesh index buffer");
		}
		else
		{
			WI_CORE_INFO("Index buffer generated correctly");
		}
		
		return model;
	}

	ModelHierarchy* Model::loadModelHierarchy(const aiNode* node) //animation
	{
		ModelHierarchy* h = new ModelHierarchy();

		h->name = node->mName.C_Str();

		// Node transform
		aiVector3D translate, scale, rot;
		//aiQuaternion quat;
		node->mTransformation.Decompose(scale, rot, translate);
		h->translation = { translate.x, translate.y, translate.z };
		h->rotation = { rot.x * 180.0f / PI_F, rot.y * 180.0f / PI_F, rot.z * 180.0f / PI_F };
		h->scale = { scale.x, scale.y, scale.z };

		//Store the transformation matrix for aniamtion
		h->Transformation = mat4_cast(node->mTransformation);
	
		// Node meshes
		for (size_t i = 0; i < node->mNumMeshes; i++) {
			h->meshIndexes.push_back(node->mMeshes[i]);
		}

		// Node children
		for (size_t i = 0; i < node->mNumChildren; i++) {

			h->children.push_back(loadModelHierarchy(node->mChildren[i]));
		}

		return h;
	}

	void Model::SetBoneInfo(const aiNode* pNode, const glm::mat4& parentTransform)
	{
		std::string nodeName(pNode->mName.data);
		aiMatrix4x4 m = pNode->mTransformation;
		glm::mat4 nodeTransformation = glm::make_mat4(m.ToPtr());

		WI_INFO("{0}", nodeName.c_str());

		glm::mat4 globalTransformation = parentTransform * nodeTransformation;

		if (m_BoneInfoMap.find(nodeName) != m_BoneInfoMap.end())
		{
			unsigned int boneIndex = m_BoneInfoMap[nodeName].id;
			boneInfo[boneIndex].globalTransformation = globalTransformation;
		}
		for (unsigned int i = 0; i < pNode->mNumChildren; i++)
		{
			SetBoneInfo(pNode->mChildren[i], globalTransformation);
		}
	}

	void Model::CreateCube()
	{
		vbo_data = {
			1,1,1, 0,0,1, 0.625,0.5,
			-1,1,1, 0,0,1, 0.875,0.5,
			-1,-1,1, 0,0,1, 0.875,0.25,
			1,-1,1, 0,0,1, 0.625,0.25,
			1,-1,-1, 0,-1,0, 0.375,0.25,
			1,-1,1, 0,-1,0, 0.625,0.25,
			-1,-1,1, 0,-1,0, 0.625,0,
			-1,-1,-1, 0,-1,0, 0.375,0,
			-1,-1,-1, -1,0,0, 0.375,1,
			-1,-1,1, -1,0,0, 0.625,1,
			-1,1,1, -1,0,0, 0.625,0.75,
			-1,1,-1, -1,0,0, 0.375,0.75,
			-1,1,-1, 0,0,-1, 0.125,0.5,
			1,1,-1, 0,0,-1, 0.375,0.5,
			1,-1,-1, 0,0,-1, 0.375,0.25,
			-1,-1,-1, 0,0,-1, 0.125,0.25,
			1,1,-1, 1,0,0, 0.375,0.5,
			1,1,1, 1,0,0, 0.625,0.5,
			1,-1,1, 1,0,0, 0.625,0.25,
			1,-1,-1, 1,0,0, 0.375,0.25,
			-1,1,-1, 0,1,0, 0.375,0.75,
			-1,1,1, 0,1,0, 0.625,0.75,
			1,1,1,	0,1,0, 0.625,0.5,
			1,1,-1, 0,1,0, 0.375,0.5
		};

		ebo_data = {
			0,1,2,0,2,3,
			4,5,6,4,6,7,
			8,9,10,8,10,11,
			12,13,14,12,14,15,
			16,17,18,16,18,19,
			20,21,22,20,22,23
		};
		is_root = false;
		generateBuffers();
	}

	void Model::CreatePlane()
	{
		vbo_data = {
			-0.50,-0.50,0.00,0.00,0.00,1.00,0.00,1.00
			,0.50,-0.50,0.00,0.00,0.00,1.00,1.00,1.00,
			0.50,0.50,0.00,0.00,0.00,1.00,1.00,0.00,
			-0.50,0.50,0.00,0.00,0.00,1.00,0.00,
			- 0.50,-0.50,0.00,0.00,0.00,1.00,0.00,
			1.00,0.50,-0.50,0.00,0.00,0.00,1.00,1.00
			,1.00,0.50,0.50,0.00,0.00,0.00,1.00,1.00,
			0.00,-0.50,0.50,0.00,0.00,0.00,1.00,0.00,0.00
		};

		ebo_data = {
			0,1,2,0,2,3
		};

		is_root = false;
		generateBuffers();
	}

	void Model::CreatePyramid()
	{
	}

	void Model::CreateSphere()
	{
		int stack_count = 18;
		int sector_count = 36;
		int radius = 1;

		float x, y, z, xy;
		float sectorStep = 2 * PI_F / sector_count;
		float stackStep = PI_F / stack_count;
		float sectorAngle, stackAngle;

		// Generate vertices
		for (int i = 0; i <= stack_count; ++i)
		{
			stackAngle = (float)PI / 2 - i * stackStep;
			xy = radius * cosf(stackAngle);
			z = radius * sinf(stackAngle);

			for (int j = 0; j <= sector_count; ++j)
			{
				sectorAngle = j * sectorStep;

				x = xy * cosf(sectorAngle);
				y = xy * sinf(sectorAngle);
				vbo_data.push_back(x);
				vbo_data.push_back(y);
				vbo_data.push_back(z);
			}
		}

		// Generate indices
		int k1, k2;
		for (int i = 0; i < stack_count; ++i)
		{
			k1 = i * (sector_count + 1);     // beginning of current stack
			k2 = k1 + sector_count + 1;      // beginning of next stack

			for (int j = 0; j < sector_count; ++j, ++k1, ++k2)
			{
				if (i != 0)
				{
					ebo_data.push_back(k1);
					ebo_data.push_back(k2);
					ebo_data.push_back(k1 + 1);
				}

				if (i != (stack_count - 1))
				{
					ebo_data.push_back(k1 + 1);
					ebo_data.push_back(k2);
					ebo_data.push_back(k2 + 1);
				}
			}
		}
		
		is_root = false;
		generateBuffers();
	}

	void Model::SaveModelHierarchy(File file, ModelHierarchy* h)
	{
		size_t name_len = h->name.size();

		file.Write(&name_len, sizeof(size_t));
		file.Write(h->name.c_str(), name_len);

		file.Write(&h->translation, sizeof(Vector3f));
		file.Write(&h->rotation, sizeof(Vector3f));
		file.Write(&h->scale, sizeof(Vector3f));

		//save transform
		file.Write(&h->Transformation, sizeof(glm::mat4));

		size_t mesh_ind_size = h->meshIndexes.size();

		file.Write(&mesh_ind_size, sizeof(size_t));

		if (mesh_ind_size > 0) {
			file.Write(h->meshIndexes.data(), mesh_ind_size);
		}

		size_t child_size = h->children.size();

		file.Write(&child_size, sizeof(size_t));

		if (child_size > 0) {
			for (size_t i = 0; i < child_size; i++) {
				SaveModelHierarchy(file, h->children[i]);
			}
		}
	}

	ModelHierarchy* Model::LoadModelHierarchy(File file)
	{
		ModelHierarchy* h = new ModelHierarchy();

		size_t name_len;

		file.Read(&name_len, sizeof(size_t));
		h->name.resize(name_len);
		file.Read(&h->name[0], name_len);

		file.Read(&h->translation, sizeof(Vector3f));
		file.Read(&h->rotation, sizeof(Vector3f));
		file.Read(&h->scale, sizeof(Vector3f));

		file.Read(&h->Transformation, sizeof(glm::mat4));

		size_t mesh_ind_size;

		file.Read(&mesh_ind_size, sizeof(size_t));

		if (mesh_ind_size > 0) {
			h->meshIndexes.resize(mesh_ind_size);
			file.Read(&h->meshIndexes[0], mesh_ind_size);
		}

		size_t child_size;

		file.Read(&child_size, sizeof(size_t));

		if (child_size > 0) {
			h->children.reserve(child_size);

			for (size_t i = 0; i < child_size; i++) {
				h->children.push_back(LoadModelHierarchy(file));
			}
		}

		return h;
	}

	void Model::GenerateAllBuffers()
	{
		generateBuffers(true);
	}

	void Model::generateBuffers(bool genchildren)
	{
		if (generatedBuffers) return;

		generatedBuffers = true;

		if (genchildren) {
			size_t msize = models.size();

			for (size_t i = 0; i < msize; i++) {
				models[i]->generateBuffers(true);
			}
		}

		if (is_root) return;

		WI_CORE_INFO("Generating buffers...");
		GL(GenBuffers(1, &vbo));
		GL(GenBuffers(1, &ebo));
		GL(GenVertexArrays(1, &vao));
		
		GL(BindVertexArray(vao));


		GL(BindBuffer(GL_ARRAY_BUFFER, vbo));
		GL(BufferData(GL_ARRAY_BUFFER, vbo_data.size() * sizeof(float), vbo_data.data(), GL_STATIC_DRAW));
		
		GL(BindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
		GL(BufferData(GL_ELEMENT_ARRAY_BUFFER, ebo_data.size() * sizeof(int), ebo_data.data(), GL_STATIC_DRAW));

		
		GL(EnableVertexAttribArray(POSITION_DATA));
		GL(VertexAttribPointer(POSITION_DATA, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0));
		GL(EnableVertexAttribArray(NORMAL_DATA));
		GL(VertexAttribPointer(NORMAL_DATA, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))));
		GL(EnableVertexAttribArray(TEXTURE_DATA));
		GL(VertexAttribPointer(TEXTURE_DATA, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))));	

	
		if (!bone_data.empty())
		{
			GL(GenBuffers(1, &bonevb));
			GL(BindBuffer(GL_ARRAY_BUFFER, bonevb));
			GL(BufferData(GL_ARRAY_BUFFER, sizeof(VertexBoneData) * bone_data.size(), bone_data.data(), GL_STATIC_DRAW));
			

			//bone location
			GL(EnableVertexAttribArray(BONE_DATA));
			GL(VertexAttribIPointer(BONE_DATA, MAX_NUM_BONES_PER_VERTEX, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0));
			//weights location
			GL(EnableVertexAttribArray(WEIGHT_DATA));
			GL(VertexAttribPointer(WEIGHT_DATA, MAX_NUM_BONES_PER_VERTEX, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData),(const GLvoid*)16));
			
			if(bonevb == -1)
			{
				WI_CORE_ERROR("Error generating bone vertex buffer");
			}
		}

		GL(BindBuffer(GL_ARRAY_BUFFER, 0));
		GL(BindVertexArray(0));


		for (int i = 0; i < vbo_data.size(); i += 8)
		{
			glm::vec3 vec = { vbo_data[i], vbo_data[i + 1], vbo_data[i + 2] };
			boundingBox.extend(vec);
		}
		bbebo_data = {
			0, 1, 1, 2, 2, 3, 3, 0, // Front
			4, 5, 5, 6, 6, 7, 7, 4, // Back
			0, 4, 1, 5, 2, 6, 3, 7
		};
		bbvbo_data = {
			boundingBox.getMax().x, boundingBox.getMax().y, boundingBox.getMax().z, //TOP
			boundingBox.getMin().x, boundingBox.getMax().y, boundingBox.getMax().z,
			boundingBox.getMin().x, boundingBox.getMax().y, boundingBox.getMin().z,
			boundingBox.getMax().x, boundingBox.getMax().y, boundingBox.getMin().z,

			boundingBox.getMax().x, boundingBox.getMin().y, boundingBox.getMax().z, //Bottom
			boundingBox.getMin().x, boundingBox.getMin().y, boundingBox.getMax().z,
			boundingBox.getMin().x, boundingBox.getMin().y, boundingBox.getMin().z,
			boundingBox.getMax().x, boundingBox.getMin().y, boundingBox.getMin().z,
		};

		GL(GenBuffers(1, &bbvbo));
		GL(GenBuffers(1, &bbebo));
		GL(GenVertexArrays(1, &bbvao));

		GL(BindVertexArray(bbvao));
		GL(BindBuffer(GL_ARRAY_BUFFER, bbvbo));
		GL(BufferData(GL_ARRAY_BUFFER, bbvbo_data.size() * sizeof(float), bbvbo_data.data(), GL_STATIC_DRAW));

		GL(BindBuffer(GL_ELEMENT_ARRAY_BUFFER, bbebo));
		GL(BufferData(GL_ELEMENT_ARRAY_BUFFER, bbebo_data.size() * sizeof(int), bbebo_data.data(), GL_STATIC_DRAW));

		GL(VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));
		GL(EnableVertexAttribArray(0));


		GL(BindBuffer(GL_ARRAY_BUFFER, 0));
		GL(BindVertexArray(0));
	}

	Model::Model(const char* file)
		: ebo(0), vbo(0), vao(0),
		generatedBuffers(false)
	{
		is_root = true;
		model_hierarchy = NULL;

		if (file) {
			if (strcmp(file, "cube") == 0)
			{
				CreateCube();
			}
			else if (strcmp(file, "plane") == 0)
			{
				CreatePlane();
			}
			else if (strcmp(file, "pyramid") == 0)
			{
				CreatePyramid();
			}
			else if (strcmp(file, "sphere") == 0)
			{
				CreateSphere();
			}
			else
			{
				ModelSettings settings;
				getMeshFromFile(file, &settings);
			}
		}
		else {
			is_root = false;
		}
	}

	Model::~Model()
	{
		size_t m_size = models.size();

		for (size_t i = 0; i < m_size; i++) {
			delete models[i];
		}

		if (model_hierarchy) {
			delete model_hierarchy;
		}

		GL(DeleteBuffers(1, &vbo));
		GL(DeleteBuffers(1, &ebo));
		GL(DeleteBuffers(1, &bbvbo));
		GL(DeleteBuffers(1, &bbvbo));

		GL(DeleteVertexArrays(1, &vao));
		GL(DeleteVertexArrays(1, &bbvao));
	}

	void Model::Render()
	{
		if(is_root){
			size_t meshCount = models.size();

			for (size_t i = 0; i < meshCount; i++) {
				models[i]->Render();
			}
		}
		else {
			GL(BindVertexArray(vao));
			GL(DrawElements(GL_TRIANGLES, (GLsizei)ebo_data.size(), GL_UNSIGNED_INT, 0));
			GL(BindVertexArray(0));
		}
	}

	void Model::DrawBoudingBox()
	{
		if (is_root) {
			size_t meshCount = models.size();

			for (size_t i = 0; i < meshCount; i++) {
				models[i]->DrawBoudingBox();
			}
		}
		else {
			GL(BindVertexArray(bbvao));
			GL(DrawElements(GL_LINES, (GLsizei)bbebo_data.size(), GL_UNSIGNED_INT, 0));
			GL(BindVertexArray(0));
		}
	}

	void Model::LoadMesh(const char* file, ModelSettings* settings)
	{
		getMeshFromFile(file, settings);
	}

	void Model::LoadWiMesh(const char* file, bool genBuffers)
	{
		getWiMeshFromFile(file, genBuffers);
	}

	void Model::LoadMeshBones(unsigned int meshIndex,const aiMesh* mesh)
	{
		for (size_t i = 0; i < mesh->mNumBones; i++)
		{
			LoadSingleBone(meshIndex, mesh->mBones[i]);
		}
	}

	void Model::LoadSingleBone(int meshIndex, aiBone* bone)
	{ 		
		int BoneId = -1;
		if (m_BoneInfoMap.find(bone->mName.C_Str()) == m_BoneInfoMap.end())
		{
			BoneInfo newBoneInfo;
			newBoneInfo.id = m_BoneCounter;
			glm::mat4 offset = mat4_cast(bone->mOffsetMatrix);
			newBoneInfo.offsetMatrix = mat4_cast(bone->mOffsetMatrix);
			parent->m_BoneInfoMap[bone->mName.C_Str()] = newBoneInfo;
			BoneId = m_BoneCounter;
			m_BoneCounter++;
		}
		else
		{
			BoneId = parent->m_BoneInfoMap[bone->mName.C_Str()].id;
		}

		for (unsigned int i = 0; i < bone->mNumWeights; i++) {
			const aiVertexWeight& vw = bone->mWeights[i];
			unsigned int GlobalVertexID = parent->meshes[meshIndex] + bone->mWeights[i].mVertexId;
			bone_data[GlobalVertexID].AddBoneData(BoneId, vw.mWeight);
		}

	}

	void Model::PrintAssimpBoneMatrix(const aiBone* mat)
	{
		const aiMatrix4x4 m = mat->mOffsetMatrix;
		WI_INFO("{0} {1} {2} {3}\n", m.a1, m.a2, m.a3, m.a4);
		WI_INFO("{0} {1} {2} {3}\n", m.b1, m.b2, m.b3, m.b4);
		WI_INFO("{0} {1} {2} {3}\n", m.c1, m.c2, m.c3, m.c4);
		WI_INFO("{0} {1} {2} {3}\n", m.d1, m.d2, m.d3, m.d4);
	}
	void Model::PrintAssimpNodeMatrix(const aiNode* mat)
	{
		const aiMatrix4x4 m = mat->mTransformation;
		WI_INFO("{0} {1} {2} {3}\n", m.a1, m.a2, m.a3, m.a4);
		WI_INFO("{0} {1} {2} {3}\n", m.b1, m.b2, m.b3, m.b4);
		WI_INFO("{0} {1} {2} {3}\n", m.c1, m.c2, m.c3, m.c4);
		WI_INFO("{0} {1} {2} {3}\n", m.d1, m.d2, m.d3, m.d4);
	}
	void Model::PrintGlmMatrix(const glm::mat4& mat)
	{
		WI_INFO("{0}_{1}_{2}_{3}", mat[0][0], mat[0][1], mat[0][2], mat[0][3]);
		WI_INFO("{0}_{1}_{2}_{3}", mat[1][0], mat[1][1], mat[1][2], mat[1][3]);
		WI_INFO("{0}_{1}_{2}_{3}", mat[2][0], mat[2][1], mat[2][2], mat[2][3]);
		WI_INFO("{0}_{1}_{2}_{3}", mat[3][0], mat[3][1], mat[3][2], mat[3][3]);
		WI_INFO("\n");
	}

	Model* Model::GetModelFromFile(const char* file, ModelSettings* settings)
	{
		Model* model = new Model(NULL);

		if (!model->getMeshFromFile(file, settings, false)) 
		{
			delete model;
			model = nullptr; 
			WI_ERROR("Error loading {0}", file); 
		}

		return model;
	}

	void Model::SaveModel(Model* model, const char* file)
	{
		File f = FileSystem::OpenOB(file);

		if (f.IsOpen()) {
			// Model size
			size_t model_size = model->models.size();
			f.Write(&model_size, sizeof(size_t));

			// Model hierarchy
			SaveModelHierarchy(f, model->model_hierarchy);
			
			//save global inverse matrix
			f.Write(&model->globalInverseTransform, sizeof(glm::mat4));

			//bone to index map			
			size_t bone_index_size = model->m_BoneInfoMap.size();
			f.Write(&bone_index_size, sizeof(size_t));
			std::map<std::string, BoneInfo>::iterator it;
			for (it = model->m_BoneInfoMap.begin(); it != model->m_BoneInfoMap.end();it++)
			{
				size_t name_size = it->first.size();
				f.Write(&name_size, sizeof(size_t));
				f.Write(it->first.c_str(), name_size);
				f.Write(&it->second.id, sizeof(unsigned int));
				f.Write(&it->second.offsetMatrix, sizeof(glm::mat4));
				f.Write(&it->second.globalTransformation, sizeof(glm::mat4));
			}

			// Material size
			size_t mat_size = model->materials.size();

			f.Write(&mat_size, sizeof(size_t));
			for (size_t i = 0; i < mat_size; i++) {
				size_t name_size = model->materials[i].size();

				f.Write(&name_size, sizeof(size_t));
				f.Write(model->materials[i].c_str(), name_size);
			}

			// Model list
			for (size_t i = 0; i < model_size; i++) {
				Model* c_model = model->models[i];

				// Material index
				size_t mat_ind = c_model->getMaterialIndex();
				f.Write(&mat_ind, sizeof(size_t));

				// Model vbo
				size_t vbo_size = c_model->vbo_data.size();
				f.Write(&vbo_size, sizeof(size_t));
				f.Write(c_model->vbo_data.data(), vbo_size * sizeof(float));

				// Model ebo
				size_t ebo_size = c_model->ebo_data.size();
				f.Write(&ebo_size, sizeof(size_t));
				f.Write(c_model->ebo_data.data(), ebo_size * sizeof(int));

				//Model bones
				size_t bones_size = c_model->bone_data.size();
				f.Write(&bones_size, sizeof(size_t));
				f.Write(c_model->bone_data.data(), bones_size * sizeof(VertexBoneData));
			}

		}

		f.Close();
	}

	static void CheckImportObj(const std::filesystem::path& path)
	{
		std::string p = path.string();
		Wiwa::Resources::standarizePath(p);
		if (path.extension() == ".obj") {
			std::filesystem::path rpath = Wiwa::Resources::_assetToLibPath(path.string().c_str());
			std::filesystem::path rp = rpath.remove_filename();
			std::filesystem::create_directories(rp);
			Wiwa::FileSystem::Copy(path.string().c_str(), rpath.string().c_str());
		}
	}

	void Model::SaveModelAsOBJ(Model* model, const char* file)
	{
		unsigned int flags = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals;//aiProcessPreset_TargetRealtime_Quality | aiProcess_FlipUVs;//aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_FlipUVs;
		
		// Convert from library to assets path, change extension to fbx
		std::string input_path = file;
		Wiwa::Resources::SetAssetPath(input_path);
		input_path = input_path.substr(0, input_path.find_last_of('.')) + ".fbx";

		sbyte* file_data = NULL;
		size_t file_buf_size = FileSystem::ReadAll(input_path.c_str(), &file_data);

		WI_INFO("Loaded with bytes: {}", file_buf_size);
		const aiScene* scene = aiImportFileFromMemory(file_data, file_buf_size, flags, NULL);

		delete[] file_data;

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			WI_ERROR("Couldn't load mesh file: {0}", input_path.c_str());
			return;
		}
		// Convert from assets to library path, to navmesh folder, change extension to obj
		//std::string library_output = "library\\navmesh\\";
		std::string assets_output = "assets\\navmesh\\";
		//FileSystem::CreateDir(library_output.c_str());
		FileSystem::CreateDir(assets_output.c_str());

		std::filesystem::path pathObj(input_path);
		/*library_output += pathObj.filename().string();
		library_output = library_output.substr(0, library_output.find_last_of('.')) + ".obj";*/
		
		assets_output += pathObj.filename().string();
		assets_output = assets_output.substr(0, assets_output.find_last_of('.')) + ".obj";

		Assimp::Exporter exporter;
		// Export the scene to an OBJ file
		/*if (exporter.Export(scene, "obj", library_output.c_str())) {
			std::cout << "FBX file successfully converted to OBJ." << std::endl;
		}*/
		if (exporter.Export(scene, "obj", assets_output.c_str())) {
			std::cout << "FBX file successfully converted to OBJ." << std::endl;
		}
		/*else {
			std::cerr << "Failed to convert FBX file to OBJ." << std::endl;
			aiReleaseImport(scene);
			return;
		}*/
		aiReleaseImport(scene);
		CheckImportObj(assets_output.c_str());
	}
}