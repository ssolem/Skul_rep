#pragma once

class Texture
{
private:
    Texture(ID3D11ShaderResourceView* srv, ScratchImage& image, wstring file);
    ~Texture();

public:
    void PSSet(UINT slot = 0);

    ID3D11ShaderResourceView* GetSRV() { return srv; }

    Vector2 GetSize()
    {
        return Vector2(image.GetMetadata().width, image.GetMetadata().height);
    }

    wstring GetFile() { return file; }
    ID3D11Resource* GetResource()
    {
        ID3D11Resource* resource = nullptr;
        srv->GetResource(&resource);
        return resource;
    }

public:
    static Texture* Add(wstring file);
    static Texture* Add(wstring key, ID3D11ShaderResourceView* srv);

    static void Delete();

private:
    wstring file;

    ScratchImage image;
    ID3D11ShaderResourceView* srv;

    static unordered_map<wstring, Texture*> textures;
};