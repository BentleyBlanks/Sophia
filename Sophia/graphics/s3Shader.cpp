#include <graphics/s3Shader.h>
#include <graphics/s3Renderer.h>
#include <fake_unity_shader/shader.h>
#include <fake_unity_shader/shader_parser.h>
#include <core/log/s3Log.h>

static bool fakeUnityShaderInitialized = false;

// =============================================== Utility ===============================================
class s3ShaderConstantBuffer
{
public:
	std::vector<void*> dataList;
	std::vector<ID3D11Buffer*> bufferList;
	int32 cbCount;
};

class s3ShaderParser
{
public:
	s3ShaderParser():
		shader(nullptr),
		shaderName(""),
		passList(nullptr),
		passNum(-1)
	{}

	// fake unity shader
	shader* shader;
	std::string shaderName;
	int32 passNum;
	
	std::vector<shader_pass_permutation>* passList;
	std::vector<s3ShaderConstantBuffer> passConstantBufferList;

	std::vector<ID3D11PixelShader*> pixelShaderList;
	std::vector<ID3D11VertexShader*> vertexShaderList;
};

class s3ShaderValue
{
public:
	t3Matrix4x4 matrix;
	int32 iValue;
	float32 fValue;
	bool bValue;

	t3Vector4f vector4f;
	t3Vector3f vector3f;
	t3Vector2f vector2f;
	t3Vector4i vector4i;
	t3Vector3i vector3i;
	t3Vector2i vector2i;

	variable_type type;
};

bool s3Shader::GetValue(std::string name, int32 type, s3ShaderValue& value) const
{
	value.type = (variable_type)type;

	// multiple pass
	auto& passList = *shaderParser->passList;
	for (int32 i = 0; i < passList.size(); i++)
	{
		// loop whole cb info list in different pass
		auto& pass = passList[i];
		auto passIter = pass.cb_list.begin();
		while (passIter != pass.cb_list.end())
		{
			//--! same variable name may existed in multiple cb
			auto& cbInfo = passIter->second;
			auto& cbInfoList = cbInfo.items;
			auto& cbIter = cbInfoList.find(name);
			if (cbIter != cbInfoList.end())
			{
				// find the first variable with the same name and type 
				auto& cbMapping = cbIter->second;
				if (cbMapping.type == type)
				{
					char* valuePtr = (char*)cbInfo.data + cbMapping.offset;
					switch (type)
					{
						case eVT_FLOAT:
							value.fValue = *((float32*)valuePtr);
							break;
						case eVT_FLOAT2:
							value.vector2f = *((t3Vector2f*)valuePtr);
							break;
						case eVT_FLOAT3:
							value.vector3f = *((t3Vector3f*)valuePtr);
							break;
						case eVT_FLOAT4:
							value.vector4f = *((t3Vector4f*)valuePtr);
							break;
						case eVT_FLOAT4X4:
							value.matrix = *((t3Matrix4x4*)valuePtr);
							break;
						case eVT_INT:
							value.iValue = *((int32*)valuePtr);
							break;
						case eVT_INT2:
							value.vector2i = *((t3Vector2i*)valuePtr);
							break;
						case eVT_INT3:
							value.vector3i = *((t3Vector3i*)valuePtr);
							break;
						case eVT_INT4:
							value.vector4i = *((t3Vector4i*)valuePtr);
							break;
						case eVT_BOOL:
							value.bValue = *((bool*)valuePtr);
							break;
					}
					return true;
				}
			}
			passIter++;
		}
	}

	// variable not existed with type unknown
	value.type = eVT_Unknow;
	return false;
}

