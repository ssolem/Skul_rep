#include "Framework.h"
#include "SceneManager.h"

//비동기 씬, 혹은 자동 씬 넘어가기 (둘 중 하나) : 후자는 한 씬에서 시간이 일정 이상 지나면 다른 씬으로 넘어가는 걸 말합니다.

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
	for (Scene* scene : curScenes)
		scene->End();

	for (pair<string, Scene*> scene : scenes)
	{
		delete scene.second;
	}
}

void SceneManager::Update()
{
	//현재 씬은 업데이트
	for (Scene* scene : curScenes)
		scene->Update();

	//예약 목록이 남아있으면 업데이트에서 처리
	if (addScenes.size() > 0)
		AddScenes();

	if (removeScenes.size() > 0)
		RemoveScenes();
}

void SceneManager::PreRender()
{
	for (Scene* scene : curScenes)
		scene->PreRender();
}

void SceneManager::Render()
{
	for (Scene* scene : curScenes)
		scene->Render();
}

void SceneManager::PostRender()
{
	for (Scene* scene : curScenes)
		scene->PostRender();
}

void SceneManager::Create(string key, Scene* scene)
{
	//맵목록에 씬 등재
	if (scenes.count(key) > 0) return;
	scenes[key] = scene;
}

Scene* SceneManager::Add(string key)
{
	//씬을 재생 예약에 추가
	if (scenes.count(key) < 1) return nullptr;

	addScenes.push_back(key); //예약 목록에 key(string) 추가
								// 리스트 검색과 씬 실제 추가는 AddScenes();

	return scenes[key];
}

void SceneManager::Remove(string key)
{	
	if (scenes.count(key) < 1) return ;

	removeScenes.push_back(key);
}

void SceneManager::Reset(string key)
{
	if (scenes.count(key) < 1) return;

	scenes[key]->Reset();
}

void SceneManager::AddScenes()
{	
	//예약 목록으로부터 씬 시작 일괄 처리
	for (string SceneName : addScenes)
	{
		//등재된 씬이 있는지 확인
		list<Scene*>::iterator findScene = find(curScenes.begin(), curScenes.end(), scenes[SceneName]);

		if (findScene != curScenes.end()) continue; // 씬이 있는 경우

		curScenes.push_back(scenes[SceneName]);
		//scenes[SceneName]->Start();
		curScenes.back()->Start();
	}

	addScenes.clear();
}

void SceneManager::RemoveScenes()
{
	//예약 목록으로부터 씬 시작 일괄 처리
	for (string SceneName : removeScenes)
	{
		//등재된 씬이 있는지 확인
		list<Scene*>::iterator findScene = find(curScenes.begin(), curScenes.end(), scenes[SceneName]);

		if (findScene == curScenes.end()) continue; //현재 씬에 삭제하려는 씬이 없는경우

		//(*findScene)->End();
		scenes[SceneName]->End();
		curScenes.erase(findScene);
	}

	removeScenes.clear();
}
