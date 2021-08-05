#include <string>
#ifndef SPEECH_HPP
#define SPEECH_HPP

#if defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || \
    defined(__MINGW32__)
#define DLLEXPORT __declspec(dllexport)
#define ADDCALL __stdcall
#ifdef _MSVC_VER
#endif
#else
#define DLLEXPORT
#define ADDCALL
#define fopen_s(pFile, filename, mode) \
  ((*(pFile)) = fopen((filename), (mode))) == NULL
typedef int errno_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

DLLEXPORT int ADDCALL PitchAnalyzer(char* const, char* const);

DLLEXPORT char* ADDCALL PitchAnalyzer2(const char*);

#ifdef __cplusplus
}
#endif

#endif  // SPEECH_HPP