bool s3Shader::SetValue(std::string name, s3ShaderValue& value)
{
	auto existed = false;
	auto& passList = *shaderParser->passList;
	for (int32 i = 0; i < passList.size(); i++)
	{
		// loop whole cb info list
		auto& pass = passList[i];
		auto passIter = pass.cb_list.begin();
		while (passIter != pass.cb_list.end())
		{
			//--! same variable name may existed in multiple cb
			auto& cbInfo = passIter->second;
			auto& cbInfoList = cbInfo.items;
			auto& cbIter = cbInfoList.find(name);
			if (cbIter != cbInfoList.end())
			{
				// find the variable with the same name and type 
				auto& cbMapping = cbIter->second;
				if (cbMapping.type == value.type)
				{
					char* valuePtr = (char*)(cbInfo.data) + cbIter->second.offset;
					switch (value.type)
					{
					case eVT_FLOAT:
					{
						float32* ptr = (float32*)valuePtr;
						*ptr = value.fValue;
						break;
					}
					case eVT_FLOAT2:
					{
						t3Vector2f* ptr = (t3Vector2f*)valuePtr;
						*ptr = value.vector2f;
						break;
					}
					case eVT_FLOAT3:
					{
						t3Vector3f* ptr = (t3Vector3f*)valuePtr;
						*ptr = value.vector3f;
						break;
					}
					case eVT_FLOAT4:
					{
						t3Vector4f* ptr = (t3Vector4f*)valuePtr;
						*ptr = value.vector4f;
						break;
					}
					case eVT_FLOAT4X4:
					{
						t3Matrix4x4* ptr = (t3Matrix4x4*)valuePtr;
						*ptr = value.matrix;
						break;
					}
					case eVT_INT:
					{
						int* ptr = (int*)valuePtr;
						*ptr = value.iValue;
						break;
					}
					case eVT_INT2:
					{
						t3Vector2i* ptr = (t3Vector2i*)valuePtr;
						*ptr = value.vector2i;
						break;
					}
					case eVT_INT3:
					{
						t3Vector3i* ptr = (t3Vector3i*)valuePtr;
						*ptr = value.vector3i;
						break;
					}
					case eVT_INT4:
					{
						t3Vector4i* ptr = (t3Vector4i*)valuePtr;
						*ptr = value.vector4i;
						break;
					}
					case eVT_BOOL:
					{
						bool* ptr = (bool*)valuePtr;
						*ptr = value.bValue;
						break;
					}
					}
					existed = true;
				}
			}
			passIter++;
		}
	}

	return existed;
}

// =============================================== s3Shader ===============================================
s3Shader::s3Shader():
	isLoaded(false),
	shaderParser(nullptr)
{}

s3Shader::~s3Shader()
{
	S3_SAFE_DELETE(shaderParser);
}

bool s3Shader::load(std::string fileName)
{	
	if (!fakeUnityShaderInitialized)
	{
		//--! the fakeUnityShader init could be moved into renderer
		shader_init(s3Renderer::get().getDevice(), "../Sophia/thirdparty/fakeUnityShader/fake_unity_shader/shader_parser/");
	}

	const char* name = shader_load(fileName.c_str());
	if (!name)
	{
		s3Log::warning("s3Shader::load() create shader %s failed\n", fileName.c_str());
		return false;
	}
	else
	{
		S3_SAFE_DELETE(shaderParser);

		auto& shader = g_shaderMap[shaderParser->shaderName];

		// s3ShaderParser initialize
		shaderParser = new s3ShaderParser();
		shaderParser->shader     = &shader;
		shaderParser->shaderName = name;

		//--! Num of tech should be the same with passList
		auto& techList = g_shaderMap[name].tech_list;
		if (techList.size() == 1)
		{
			shaderParser->passList = &techList[0].pass_list;
		}
		else
		{
			S3_SAFE_DELETE(shaderParser);
			s3Log::warning("s3Shader::load() create shader %s failed, passList count error\n");
			isLoaded = false;
			return false;
		}

		auto passList = shaderParser->passList;
		shaderParser->passNum = passList->size();
		for (int32 j = 0; j < passList->size(); j++)
		{
			auto& permutation = (*passList)[j];

			// pixel/vertex shaderList and passNum initialize
			if (permutation.pixelShader)
				shaderParser->pixelShaderList.push_back(permutation.pixelShader);

			if (permutation.vertexShader)
				shaderParser->vertexShaderList.push_back(permutation.vertexShader);

			// constant buffer list initialzie
			auto& cbList = permutation.cb_list;
			auto cbIter = cbList.begin();
			while (cbIter != cbList.end())
			{
				s3ShaderConstantBuffer cb;
				cb.dataList.push_back(cbIter->second.data);
				cb.bufferList.push_back(cbIter->second.buffer);
				shaderParser->passConstantBufferList.push_back(cb);

				cbIter++;
			}
		}

		isLoaded = true;
		s3Log::success("s3Shader::load() create shader %s successfully\n", fileName.c_str());
		return true;
	}
}

