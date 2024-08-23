// © Rollingmedia 2016 all rights reserved

#pragma once

#include "Modules/ModuleManager.h"
#include"PolyMesh.h"

class UESHAREDPLUGIN_API FGeometrySystem
{
public:


	static  FPolymesh GetIcosahedronMuiltFour(bool dual ,int level=0);
	static  FPolymesh GetIcosahedron(bool dual,int m=1, int n = 0 );

	//static inline bool IsAvailable() { return FModuleManager::Get().IsModuleLoaded("SDLInputDevicePlugin"); }
	//
	//virtual TArray<TWeakPtr<class IInputDevice>> GetCreatedDevices() = 0;
	//virtual void SetPrimaryController(int32 primaryControllerId) = 0;
};