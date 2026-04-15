
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
│   ├── comm/
│   └── core/
├── src/
│   └── main.cpp
├── tests/
│   ├── CMakeLists.txt
│   └── test_main.cpp
├── .vscode/
│   ├── launch.json
│   └── tasks.json
└── vcpkg/
```

## 前提

### Windows
| ツール | バージョン | 備考 | winget コマンド |
|--------|-----------|------|-----------------|
| Windows | 11 | | (OS のため対象外) |
| Visual Studio 2022 | Community / Build Tools | MSVC v143 | `winget install --id Microsoft.VisualStudio.2022.BuildTools -e --override "--wait --quiet --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended"` |
| LLVM | 最新推奨 | clang-cl, clang-tidy, clang-format | `winget install --id LLVM.LLVM -e` |
| CMake | 3.23+ | | `winget install --id Kitware.CMake -e` |
| Ninja | 最新推奨 | | `winget install --id Ninja-build.Ninja -e` |
| Graphviz | 最新推奨 | Doxygen の依存グラフ生成に使用 | `winget install --id Graphviz.Graphviz -e` |

注意:
- msvc-* プリセットは cl.exe を使用します。
- VS Code タスクでは tools/cmake-msvc-x64.cmd 経由で MSVC 環境を設定します。

環境変数 Path に以下が含まれていることを確認してください。

| パス例 | 対象ツール |
|--------|----------|
| `C:\Program Files\LLVM\bin` | clang-cl, clang-tidy, clang-format |
| `C:\Program Files\CMake\bin` | cmake |
| `C:\Program Files\Ninja` | ninja |
| `C:\Program Files\Graphviz\bin` | dot (Doxygen グラフ生成) |

winget でインストールした場合は自動的に Path に追加されますが、反映には**シェルの再起動**が必要です。

### Linux / WSL2 (Ubuntu 22.04+)
| ツール | バージョン | 備考 |
|--------|-----------|------|
| Ubuntu 22.04+ | | WSL2 (Ubuntu-24.04) でも動作 |
| LLVM / Clang | 最新推奨 | clang, clang++, clang-tidy, clang-format |
| CMake | 3.23+ | |
| Ninja | 最新推奨 | |
| curl, zip, unzip, tar | | vcpkg 依存 |
| autoconf, autoconf-archive, automake, libtool | | vcpkg が libbacktrace をビルドする際に必要 |
| python3 | 3.x | vcpkg 内部処理で使用 |
| Graphviz | 最新推奨 | Doxygen の依存グラフ生成に使用 |

```bash
sudo apt install -y clang clang-tidy clang-format cmake ninja-build \
                    curl zip unzip tar pkg-config \
                    autoconf autoconf-archive automake libtool python3 graphviz
```

apt でインストールした場合は `/usr/bin` に配置され、通常 PATH への追加は不要です。  
カスタムビルドの LLVM を使用する場合は、以下を `~/.bashrc` 等に追加してください。

```bash
export PATH="/usr/lib/llvm-<version>/bin:$PATH"
```

#### WSL2 固有の注意
WSL2 カーネルが `CONFIG_HZ=100` でビルドされている場合（Microsoft 公式カーネルを含む）、  
ファイルシステムの mtime 解像度が 10ms 程度になり、vcpkg ライブラリのビルド中に  
`ninja: manifest 'build.ninja' still dirty after 100 tries` エラーが発生することがあります。  
本プロジェクトの `vcpkg/triplets/x64-linux.cmake` に以下の workaround が適用済みです。

```cmake
# WSL2 CONFIG_HZ=100 workaround
set(VCPKG_CMAKE_CONFIGURE_OPTIONS "-DCMAKE_SUPPRESS_REGENERATION=ON")
```

## セットアップ
```powershell
# 既に submodule 登録済みの場合
git submodule update --init --recursive

# Windows
./vcpkg/bootstrap-vcpkg.bat

# Linux / WSL2
./vcpkg/bootstrap-vcpkg.sh
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

# 4) インストール
cmake --install out/build/clangcl-debug-static
```

### Windows (MSVC, static Debug)
```powershell
# 1) 構成
tools/cmake-msvc-x64.cmd --preset msvc-debug-static

# 2) リビルド（clean-first）
tools/cmake-msvc-x64.cmd --build --preset msvc-debug-static --clean-first

# 3) テスト
ctest --preset msvc-debug-static

# 4) インストール
cmake --install out/build/msvc-debug-static
```

### Linux (clang, Debug static)
```bash
# 1) 構成
cmake --preset clang-debug-static

# 2) リビルド（clean-first）
cmake --build --preset clang-debug-static --clean-first

