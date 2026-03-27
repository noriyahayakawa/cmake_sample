
# CMake + clang-cl / MSVC サンプル

## 概要
このリポジトリは、Windows/Linux 両対応の C++ テンプレートです。

- CMakePresets.json によるビルド設定の統一
- clang-cl / MSVC(cl) / clang の切り替え
- clang-tidy / clang-format 統合
- CMake install による成果物配置
- Boost / GoogleTest を vcpkg (manifest mode) で管理
- C++23 ベース

## ディレクトリ構成
```text
.
├── CMakeLists.txt
├── CMakePresets.json
├── etc/
│   └── app.jsonc
├── vcpkg.json
├── libs/
│   └── core/
│       ├── include/
│       └── src/
├── src/
│   └── main.cpp
├── tests/
│   ├── CMakeLists.txt
│   └── src/
│       └── test_main.cpp
├── .vscode/
│   ├── tasks.json
│   └── launch.json
└── vcpkg/
```

## 前提

### Windows
| ツール | バージョン | 備考 | winget コマンド |
|--------|-----------|------|-----------------|
| Windows | 11 | | (OS のため対象外) |
| Visual Studio 2022 Build Tools | Build Tools | MSVC v143 | `winget install --id Microsoft.VisualStudio.2022.BuildTools -e --override "--wait --quiet --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended"` |
| LLVM | 最新推奨 | clang-cl, clang-tidy, clang-format | `winget install --id LLVM.LLVM -e` |
| CMake | 3.23+ | | `winget install --id Kitware.CMake -e` |
| Ninja | 最新推奨 | | `winget install --id Ninja-build.Ninja -e` |

一括インストール:
```powershell
winget install --id Microsoft.VisualStudio.2022.BuildTools -e --override "--wait --quiet --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended"
winget install --id LLVM.LLVM -e
winget install --id Kitware.CMake -e
winget install --id Ninja-build.Ninja -e
```

注意:
- msvc-* プリセットは cl.exe を使用します。
- VS Code タスクでは tools/cmake-msvc-x64.cmd 経由で MSVC 環境を設定します。
- Visual Studio Build Tools は無償でインストールできますが、利用条件は Visual Studio ライセンス条項に従います。
- Build Tools の商用利用可否は組織形態・利用形態・契約条件で変わる場合があるため、導入前に最新の Visual Studio ライセンスを確認してください。
- Visual Studio Professional / Enterprise なども、適切なライセンスがある環境では利用できます。
- Visual Studio Community を利用する場合は、組織規模や利用目的によってライセンス制限が適用される可能性があります。
- 商用利用時は Visual Studio の最新ライセンス条項を必ず確認してください（本 README は法的助言ではありません）。

### Linux
| ツール | バージョン | 備考 | apt コマンド |
|--------|-----------|------|-------------|
| Ubuntu 22.04+ など | | | (OS のため対象外) |
| LLVM / Clang | 最新推奨 | clang, clang++, clang-tidy, clang-format | `sudo apt install -y clang clang-tidy clang-format` |
| CMake | 3.23+ | | `sudo apt install -y cmake` |
| Ninja | 最新推奨 | | `sudo apt install -y ninja-build` |
| curl, zip, unzip, tar, pkg-config | | vcpkg 依存 | `sudo apt install -y curl zip unzip tar pkg-config` |

一括インストール:
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

### core サブディレクトリ単体ビルド

`libs/core/CMakePresets.json` を追加しているため、`libs/core` へ移動して
サブディレクトリ単体で構成・ビルドできます。

#### Windows (clang-cl, static Debug)
```powershell
cd libs/core
cmake --preset clangcl-debug-static
cmake --build --preset clangcl-debug-static --clean-first
```

#### Windows (MSVC, static Debug)
```powershell
cd libs/core
../../tools/cmake-msvc-x64.cmd --preset msvc-debug-static
../../tools/cmake-msvc-x64.cmd --build --preset msvc-debug-static --clean-first
```

#### Linux (clang, Debug)
```bash
cd libs/core
cmake --preset clang-debug
cmake --build --preset clang-debug --clean-first
```

補足:
- core 単体プリセットのビルド出力先は `libs/core/out/build/<preset名>/`。
- core 単体プリセットは tests ターゲットを含みません。

### Windows (clang-cl, static Debug)
```powershell
# 1) 構成
cmake --preset clangcl-debug-static

# 2) リビルド（clean-first）
cmake --build --preset clangcl-debug-static --clean-first

# 3) テスト
ctest --preset clangcl-debug-static

# 4) インストール（任意）
cmake --install out/build/clangcl-debug-static --prefix out/install/clangcl-debug-static
```

