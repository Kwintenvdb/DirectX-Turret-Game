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
#include "../Base/stdafx.h"

PxFilterFlags OverlordSimulationFilterShader(
	PxFilterObjectAttributes attribute0, PxFilterData filterData0,
	PxFilterObjectAttributes attribute1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	UNREFERENCED_PARAMETER(constantBlock);
	UNREFERENCED_PARAMETER(constantBlockSize);

	if ((filterData0.word0&filterData1.word1) == filterData0.word0 || (filterData1.word0&filterData0.word1) == filterData1.word0)
	{
		return PxFilterFlag::eSUPPRESS;
	}

	if ((attribute0 & PxFilterObjectFlag::eTRIGGER) != 0 || (attribute1 & PxFilterObjectFlag::eTRIGGER) != 0)
	{
		//pairFlags |= PxPairFlag::eNOTIFY_TOUCH_LOST;
		pairFlags |= PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}

	if((filterData0.word0 & filterData1.word0) != 0)
	{
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
		//pairFlags |= PxPairFlag::eNOTIFY_TOUCH_LOST;
	}

	pairFlags |= PxPairFlag::eCONTACT_DEFAULT;

	return PxFilterFlag::eDEFAULT;
}
