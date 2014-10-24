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
//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "PhysxErrorCallback.h"
#include <string>
#include <sstream>
#include "../Diagnostics/Logger.h"
using namespace std;

PhysxErrorCallback::PhysxErrorCallback(void){}
PhysxErrorCallback::~PhysxErrorCallback(void){}

void PhysxErrorCallback::reportError(PxErrorCode::Enum code , const char* message, const char* file, int line)
{
	wstringstream ss;
	ss<<L"PHYSX CALLBACK REPORT:"<<endl;
	ss<<"Error Code: "<<code<<endl;
	ss<<"Message: "<<message<<endl;
	ss<<"File: "<<file<<" (line: "<<line<<")";

	switch(code)
	{
	case PxErrorCode::eDEBUG_INFO:
		Logger::LogInfo(ss.str());
		break;
	case PxErrorCode::ePERF_WARNING:
	case PxErrorCode::eDEBUG_WARNING:
			Logger::LogWarning(ss.str());
			break;
	default:
		Logger::LogError(ss.str());
		break;
	}
}
