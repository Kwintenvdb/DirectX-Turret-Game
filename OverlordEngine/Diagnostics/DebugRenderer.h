//------------------------------------------------------------------------------------------------------
//   _____     _______ ____  _     ___  ____  ____    _____ _   _  ____ ___ _   _ _____   ______  ___ _ 
//  / _ \ \   / / ____|  _ \| |   / _ \|  _ \|  _ \  | ____| \ | |/ ___|_ _| \ | | ____| |  _ \ \/ / / |
// | | | \ \ / /|  _| | |_) | |  | | | | |_) | | | | |  _| |  \| | |  _ | ||  \| |  _|   | | | \  /| | |
// | |_| |\ V / | |___|  _ <| |__| |_| |  _ <| |_| | | |___| |\  | |_| || || |\  | |___  | |_| /  \| | |
//  \___/  \_/  |_____|_| \_\_____\___/|_| \_\____/  |_____|_| \_|\____|___|_| \_|_____| |____/_/\_\_|_|
//
// Overlord Engine v1.82
// Copyright Overlord Thomas Goussaert & Overlord Brecht Kets
// http://www.digitalartsandentertainment.com/
//------------------------------------------------------------------------------------------------------
#pragma once

struct VertexPosCol;
struct GameContext;

class DebugRenderer
{
public:

	static void InitRenderer(ID3D11Device *pDevice, UINT bufferSize = 100);
	static void ToggleDebugRenderer();
	static void Release();

	//RENDERING
	static void Draw(const GameContext& gameContext);
	static void DrawLine(XMFLOAT3 start, XMFLOAT3 end, XMFLOAT4 color = (XMFLOAT4)Colors::Red);
	static void DrawLine(XMFLOAT3 start, XMFLOAT4 colorStart, XMFLOAT3 end, XMFLOAT4 colorEnd);
	static void DrawPhysX(PxScene* pScene);


private:
	DebugRenderer(void);
	~DebugRenderer(void);

	//RENDERING
	static void CreateVertexBuffer(ID3D11Device *pDevice);
	static void CreateFixedLineList();
	static XMFLOAT4 ConvertPxColor(PxU32 color);
	static ID3DX11Effect* m_pEffect;
	static ID3DX11EffectTechnique* m_pTechnique;
	static UINT m_BufferSize, m_FixedBufferSize;
	static ID3D11InputLayout *m_pInputLayout;
	static ID3D11Buffer *m_pVertexBuffer;
	static ID3DX11EffectMatrixVariable *m_pWvpVariable;
	static vector<VertexPosCol> m_LineList;
	static vector<VertexPosCol> m_FixedLineList;
	static bool m_RendererEnabled;

	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	DebugRenderer(const DebugRenderer& t);
	DebugRenderer& operator=(const DebugRenderer& t);
};

