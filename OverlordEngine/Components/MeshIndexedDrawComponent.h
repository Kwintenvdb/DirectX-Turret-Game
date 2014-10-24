#pragma once
#include <vector>
#include "BaseComponent.h"
#include "../Helpers/VertexHelper.h"



class MeshIndexedDrawComponent : public BaseComponent
{
public:
	MeshIndexedDrawComponent(UINT vertexCapacity = 50, UINT indexCapacity = 50);
	virtual ~MeshIndexedDrawComponent(void);

	void AddVertex(VertexPosNormCol vertex, bool updateBuffer = false);
	void AddIndex(UINT index, bool updateBuffer = false);

	void ClearVertexList();
	void ClearIndexList();

	void UpdateVertexBuffer();
	void UpdateIndexBuffer();

protected:
	
	virtual void Update(const GameContext& context);
	virtual void Draw(const GameContext& context);
	virtual void Initialize(const GameContext& gameContext);

private:

	
	void LoadEffect(const GameContext& gameContext);
	void InitializeVertexBuffer(const GameContext& gameContext);
	void InitializeIndexBuffer(const GameContext& gameContext);

	vector<VertexPosNormCol> m_vecVertices;
	vector<UINT> m_vecIndices;

	ID3D11Buffer *m_pVertexBuffer, *m_pIndexBuffer;
	UINT m_VertexCapacity, m_IndexCapacity;

	ID3DX11Effect* m_pEffect;
	ID3DX11EffectTechnique* m_pTechnique;
	ID3D11InputLayout* m_pInputLayout;
	static ID3DX11EffectMatrixVariable *m_pWorldVar,*m_pWvpVar;
	
private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	MeshIndexedDrawComponent(const MeshIndexedDrawComponent& yRef);									
	MeshIndexedDrawComponent& operator=(const MeshIndexedDrawComponent& yRef);

};

