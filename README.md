
# CMake + clang-cl / MSVC サンプル（Windows）

## 概要
社内展開用の **完成版テンプレート** です。

- CMakePresets.json による **ビルド設定の統一**
- **clang-cl / MSVC(cl)** 切替対応
- **clang-tidy / clang-format** 統合
- **Boost / GoogleTest** は vcpkg (manifest mode) で管理
- C++20 対応

## ディレクトリ構成
```
.
├── CMakeLists.txt
├── CMakePresets.json
├── vcpkg.json          # 依存: boost, gtest
├── libs/
│   └── core/           # 共有ライブラリ
│       ├── include/
│       └── src/
├── src/
│   └── main.cpp
├── tests/
│   └── test_main.cpp
└── vcpkg/              # git submodule
```

## 前提

### Windows
| ツール | バージョン | 備考 |
|--------|-----------|------|
| Windows | 10 / 11 | |
| Visual Studio 2022 | Community / Build Tools | MSVC v143 (VC++ ツール) |
| LLVM | 最新推奨 | clang-cl, clang-tidy, clang-format |
| CMake | 3.23+ | |
| Ninja | 最新推奨 | |

> **注意:** `CMakePresets.json` にはコンパイラの絶対パス (`C:/Program Files/LLVM/...`,
> `C:/Program Files/Microsoft Visual Studio/2022/Community/...`) がハードコードされています。
> 環境が異なる場合は該当箇所を修正してください。

### Linux
| ツール | バージョン | 備考 |
|--------|-----------|------|
| Ubuntu 22.04+ など | | |
| LLVM / Clang | 最新推奨 | clang, clang++, clang-tidy, clang-format |
| CMake | 3.23+ | |
| Ninja | 最新推奨 | |
| curl, zip, unzip, tar | | vcpkg の依存ツール |

```bash
# Ubuntu / Debian 系
sudo apt install -y clang clang-tidy clang-format cmake ninja-build \
                    curl zip unzip tar pkg-config
```

## セットアップ
```powershell
# テンプレートをコピー後、vcpkg サブモジュールを初期化
git submodule add https://github.com/microsoft/vcpkg.git vcpkg
./vcpkg/bootstrap-vcpkg.bat
```

## ビルド

### プリセット一覧

#### Windows
| プリセット | コンパイラ | 種別 |
|-----------|-----------|------|
| `clangcl-debug` | clang-cl | Debug |
| `clangcl-release` | clang-cl | Release |
| `msvc-debug` | MSVC (cl) | Debug |
| `msvc-release` | MSVC (cl) | Release |

#### Linux
| プリセット | コンパイラ | 種別 |
|-----------|-----------|------|
| `clang-debug` | clang / clang++ | Debug |
| `clang-release` | clang / clang++ | Release |

ビルド成果物は `out/build/<プリセット名>/` に出力されます。

### clang-cl – Windows (Debug)
```powershell
cmake --preset clangcl-debug
cmake --build --preset clangcl-debug
ctest --preset clangcl-debug
```

### clang-cl – Windows (Release)
```powershell
cmake --preset clangcl-release
cmake --build --preset clangcl-release
```

### MSVC に切り替える – Windows
```powershell
cmake --preset msvc-debug
cmake --build --preset msvc-debug
```

### clang – Linux (Debug)
```bash
cmake --preset clang-debug
cmake --build --preset clang-debug
ctest --preset clang-debug
```

### clang – Linux (Release)
```bash
cmake --preset clang-release
cmake --build --preset clang-release
```

## 静的解析 / 整形

### Windows
```powershell
# clang-tidy
cmake --preset clangcl-debug-tidy
cmake --build --preset clangcl-debug-tidy

# clang-format (src/ と tests/ 以下を一括整形)
cmake --build --preset format
```

### Linux
```bash
# clang-tidy
cmake --preset clang-debug-tidy
cmake --build --preset clang-debug-tidy

# clang-format
cmake --build --preset format-linux
```
