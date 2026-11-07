# Overlay triplet: iOS arm64 static, deployment target pinned to iOS 14.0.
# Without this, vcpkg builds dependencies against the SDK default (e.g. 26.5),
# which links into a 14.0 binary as "built for newer iOS" and crashes on
# older devices at launch.
set(VCPKG_TARGET_ARCHITECTURE arm64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)
set(VCPKG_CMAKE_SYSTEM_NAME iOS)
set(VCPKG_OSX_ARCHITECTURES arm64)
set(VCPKG_OSX_DEPLOYMENT_TARGET 14.0)
set(VCPKG_OSX_SYSROOT iphoneos)
