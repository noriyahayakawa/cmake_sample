
# CMake + clang-cl / MSVC サンプル

## 概要
このリポジトリは、Windows/Linux 両対応の C++ テンプレートです。

- CMakePresets.json によるビルド設定の統一
- clang-cl / MSVC(cl) / clang の切り替え
- clang-tidy / clang-format 統合
- Boost / GoogleTest を vcpkg (manifest mode) で管理
- C++23 ベース

## ディレクトリ構成
```text
.
├── CMakeLists.txt
├── CMakePresets.json
├── vcpkg.json
├── libs/
│   └── core/
├── src/
│   └── main.cpp
├── tests/
│   ├── CMakeLists.txt
│   └── test_main.cpp
├── .vscode/
│   ├── tasks.json
│   └── launch.json
└── vcpkg/
```

## 前提

### Windows
| ツール | バージョン | 備考 | winget コマンド |
|--------|-----------|------|-----------------|
| Windows | 10 / 11 | | (OS のため対象外) |
| Visual Studio 2022 | Community / Build Tools | MSVC v143 | `winget install --id Microsoft.VisualStudio.2022.BuildTools -e --override "--wait --quiet --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended"` |
| LLVM | 最新推奨 | clang-cl, clang-tidy, clang-format | `winget install --id LLVM.LLVM -e` |
| CMake | 3.23+ | | `winget install --id Kitware.CMake -e` |
| Ninja | 最新推奨 | | `winget install --id Ninja-build.Ninja -e` |

注意:
- msvc-* プリセットは cl.exe を使用します。
- VS Code タスクでは tools/cmake-msvc-x64.cmd 経由で MSVC 環境を設定します。

### Linux
| ツール | バージョン | 備考 |
|--------|-----------|------|
| Ubuntu 22.04+ など | | |
| LLVM / Clang | 最新推奨 | clang, clang++, clang-tidy, clang-format |
| CMake | 3.23+ | |
| Ninja | 最新推奨 | |
| curl, zip, unzip, tar | | vcpkg 依存 |

```bash
sudo apt install -y clang clang-tidy clang-format cmake ninja-build \
                    curl zip unzip tar pkg-config
```

## セットアップ
```powershell
# 既に submodule 登録済みの場合
git submodule update --init --recursive
./vcpkg/bootstrap-vcpkg.bat
```

## 推奨実行順

### Windows (clang-cl, static Debug)
```powershell
# 1) 構成
cmake --preset clangcl-debug-static

# 2) リビルド（clean-first）
cmake --build --preset clangcl-debug-static --clean-first

# 3) テスト
ctest --preset clangcl-debug-static
```

### Windows (MSVC, static Debug)
```powershell
# 1) 構成
tools/cmake-msvc-x64.cmd --preset msvc-debug-static

# 2) リビルド（clean-first）
tools/cmake-msvc-x64.cmd --build --preset msvc-debug-static --clean-first

# 3) テスト
ctest --preset msvc-debug-static
```

### Linux (clang, Debug)
```bash
# 1) 構成
cmake --preset clang-debug

# 2) リビルド（clean-first）
cmake --build --preset clang-debug --clean-first

# 3) テスト
ctest --preset clang-debug
```

### VS Code タスクを使う場合

1. CMake: 構成 (<preset名>) を実行
2. CMake: ビルド (<preset名>) を実行
3. CTest: テスト (<preset名>) を実行

補足:
- clang-cl / MSVC の preset では、ENABLE_AUTO_CLANG_FORMAT が ON のため、ビルド前に clang-format が自動実行されます。
- フォーマットのみ実行したい場合は CMake: フォーマット (Windows) または CMake: フォーマット (Linux) を使用してください。

## プリセット

### configure/build preset

#### Windows (clang-cl)
- clangcl-debug-static
- clangcl-debug-dll
- clangcl-release-static
- clangcl-release-dll

#### Windows (MSVC)
- msvc-debug-static
- msvc-debug-dll
- msvc-release-static
- msvc-release-dll

#### Linux (clang)
- clang-debug
- clang-release

#### tidy 用 build preset
- clangcl-debug-tidy
- clangcl-debug-tidy-dll
- clang-debug-tidy

#### format 用 build preset
- format (Windows)
- format-linux (Linux)

### test preset
- clangcl-debug-static
- clangcl-debug-dll
- clangcl-release-static
- clangcl-release-dll
- msvc-debug-static
- msvc-debug-dll
- msvc-release-static
- msvc-release-dll
- clang-debug
- clang-release
- clangcl-debug (エイリアス)

## ビルドとテスト

### clang-cl (Windows, Debug static)
```powershell
cmake --preset clangcl-debug-static
cmake --build --preset clangcl-debug-static
ctest --preset clangcl-debug-static
```

### clang-cl (Windows, Release static)
```powershell
cmake --preset clangcl-release-static
cmake --build --preset clangcl-release-static
ctest --preset clangcl-release-static
```

### MSVC (Windows, Debug static)
```powershell
tools/cmake-msvc-x64.cmd --preset msvc-debug-static
tools/cmake-msvc-x64.cmd --build --preset msvc-debug-static
ctest --preset msvc-debug-static
```

### clang (Linux, Debug)
```bash
cmake --preset clang-debug
cmake --build --preset clang-debug
ctest --preset clang-debug
```

成果物は out/build/<preset名>/ に出力されます。

## clang-format 自動実行

- CMake オプション ENABLE_AUTO_CLANG_FORMAT が ON のとき、app/tests ビルド前に format ターゲットを実行します。
- format ターゲットは format-app + core-format + tests-format の集約です。
- clang-format が見つからない場合は警告を出してスキップします。

明示的に無効化する例:
```powershell
cmake --preset clangcl-debug-static -DENABLE_AUTO_CLANG_FORMAT=OFF
```

## VS Code タスク

.vscode/tasks.json には以下が定義されています。

- 構成: CMakePresets.json の configurePresets（hidden を除く）を網羅
- ビルド: CMakePresets.json の buildPresets を網羅
- テスト: CMakePresets.json の testPresets（hidden を除く）を網羅
- 補助: フォーマット (format / format-linux)

例:
- CMake: 構成 (clangcl-debug-static)
- CMake: ビルド (msvc-release-dll)
- CTest: テスト (clangcl-release-static)

## VS Code デバッグ

.vscode/launch.json には以下のデバッグ構成が含まれます。

- Debug app (clangcl-debug-static)
- Debug tests (clangcl-debug-static)
- Debug app (msvc-debug-static)
- Debug tests (msvc-debug-static)

これらは対応する `preLaunchTask` を実行してから起動されます。
