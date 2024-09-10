#pragma once

#include "resource.h"
#include <commdlg.h>
#include <shellapi.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <map>
//DLL Injector Imports
#include <tlhelp32.h>
#include <shlwapi.h>
#pragma comment( lib, "shlwapi.lib")

#ifdef UNICODE
using string = std::wstring;
#define tstrcpy_s wcscpy_s
#define print(format, ...) std::wprintf(format, __VA_ARGS__)
#else
using string = std::string;
#define tstrcpy_s strcpy_s
#define print(format, ...) std::fprintf(stdout, format, __VA_ARGS__)
#endif
//If not Build in DEBUG-Mode, than remove ignore all print's for performance hihi
#ifndef DEBUG
#define print(format, ...) ;
#endif

/**
* For now only stores a sub-part of the PROCESSENTRY32 Info
*/
struct Process {
	UINT pid;
	string pName;
};

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void                CreateMainLayout(HWND hWnd);
void                OpenFileSelector(HWND hWnd);
std::map<DWORD, Process> ListProcesses(std::map<DWORD, Process>& pList);
void				UpdateProcessBox(std::map<DWORD, Process>& pList);
DWORD				GetProcIDByDropBoxText(string text);
void				InjectButtonHandler(HWND hWnd);
BOOL				InjectDLLWithPath(DWORD procID, const TCHAR* dllPath);
DWORD				GetProcessIDByName(string processName);

#define MAX_LOADSTRING 256
#define DEFAULT_MARGIN 10
#define DEFAULT_SIZE_X 400
#define DEFAULT_SIZE_Y 450


/**
* JackysDefined MenuIDs:
*/
#define IDM_BUTTON_SEARCHDLL					32773
#define IDM_COMBOBOX							32774
#define IDM_INJECT_BUTTON						32775
