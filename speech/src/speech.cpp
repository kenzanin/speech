/*
 * @file speech.cpp
 * @author suka isnaini (kenzanin)
 * @date 26-08-21
 * @brief code for getting f0 from libworld (high quality speech analysis)
 *
Authored by Suka Isnaini on 9th of August 2021
Created for PT Sejahtera Empati Pratama
All Copyrights belong to PT Sejahtera Empati Pratama
*/

#include "speech.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <future>
#include <iostream>
#include <thread>
#include <utility>
#include <vector>

#include "audioio.h"
#include "jsonString.hpp"
#include "world/cheaptrick.h"
#include "world/constantnumbers.h"
#include "world/dio.h"
#include "world/harvest.h"
#include "world/stonemask.h"

#define __HARVEST__ 1

/*
 * @brief error definition
 *
 */
static const std::map<int, std::string> errCode{
    {0000, "success"},
    {1000, "Error : file not found"},
    {1001, "Error : file cannot be read"},
    {1002, "Error : file is not on correct format"},
    {2000, "Error : no speech detected"},
    {2001, "Error : cannot calculate pitch 1. Reason : ..."},
    {2002, "Error : cannot calculate pitch 2. Reason : ..."},
    {2003, "Error : cannot calculate pitch 3. Reason : ..."},
    {2004, "Error : cannot calculate pitch 4. Reason : ..."},
    {3000, "Error : Memory Allocation Error"}};

/*
 * @brief _wavFile struct
 * @detail this struct hold wav related data such as
 * - fileName == wav filename in c string
 * - fs == needed by worldlib
 * - nbit == needed by worldlib
 * - length == needed by worldlib
 * default constructor with parameter wav file location and file name in c
 * string.
 */

struct _wavFile {
 public:
  const char *fileName{};
  int fs{};
  int nbit{};
  int length{};
  const double *buf{};
  ~_wavFile() { delete[] buf; }
  _wavFile(const char *file = {}) : fileName(file) {
    try {
      length = GetAudioLength(fileName);
      if (length == 0 || length == -1) {
        throw 1002;
      }
    } catch (int e) {
      jsonResult.at("status") = e;
      jsonResult.at("comment") = errCode.at(1002);
      std::cerr << 1002 << " " << errCode.at(1002) << "\n";
      throw;
    }

    try {
      buf = new const double[length]{};
    } catch (std::bad_alloc &e) {
      std::cerr << 3000 << " " << errCode.at(3000) << " " << e.what() << "\n";

      jsonResult.at("status") = 3000;
      jsonResult.at("comment") = errCode.at(3000) + e.what();
      throw 3000;
    }
    wavread(fileName, &fs, &nbit, const_cast<double *>(buf));
  }

  //! linter be quiet!
  //_wavFile(_wavFile const &other){};
  //_wavFile operator=(_wavFile const &other) { return *this; }
};

/*
 * @brief _f0 struct
 * @detail
 * this struct provide space to hold f0 data, generate by worldlib such as
 * - f0 array of double
 * - temporalPossition array of double
 * default constructor with parameter the size of array in int
 */
struct _f0 {
  double *f0{};
  double *temporalPossition{};
  int numOfFrame{};
  _f0(int in = {}) : numOfFrame(in) {
    try {
      f0 = new double[numOfFrame]{};
      temporalPossition = new double[numOfFrame]{};
    } catch (std::bad_alloc &e) {
      std::cerr << 3000 << " " << errCode.at(3000) << " " << e.what() << "\n";

      jsonResult.at("status") = 3000;
      jsonResult.at("comment") = errCode.at(3000) + e.what();
      throw 3000;
    }
  }
  ~_f0() {
    delete[] f0;
    delete[] temporalPossition;
  };

  //! linter be quiet!
  //_f0(const _f0 &other) {}
  //_f0 operator=(const _f0 &other) { return *this; }
};

/*
 * @brief getPitch1
 * @param f0 data array
 * @param dat_length f0 array length
 * @return average value for f0
 */
double getPitch1(const double *dat, int const dat_length) {
  double sum{};
  for (int i = 0; i < dat_length; i++) {
    if (dat[i] == 0.0) continue;
    sum += dat[i];
  }
  return (double)sum / dat_length;
}

/*
 * @brief getPitch2
 * @param f0 data array
 * @param dat_length f0 array length
 * @return standard deviation of f0
 */

double getPitch2(const double *dat, int const dat_length) {
  double sum{};
  std::for_each(dat, dat + dat_length, [&sum](double each) { sum += each; });
  double mean = (double)sum / dat_length;
  double sd{};
  std::for_each(dat, dat + dat_length,
                [mean, &sd](double each) { sd += std::pow(each - mean, 2); });

  return std::sqrt(sd / dat_length);
}

/*
 * @brief getPitch3
 * @param f0 data array
 * @param dat_length f0 array length
 * @return (average of first half of f0 data) - (average of last half of f0
 * data)
 */
