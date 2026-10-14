#pragma once

class AtlasMaker
{
public:
	struct ImageUV
	{
		wstring name;              // 원본 파일명 - 키
		int posX = 0, posY = 0;    // 아틀라스 내 픽셀 좌표
		float x = 0, y = 0;        // 아틀라스 내 UV 좌표 (0~1)
		float w = 0, h = 0;
	};

public:
	AtlasMaker();
	~AtlasMaker();

	void CreateAtlas(const wstring& filePath);
	void CreateEffectAtlas(const wstring& filePath); // Effect용 : 프레임을 빈칸 없이 한 줄로 나열, 셀 중앙 정렬
	void SaveAtlasToFile(const wstring& filePath);

	int GetAtlasWidth() const { return atlasWidth; }
	int GetAtlasHeight() const { return atlasHeight; }

	Vector2 GetMaxFrame() const { return Vector2((float)imageCount, 1); } // CreateEffectAtlas 결과를 Effect 생성자에 넘길 때 사용

	void SaveUVData(const wstring& filePath);
	void LoadUVData(const wstring& filePath);

	const vector<ImageUV>& GetImageUVs() { return imageUVs; }
	int GetImageCount() { return imageCount; }
	Texture* GetTexture() { return curTex; }

private:
	Vector2 SetAtlasIndex(int imageCount);
	void ReleaseAtlasTexture();
private:
	vector<ImageUV> imageUVs;
	int imageCount = 0;
	int atlasWidth = 0;
	int atlasHeight = 0;

	ID3D11Resource* atlasTex = nullptr;
	Texture* curTex = nullptr;
};

