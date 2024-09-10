#include "framework.h"
#include "JackysApp.h"

// Global Variables:
HINSTANCE hInst;                                // Current Instance
HWND hTestButton, hTextField, hProcesses;   
TCHAR szTitle[MAX_LOADSTRING];                  // Title Text
TCHAR szWindowClass[MAX_LOADSTRING];            // ClassName of the Main window
TCHAR g_szFileName[MAX_LOADSTRING];
std::map<DWORD,Process> processList = {};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Add code (Template)
#ifdef DEBUG
    AllocConsole();
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
#endif
    // Load Ressource Strings by Template
    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_JACKYSAPP, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_JACKYSAPP));

    MSG msg;

    // Message Loop
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//
//  function: MyRegisterClass()
//
//  Purpose: Register Window-Class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_JACKYSAPP));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = CreateSolidBrush(RGB(160, 160, 160));
    wcex.lpszMenuName   = MAKEINTRESOURCE(IDC_JACKYSAPP);
    //wcex.lpszMenuName = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance;

   HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_MINIMIZEBOX,
      CW_USEDEFAULT, CW_USEDEFAULT, DEFAULT_SIZE_X, DEFAULT_SIZE_Y, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        CreateMainLayout(hWnd);
        DragAcceptFiles(hWnd, TRUE);
        ListProcesses(processList);
        UpdateProcessBox(processList);
        break;
    case WM_GETMINMAXINFO:
    {
        LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
        lpMMI->ptMinTrackSize.x = DEFAULT_SIZE_X;
        lpMMI->ptMinTrackSize.y = DEFAULT_SIZE_Y;
        lpMMI->ptMaxTrackSize.x = DEFAULT_SIZE_X;
        lpMMI->ptMaxTrackSize.y = DEFAULT_SIZE_Y;
        break;
    }
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            case IDM_BUTTON_SEARCHDLL:
                //print(_T("Button SearchDLL pressed\n"));
                OpenFileSelector(hWnd);
                SetWindowText(hTextField, g_szFileName);
                break;
            case IDM_COMBOBOX:
                //print(_T("COMBOBOX Command: %x"), HIWORD(wParam));
                if (HIWORD(wParam) == CBN_DROPDOWN) {
                    ListProcesses(processList);
                    UpdateProcessBox(processList);
                }
                //print(_T("SelectedIndex: %i"), (DWORD)SendMessage(hProcesses, CB_GETCURSEL, 0, 0));
                break;
            case IDM_INJECT_BUTTON:
                InjectButtonHandler(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_DROPFILES:
        print(_T("File Dropped :)\n"));
        HDROP droppedFile;
        droppedFile = reinterpret_cast<HDROP>(wParam);
        if (DragQueryFile(droppedFile, 0xFFFFFFFF, NULL, 0) != 1) {
            MessageBox(hWnd, _T("More than 1 File per Drag and Drop is not allowed!"),
                _T("Warning"), MB_ICONWARNING);
            return 1;
        }
        DragQueryFile(droppedFile, 0, g_szFileName, MAX_LOADSTRING);
        if (string(g_szFileName).find(_T(".dll")) == -1) {
            MessageBox(hWnd, _T("Only DLL-Files are allowed!"),
                _T("Warning"), MB_ICONWARNING);
            return 1;
        }
        SetWindowText(hTextField, g_szFileName);
        print(_T("File: %s\n"), g_szFileName);
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Pain things
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

//Aboutfield Callback
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void CreateMainLayout(HWND hWnd) {
    hTestButton = CreateWindow(
        _T("BUTTON"),  // Predefined class;
        _T("SearchFile"),      // Button text 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
        15,         // x position 
        60 + 20 + DEFAULT_MARGIN,         // y position 
        100,        // Button width
        20,        // Button height
        hWnd,     // Parent window
        (HMENU)IDM_BUTTON_SEARCHDLL,    // ID. 
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL);      // Pointer not needed.

    hTextField = CreateWindow(
        _T("Edit"),
        NULL,
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
        15,
        15 + 35 + DEFAULT_MARGIN,
        DEFAULT_SIZE_X - 50,
        20,
        hWnd,
        NULL,
        NULL,
        NULL);

    CreateWindow(
        _T("Static"),
        _T("Select DLL-File or drop a DLL File into this Application: "),
        WS_VISIBLE | WS_CHILD,
        15,
        15,
        DEFAULT_SIZE_X - 50,
        35,
        hWnd,
        NULL,
        NULL,
        NULL);

    CreateWindow(
        _T("Static"),
        _T("Select Process or write ProcessName: "),
        WS_VISIBLE | WS_CHILD,
        15,
        90 + 20 + DEFAULT_MARGIN * 3, // 140 +35 
        DEFAULT_SIZE_X - 50,
        35,
        hWnd,
        NULL,
        NULL,
        NULL);

    hProcesses = CreateWindow(
        _T("ComboBox"),
        _T(""),
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWN| CBS_HASSTRINGS | CBS_AUTOHSCROLL | WS_VSCROLL,
        15,
        140 + 35 + DEFAULT_MARGIN,
        DEFAULT_SIZE_X - 50,
        200,
        hWnd,
        (HMENU)IDM_COMBOBOX,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL);

    CreateWindow(
        _T("Button"),
        _T("Start Injecting"),
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        15,
        210 + DEFAULT_MARGIN,
        100,
        20,
        hWnd,
        (HMENU)IDM_INJECT_BUTTON,
        NULL,
        NULL);
}

void OpenFileSelector(HWND hWnd) {
    OPENFILENAME ofn;
    TCHAR filePath[MAX_LOADSTRING];
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFilter = _T("Dynamic Link Libraries: *.dll\0*.dll\0All Files: *.*\0*.*\0");
    ofn.lpstrFile = filePath;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(filePath);
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = _T("dll");
    GetOpenFileName(&ofn);
    if (string(filePath) == _T("")) {
        return;
    }
    if (string(filePath).find(_T(".dll")) == -1) {
        MessageBox(hWnd, _T("Only DLL-Files are allowed here!"),
            _T("Warning"), MB_ICONWARNING);
        return;
    }
    tstrcpy_s(g_szFileName,filePath);
    SetWindowText(hTextField, g_szFileName);
}

std::map < DWORD, Process > ListProcesses(std::map<DWORD,Process>& pList) {
    pList.clear();
    DWORD procID = NULL;
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        print(_T("Could not get a Snapshot\n"));
        return pList;
    }

    if (Process32First(snapshot, &entry)) { // Init Process here
        while (Process32Next(snapshot, &entry)) {
            pList.insert({ entry.th32ProcessID, { entry.th32ProcessID ,entry.szExeFile } });
        }
    }
    CloseHandle(snapshot);
    return pList;
}