# 3) テスト
ctest --preset clang-debug-static

# 4) インストール
cmake --install out/build/clang-debug-static
```

### VS Code タスクを使う場合

1. CMake: 構成 (...) を実行
2. CMake: リビルド (...) を実行
3. CTest: テスト (...) を実行
4. CMake: インストール (...) を実行

補足:
- clang-cl / MSVC の preset では、ENABLE_AUTO_CLANG_FORMAT が ON のため、ビルド前に clang-format が自動実行されます。
- clean のみ必要な場合は CMake: クリーン (...) タスクを使用してください。

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
- clang-debug-static
- clang-debug-shared
- clang-release-static
- clang-release-shared

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
- clang-debug-static
- clang-debug-shared
- clang-release-static
- clang-release-shared
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

### clang (Linux, Debug static)
```bash
cmake --preset clang-debug-static
cmake --build --preset clang-debug-static
ctest --preset clang-debug-static
```

### clang (Linux, Debug shared)
```bash
cmake --preset clang-debug-shared
cmake --build --preset clang-debug-shared
ctest --preset clang-debug-shared
```

### clang (Linux, Release static)
```bash
cmake --preset clang-release-static
cmake --build --preset clang-release-static
ctest --preset clang-release-static
```

ビルド成果物は `out/build/<preset名>/`、インストール成果物は `out/install/<preset名>/` に出力されます。

## clang-format 自動実行

- CMake オプション ENABLE_AUTO_CLANG_FORMAT が ON のとき、app/tests ビルド前に format ターゲットを実行します。
- format ターゲットは format-app + core-format + tests-format の集約です。
- clang-format が見つからない場合は警告を出してスキップします。
- CMAKE_EXPORT_COMPILE_COMMANDS が ON のとき、ビルド時に root の compile_commands.json にも同期します。

明示的に無効化する例:
```powershell
cmake --preset clangcl-debug-static -DENABLE_AUTO_CLANG_FORMAT=OFF
```

## VS Code タスク

.vscode/tasks.json には以下が定義されています。

- 構成: clang-cl / MSVC (Debug/Release × static/dll)
- ビルド: clang-cl / MSVC (Debug/Release × static/dll)
- クリーン: clang-cl / MSVC (Debug/Release × static/dll)
- リビルド: clang-cl / MSVC (Debug/Release × static/dll)
- インストール: clang-cl / MSVC (Debug/Release × static/dll)
- テスト: CTest (clang-cl / MSVC × Debug/Release × static/dll)
- 補助: 静的解析 (clang-tidy), フォーマット
- Linux / WSL2: clang (Debug/Release × static/shared) の構成・ビルド・クリーン・リビルド・インストール・テスト

例:
- CMake: リビルド (clang-cl Debug static)
- CMake: ビルド (clang-cl Debug dll)
- CMake: クリーン (MSVC Release dll)
- CMake: インストール (clang-cl Debug static)
- CTest: テスト (clang-cl Debug static)
- clang-tidy (project sources)
- clang-tidy (project sources + analyzer)

clang-tidy タスクの使い分け:
- clang-tidy (project sources): src と libs 配下の自前ソースだけを対象に、clang-analyzer を外した軽量チェックを -fix 付きで実行します。
- clang-tidy (project sources + analyzer): 同じ対象範囲に対して clang-analyzer 系チェックだけを実行します。軽量タスクと役割を分け、依存ヘッダ由来のノイズを抑えます。

PowerShell セッションの中で手動実行する場合の例:
```powershell
$checks = '-*,clang-analyzer-*'
$headerFilter = '^(src|libs/(comm|core)/(src|include))/'
Get-ChildItem -Recurse -Include *.cpp,*.cxx,*.cc,*.c -Path src,libs |
    Where-Object { !$_.PSIsContainer -and ($_.FullName -notmatch 'tests') } |
    ForEach-Object {
        clang-tidy $_.FullName -p out/build/clangcl-debug-static --checks=$checks --header-filter=$headerFilter --quiet
    }
```

PowerShell の外から 1 コマンドで呼ぶ場合の例:
```powershell
pwsh -Command '$checks = ''-*,clang-analyzer-*''; $headerFilter = ''^(src|libs/(comm|core)/(src|include))/''; Get-ChildItem -Recurse -Include *.cpp,*.cxx,*.cc,*.c -Path src,libs | Where-Object { !$_.PSIsContainer -and ($_.FullName -notmatch ''tests'') } | ForEach-Object { clang-tidy $_.FullName -p out/build/clangcl-debug-static --checks=$checks --header-filter=$headerFilter --quiet }'
```