double getPitch3(const double *dat, int const dat_length) {
  double sum1{};
  for (int i = 0; i < dat_length / 2; i++) {
    sum1 += dat[i];
  }
  sum1 /= dat_length / 2;

  double sum2{};
  for (int i = (dat_length / 2); i < dat_length; i++) {
    sum2 += dat[i];
  }
  sum2 /= dat_length / 2;

  return sum2 - sum1;
}

/*
 * @brief getPitch4
 * @param f0 data array
 * @param dat_length f0 array length
 * @return (the average of f0[0 .. max-5]) - (the average of f0[max-5 .. max])
 */
double getPitch4(const double *dat, int const dat_length) {
  double sum1{};
  for (int i = 0; i < dat_length - 5; i++) {
    sum1 += dat[i];
  }
  sum1 /= (dat_length - 5);

  double sum2{};
  for (int i = (dat_length - 5); i < dat_length; i++) {
    sum2 = dat[i];
  }
  sum2 /= 5.0;

  return sum2 - sum1;
}

int __PitchAnalyzer(const char *fileName) {
  {
    std::FILE *file;
    try {
      errno_t err = fopen_s(&file, fileName, "r");
      if (err) throw 1000;
    } catch (int e) {
      std::cerr << errCode.at(e) << "\n";
      jsonResult.at("status") = e;
      jsonResult.at("comment") = errCode.at(e);
      return 1000;
    }
    std::fclose(file);
  }

  _wavFile *wav{};
  try {
    wav = new _wavFile(fileName);
  } catch (int e) {
    return e;
  }

#if __DEBUG__ == 1
  std::printf("\n\nSTART: list dari buf wav\n\n");
  for (int i = 0; i < wav->length; i++) {
    std::printf(" %.2f ", wav->buf[i]);
  }
  std::printf("\n\nEND: list dari buf wav\n\n");
#endif

#if __HARVEST__ == 1
  HarvestOption option{};
  InitializeHarvestOption(&option);
#else
  DioOption option{};
  InitializeDioOption(&option);
#endif

  _f0 *f0{};
  try {
#if __HARVEST__ == 1
    f0 = new _f0(
        GetSamplesForHarvest(wav->fs, wav->length, option.frame_period));
#else
    f0 = new _f0(GetSamplesForDIO(wav->fs, wav->length, option.frame_period));
#endif
  } catch (int e) {
    delete f0;
    return e;
  }

#if __HARVEST__ == 1
  Harvest(wav->buf, wav->length, wav->fs, &option, f0->temporalPossition,
          f0->f0);
#else
  Dio(wav->buf, wav->length, wav->fs, &option, f0->temporalPossition, f0->f0);
#endif

#if __DEBUG__ == 1
  std::printf("\n\nSTART: list dari F0:\n\n");
  for (int i = 0; i < f0->numOfFrame; i++) {
    std::printf(" %.2f ", f0->f0[i]);
  }
  std::printf("\n\nEND: list dari F0:\n\n");
#endif

  std::future<double> ret1 = std::async(&getPitch1, f0->f0, f0->numOfFrame);
  std::future<double> ret2 = std::async(&getPitch2, f0->f0, f0->numOfFrame);
  std::future<double> ret3 = std::async(&getPitch3, f0->f0, f0->numOfFrame);
  std::future<double> ret4 = std::async(&getPitch4, f0->f0, f0->numOfFrame);

  jsonResult.at("pitch1") = ret1.get();
  jsonResult.at("pitch2") = ret2.get();
  jsonResult.at("pitch3") = ret3.get();
  jsonResult.at("pitch4") = ret4.get();
  jsonResult.at("comment") = errCode.at(0);

  delete f0;
  delete wav;
  return {};
}

#ifdef __cplusplus
extern "C" {
#endif

/*
 * @brief PitchAnalyzer
 * @param fileName == wav file name in c string
 * @param dst == pointer of string to store the c string result, the caller need
 * to allocated this first and then free it.
 * @return 0 == succes, non zero err in error
 */
DLLEXPORT int ADDCALL PitchAnalyzer(char *const fileName, char *const dst) {
#if defined(_MSC_VER) && !defined(__clang__)
  __pragma(comment(linker, "/export:PitchAnalyzer=_PitchAnalyzer@8"));
#endif
  auto err = __PitchAnalyzer(fileName);
  auto x = jsonResult.dump();
  x.copy(dst, x.length() + 1, 0);
  return err == 0 ? 0 : err;
}

/*
 * @brief PitchAnalyzer2
 * @param fileName == wav file name in c string
 * @return pointer of c string result.
 */
DLLEXPORT char *ADDCALL PitchAnalyzer2(char const *fileName) {
#if defined(_MSC_VER) && !defined(__clang__)
  __pragma(comment(linker, "/export:PitchAnalyzer2=_PitchAnalyzer2@4"));
#endif
  __PitchAnalyzer(fileName);
  auto x = jsonResult.dump();
  char *json_return = new char[x.length() + 1]{};
  x.copy(json_return, x.length(), 0);
  return json_return;
}

#ifdef __cplusplus
}
#endif
