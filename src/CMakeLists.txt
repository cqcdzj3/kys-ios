cmake_minimum_required(VERSION 3.20)
project(kys CXX)

# ---------------------------------------------------------------
# 目标平台判定：iOS / 桌面
# ---------------------------------------------------------------
if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
  set(KYS_IOS ON)
else()
  set(KYS_IOS OFF)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OBJCXX_STANDARD 17)

if(KYS_IOS)
  set(CMAKE_OSX_ARCHITECTURES arm64 CACHE STRING "" FORCE)
  set(CMAKE_OSX_DEPLOYMENT_TARGET "14.0" CACHE STRING "" FORCE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -DNDEBUG -fvisibility=hidden")
  add_definitions(-DKYS_IOS=1)
elseif(APPLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -std=c++23 -pthread -DNDEBUG")
elseif(WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest /O2 /DNOMINMAX")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -std=c++23 -pthread -DNDEBUG")
endif()

# ---------------------------------------------------------------
# 依赖：优先使用 vcpkg 安装目录（CI 用 arm64-ios triplet）
# ---------------------------------------------------------------
if(DEFINED ENV{VCPKG_ROOT} AND EXISTS "$ENV{VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}")
  set(VCPKG_INC "$ENV{VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/include")
  set(VCPKG_LIB "$ENV{VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/lib")
  include_directories(${VCPKG_INC})
  link_directories(${VCPKG_LIB})
endif()

set(PROJECT_INCLUDE_DIRS
    ..
    .
    ../mlcc
    ../others
    ../local/include
    ../cifa
)

include_directories(${PROJECT_INCLUDE_DIRS})

# 源码（iOS 不编译网络模块，BattleNetwork 已被 WITH_NETWORK 宏隔离）
file(GLOB SRC_LIST
    ./*.cpp
    ../cifa/Cifa.cpp
    ../mlcc/strfunc.cpp
    ../mlcc/filefunc.cpp
    ../mlcc/PotConv.cpp
    ../mlcc/SimpleCC.cpp
    ../mlcc/SQLite3Wrapper.cpp
    ../mlcc/ZipFile2.cpp
    ../others/Hanz2Piny.cpp
)

# 静态库（vcpkg arm64-ios 提供 <name>.a / lib<name>.a）
set(LINK_LIBS "")
foreach(lib sdl3 SDL3 SDL3_ttf SDL3_image SDL3_mixer sqlite3 yaml-cpp iconv)
  find_library(${lib}_LIB
      NAMES ${lib} lib${lib}
      PATHS ${VCPKG_LIB}
      NO_DEFAULT_PATH)
  if(${lib}_LIB)
    list(APPEND LINK_LIBS ${${lib}_LIB})
  endif()
endforeach()

if(KYS_IOS)
  # iOS 系统框架（SDL3 静态库会用到这些）
  list(APPEND LINK_LIBS
      "-framework UIKit"
      "-framework Foundation"
      "-framework CoreGraphics"
      "-framework CoreAudio"
      "-framework AudioToolbox"
      "-framework AVFoundation"
      "-framework CoreHaptics"
      "-framework QuartzCore"
      "-framework Metal"
      "-framework OpenGLES"
      "-framework GameController"
      "-framework CoreMotion"
      "-framework CoreBluetooth"
      "-framework UniformTypeIdentifiers"
      "-framework Security"
      "-framework CoreFoundation"
      "-framework CoreServices"
      "-lz"
      "-lbz2"
  )
endif()

if(KYS_IOS)
  add_executable(kys MACOSX_BUNDLE ${SRC_LIST})
  set_target_properties(kys PROPERTIES
      MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/../ios/Info.plist"
      MACOSX_BUNDLE_BUNDLE_NAME "kys"
      MACOSX_BUNDLE_GUI_IDENTIFIER "com.kys.port"
      MACOSX_BUNDLE_BUNDLE_VERSION "1.0"
      MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
      XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO"
      XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ""
  )
else()
  add_executable(kys ${SRC_LIST})
endif()

target_link_libraries(kys ${LINK_LIBS})
