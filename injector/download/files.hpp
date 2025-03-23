#include <Windows.h>
#include <iostream>
#include <string>
#include "../rebrand/rebrand.h"
#include "../skStr.h"
#include "../keyauth/json.hpp"
#include "../mapper/KD.h"
#include "../driver.hpp"


inline std::wstring string_to_wstring(const std::string& str) {

    if (str.empty()) {
        return std::wstring();
    }

    // Determine the size of the destination wide string, including null terminator
    size_t requiredSize = 0;
    mbstowcs_s(&requiredSize, nullptr, 0, str.c_str(), 0);

    if (requiredSize == 0) {
        throw std::runtime_error("Conversion error occurred");
    }

    // Create a wide string buffer of the appropriate size
    std::wstring wstr(requiredSize, L'\0');

    // Perform the conversion
    size_t convertedChars = 0;
    mbstowcs_s(&convertedChars, &wstr[0], requiredSize, str.c_str(), requiredSize - 1);

    // Remove the null terminator that mbstowcs_s adds to the string
    wstr.resize(convertedChars - 1);

    return wstr;
}

inline bool download_file(const std::string& url, const std::string& filename) {
    std::string command = std::string(skCrypt("curl -o \"")) + filename + std::string(skCrypt("\" \"")) + url + std::string(skCrypt("\"")) + std::string(skCrypt(" > nul 2>&1"));
    return (system(command.c_str()) == 0);
}

inline bool delete_file(const std::string& filename) {
    return std::filesystem::remove(filename);
}



inline void loadDriver() {
    system(skCrypt("taskkill /F /IM RainbowSix.exe > nul 2>&1"));
    system(skCrypt("taskkill /F /IM EpicGamesLauncher.exe > nul 2>&1"));
    system(skCrypt("taskkill /F /IM EasyAntiCheatLauncher.exe > nul 2>&1"));
    system(skCrypt("taskkill /F /IM BEService.exe > nul 2>&1"));
    system(skCrypt("taskkill /F /IM BattleEyeLauncher.exe > nul 2>&1"));
    //system(skCrypt("taskkill /F /IM Steam.exe > nul 2>&1"));
    system(skCrypt("wevtutil cl System"));
    system(skCrypt("cls"));

    std::string url = "";
    std::string filename = std::string(skCrypt("C:\\Windows\\System32\\drivers\\Driver.sys"));
    if (download_file(url, filename)) {
    }
    else {
        MessageBoxA(NULL, skCrypt("Common Fixes: \n- Disable any antivirus\n- Try again\n- Restarting your PC"), "", 0);
        system(skCrypt("SFC /scannow"));
        exit(0);
        return;
    }

    int r = kdmain(string_to_wstring(filename));

    delete_file(filename);
    system(skCrypt("wevtutil cl System"));
    system(skCrypt("for /F \"tokens = \" %1 in ('wevtutil el') DO wevtutil cl \" % 1\""));
    system(skCrypt("cls"));
    if (r == -1) {
        //SFC /scannow
        std::cout << skCrypt("FAILD TO LOAD DRIVER") << std::endl;
        MessageBoxA(NULL, "Common Fixes: \n- Disable any antivirus\n- Try again\n- Restarting your PC", "", 0);
        system(skCrypt("SFC /scannow"));
        exit(0);
        return;
    }
    std::cout << skCrypt("[+] LOADED DRIVER") << std::endl;
}

inline std::string downlaod_dll() {
    ///cheats/maesfeshfjmfes/Internal.dll
    system(skCrypt("cls"));

    std::string url = downloadURL;
    std::string filename = std::string(skCrypt("C:\\Windows\\System32\\Driver.sys"));
    if (download_file(url, filename)) {
    }
    else {
        MessageBoxA(NULL, skCrypt("Common Fixes: \n- Disable any antivirus\n- Try again\n- Restarting your PC"), "", 0);
        system(skCrypt("SFC /scannow"));
        exit(0);
        return "";
    }

    return filename;
}