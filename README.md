
# CMake + clang-cl / MSVC サンプル（Windows）

## 概要
社内展開用の **完成版テンプレート** です。

- CMakePresets.json による **ビルド設定の統一**
- **clang-cl / MSVC(cl)** 切替対応
- **clang-tidy / clang-format** 統合
- **Boost / GoogleTest** は vcpkg(manifest mode)

## 前提
- Windows 10/11
- Visual Studio Build Tools (MSVC v143)
- LLVM (clang-cl, clang-tidy, clang-format)
- CMake 3.23+
- Ninja

## セットアップ
```powershell
git init
git submodule add https://github.com/microsoft/vcpkg.git vcpkg
.cpkgootstrap-vcpkg.bat
```

## ビルド
```powershell
cmake --preset clangcl-debug
cmake --build --preset clangcl-debug
ctest --preset clangcl-debug
```

## MSVC 切替
```powershell
cmake --preset msvc-debug
cmake --build --preset msvc-debug
```

## 静的解析 / 整形
```powershell
cmake --preset clangcl-debug-tidy
cmake --build --preset clangcl-debug-tidy

cmake --build --preset format
```
