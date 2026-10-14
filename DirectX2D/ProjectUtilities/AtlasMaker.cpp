#include "Framework.h"
#include "AtlasMaker.h"
#include <algorithm>

AtlasMaker::AtlasMaker()
{
}

AtlasMaker::~AtlasMaker()
{
	ReleaseAtlasTexture();
}

void AtlasMaker::CreateAtlas(const wstring& filePath)
{
	WIN32_FIND_DATA findData;
	HANDLE handle = FindFirstFile((filePath + L"\\*.png").c_str(), &findData);
	if (handle == INVALID_HANDLE_VALUE)	return;

	vector<wstring> fileNames;
	do
	{
		fileNames.push_back(findData.cFileName);
	} while (FindNextFile(handle, &findData));
	FindClose(handle);

	if (fileNames.empty()) return;

	sort(fileNames.begin(), fileNames.end(),
		[](const wstring& a, const wstring& b)
		{
			auto strNum = [](const wstring& fileName) -> int
				{
					wstring str = fileName;
					size_t dot = str.find_last_of(L'.');
					size_t ub = str.find_last_of(L'_');
					if (dot != wstring::npos && ub != wstring::npos && ub + 1 < dot)
						str = fileName.substr(ub + 1, dot - ub - 1);

					if (str.empty() || !iswdigit(str[0]))
						return -1;   

					return stoi(str);
				};

			int numA = strNum(a);
			int numB = strNum(b);

			if (numA != -1 && numB != -1)
				return numA < numB;      // 둘 다 숫자면 숫자 비교

			return a < b;
		}
	);

	wstring key = fileNames[0];
	size_t ub = key.find_last_of(L'_');
	if (ub != wstring::npos) key = key.substr(0, ub);

	ReleaseAtlasTexture();

	imageCount = 0;
	imageUVs.clear();

	// GPU 텍스처끼리 복사(CopySubresourceRegion)하면 포맷을 서로 맞춰야 하는 문제가 있어서,
	// 대신 CPU 메모리(ScratchImage)에서 다 붙여넣은 뒤 마지막에 한 번만 GPU로 올린다.
	vector<ScratchImage> sourceImages;

	int maxWidth = 0;
	int maxHeight = 0;

	for(const wstring& name : fileNames)
	{
		ImageUV img;

		img.name = name;
		wstring fileName = filePath + L"\\" + name;

		ScratchImage source;
		LoadFromWICFile(fileName.c_str(), WIC_FLAGS_FORCE_RGB, nullptr, source);

		const TexMetadata& meta = source.GetMetadata();
		img.w = (float)meta.width;
		img.h = (float)meta.height;

		if (maxWidth < (int)meta.width) maxWidth = (int)meta.width;
		if (maxHeight < (int)meta.height) maxHeight = (int)meta.height;

		imageUVs.push_back(img);
		sourceImages.push_back(move(source));
		imageCount++;
	}

	if (imageCount <= 0) return;

	Vector2 atlasIndex = SetAtlasIndex(imageCount);
	atlasWidth = (int)atlasIndex.x * maxWidth;
	atlasHeight = (int)atlasIndex.y * maxHeight;

	ScratchImage atlasImage;
	DXGI_FORMAT atlasFormat = sourceImages[0].GetMetadata().format;
	HRESULT hr = atlasImage.Initialize2D(atlasFormat, atlasWidth, atlasHeight, 1, 1);
	if (FAILED(hr)) return;

	// 빈 영역은 투명하게 남도록 먼저 0으로 채운다.
	ZeroMemory(atlasImage.GetPixels(), atlasImage.GetPixelsSize());

	for (int i = 0; i < imageCount; ++i)
	{
		int x = (i % (int)atlasIndex.x) * maxWidth;
		int y = (i / (int)atlasIndex.x) * maxHeight;

		imageUVs[i].posX = x;
		imageUVs[i].posY = y;
		imageUVs[i].x = (float)x / atlasWidth;
		imageUVs[i].y = (float)y / atlasHeight;

		CopyRectangle(
			*sourceImages[i].GetImages(),
			Rect(0, 0, (size_t)imageUVs[i].w, (size_t)imageUVs[i].h),
			*atlasImage.GetImages(),
			TEX_FILTER_DEFAULT, x, y);
	}

	ID3D11ShaderResourceView* srv = nullptr;
	hr = CreateShaderResourceView(DEVICE, atlasImage.GetImages(), atlasImage.GetImageCount(),
		atlasImage.GetMetadata(), &srv);
	if (FAILED(hr)) return;

	srv->GetResource(&atlasTex);

	curTex = Texture::Add(key, srv);
}