float32 s3Shader::GetFloat(std::string name) const
{
	s3ShaderValue value;
	bool exsited = GetValue(name, eVT_FLOAT, value);

	if (!exsited)
	{
		s3Log::warning("s3Shader::GetFloat() variable %s doesn't existed\n", name.c_str());
		return 0.0f;
	}
	else
	{
		return value.fValue;
	}
}

int32 s3Shader::GetInt(std::string name) const
{
	s3ShaderValue value;
	bool exsited = GetValue(name, eVT_INT, value);

	if (!exsited)
	{
		s3Log::warning("s3Shader::GetInt() variable %s doesn't existed\n", name.c_str());
		return 0;
	}
	else
	{
		return value.iValue;
	}
}

t3Matrix4x4 s3Shader::GetMatrix(std::string name) const
{
	s3ShaderValue value;
	bool exsited = GetValue(name, eVT_FLOAT4X4, value);

	if (!exsited)
	{
		s3Log::warning("s3Shader::GetMatrix() variable %s doesn't existed\n", name.c_str());
		return t3Matrix4x4::newIdentityMatrix();
	}
	else
	{
		return value.matrix;
	}
}

t3Vector4f s3Shader::GetFloat4(std::string name) const
{
	s3ShaderValue value;
	bool exsited = GetValue(name, eVT_FLOAT4, value);

	if (!exsited)
	{
		s3Log::warning("s3Shader::GetFloat4() variable %s doesn't existed\n", name.c_str());
		return t3Vector4f::zero();
	}
	else
	{
		return value.vector4f;
	}
}

t3Vector3f s3Shader::GetFloat3(std::string name) const
{
	s3ShaderValue value;
	bool exsited = GetValue(name, eVT_FLOAT3, value);

	if (!exsited)
	{
		s3Log::warning("s3Shader::GetFloat3() variable %s doesn't existed\n", name.c_str());
		return t3Vector3f::zero();
	}
	else
	{
		return value.vector3f;
	}
}

t3Vector2f s3Shader::GetFloat2(std::string name) const
{
	s3ShaderValue value;
	bool exsited = GetValue(name, eVT_FLOAT2, value);

	if (!exsited)
	{
		s3Log::warning("s3Shader::GetFloat2() variable %s doesn't existed\n", name.c_str());
		return t3Vector2f::zero();
	}
	else
	{
		return value.vector2f;
	}
}

t3Vector4i s3Shader::GetInt4(std::string name) const
{
	s3ShaderValue value;
	bool exsited = GetValue(name, eVT_INT4, value);

	if (!exsited)
	{
		s3Log::warning("s3Shader::GetInt4() variable %s doesn't existed\n", name.c_str());
		return t3Vector4i::zero();
	}
	else
	{
		return value.vector4i;
	}
}

t3Vector3i s3Shader::GetInt3(std::string name) const
{
	s3ShaderValue value;
	bool exsited = GetValue(name, eVT_INT3, value);

	if (!exsited)
	{
		s3Log::warning("s3Shader::GetInt3() variable %s doesn't existed\n", name.c_str());
		return t3Vector3i::zero();
	}
	else
	{
		return value.vector3i;
	}
}

