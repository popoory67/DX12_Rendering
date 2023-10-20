#pragma once
#include "WindowMain.h"
#include "D3DUtil.h"
#if defined(DEBUG) | defined(_DEBUG)
#include <vld.h>
#include <iostream>
#endif

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// Enter your path where WinPixGpuCapturer.dll exist
    //HMODULE hModule = LoadLibrary(TEXT("path"));
    //if (hModule == NULL)
    //{
    //    return EXIT_FAILURE;
    //}
#endif

	try
	{
		WindowMain app(hInstance);

		if (app.Initialize())
		{
			return app.Run();
		}

		return 0;
	}
	catch (DxException& e)
	{
		MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
		return 0;
	}

	return 0;
}
