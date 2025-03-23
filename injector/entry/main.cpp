#include <iostream>
#include <string_view>
#include <Windows.h>
#include <TlHelp32.h>
#include <memory>
#include <cstdint>
#include <vector>
#include "../injector.hpp"
#include "../xor.h"
#include "../skStr.h"
#include "../keyauth/auth.hpp"
#include "../keyauth/json.hpp"
#include "../keyauth/utils.hpp"
#include "../protection/cpp-anti-debug/antidbg.h"
#include "../download/files.hpp"
#include "../rebrand/rebrand.h"



std::string timeSt = "";

std::string tm_to_readable_time(tm ctx);
static std::time_t string_to_timet(std::string timestamp);
static std::tm timet_to_tm(time_t timestamp);
const std::string compilation_date = (std::string)skCrypt(__DATE__);
const std::string compilation_time = (std::string)skCrypt(__TIME__);

using namespace KeyAuth;

api KeyAuthApp(name, ownerid, secret, version, url, path);

void check() {
    adbg_IsDebuggerPresent();
    adbg_BeingDebuggedPEB();
    adbg_NtGlobalFlagPEB();
    adbg_CheckRemoteDebuggerPresent();
    adbg_NtQueryInformationProcess();
    adbg_CheckWindowClassName();
    adbg_CheckWindowName();
    adbg_ProcessFileName();
    adbg_NtSetInformationThread();
    adbg_HardwareDebugRegisters();
    adbg_MovSS();
    adbg_RDTSC();
    adbg_QueryPerformanceCounter();
    adbg_GetTickCount();
    adbg_CloseHandleException();
    adbg_PrefixHop();
}
//
// console 
//
void SetCenteredConsoleTitle(const std::string& title) {
    SetConsoleTitleA("");

    // Clear the console screen
    system("cls");

    // Get the console screen buffer info
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        int consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        int titleLength = title.length();
        int padding = (consoleWidth - titleLength) / 2;

        // Print the centered title in the console output area
        std::string centeredTitle = std::string(padding, ' ') + title;
        std::cout << centeredTitle << std::endl;
    }
}

void RemoveScrollBars() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);

    SHORT width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    SHORT height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    COORD bufferSize;
    bufferSize.X = width;
    bufferSize.Y = height;
    SetConsoleScreenBufferSize(hConsole, bufferSize);

    SMALL_RECT windowSize = { 0, 0, width - 1, height - 1 };
    SetConsoleWindowInfo(hConsole, TRUE, &windowSize);
}

