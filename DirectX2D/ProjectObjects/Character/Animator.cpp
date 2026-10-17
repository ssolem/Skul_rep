#include "Framework.h"
#include "Animator.h"
#include "ProjectUtilities/AtlasMaker.h"

// 아틀라스 경로별 프레임 공유 캐시 - 같은 클립을 쓰는 객체가 여러 개여도 .uv 파싱/버퍼 생성은 한 번만
map<wstring, vector<Frame*>> Animator::frameCache;

Animator::Animator():
	clips(), curClip(nullptr)
{
}

Animator::~Animator()
{
	for (pair<string, Clip*> clip : clips)
		delete clip.second;
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

	vector<Frame*>* frames = GetAtlasFrames(path + L"\\" + ToWString(fileName) + L".png");
	if (frames == nullptr) return;

	//Clip은 프레임을 빌려 쓰기만 함 (ownsFrames = false) - 재생 상태/이벤트는 Clip마다 따로
	clips[key] = new Clip(*frames, isLoop, speed, false);
}

vector<Frame*>* Animator::GetAtlasFrames(const wstring& atlasPath, bool isCenterPivot)
{
	//같은 아틀라스라도 피벗이 다르면 정점이 다르므로 캐시 키를 나눔
	wstring cacheKey = atlasPath + (isCenterPivot ? L"|center" : L"");

	auto cached = frameCache.find(cacheKey);
	if (cached != frameCache.end()) return &cached->second;

	//처음 쓰는 아틀라스일 때만 .uv 읽고 Frame 생성
	size_t dot = atlasPath.find_last_of(L'.');
	wstring uvPath = (dot == wstring::npos ? atlasPath : atlasPath.substr(0, dot)) + L".uv";

	if (GetFileAttributes(atlasPath.c_str()) == INVALID_FILE_ATTRIBUTES) return nullptr;
	if (GetFileAttributes(uvPath.c_str()) == INVALID_FILE_ATTRIBUTES) return nullptr;

	AtlasMaker atlas;
	atlas.LoadUVData(uvPath);

	float atlasW = (float)atlas.GetAtlasWidth();
	float atlasH = (float)atlas.GetAtlasHeight();

	vector<Frame*> frames = {};

	for (const AtlasMaker::ImageUV& img : atlas.GetImageUVs())
	{
		Vector2 startUV(img.x, img.y);
		Vector2 endUV(img.x + img.w / atlasW, img.y + img.h / atlasH);

		//캐릭터: 발밑 기준 (0, h/2) / 이펙트: 프레임 중앙 기준
		Vector2 pivot = isCenterPivot ? Vector2() : Vector2(0, img.h * 0.5f);

		frames.push_back(new Frame(atlasPath, startUV, endUV, pivot));
	}

	return &frameCache.emplace(cacheKey, frames).first->second;
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
//격자/한 줄 어떤 배치든 .uv의 프레임별 UV로 재생 (Make Anim Atlas로 구워도 됨)
void Animator::CreateEffectByAtlas(const string& key, const wstring& atlasFile, UINT poolSize, Vector2 scale,
	float speed, bool isAdditive, EffectPivot pivot)
{
	//전역 풀이라 같은 적이 여러 마리여도 한 번만 등록 - uv 읽기 전에 거름
	if (EffectManager::Get()->HasEffect(key)) return;

	vector<Frame*>* frames = GetAtlasFrames(atlasFile, pivot == EffectPivot::Center);
	if (frames == nullptr) return;

	EffectManager::Get()->Add(key, poolSize, *frames, speed, isAdditive);
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