### Windows (MSVC, static Debug)
```powershell
# 1) 構成
tools/cmake-msvc-x64.cmd --preset msvc-debug-static

# 2) リビルド（clean-first）
tools/cmake-msvc-x64.cmd --build --preset msvc-debug-static --clean-first

# 3) テスト
ctest --preset msvc-debug-static

# 4) インストール（任意）
cmake --install out/build/msvc-debug-static --prefix out/install/msvc-debug-static
```

### Linux (clang, Debug)
```bash
# 1) 構成
cmake --preset clang-debug

# 2) リビルド（clean-first）
cmake --build --preset clang-debug --clean-first

# 3) テスト
ctest --preset clang-debug

# 4) インストール（任意）
cmake --install out/build/clang-debug --prefix out/install/clang-debug
```

### VS Code タスクを使う場合

1. CMake: 構成 (<preset名>) を実行
2. CMake: リビルド (<preset名>) を実行（または CMake: ビルド）
3. CTest: テスト (<preset名>) を実行（対応プリセットのみ）
4. CMake: インストール (<preset名>) を実行（必要な場合）

補足:
- clang-cl / MSVC の preset では、ENABLE_AUTO_CLANG_FORMAT が ON のため、ビルド前に clang-format が自動実行されます。
- clean のみ実行したい場合は CMake: クリーン (<preset名>) を使用してください。
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

## ビルドとテスト

### clang-cl (Windows, Debug static)
```powershell
cmake --preset clangcl-debug-static
cmake --build --preset clangcl-debug-static --clean-first
ctest --preset clangcl-debug-static
```

### clang-cl (Windows, Release static)
```powershell
cmake --preset clangcl-release-static
cmake --build --preset clangcl-release-static --clean-first
ctest --preset clangcl-release-static
```

### MSVC (Windows, Debug static)
```powershell
tools/cmake-msvc-x64.cmd --preset msvc-debug-static
tools/cmake-msvc-x64.cmd --build --preset msvc-debug-static --clean-first
ctest --preset msvc-debug-static
```

### clang (Linux, Debug)
```bash
cmake --preset clang-debug
cmake --build --preset clang-debug --clean-first
ctest --preset clang-debug
```

成果物は out/build/<preset名>/ に出力されます。

## CMake install

このプロジェクトは `install()` に対応しており、`app` と `core`（ライブラリ・公開ヘッダ）を配置できます。

- インストール先を明示する例

```powershell
cmake --install out/build/clangcl-debug-static --prefix out/install/clangcl-debug-static
```

- 既定インストール先

`CMAKE_INSTALL_PREFIX` が明示されていない場合、`${CMAKE_BINARY_DIR}/install` を既定値として使用します。

- 代表的な配置先

    - 実行ファイル: `bin/`
    - ライブラリ: `lib/`
    - ヘッダ: `include/`
    - 設定ファイル: `etc/`（プロジェクトルートの `etc/` をインストール）

- 実行時ディレクトリへの設定ファイルコピー

`etc/` ディレクトリが存在する場合、`app` のビルド後に `copy_directory` で
`out/build/<preset名>/` 配下（`app.exe` と同じ場所）へコピーされます。

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
- クリーン: 各 build preset に対応する clean タスクを網羅
- リビルド: `クリーン -> ビルド` を順次実行するタスクを網羅
- テスト: CMakePresets.json の testPresets（hidden を除く）を網羅
- インストール: 各 build preset に対応する install タスクを網羅
- 補助: フォーマット (format / format-linux)

例:
- CMake: 構成 (clangcl-debug-static)
- CMake: リビルド (clangcl-debug-static)
- CMake: ビルド (msvc-release-dll)
- CMake: クリーン (clang-release)
- CTest: テスト (clangcl-release-static)
- CMake: インストール (clangcl-debug-static)

## VS Code デバッグ

.vscode/launch.json には以下のデバッグ構成が含まれます。

- Debug app (clangcl-debug-static)
- Debug tests (clangcl-debug-static)
- Debug app (msvc-debug-static)
- Debug tests (msvc-debug-static)

補足:
- app のデバッグ構成は `-i app.jsonc` を既定引数として渡します。
- すべてのデバッグ構成は `preLaunchTask` として `CMake: リビルド (...)` を実行してから起動されます。

これらは対応する `preLaunchTask` を実行してから起動されます。
