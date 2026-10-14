#include "Framework.h"
#include "Action.h"

Action::Action(string path, string file, bool isLoop, float speed)
{
	LoadClip(path, file, isLoop, speed);
}

Action::~Action()
{
	delete clip;
}

void Action::Update()
{
	clip->Update();
}

void Action::Render()
{
	clip->Render();
}

void Action::Start()
{
	clip->Play();
}

void Action::End()
{
	// 클립은 끝났고, 그 뒤에 할 일이 있으면 진행
	// -> 액션마다 공통 이벤트가 있다면 그걸 여기 작성 (지금은 공백)
}

void Action::LoadClip(string path, string file, bool isLoop, float speed)
{
	tinyxml2::XMLDocument* document = // XML 문서 유틸리티에서 문서 데이터 준비
		new tinyxml2::XMLDocument();

	document->LoadFile((path + file).c_str()); // 문서 로딩 함수

	tinyxml2::XMLElement* atlas = document->FirstChildElement(); // 전체 그림(아틀라스) 지칭 읽기
	string textureFile = path + atlas->Attribute("imagePath"); // 그림 파일을 스프라이트의 폴더인 것처럼 간주

	vector<Frame*> frames;
	tinyxml2::XMLElement* sprite = atlas->FirstChildElement(); // 각 스프라이트의 지칭 읽기

	while (sprite != nullptr) // 이 스프라이트가 끝까지 가기 전에 각 항목 반복
	{
		float x, y, w, h; // 픽셀 좌표와 크기

		x = sprite->FloatAttribute("x");
		y = sprite->FloatAttribute("y");
		w = sprite->FloatAttribute("w");
		h = sprite->FloatAttribute("h");

		frames.push_back(new Frame(ToWString(textureFile), x, y, w, h)); // 생성과 푸시

		sprite = sprite->NextSiblingElement(); // 다음 스프라이트, 그리고 그 항목 준비
	}

	// 반복문이 끝나면 쪼개진 그림들이 벡터를 이룰 것이다

	clip = new Clip(frames, isLoop, speed);

	delete document;
}