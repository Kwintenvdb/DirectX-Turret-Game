#pragma once
#include "../Helpers/Singleton.h"

class OverlordGame;
class SpriteFont;
class GameScene;
struct TextCache;

class TextRenderer : public Singleton<TextRenderer>
{
public:

#undef DrawText
	void DrawText(SpriteFont* pFont, const wstring& text, XMFLOAT2 position, XMFLOAT4 color = (XMFLOAT4) Colors::White);

private:

	struct TextVertex
	{
		UINT ChannelId;
		XMFLOAT3 Position;
		XMFLOAT4 Color;
		XMFLOAT2 TexCoord;
		XMFLOAT2 CharacterDimension;
	};

	friend class Singleton<TextRenderer>;
	friend class GameScene;
	friend class OverlordGame;

	TextRenderer();
	~TextRenderer();

	void InitRenderer(ID3D11Device* pDevice);
	void Draw(const GameContext& gameContext);
	void UpdateBuffer(const GameContext& gameContext);
	void CreateTextVertices(SpriteFont* pFont, const TextCache& info, TextVertex* pBuffer, int& bufferPosition);

	UINT m_BufferSize, m_InputLayoutSize, m_NumCharacters;
	XMFLOAT4X4 m_Transform;
	ID3DX11Effect* m_pEffect;
	ID3DX11EffectTechnique* m_pTechnique;
	ID3DX11EffectMatrixVariable* m_pTransfromMatrixV;
	ID3DX11EffectVectorVariable* m_pTextureSizeV;
	ID3DX11EffectShaderResourceVariable* m_pTextureSRV;

	ID3D11InputLayout* m_pInputLayout;
	ID3D11Buffer* m_pVertexBuffer;

	vector<SpriteFont*> m_SpriteFonts;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	TextRenderer(const TextRenderer& t);
	TextRenderer& operator=(const TextRenderer& t);
};
