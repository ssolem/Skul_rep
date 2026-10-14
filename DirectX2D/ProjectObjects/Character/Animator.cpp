#include "Framework.h"
#include "Animator.h"
#include "ProjectUtilities/AtlasMaker.h"

// 아틀라스 경로별 프레임 공유 캐시 - 같은 클립을 쓰는 객체가 여러 개여도 .uv 파싱/버퍼 생성은 한 번만
map<wstring, vector<Frame*>> Animator::frameCache;

Animator::Animator():
	clips(), curClip(nullptr)
{
	atlas = new AtlasMaker;
}

Animator::~Animator()
{
	for (pair<string, Clip*> clip : clips)
		delete clip.second;

	delete atlas;
}

void Animator::Update()
{
	if (curClip)
		curClip->Update();
}

void Animator::Render()
{
	if (curClip)
		curClip->Render();
}

void Animator::CreateClip(const string& key, const wstring& path, vector<Frame*>& frames, bool isLoop, float speed)
{
	if (clips.count(key) > 0)
	{
		for (Frame* frame : frames)
			delete frame;
		return;
	}

	Clip* newClip = new Clip(frames, isLoop, speed);
	clips[key] = newClip;
}

void Animator::CreateClipByFolder(const string& key, const wstring& path, bool isLoop, float speed)
{
	if (clips.count(key) > 0) return;

	WIN32_FIND_DATA findData;	
	HANDLE handle = FindFirstFile((path + L"\\*.png").c_str(), &findData);

	bool result = true;
	wstring fileName;

	vector<Frame*> frames = {};

	while (result)
	{
		fileName = path + L"\\" + findData.cFileName;

		result = FindNextFile(handle, &findData);

		UINT index = frames.size();

		Frame* frame = new Frame(fileName);
		frames.push_back(frame);
	}

	CreateClip(key, path, frames, isLoop, speed);
	FindClose(handle);
}

void Animator::CreateClipByAtlas(const string& tag, const string& fileName, const wstring& path, bool isLoop, float speed)
{
	// 등록 키는 tag+fileName으로 합쳐서 스컬별 충돌을 막고, 파일 탐색은 fileName 원본 그대로 사용
	string key = tag + fileName;
	if (clips.count(key) > 0) return;

	wstring base = path + L"\\" + ToWString(fileName);
	wstring atlasPath = base + L".png";
	wstring uvPath = base + L".uv";

	auto cached = frameCache.find(atlasPath);
	if (cached == frameCache.end())
	{
		//처음 쓰는 아틀라스일 때만 .uv 읽고 Frame 생성
		if (GetFileAttributes(atlasPath.c_str()) == INVALID_FILE_ATTRIBUTES) return;
		if (GetFileAttributes(uvPath.c_str()) == INVALID_FILE_ATTRIBUTES) return;

		atlas->LoadUVData(uvPath);
		vector<AtlasMaker::ImageUV> imageUV = atlas->GetImageUVs();
		int atlasW = atlas->GetAtlasWidth();
		int atlasH = atlas->GetAtlasHeight();

		vector<Frame*> frames = {};

		for (const auto& img : imageUV)
		{
			Vector2 startUV(img.x, img.y);
			Vector2 endUV(img.x + img.w / atlasW, img.y + img.h / atlasH);

			frames.push_back(new Frame(atlasPath, startUV, endUV, Vector2(0, img.h * 0.5f)));
		}

		cached = frameCache.emplace(atlasPath, frames).first;
	}

	//Clip은 프레임을 빌려 쓰기만 함 (ownsFrames = false) - 재생 상태/이벤트는 Clip마다 따로
	clips[key] = new Clip(cached->second, isLoop, speed, false);
}

void Animator::ClearFrameCache()
{
	for (auto& pair : frameCache)
		for (Frame* frame : pair.second)
			delete frame;

	frameCache.clear();
}

//key: EffectManager에 등록/재생할 이름 (파일 이름과 무관)
//atlasFile: 아틀라스 png 전체 경로 - uv는 같은 위치, 같은 이름의 .uv
void Animator::CreateEffectByAtlas(const string& key, const wstring& atlasFile, UINT poolSize, Vector2 scale,
	float speed, bool isAdditive)
{
	//전역 풀이라 같은 적이 여러 마리여도 한 번만 등록 - uv 읽기 전에 거름
	if (EffectManager::Get()->HasEffect(key)) return;

	wstring atlasPath = atlasFile;
	size_t dot = atlasPath.find_last_of(L'.');
	wstring uvPath = (dot == wstring::npos ? atlasPath : atlasPath.substr(0, dot)) + L".uv";

	if (GetFileAttributes(atlasPath.c_str()) == INVALID_FILE_ATTRIBUTES) return;
	if (GetFileAttributes(uvPath.c_str()) == INVALID_FILE_ATTRIBUTES) return;

	atlas->LoadUVData(uvPath);
	Vector2 maxFrame = atlas->GetMaxFrame();

	EffectManager::Get()->Add(key, poolSize, atlasPath, maxFrame.x, maxFrame.y, speed, isAdditive);
	EffectManager::Get()->SetScale(key, scale);
}

Clip* Animator::FindClip(const string& key)
{
	auto iter = clips.find(key);

	if (iter == clips.end()) return nullptr;

	return iter->second;
}

void Animator::PlayClip(const string& key)
{
	Clip* clip = FindClip(key);
	if (clip == nullptr) return;

	curClip = clip;
	curClip->Play();
}