void UpdateProcessBox(std::map<DWORD, Process>& pList) {
    SendMessage(hProcesses, CB_RESETCONTENT, 0, 0);
    for (auto process = pList.begin(); process != pList.end(); process++) {
        string s = process->second.pName.c_str();
        s.append(_T(" ("));
        s.append(std::to_string(process->first));
        s.append(_T(")"));
        //print(_T("Name is: %s\n"), s.c_str());
        SendMessage(hProcesses, CB_ADDSTRING, NULL, (LPARAM)s.c_str());
    }
}

DWORD GetProcIDByDropBoxText(string text) {
    DWORD retVal = NULL;
    size_t init = text.find(_T("(")) + 1;
    size_t length = text.find(_T(")")) - init;
    print(_T("Init: %i\tLength:%i\n"), init, length);
    try {
        string sub = text.substr(init, length);
        retVal = stoul(sub);
        print(_T("Substring PID = %i\n"), retVal);
    }
    catch (std::exception e) {
        retVal = GetProcessIDByName(text);
    }
    return retVal;
}

DWORD GetProcessIDByName(string processName) {
    DWORD procID = NULL;
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        print(_T("Could not get a Snapshot\n"));
        return procID;
    }

    if (Process32First(snapshot, &entry)) {
        do {
            if (!std::string(entry.szExeFile).compare(processName)) {
                print(_T("Found the Process with ProcessID: %u\n"), entry.th32ProcessID);
                print(_T("Name is: %s\n"), entry.szExeFile);
                procID = entry.th32ProcessID;
                break;
            }
        } while (Process32Next(snapshot, &entry));
    }
    CloseHandle(snapshot);
    return procID;
}

void InjectButtonHandler(HWND hWnd) {
    TCHAR bufferPath[MAX_LOADSTRING];
    GetWindowText(hTextField, bufferPath, MAX_LOADSTRING);
    print(_T("Selected DLL-Path: %s\n"), bufferPath);
    if (!PathFileExists(bufferPath)) {
        MessageBox(hWnd, _T("DLL File does noch exist!\n"), _T("Error"), MB_ICONERROR);
        return;
    }
    TCHAR str[MAX_LOADSTRING];
    GetWindowText(hProcesses, str, MAX_LOADSTRING);
    print(_T("Selected Process: %s\n"), str);
    DWORD pid = 0;
    pid = GetProcIDByDropBoxText(str);
    if (pid == NULL) {
        MessageBox(hWnd, _T("Process PID could not be found with the given Process Name or Selection"),
            _T("Error: PID Not found"), MB_ICONERROR);
        return;
    }
    //InjectDLLWithPath(pid, buffer);
    if (InjectDLLWithPath(pid, bufferPath) == FALSE) {
        MessageBox(hWnd, _T("Something went wrong, when trying to inject the dll!"),
            _T("Error"), MB_ICONERROR);
    }
    else {
        MessageBox(hWnd, _T("DLL injected sucessfully!"),
            _T("Info"), MB_ICONINFORMATION);
    }
}

BOOL InjectDLLWithPath(DWORD procID, const TCHAR* dllPath) {
    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);
    if (processHandle == INVALID_HANDLE_VALUE) {
        print("Could Not OpenProcess with ID: %u\n", procID);
        return FALSE;
    }
    print("Handle Opened Successfully\n");
    LPVOID memLoc = VirtualAllocEx(processHandle, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (memLoc == nullptr) {
        print("Could Not Allocate Mem\n");
        CloseHandle(processHandle);
        return FALSE;
    }
    BOOL WPM = WriteProcessMemory(processHandle, memLoc, dllPath, strlen(dllPath) + 1, 0);
    if (!WPM) {
        print("Could Not Write in the ProcessMemory\n");
        CloseHandle(processHandle);
        return FALSE;
    }
    print("Allocated PathName Success\n");

    HANDLE remoteThread = CreateRemoteThread(processHandle, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, memLoc, 0, 0);
    if (!remoteThread) {
        print("Error in Creating RemoteThread (LoadLibraryA)\n");
        VirtualFree(memLoc, NULL, MEM_RELEASE);
        CloseHandle(processHandle);
        return FALSE;
    }
    print("DLL Injected\n");
    CloseHandle(remoteThread);
    VirtualFree(memLoc, NULL, MEM_RELEASE);
    CloseHandle(processHandle);
    return TRUE;
}