t3Vector2i s3Shader::GetInt2(std::string name) const
{
	s3ShaderValue value;
	bool exsited = GetValue(name, eVT_INT2, value);

	if (!exsited)
	{
		s3Log::warning("s3Shader::GetInt2() variable %s doesn't existed\n", name.c_str());
		return t3Vector2i::zero();
	}
	else
	{
		return value.vector2i;
	}
}

bool s3Shader::GetKeyword(std::string name) const
{
	return false;
}

bool s3Shader::GetBool(std::string name) const
{
	s3ShaderValue value;
	bool exsited = GetValue(name, eVT_BOOL, value);

	if (!exsited)
	{
		s3Log::warning("s3Shader::GetBool() variable %s doesn't existed\n", name.c_str());
		return false;
	}
	else
	{
		return value.bValue;
	}
}

bool s3Shader::IsLoaded() const
{
	return isLoaded;
}

bool s3Shader::SetKeyword(std::string name, bool enableKeyword)
{
	return false;
}

bool s3Shader::SetBool(std::string name, bool value)
{
	s3ShaderValue shaderValue;
	shaderValue.type   = eVT_BOOL;
	shaderValue.bValue = value;

	if(!SetValue(name, shaderValue))
	{
		s3Log::warning("s3Shader::SetBool() variable %s doesn't existed\n", name.c_str());
		return false;
	}
	return true;
}

bool s3Shader::SetInt(std::string name, int32 value)
{
	s3ShaderValue shaderValue;
	shaderValue.type   = eVT_INT;
	shaderValue.iValue = value;

	if (!SetValue(name, shaderValue))
	{
		s3Log::warning("s3Shader::SetInt() variable %s doesn't existed\n", name.c_str());
		return false;
	}
	return true;
}

bool s3Shader::SetFloat(std::string name, float32 value)
{
	s3ShaderValue shaderValue;
	shaderValue.type   = eVT_FLOAT;
	shaderValue.fValue = value;

	if (!SetValue(name, shaderValue))
	{
		s3Log::warning("s3Shader::SetFloat() variable %s doesn't existed\n", name.c_str());
		return false;
	}
	return true;
}

bool s3Shader::SetMatrix(std::string name, t3Matrix4x4 value)
{
	s3ShaderValue shaderValue;
	shaderValue.type   = eVT_FLOAT4X4;
	shaderValue.matrix = value;

	if (!SetValue(name, shaderValue))
	{
		s3Log::warning("s3Shader::SetMatrix() variable %s doesn't existed\n", name.c_str());
		return false;
	}
	return true;
}

std::vector<void*>* s3Shader::GetConstantBufferDataList(int32 pass) const
{
	auto& cbList = shaderParser->passConstantBufferList;
	if (pass < 0 || pass >= cbList.size())
	{
		s3Log::warning("s3Shader::GetConstantBufferList() pass %d illegal\n", pass);
		return nullptr;
	}
	auto& cb = cbList[pass];
	return &cb.dataList;
}

std::vector<ID3D11Buffer*>* s3Shader::GetConstantBufferList(int32 pass) const
{
	auto& cbList = shaderParser->passConstantBufferList;
	if (pass < 0 || pass >= cbList.size())
	{
		s3Log::warning("s3Shader::GetConstantBufferList() pass %d illegal\n", pass);
		return nullptr;
	}
	auto& cb = cbList[pass];
	return &cb.bufferList;
}

int32 s3Shader::GetPassConstantBufferNum(int32 pass) const
{
	auto cb = GetConstantBufferList(pass);
	if(cb) return cb->size();
	else return 1;
}

ID3D11PixelShader* s3Shader::GetPixelShader(int32 pass) const
{
	if (pass >= 0 && pass < shaderParser->pixelShaderList.size())
		return shaderParser->pixelShaderList[pass];
	else
		return nullptr;
}

ID3D11VertexShader* s3Shader::GetVertexShader(int32 pass) const
{
	if (pass >= 0 && pass < shaderParser->vertexShaderList.size())
		return shaderParser->vertexShaderList[pass];
	else
		return nullptr;
}

int32 s3Shader::GetPassNum() const
{
	return shaderParser->passNum;
}

