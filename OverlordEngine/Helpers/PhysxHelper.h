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

inline PxVec3 ToPxVec3(XMFLOAT3 vec)
{
	return PxVec3(vec.x, vec.y, vec.z);
}

inline PxExtendedVec3 ToPxExtendedVec3(XMFLOAT3 vec)
{
	return PxExtendedVec3(vec.x, vec.y, vec.z);
}

inline PxVec2 ToPxVec2(XMFLOAT2 vec)
{
	return PxVec2(vec.x, vec.y);
}

inline PxVec4 ToPxVec4(XMFLOAT4 vec)
{
	return PxVec4(vec.x, vec.y, vec.z, vec.w);
}

inline PxQuat ToPxQuat(XMFLOAT4 vec)
{
	return PxQuat(vec.x, vec.y, vec.z, vec.w);
}
inline XMFLOAT3 ToXMFLOAT3(PxVec3 vec)
{
	return XMFLOAT3(vec.x, vec.y, vec.z);
}

inline XMFLOAT3 ToXMFLOAT3(PxExtendedVec3 vec)
{
	return XMFLOAT3(static_cast<float>(vec.x), static_cast<float>(vec.y), static_cast<float>(vec.z));
}