void RemoveConsoleIcon() {
    HWND hWnd = GetConsoleWindow();

    LONG_PTR style = GetWindowLongPtr(hWnd, GWL_STYLE);
    style &= ~(WS_SYSMENU);
    SetWindowLongPtr(hWnd, GWL_STYLE, style);

    SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

//
//driver
//

std::string generateRandomString(size_t size) {
    const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    const size_t max_index = sizeof(charset) - 2; // Adjusted to avoid out-of-bounds access

    std::string randomString(size, 0);
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distribution(0, max_index);

    for (size_t i = 0; i < size; ++i) {
        randomString[i] = charset[distribution(generator)];
    }

    return randomString;
}

//
// loader part
//

bool checkIfGameIsRunning() {

    DWORD process_id = 0;
    DWORD p_thread_id = GetWindowThreadProcessId(FindWindowA(skCrypt("R6Game"), NULL), &process_id); Sleep(20);

    if (!(process_id == 0)) {
        MessageBoxA(NULL, skCrypt("CLOSE R6 BEFORE RUNNING ARE LOADER"), "", 0);
        exit(0);
    }
}

int DU_MessageBoxTimeout(HWND hWnd, const WCHAR* sText, const WCHAR* sCaption, UINT uType, DWORD dwMilliseconds)
{
    // Displays a message box, and dismisses it after the specified timeout.
    typedef int(__stdcall* MSGBOXWAPI)(IN HWND hWnd, IN LPCWSTR lpText, IN LPCWSTR lpCaption, IN UINT uType, IN WORD wLanguageId, IN DWORD dwMilliseconds);

    int iResult;

    HMODULE hUser32 = LoadLibraryA("user32.dll");
    if (hUser32)
    {
        auto MessageBoxTimeoutW = (MSGBOXWAPI)GetProcAddress(hUser32, skCrypt("MessageBoxTimeoutW"));

        iResult = MessageBoxTimeoutW(hWnd, sText, sCaption, uType, 0, dwMilliseconds);

        FreeLibrary(hUser32);
    }
    else
        iResult = MessageBox(hWnd, sText, sCaption, uType);         // oups, fallback to the standard function!

    return iResult;
}
const std::uint32_t get_process_id(const std::wstring& proc_name)
{
    PROCESSENTRY32 proc_info;
    proc_info.dwSize = sizeof(proc_info);

    HANDLE proc_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if (proc_snapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    Process32First(proc_snapshot, &proc_info);
    if (!wcscmp(proc_info.szExeFile, proc_name.c_str()))
    {
        CloseHandle(proc_snapshot);
        return proc_info.th32ProcessID;
    }

    while (Process32Next(proc_snapshot, &proc_info))
    {
        if (!wcscmp(proc_info.szExeFile, proc_name.c_str()))
        {
            CloseHandle(proc_snapshot);
            return proc_info.th32ProcessID;
        }
    }

    CloseHandle(proc_snapshot);
    return 0;
}



void inject() {
    checkIfGameIsRunning();
    loadDriver();

    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_HIDE);

    timeSt = std::string(std::string(skCrypt("\n\n\n\n Press F2 when on main menu")));
    std::wstring timeStW = std::wstring(timeSt.begin(), timeSt.end());
    DU_MessageBoxTimeout(NULL, timeStW.c_str(), skCrypt(L"Auto-Close Message"), MB_OK, 6000);

    //Notepad, 0x3074e, Untitled - Notepad, notepad.exe (18064): 18028
    //Test-DLL.dll
    
    //R6Game
    //Internal.dll
    check();

    while (true) {
        if (GetAsyncKeyState(VK_F2)) {
            break;
        }
        Sleep(10);
    }

    // make an download dll function
    std::string dllFile = downlaod_dll();
    pysen(skCrypt("R6Game"), string_to_wstring(dllFile).c_str());
    delete_file(dllFile);

    exit(0);
    return;
}

int main()
{
    checkIfGameIsRunning();
    check();

    HANDLE hMutex = CreateMutex(NULL, FALSE, cheatNameW.c_str());

    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        std::cerr << "Another instance is already running." << std::endl;
        MessageBoxA(NULL, skCrypt("Another instance is already running."), "Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    std::string consoleTitle = cheatName + skCrypt("-").decrypt() + compilation_date + " " + compilation_time;
    SetCenteredConsoleTitle(consoleTitle.c_str());
    RemoveConsoleIcon();
    RemoveScrollBars();
    std::cout << skCrypt("\n\n [1] Load Cheat\n [2] Exit\n\n Choose option: ");

    int option;
    std::cin >> option;
    if (option == 1) {

    }
    else if (option == 2) {
        exit(1);
    }
    else {
       main();
    }

    // Freeing memory to prevent memory leak or memory scraping
    name.clear(); ownerid.clear(); secret.clear(); version.clear(); url.clear();
    std::cout << skCrypt("\n\n[!] Connecting..");

    KeyAuthApp.init();
    if (!KeyAuthApp.response.success)
    {
        std::cout << skCrypt("\n Status: ") << KeyAuthApp.response.message;
        Sleep(1500);
        exit(1);
    }
    SetCenteredConsoleTitle(consoleTitle.c_str());

    if (std::filesystem::exists("key.json")) //change test.txt to the path of your file :smile:
    {
        if (!CheckIfJsonKeyExists(std::string(skCrypt("key.json")), std::string(skCrypt("username"))))
        {
            std::string key = ReadFromJson(std::string(skCrypt("key.json")), std::string(skCrypt("license")));
            KeyAuthApp.license(key);
            if (!KeyAuthApp.response.success)
            {
                std::remove("key.json");
                std::cout << skCrypt("\n Status: ") << KeyAuthApp.response.message;
                Sleep(1500);
                exit(1);
            }
            //std::cout << skCrypt("\n\n[+] Successfully Automatically Logged In\n");
        }
        else
        {
            std::string username = ReadFromJson(std::string(skCrypt("key.json")), std::string(skCrypt("username")));
            std::string password = ReadFromJson(std::string(skCrypt("key.json")), std::string(skCrypt("password")));
            KeyAuthApp.login(username, password);
            if (!KeyAuthApp.response.success)
            {
                std::remove("key.json");
                std::cout << skCrypt("\n Status: ") << KeyAuthApp.response.message;
                Sleep(1500);
                exit(1);
            }
            //std::cout << skCrypt("\n\n Successfully Automatically Logged In\n");
        }
    }
    else
    {
        std::string username;
        std::string password;
        std::string key;

        std::cout << skCrypt("\n Enter license: ");
        std::cin >> key;
        KeyAuthApp.license(key);

        check();

        if (!KeyAuthApp.response.success)
        {
            std::cout << skCrypt("\n Status: ") << KeyAuthApp.response.message;
            Sleep(3500);
            exit(1);
        }
        if (username.empty() || password.empty())
        {
            WriteToJson(std::string(skCrypt("key.json")), std::string(skCrypt("license")), key, false, "", "");
            std::cout << skCrypt("Successfully Created File For Auto Login");
        }
        else
        {
            WriteToJson(std::string(skCrypt("key.json")), std::string(skCrypt("username")), username, true, std::string(skCrypt("password")), password);
            std::cout << skCrypt("Successfully Created File For Auto Login");
        }


    }

    //std::cout << skCrypt("\n User data:");
    //std::cout << skCrypt("\n Username: ") << KeyAuthApp.user_data.username;
    //std::cout << skCrypt("\n IP address: ") << KeyAuthApp.user_data.ip;
    //std::cout << skCrypt("\n Hardware-Id: ") << KeyAuthApp.user_data.hwid;
    //std::cout << skCrypt("\n Create date: ") << tm_to_readable_time(timet_to_tm(string_to_timet(KeyAuthApp.user_data.createdate)));
    //std::cout << skCrypt("\n Last login: ") << tm_to_readable_time(timet_to_tm(string_to_timet(KeyAuthApp.user_data.lastlogin)));
    //std::cout << skCrypt("\n Subscription(s): ");


    for (int i = 0; i < KeyAuthApp.user_data.subscriptions.size(); i++) {
        auto sub = KeyAuthApp.user_data.subscriptions.at(i);
        //std::cout << skCrypt("\n name: ") << sub.name;
        std::cout << skCrypt("\n\n[!] R6 - by ") << cheatName << "\n";
        timeSt = tm_to_readable_time(timet_to_tm(string_to_timet(sub.expiry)));
        std::cout << std::endl;
    }

    inject();

    return 0;
}

std::string tm_to_readable_time(tm ctx) {
    char buffer[80];

    strftime(buffer, sizeof(buffer), skCrypt("%a %m/%d/%y %H:%M:%S %Z"), &ctx);

    return std::string(buffer);
}

static std::time_t string_to_timet(std::string timestamp) {
    auto cv = strtol(timestamp.c_str(), NULL, 10); // long

    return (time_t)cv;
}

static std::tm timet_to_tm(time_t timestamp) {
    std::tm context;

    localtime_s(&context, &timestamp);

    return context;
}