bool s3Shader::SetFloat4(std::string name, t3Vector4f value)
{
	s3ShaderValue shaderValue;
	shaderValue.type     = eVT_FLOAT4;
	shaderValue.vector4f = value;

	if (!SetValue(name, shaderValue))
	{
		s3Log::warning("s3Shader::SetFloat4() variable %s doesn't existed\n", name.c_str());
		return false;
	}
	return true;
}

bool s3Shader::SetFloat3(std::string name, t3Vector3f value)
{
	s3ShaderValue shaderValue;
	shaderValue.type     = eVT_FLOAT3;
	shaderValue.vector3f = value;

	if (!SetValue(name, shaderValue))
	{
		s3Log::warning("s3Shader::SetFloat3() variable %s doesn't existed\n", name.c_str());
		return false;
	}
	return true;
}

bool s3Shader::SetFloat2(std::string name, t3Vector2f value)
{
	s3ShaderValue shaderValue;
	shaderValue.type     = eVT_FLOAT2;
	shaderValue.vector2f = value;

	if (!SetValue(name, shaderValue))
	{
		s3Log::warning("s3Shader::SetFloat2() variable %s doesn't existed\n", name.c_str());
		return false;
	}
	return true;
}

bool s3Shader::SetInt4(std::string name, t3Vector4i value)
{
	s3ShaderValue shaderValue;
	shaderValue.type     = eVT_INT4;
	shaderValue.vector4i = value;

	if (!SetValue(name, shaderValue))
	{
		s3Log::warning("s3Shader::SetInt4() variable %s doesn't existed\n", name.c_str());
		return false;
	}
	return true;
}

bool s3Shader::SetInt3(std::string name, t3Vector3i value)
{
	s3ShaderValue shaderValue;
	shaderValue.type     = eVT_INT3;
	shaderValue.vector3i = value;

	if (!SetValue(name, shaderValue))
	{
		s3Log::warning("s3Shader::SetInt3() variable %s doesn't existed\n", name.c_str());
		return false;
	}
	return true;
}

bool s3Shader::SetInt2(std::string name, t3Vector2i value)
{
	s3ShaderValue shaderValue;
	shaderValue.type     = eVT_INT2;
	shaderValue.vector2i = value;

	if (!SetValue(name, shaderValue))
	{
		s3Log::warning("s3Shader::SetInt2() variable %s doesn't existed\n", name.c_str());
		return false;
	}
	return true;
}

