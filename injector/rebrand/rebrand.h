#pragma once
#include <string>
#include "../skStr.h"

inline std::string name = skCrypt("").decrypt(); // Application Name
inline std::string ownerid = skCrypt("").decrypt(); // Owner ID
inline std::string secret = skCrypt("").decrypt(); // Application Secret
inline std::string version = skCrypt("1.0").decrypt(); // Application Version
inline std::string url = skCrypt("https://keyauth.win/api/1.2/").decrypt(); // change if you're self-hosting
inline std::string path = skCrypt("").decrypt(); // (OPTIONAL) see tutorial here https://www.youtube.com/watch?v=I9rxt821gMk&t=1s


inline std::string cheatName = skCrypt("").decrypt();
inline std::wstring cheatNameW = _W(L"").decrypt();
inline std::string downloadURL = skCrypt("").decrypt();