#include <graphics/s3Shader.h>
#include <graphics/s3Renderer.h>
#include <fake_unity_shader/shader.h>
#include <fake_unity_shader/shader_parser.h>
#include <core/log/s3Log.h>

static bool fakeUnityShaderInitialized = false;

s3Shader::s3Shader():
	isLoaded(false),
	shaderName("")
{}

s3Shader::~s3Shader()
{}

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
		isLoaded   = true;
		shaderName = name;
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

s3Shader* s3Shader::GetShader() const
{
	return nullptr;
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
	return false;
}

bool s3Shader::SetFloat(std::string name, float32 value)
{
	return false;
}

bool s3Shader::SetMatrix(std::string name, t3Matrix4x4 value)
{
	return false;
}

bool s3Shader::SetVector(std::string name, t3Vector3f value)
{
	return false;
}

void s3Shader::print() const
{
	if (!isLoaded) return;

	s3Log::debug("=============================== s3Shader::print() begin ===============================\n");

	auto& shader = g_shaderMap[shaderName];
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

			// shader keywords
			auto& keyWordList = permutation.keyword_list;
			for (int32 keyword = 0; keyword < keyWordList.size(); keyword++)
			{
				s3Log::debug("keyword[%d]: %s\n", keyword, keyWordList[keyword].c_str());
			}

			// constant buffer
			auto& cbList = permutation.cb_list;
			for (int32 cb = 0; cb < cbList.size(); cb++)
			{
				s3Log::debug("cb[%d]: %s\n", cb, cbList[cb].c_str());
			}

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
				for (int x = 0; x < ST_Num; x++)
					s3Log::debug("texture.slot: %d\n", textureInfo.slot[x]);

				for (int y = 0; y < ST_Num; y++)
					s3Log::debug("texture.bindStage: %d\n", textureInfo.bindStage[y]);

				texIter++;
			}

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

				for (int x = 0; x < ST_Num; x++)
					s3Log::debug("samplerInfo.slot: %d\n", samplerInfo.slot[x]);

				for (int y = 0; y < ST_Num; y++)
					s3Log::debug("samplerInfo.bindStage: %d\n", samplerInfo.bindStage[y]);

				samplerIter++;
			}

			// input layout
			auto& inputLayoutElem = permutation.inputLayoutElem;
			for (int32 in = 0; in < inputLayoutElem.size(); in++)
			{
				s3Log::debug("inputLayoutElem[%d].semantic_name: %s\n", in, inputLayoutElem[in].semantic_name.c_str());
				s3Log::debug("inputLayoutElem[%d].semantic_index: %d\n", in, inputLayoutElem[in].semantic_index);
			}
		}
	}

	s3Log::debug("=============================== s3Shader::print() end ===============================\n");
}
