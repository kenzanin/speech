cmake_minimum_required(VERSION 3.5)

add_subdirectory(speech)
project(main LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_FLAGS "-Wall -Wextra")
#set(CMAKE_CC_FLAGS "-Wall -Wextra")
add_executable(main src/main.cpp)
target_link_libraries(main speech)
target_include_directories(main PUBLIC speech/src)

target_sources(main PRIVATE
  123.wav
  ID0001_channel1.wav
  lagu.wav
  test.wav
  out/linux/libspeech.so
  out/win64/speech.dll
  out/win32/speech.dll
)
