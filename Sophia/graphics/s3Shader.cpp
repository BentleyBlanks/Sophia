#include <graphics/s3Shader.h>
#include <graphics/s3Renderer.h>
#include <fake_unity_shader/shader.h>
#include <fake_unity_shader/shader_parser.h>
#include <core/log/s3Log.h>

static bool fakeUnityShaderInitialized = false;

class s3ShaderParser
{
public:
	s3ShaderParser():
		shaderName(""),
		techList(nullptr)
	{}

	// fake unity shader
	shader* shader;
	std::vector<shader_pass>* techList;
	std::string shaderName;
};

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
	//--! the init could be moved into renderer
	if (!fakeUnityShaderInitialized)
	{
		shader_init(s3Renderer::get().getDevice(), "../Sophia/thirdparty/fakeUnityShader/fake_unity_shader/shader_parser/");
	}

	const char* name = shader_load(fileName.c_str());
	if (!name)
	{
		s3Log::warning("s3Shader::s3Shader() create shader %s failed\n", fileName.c_str());
		return false;
	}
	else
	{
		S3_SAFE_DELETE(shaderParser);
		shaderParser = new s3ShaderParser();
		shaderParser->shader     = &g_shaderMap[name];
		shaderParser->techList   = &g_shaderMap[name].tech_list;
		shaderParser->shaderName = name;
		isLoaded = true;

		s3Log::success("s3Shader::s3Shader() create shader %s successfully\n", fileName.c_str());
		return true;
	}
}

float32 s3Shader::GetFloat(std::string name) const
{
	return float32();
}

int32 s3Shader::GetInt(std::string name) const
{
	return int32();
}

t3Matrix4x4 s3Shader::GetMatrix(std::string name) const
{
	return t3Matrix4x4();
}

t3Vector4f s3Shader::GetVector(std::string name) const
{
	return t3Vector4f();
}

bool s3Shader::IsKeywordEnabled(std::string name) const
{
	return false;
}

bool s3Shader::IsLoaded() const
{
	return isLoaded;
}

bool s3Shader::SetKeyword(std::string name, bool enableKeyword)
{
	return false;
}

bool s3Shader::SetInt(std::string name, int32 value)
{
	auto existed   = false;
	auto& techList = *shaderParser->techList;
	for (int32 techIndex = 0; techIndex < techList.size(); techIndex++)
	{
		auto& passList = techList[techIndex].pass_list;
		for (int32 i = 0; i < passList.size(); i++)
		{
			// loop whole cb info list
			auto& pass    = passList[i];
			auto passIter = pass.cb_list.begin();
			while (passIter != pass.cb_list.end())
			{
				//--! same variable name may existed in multiple cb
				auto& cbInfo     = passIter->second;
				auto& cbInfoList = cbInfo.items;
				auto& cbIter     = cbInfoList.find(name);
				if (cbIter != cbInfoList.end())
				{
					int* valuePtr = (int*)((char*)(cbInfo.data) + cbIter->second.offset);
					*valuePtr = value;

					existed = true;
				}
			}
		}
	}

	if (!existed)
	{
		s3Log::warning("s3Shader::SetInt() variable %s doesn't existed", name.c_str());
		return false;
	}
	return true;
}

bool s3Shader::SetFloat(std::string name, float32 value)
{
	auto existed = false;
	auto& techList = *shaderParser->techList;
	for (int32 techIndex = 0; techIndex < techList.size(); techIndex++)
	{
		auto& passList = techList[techIndex].pass_list;
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
					float* valuePtr = (float*)((char*)(cbInfo.data) + cbIter->second.offset);
					*valuePtr = value;

					existed = true;
				}
			}
		}
	}

	if (!existed)
	{
		s3Log::warning("s3Shader::SetFloat() variable %s doesn't existed", name.c_str());
		return false;
	}
	return true;
}

bool s3Shader::SetMatrix(std::string name, t3Matrix4x4 value)
{
	auto existed = false;
	auto& techList = *shaderParser->techList;
	for (int32 techIndex = 0; techIndex < techList.size(); techIndex++)
	{
		auto& passList = techList[techIndex].pass_list;
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
					t3Matrix4x4* valuePtr = (t3Matrix4x4*)((char*)(cbInfo.data) + cbIter->second.offset);
					t3Matrix4x4 temp = t3Matrix4x4::getTransposedOf(value);
					*valuePtr = temp;

					existed = true;
				}
			}
		}
	}

	if (!existed)
	{
		s3Log::warning("s3Shader::SetFloat() variable %s doesn't existed", name.c_str());
		return false;
	}
	return true;
}

bool s3Shader::SetVector(std::string name, t3Vector4f value)
{
	auto existed = false;
	auto& techList = *shaderParser->techList;
	for (int32 techIndex = 0; techIndex < techList.size(); techIndex++)
	{
		auto& passList = techList[techIndex].pass_list;
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
					t3Vector4f* valuePtr = (t3Vector4f*)((char*)(cbInfo.data) + cbIter->second.offset);
					*valuePtr = value;

					existed = true;
				}
			}
		}
	}

	if (!existed)
	{
		s3Log::warning("s3Shader::SetFloat() variable %s doesn't existed", name.c_str());
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
