#pragma once
class SceneManager : public Singleton<SceneManager>
{
public:
	SceneManager();
	~SceneManager();

	void Update();

	void PreRender();
	void Render();
	void PostRender();

	void Create(string key, Scene* scene); //씬 만들기

	Scene* Add(string key); // 예약 목록에 추가
	void Remove(string key); // 예약 목록에서 제거

	void Reset(string key); //최초 재생(시작 혹은 리셋)

private:
	void AddScenes(); // 등재 목록에 올라간 씬을 현재 목록으로 옮기기
	void RemoveScenes(); // 등재 목록에 올라간 씬을 현재 목록에서 내리기


private:

	map<string, Scene*> scenes; //씬의 등재 목록
	list<Scene*> curScenes; //현재 대상 씬들

	vector<string> addScenes, removeScenes; //복수 씬 추가, 삭제 대응
};