void AtlasMaker::CreateEffectAtlas(const wstring& filePath)
{
	WIN32_FIND_DATA findData;
	HANDLE handle = FindFirstFile((filePath + L"\\*.png").c_str(), &findData);
	if (handle == INVALID_HANDLE_VALUE)	return;

	vector<wstring> fileNames;
	do
	{
		fileNames.push_back(findData.cFileName);
	} while (FindNextFile(handle, &findData));
	FindClose(handle);

	if (fileNames.empty()) return;

	sort(fileNames.begin(), fileNames.end(),
		[](const wstring& a, const wstring& b)
		{
			auto strNum = [](const wstring& fileName) -> int
				{
					wstring str = fileName;
					size_t dot = str.find_last_of(L'.');
					size_t ub = str.find_last_of(L'_');
					if (dot != wstring::npos && ub != wstring::npos && ub + 1 < dot)
						str = fileName.substr(ub + 1, dot - ub - 1);

					if (str.empty() || !iswdigit(str[0]))
						return -1;

					return stoi(str);
				};

			int numA = strNum(a);
			int numB = strNum(b);

			if (numA != -1 && numB != -1)
				return numA < numB;      // 둘 다 숫자면 숫자 비교

			return a < b;
		}
	);

	wstring key = fileNames[0];
	size_t ub = key.find_last_of(L'_');
	if (ub != wstring::npos) key = key.substr(0, ub);

	ReleaseAtlasTexture();

	imageCount = 0;
	imageUVs.clear();

	vector<ScratchImage> sourceImages;

	int maxWidth = 0;
	int maxHeight = 0;

	for (const wstring& name : fileNames)
	{
		ImageUV img;

		img.name = name;
		wstring fileName = filePath + L"\\" + name;

		ScratchImage source;
		LoadFromWICFile(fileName.c_str(), WIC_FLAGS_FORCE_RGB, nullptr, source);

		const TexMetadata& meta = source.GetMetadata();
		img.w = (float)meta.width;
		img.h = (float)meta.height;

		if (maxWidth < (int)meta.width) maxWidth = (int)meta.width;
		if (maxHeight < (int)meta.height) maxHeight = (int)meta.height;

		imageUVs.push_back(img);
		sourceImages.push_back(move(source));
		imageCount++;
	}

	if (imageCount <= 0) return;

	// Effect는 텍스처 전체를 maxFrame 격자로 균등 재생하므로,
	// 빈 칸이 생기지 않도록 정사각형이 아닌 한 줄(가로 스트립)로 배치한다
	atlasWidth = imageCount * maxWidth;
	atlasHeight = maxHeight;

	ScratchImage atlasImage;
	DXGI_FORMAT atlasFormat = sourceImages[0].GetMetadata().format;
	HRESULT hr = atlasImage.Initialize2D(atlasFormat, atlasWidth, atlasHeight, 1, 1);
	if (FAILED(hr)) return;

	// 빈 영역은 투명하게 남도록 먼저 0으로 채운다.
	ZeroMemory(atlasImage.GetPixels(), atlasImage.GetPixelsSize());

	for (int i = 0; i < imageCount; ++i)
	{
		int cellX = i * maxWidth;

		// 프레임마다 원본 크기가 달라도 셀 중앙에 오도록 정렬한다
		// (Effect는 프레임별 크기 정보 없이 격자를 그대로 잘라서 재생하기 때문)
		int x = cellX + (maxWidth - (int)imageUVs[i].w) / 2;
		int y = (maxHeight - (int)imageUVs[i].h) / 2;

		imageUVs[i].posX = x;
		imageUVs[i].posY = y;
		imageUVs[i].x = (float)x / atlasWidth;
		imageUVs[i].y = (float)y / atlasHeight;

		CopyRectangle(
			*sourceImages[i].GetImages(),
			Rect(0, 0, (size_t)imageUVs[i].w, (size_t)imageUVs[i].h),
			*atlasImage.GetImages(),
			TEX_FILTER_DEFAULT, x, y);
	}

	ID3D11ShaderResourceView* srv = nullptr;
	hr = CreateShaderResourceView(DEVICE, atlasImage.GetImages(), atlasImage.GetImageCount(),
		atlasImage.GetMetadata(), &srv);
	if (FAILED(hr)) return;

	srv->GetResource(&atlasTex);

	curTex = Texture::Add(key, srv);
}

Vector2 AtlasMaker::SetAtlasIndex(int imageCount)
{
	Vector2 size;

	int root = (int)(sqrt(imageCount));

	if (root * root >= imageCount)
	{
		size.x = root;
		size.y = root;
	}
	else if ((root + 1) * root >= imageCount)
	{
		size.x = root + 1;
		size.y = root;
	}
	else
	{
		size.x = root + 1;
		size.y = root + 1;
	}

	return size;
}

void AtlasMaker::SaveUVData(const wstring& filePath)
{
	BinaryWriter* writer = new BinaryWriter(filePath);

	writer->UInt(imageCount);
	writer->Int(atlasWidth);
	writer->Int(atlasHeight);

	for (const auto& img : imageUVs)
	{
		writer->WString(img.name);
		writer->UInt(img.posX);
		writer->UInt(img.posY);
		writer->Float(img.x);
		writer->Float(img.y);
		writer->Float(img.w);
		writer->Float(img.h);
	}

	delete writer;
}

void AtlasMaker::LoadUVData(const wstring& filePath)
{
	BinaryReader* reader = new BinaryReader(filePath);
	imageCount = reader->UInt();
	atlasWidth = reader->Int();
	atlasHeight = reader->Int();

	imageUVs.clear();
	for (int i = 0; i < imageCount; ++i)
	{
		ImageUV img;
		img.name = reader->WString();
		img.posX = reader->UInt();
		img.posY = reader->UInt();
		img.x = reader->Float();
		img.y = reader->Float();
		img.w = reader->Float();
		img.h = reader->Float();
		imageUVs.push_back(img);
	}
	delete reader;
}

void AtlasMaker::ReleaseAtlasTexture()
{
	if (atlasTex) { atlasTex->Release(); atlasTex = nullptr; }
}


void AtlasMaker::SaveAtlasToFile(const wstring& filePath)
{
	if (!atlasTex) return;

	ScratchImage image;
	HRESULT hr = CaptureTexture(DEVICE, DC, atlasTex, image);
	if (FAILED(hr))
		return;

	hr = SaveToWICFile(
		*image.GetImages(),
		WIC_FLAGS_NONE,
		GetWICCodec(WIC_CODEC_PNG),
		filePath.c_str()
	);
	if (FAILED(hr)) return;

	wstring uvPath = filePath;
	size_t dot = uvPath.find_last_of(L'.');
	if (dot != wstring::npos) uvPath = uvPath.substr(0, dot);
	uvPath += L".uv";

	SaveUVData(uvPath);
}