void s3Shader::print() const
{
	if (!isLoaded) return;

	s3Log::debug("=============================== s3Shader::print() begin ===============================\n");

	auto& shader = g_shaderMap[shaderParser->shaderName];
	s3Log::debug("shader.name: %s\n", shader.name.c_str());

	auto& techList = shader.tech_list;
	for (int32 i = 0; i < techList.size(); i++)
	{
		auto& pass = techList[i];
		s3Log::debug("pass[%d].name: %s\n", i, pass.name.c_str());

		auto& passList = pass.pass_list;
		for (int32 j = 0; j < passList.size(); j++)
		{
			auto& permutation = passList[j];

			// pass name and shader ptr
			s3Log::debug("permutation[%d].name: %s\n", j, permutation.name.c_str());
			s3Log::debug("permutation[%d].vertexShader: %d\n", j, permutation.vertexShader);
			s3Log::debug("permutation[%d].pixelShader: %d\n", j, permutation.pixelShader);
			s3Log::debug("permutation[%d].hullShader: %d\n", j, permutation.hullShader);
			s3Log::debug("permutation[%d].domainShader: %d\n", j, permutation.domainShader);
			s3Log::debug("permutation[%d].geometryShader: %d\n", j, permutation.geometryShader);
			s3Log::debug("\n");

			// shader keywords
			auto& keyWordList = permutation.keyword_list;
			for (int32 keyword = 0; keyword < keyWordList.size(); keyword++)
			{
				s3Log::debug("keyword[%d]: %s\n", keyword, keyWordList[keyword].c_str());
			}
			s3Log::debug("\n");

			// constant buffer
			auto& cbList = permutation.cb_list;
			auto& cbIter = cbList.begin();
			while (cbIter != cbList.end())
			{
				s3Log::debug("cb[%s].name: %s\n", cbIter->first.c_str(), cbIter->first.c_str());

				auto& cbInfo = cbIter->second;
				s3Log::debug("cb.size: %d\n", cbInfo.size);
				
				auto& cbInfoIter = cbInfo.items.begin();
				while (cbInfoIter != cbInfo.items.end())
				{
					auto& cbName    = cbInfoIter->first;
					auto& cbMapping = cbInfoIter->second;
					s3Log::debug("cb.items[%s].variable_name: %s\n", cbName.c_str(), cbMapping.variable_name.c_str());
					s3Log::debug("cb.items[%s].offset: %d\n", cbName.c_str(), cbMapping.offset);
					s3Log::debug("cb.items[%s].size: %d\n", cbName.c_str(), cbMapping.size);
					s3Log::debug("cb.items[%s].slot: %d\n", cbName.c_str(), cbMapping.slot);
					s3Log::debug("cb.items[%s].type: %d\n", cbName.c_str(), cbMapping.type);
					s3Log::debug("\n");
					cbInfoIter++;
				}

				for (int32 x = 0; x < ST_Num; x++)
					s3Log::debug("cbInfo.slot: %d\n", cbInfo.slot[x]);

				for (int32 y = 0; y < ST_Num; y++)
					s3Log::debug("cbInfo.bindStage: %d\n", cbInfo.bindStage[y]);

				cbIter++;
			}
			s3Log::debug("\n");
			
			// texture
			auto& texList = permutation.tex_list;
			auto& texIter = texList.begin();
			while (texIter != texList.end())
			{
				// texture name
				s3Log::debug("texture.name: %s\n", texIter->first.c_str());

				// texture info
				auto& textureInfo = texIter->second;
				s3Log::debug("texture.type: %d\n", textureInfo.type);
				for (int32 x = 0; x < ST_Num; x++)
					s3Log::debug("texture.slot: %d\n", textureInfo.slot[x]);

				for (int32 y = 0; y < ST_Num; y++)
					s3Log::debug("texture.bindStage: %d\n", textureInfo.bindStage[y]);

				texIter++;
			}
			s3Log::debug("\n");

			// sampler
			auto& samplerList = permutation.sampler_list;
			auto& samplerIter = samplerList.begin();
			while (samplerIter != samplerList.end())
			{
				// sampler name
				s3Log::debug("sampler.name: %s\n", samplerIter->first.c_str());

				// sampler info
				auto& samplerInfo = samplerIter->second;
				s3Log::debug("samplerInfo.followTexture: %d\n", samplerInfo.followTexture);
				s3Log::debug("samplerInfo.default_filte: %d\n", samplerInfo.default_filter);
				s3Log::debug("samplerInfo.default_address: %d\n", samplerInfo.default_address);
				s3Log::debug("samplerInfo.default_comp: %d\n", samplerInfo.default_comp);

				for (int32 x = 0; x < ST_Num; x++)
					s3Log::debug("samplerInfo.slot: %d\n", samplerInfo.slot[x]);

				for (int32 y = 0; y < ST_Num; y++)
					s3Log::debug("samplerInfo.bindStage: %d\n", samplerInfo.bindStage[y]);

				samplerIter++;
			}

			// input layout
			auto& inputLayoutElem = permutation.inputLayoutElem;
			for (int32 in = 0; in < inputLayoutElem.size(); in++)
			{
				s3Log::debug("\ninputLayoutElem[%d].semantic_name: %s\n", in, inputLayoutElem[in].semantic_name.c_str());
				s3Log::debug("inputLayoutElem[%d].semantic_index: %d", in, inputLayoutElem[in].semantic_index);
			}
		}
	}

	s3Log::debug("=============================== s3Shader::print() end ===============================\n");
}
