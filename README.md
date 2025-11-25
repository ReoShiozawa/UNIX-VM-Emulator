# UNIX VM Emulator - Universal C Implementation

A comprehensive, feature-rich C-based virtual machine emulator suite for UNIX-like systems (Linux, macOS, BSD), featuring multiple CPU architectures and execution modes:

1. **8-bit RISC VM** - Simple 64KB educational emulator
2. **64-bit x86-64 VM** - Extended Linux-compatible emulator with syscalls

This project demonstrates both simple VM design and advanced features like Linux syscall emulation.

## Quick Start

```bash
make                    # Build launcher, 8-bit & x86-64 emulators
./bin/launcher          # Run interactive launcher menu
# OR run individual emulators:
./bin/emulator          # 8-bit RISC VM with built-in demo
./bin/vm64              # x86-64 emulator (interactive mode)
./ubuntu-quick.sh       # Launch Ubuntu with QEMU (recommended)
```

## Features

### 8-bit RISC VM (bin/emulator)
- **64 KiB RAM** with true memory management
- **8 x 64-bit registers** (R0-R7)
- **28 operations** (arithmetic, bitwise, memory, I/O, control flow)
- Interactive CLI with step-by-step debugging
- Built-in demo program
- Image generator tool

### 64-bit x86-64 VM (bin/vm64)
- **16 MB RAM** (scalable)
- **16 registers** (RAX-R15 compatible)
- **Linux syscall interface** (write, read, open, close, exit, mmap, brk)
- Extended instruction set for x86-64
- Support for loading and executing binary kernels
- Memory protection and error handling

## Build & Run

### Prerequisites
```bash
# macOS
brew install gcc

# For GUI (optional)
brew install sdl2

# For Ubuntu ISO support (optional, QEMU wrapper)
brew install qemu

# Linux (Debian/Ubuntu)
sudo apt install build-essential libsdl2-dev

# Linux (Fedora)
sudo dnf install gcc SDL2-devel
```

### Building

```bash
# Build launcher (recommended - includes all emulators)
make launcher

# Build specific targets
make cli                # 8-bit RISC VM only
make vm64               # x86-64 VM only
make gui                # GUI emulator (requires SDL2)
make imggen             # Image generator tool

# Build everything
make all cli gui imggen vm64 launcher

# Clean build artifacts
make clean

# Show help
make help
```

### Running the Launcher

The interactive launcher provides an easy way to access all emulators:

```bash
./bin/launcher

# Menu options:
# 1) 8-bit RISC VM
# 2) 8-bit RISC VM + GUI
# 3) x86-64 VM with syscalls
# 4) Image Generator
# 5) Download Ubuntu ISO
# 6) Launch Ubuntu
# 0) Exit
```

### Direct Execution

**8-bit RISC VM:**
```bash
./bin/emulator              # Run built-in demo
./bin/emulator image.bin    # Run custom image
```

**x86-64 VM:**
```bash
./bin/vm64                  # Interactive mode
./bin/vm64 kernel.bin       # Load and run kernel
```

**Interactive Mode:**
```bash
./bin/emulator
vm> help
vm> load demo.bin
vm> dump
vm> step 5
vm> run
```

**GUI Mode:**
```bash
./bin/emulator-gui          # Built-in demo
./bin/emulator-gui image.bin
```

**Ubuntu ISO (via QEMU):**
```bash
# Step 1: Install QEMU (if not already installed)
brew install qemu

# Step 2: Download Ubuntu ISO
./download-ubuntu.sh
# This will download Ubuntu 24.04.3 desktop to ./isos/

# Step 3a: Quick Launch (Recommended for macOS)
./ubuntu-quick.sh

# Step 3b: Or use the interactive launcher
./bin/launcher
# Then select option 5: Ubuntu with QEMU

# Step 3c: Or launch with custom ISO path
./ubuntu-quick.sh /path/to/ubuntu.iso
```

**Generate and Run Images:**
```bash
make imggen
./bin/imggen images/hello.bin hello
./bin/emulator images/hello.bin
```

## Instruction Set

| Opcode | Encoding | Meaning |
|--------|----------|---------|
| `0x00` | `HALT` | Stop execution |
| `0x10` | `MOVI reg, imm32` | Load 32-bit immediate into register |
| `0x20` | `ADD dst, src` | dst += src |
| `0x21` | `SUB dst, src` | dst -= src |
| `0x22` | `MUL dst, src` | dst *= src |
| `0x23` | `DIV dst, src` | dst /= src |
| `0x24` | `MOD dst, src` | dst %= src |
| `0x25` | `AND dst, src` | dst &= src |
| `0x26` | `OR dst, src` | dst \|= src |
| `0x27` | `XOR dst, src` | dst ^= src |
| `0x28` | `NOT dst` | dst = ~dst |
| `0x29` | `SHL dst, imm8` | dst <<= imm8 |
| `0x2A` | `SHR dst, imm8` | dst >>= imm8 |
| `0x30` | `LOAD dst, addr16` | Load byte from addr into dst |
| `0x31` | `STORE src, addr16` | Store low byte of src to addr |
| `0x40` | `OUT reg` | Print low 8 bits as ASCII |
| `0x41` | `IN reg` | Read character from stdin |
| `0x50` | `JMP addr16` | Unconditional jump |
| `0x51` | `JNZ reg, addr16` | Jump if non-zero |
| `0x52` | `JZ reg, addr16` | Jump if zero |
| `0x53` | `JLT reg, addr16` | Jump if negative |
| `0x54` | `JGT reg, addr16` | Jump if positive |
| `0x60` | `CMP dst, src` | Compare (dst != src) → dst |
| `0x70` | `CALL addr16` | Call subroutine |
| `0x71` | `RET` | Return from subroutine |
| `0x80` | `PUSH reg` | Push 64-bit value to stack |
| `0x81` | `POP reg` | Pop 64-bit value from stack |

## Creating Custom Programs

### Using Image Generator Tool

Build the image generator first:
```bash
make imggen
```

Generate sample programs:
```bash
./bin/imggen output.bin hello     # Hello World
./bin/imggen output.bin counter   # Count 0-9
./bin/imggen output.bin fibonacci # Fibonacci sequence
```

### Manual Binary Creation

You can create your own binary images using any hex editor or by writing raw bytes. The instruction format is defined in the "Instruction Set" section above.

Example with `xxd` (macOS/Linux):
```bash
# Create a simple "HI" program
echo "10 00 48 00 00 00 40 00 10 00 49 00 00 00 40 00 10 00 0A 00 00 00 40 00 00" | xxd -r -p > hi.bin
./bin/emulator hi.bin
```

## Architecture

```
src/
  vm.h          - 8-bit RISC VM interface
  vm.c          - 8-bit RISC VM implementation
  main.c        - 8-bit CLI interface
  gui.c         - 8-bit SDL2 GUI interface
  imggen.c      - Binary image generator
  
  vm64.h        - x86-64 VM interface (NEW)
  vm64.c        - x86-64 VM implementation with Linux syscalls (NEW)
  main64.c      - x86-64 CLI interface (NEW)

Makefile        - Build system (100% C-based)
```

## Example Programs

### Factorial (10!)
```asm
movi r0, 1      ; result = 1
movi r1, 10     ; n = 10
loop:
mul r0, r1      ; result *= n
sub r1, 1       ; n--
jnz r1, loop    ; loop while n != 0
out r0          ; print result
halt
```

### Input/Output Loop
```asm
loop:
in r0           ; read character
out r0          ; echo it
jnz r0, loop    ; continue if not EOF
halt
```

## About Running Ubuntu ISO

The Ubuntu ISO (ubuntu-24.04.3-desktop-amd64.iso) is a complete x86-64 operating system image that requires:
- Full x86-64 processor emulation
- Advanced I/O subsystem emulation  
- Device drivers and BIOS emulation
- Complex memory management

This is beyond the scope of our lightweight VM emulator. However, this project demonstrates:

1. **Basic kernel loading** - VM64 can load and execute small binary kernels
2. **Linux syscall interface** - Essential syscalls are emulated (write, read, exit, etc.)
3. **Memory management** - Dynamic memory allocation and protection

For full Ubuntu ISO support, use:
- **QEMU** - Full system emulator (recommended)
- **Bochs** - x86 emulator with debugging
- **Xen** - Hypervisor with paravirtualization

## QEMU Integration

This project includes QEMU launchers for full operating system support.

### Ubuntu 24.04 LTS

**Option 1: Via Launcher (Recommended)**
```bash
./bin/launcher
→ Option 5: Download Ubuntu ISO
→ Option 6: Launch Ubuntu
```

**Option 2: Direct Commands**
```bash
./download-ubuntu.sh    # Downloads 24.04.3 desktop edition (~4GB)
./ubuntu-quick.sh       # Launch Ubuntu
```

Features:
- Persistent virtual disk (~50GB allocation)
- Auto-memory detection (uses half of system RAM)
- Native macOS integration (cocoa display)
- Automatic process cleanup

## File Storage Locations

VM files are automatically stored in two directories within the project:

### ISO Images (`./isos/`)
- **Ubuntu Desktop ISO**: `./isos/ubuntu-24.04.3-desktop-amd64.iso` (~5.9GB)
  - Downloaded once via `./download-ubuntu.sh`
  - Persistent across sessions
  - Reused for all Ubuntu launches

### Virtual Machine Disks (`./qemu-images/`)
- **Ubuntu Disk**: `./qemu-images/ubuntu-disk.qcow2`
  - Created fresh each launch by default
  - QCOW2 sparse format (efficient disk usage)
  - Allocated: 50GB maximum
  - Current size: ~193KB (grows as Ubuntu is used)
  - To persist disk state between sessions, preserve this file

### Storage Summary
```
./isos/                              # ISO images
  └── ubuntu-24.04.3-desktop-amd64.iso    (5.9GB, downloaded once)

./qemu-images/                       # Virtual machine disks
  └── ubuntu-disk.qcow2                   (193KB → 50GB)

Total current usage: ~5.9GB
```

Both directories are created automatically when needed. Directories relative to project root.

## Linux Syscall Support in VM64

VM64 implements the following Linux x86-64 syscalls:
- `write(2)` - Write to file descriptor
- `read(2)` - Read from file descriptor
- `open(2)` - Open file
- `close(2)` - Close file descriptor
- `exit(2)` - Terminate process
- `mmap(2)` - Memory mapping (stubbed)
- `brk(2)` - Heap management (stubbed)

## Debugging

Enable debug mode to trace execution:
```bash
./bin/emulator
vm> debug on
vm> step 5
vm> dump
```

Add breakpoints:
```bash
vm> break 0x0100
vm> run
```

## Compatibility

- **macOS**: 10.13+
- **Linux**: GCC 5+, any modern distribution
- **BSD**: FreeBSD 12+, OpenBSD 6.5+

Requires only standard C library (POSIX for file I/O) and SDL2 for GUI.

Both emulators are self-contained and require no external dependencies beyond the C standard library.

## Limitations

- **8-bit VM**: Educational use, not production-ready
- **VM64**: Cannot run full operating systems like Ubuntu
  - For full OS support, use QEMU, VirtualBox, or KVM
  - VM64 demonstrates kernel loading and syscall emulation concepts
- No dynamic code generation or JIT compilation
- Single-threaded execution model

## License

This project is provided as-is for educational and development purposes.

## Related Projects

- **QEMU** - Full system emulator supporting x86-64, ARM, RISC-V, etc.
- **Bochs** - x86/x86-64 emulator with extensive debugging
- **TinyEMU** - Lightweight emulator for RISC-V and x86
- **SimpleOS** - Educational OS for emulators like this

## Summary

This project demonstrates two levels of virtual machine implementation:

1. **Simple RISC emulator** - Ideal for teaching VM basics and CPU simulation
2. **Extended x86-64 emulator** - Shows advanced features like syscall handling

Neither can run a full OS like Ubuntu. For that, use industrial-grade emulators listed above.

---

# UNIX VM エミュレーター - 日本語版

UNIX互換システム（Linux、macOS、BSD）向けの包括的で機能豊富なC言語ベースの仮想マシンエミュレータースイート。複数のCPUアーキテクチャと実行モードを搭載しています：

1. **8ビット RISC VM** - シンプルな64KBの教育用エミュレーター
2. **64ビット x86-64 VM** - Linuxに対応した拡張エミュレーター（syscall対応）

このプロジェクトは、シンプルなVM設計と、Linuxシステムコール エミュレーションなどの高度な機能を両方示しています。

## クイックスタート

```bash
make                    # ランチャー、8ビット＆x86-64エミュレーターをビルド
./bin/launcher          # インタラクティブランチャーメニュー実行
# または個別エミュレーターを実行：
./bin/emulator          # 8ビット RISC VM（内蔵デモ）
./bin/vm64              # x86-64エミュレーター（インタラクティブモード）
./ubuntu-quick.sh       # QEMUでUbuntuを起動（推奨）
```

## 機能

### 8ビット RISC VM (bin/emulator)
- **64 KiB RAM** - 真のメモリ管理
- **8個の64ビットレジスタ** (R0-R7)
- **28命令** （算術演算、ビット演算、メモリ、I/O、制御フロー）
- インタラクティブCLI（ステップバイステップデバッグ対応）
- 内蔵デモプログラム
- イメージジェネレータツール

### 64ビット x86-64 VM (bin/vm64)
- **16 MB RAM** （スケーラブル）
- **16レジスタ** (RAX-R15互換)
- **Linuxシステムコールインターフェース** (write、read、open、close、exit、mmap、brk)
- x86-64拡張命令セット
- バイナリカーネルのロードと実行に対応
- メモリ保護とエラー処理

## ビルド方法

### 前提条件
```bash
# macOS
brew install gcc

# GUI用（オプション）
brew install sdl2

# Ubuntu ISO サポート用（オプション、QEMUラッパー）
brew install qemu

# Linux (Debian/Ubuntu)
sudo apt install build-essential libsdl2-dev

# Linux (Fedora)
sudo dnf install gcc SDL2-devel
```

### ビルド

```bash
# ランチャーをビルド（推奨 - すべてのエミュレーターを含む）
make launcher

# 特定のターゲットをビルド
make cli                # 8ビット RISC VM のみ
make vm64               # x86-64 VM のみ
make gui                # GUIエミュレーター（SDL2必須）
make imggen             # イメージジェネレータツール

# すべてビルド
make all cli gui imggen vm64 launcher

# ビルド成果物をクリア
make clean

# ヘルプを表示
make help
```

### ランチャーを実行

インタラクティブランチャーで すべてのエミュレーターに簡単にアクセスできます：

```bash
./bin/launcher

# メニューオプション：
# 1) 8ビット RISC VM
# 2) 8ビット RISC VM + GUI
# 3) x86-64 VM with syscalls
# 4) イメージジェネレータ
# 5) Ubuntu ISO をダウンロード
# 6) Ubuntu を起動
# 0) 終了
```

### 直接実行

**8ビット RISC VM：**
```bash
./bin/emulator              # 内蔵デモを実行
./bin/emulator image.bin    # カスタムイメージを実行
```

**x86-64 VM：**
```bash
./bin/vm64                  # インタラクティブモード
./bin/vm64 kernel.bin       # カーネルをロードして実行
```

**インタラクティブモード：**
```bash
./bin/emulator
vm> help
vm> load demo.bin
vm> dump
vm> step 5
vm> run
```

**GUIモード：**
```bash
./bin/emulator-gui          # 内蔵デモ
./bin/emulator-gui image.bin
```

**Ubuntu ISO (QEMU経由)：**
```bash
# ステップ1: QEMUをインストール（未インストールの場合）
brew install qemu

# ステップ2: Ubuntu ISO をダウンロード
./download-ubuntu.sh
# これは Ubuntu 24.04.3 desktop を ./isos/ にダウンロードします

# ステップ3a: クイック起動（macOS推奨）
./ubuntu-quick.sh

# ステップ3b: またはインタラクティブランチャーを使用
./bin/launcher
# その後、オプション5を選択: Ubuntu with QEMU

# ステップ3c: またはカスタムISO パスで起動
./ubuntu-quick.sh /path/to/ubuntu.iso
```

**イメージを生成して実行：**
```bash
make imggen
./bin/imggen images/hello.bin hello
./bin/emulator images/hello.bin
```

## 命令セット

| オペコード | エンコーディング | 意味 |
|-----------|---------|------|
| `0x00` | `HALT` | 実行停止 |
| `0x10` | `MOVI reg, imm32` | 32ビット即値をレジスタに読み込む |
| `0x20` | `ADD dst, src` | dst += src |
| `0x21` | `SUB dst, src` | dst -= src |
| `0x22` | `MUL dst, src` | dst *= src |
| `0x23` | `DIV dst, src` | dst /= src |
| `0x24` | `MOD dst, src` | dst %= src |
| `0x25` | `AND dst, src` | dst &= src |
| `0x26` | `OR dst, src` | dst \|= src |
| `0x27` | `XOR dst, src` | dst ^= src |
| `0x28` | `NOT dst` | dst = ~dst |
| `0x29` | `SHL dst, imm8` | dst <<= imm8 |
| `0x2A` | `SHR dst, imm8` | dst >>= imm8 |
| `0x30` | `LOAD dst, addr16` | addrからdstにバイトをロード |
| `0x31` | `STORE src, addr16` | srcの下位バイトをaddrに格納 |
| `0x40` | `OUT reg` | 下位8ビットをASCII文字として出力 |
| `0x41` | `IN reg` | 標準入力から文字を読み込む |
| `0x50` | `JMP addr16` | 無条件ジャンプ |
| `0x51` | `JNZ reg, addr16` | 0でなければジャンプ |
| `0x52` | `JZ reg, addr16` | 0ならジャンプ |
| `0x53` | `JLT reg, addr16` | 負ならジャンプ |
| `0x54` | `JGT reg, addr16` | 正ならジャンプ |
| `0x60` | `CMP dst, src` | 比較 (dst != src) → dst |
| `0x70` | `CALL addr16` | サブルーチン呼び出し |
| `0x71` | `RET` | サブルーチンから戻る |
| `0x80` | `PUSH reg` | 64ビット値をスタックにプッシュ |
| `0x81` | `POP reg` | スタックから64ビット値をポップ |

## カスタムプログラムの作成

### イメージジェネレータツール使用

まずイメージジェネレータをビルドします：
```bash
make imggen
```

サンプルプログラムを生成：
```bash
./bin/imggen output.bin hello     # Hello World
./bin/imggen output.bin counter   # 0-9をカウント
./bin/imggen output.bin fibonacci # フィボナッチ数列
```

### 手動バイナリ作成

任意のヘックスエディタまたは生バイト書き込みで独自のバイナリイメージを作成できます。命令形式は上の「命令セット」セクションで定義されています。

`xxd`を使用した例（macOS/Linux）：
```bash
# シンプルな"HI"プログラムを作成
echo "10 00 48 00 00 00 40 00 10 00 49 00 00 00 40 00 10 00 0A 00 00 00 40 00 00" | xxd -r -p > hi.bin
./bin/emulator hi.bin
```

## アーキテクチャ

```
src/
  vm.h          - 8ビット RISC VM インターフェース
  vm.c          - 8ビット RISC VM 実装
  main.c        - 8ビット CLI インターフェース
  gui.c         - 8ビット SDL2 GUI インターフェース
  imggen.c      - バイナリイメージジェネレータ
  
  vm64.h        - x86-64 VM インターフェース
  vm64.c        - x86-64 VM 実装（Linuxシステムコール対応）
  main64.c      - x86-64 CLI インターフェース

Makefile        - ビルドシステム（100% C言語ベース）
```

## サンプルプログラム

### 階乗 (10!)
```asm
movi r0, 1      ; result = 1
movi r1, 10     ; n = 10
loop:
mul r0, r1      ; result *= n
sub r1, 1       ; n--
jnz r1, loop    ; n != 0 の間ループ
out r0          ; 結果を出力
halt
```

### 入出力ループ
```asm
loop:
in r0           ; 文字を読み込む
out r0          ; エコーバック
jnz r0, loop    ; EOFでない場合は続行
halt
```

## Ubuntu ISO 実行について

Ubuntu ISO (ubuntu-24.04.3-desktop-amd64.iso) は完全なx86-64オペレーティングシステムイメージで、以下が必要です：
- 完全なx86-64プロセッサエミュレーション
- 高度なI/Oサブシステムエミュレーション  
- デバイスドライバとBIOSエミュレーション
- 複雑なメモリ管理

これは軽量なVMエミュレーターの範囲を超えています。ただし、このプロジェクトは以下を実演します：

1. **基本的なカーネルロード** - VM64は小規模バイナリカーネルのロードと実行が可能
2. **Linuxシステムコールインターフェース** - 必須syscall（write、read、exit等）がエミュレートされている
3. **メモリ管理** - 動的メモリ割り当てと保護

Ubuntu ISO の完全サポートには、以下を使用してください：
- **QEMU** - フルシステムエミュレーター（推奨）
- **Bochs** - デバッグ機能付きx86エミュレーター
- **Xen** - 半仮想化ハイパーバイザー

## QEMU 統合

このプロジェクトは完全なOS サポート用のQEMUランチャーを含みます。

### Ubuntu 24.04 LTS

**オプション1: ランチャー経由（推奨）**
```bash
./bin/launcher
→ オプション5: Ubuntu ISO をダウンロード
→ オプション6: Ubuntu を起動
```

**オプション2: 直接コマンド**
```bash
./download-ubuntu.sh    # Ubuntu 24.04.3 desktop をダウンロード（約4GB）
./ubuntu-quick.sh       # Ubuntu を起動
```

機能：
- 永続的な仮想ディスク（約50GB割り当て）
- 自動メモリ検出（システムRAMの半分を使用）
- ネイティブmacOS統合（cocoaディスプレイ）
- 自動プロセスクリーンアップ

## ファイル保存場所

VMファイルはプロジェクト内の2つのディレクトリに自動保存されます：

### ISO イメージ (`./isos/`)
- **Ubuntu Desktop ISO**: `./isos/ubuntu-24.04.3-desktop-amd64.iso` (~5.9GB)
  - `./download-ubuntu.sh` で1回だけダウンロード
  - セッション間で永続的
  - すべてのUbuntu起動で再利用

### 仮想マシンディスク (`./qemu-images/`)
- **Ubuntu ディスク**: `./qemu-images/ubuntu-disk.qcow2`
  - デフォルトでは起動時に新規作成
  - QCOW2スパース形式（ディスク効率的）
  - 割り当て：最大50GB
  - 現在サイズ：~193KB（Ubuntuを使用すると拡大）
  - セッション間でディスク状態を保持するには、このファイルを保存

### ストレージサマリー
```
./isos/                              # ISO イメージ
  └── ubuntu-24.04.3-desktop-amd64.iso    (5.9GB、1回だけダウンロード)

./qemu-images/                       # 仮想マシンディスク
  └── ubuntu-disk.qcow2                   (193KB → 50GB)

現在の総使用量: ~5.9GB
```

両ディレクトリは必要に応じて自動作成されます。ディレクトリはプロジェクトルートに相対的です。

## VM64 での Linux システムコール サポート

VM64 は以下のLinux x86-64システムコールを実装しています：
- `write(2)` - ファイルディスクリプタに書き込む
- `read(2)` - ファイルディスクリプタから読み込む
- `open(2)` - ファイルを開く
- `close(2)` - ファイルディスクリプタをクローズ
- `exit(2)` - プロセスを終了
- `mmap(2)` - メモリマッピング（スタブ）
- `brk(2)` - ヒープ管理（スタブ）

## デバッグ

デバッグモードを有効にして実行をトレース：
```bash
./bin/emulator
vm> debug on
vm> step 5
vm> dump
```

ブレークポイントを追加：
```bash
vm> break 0x0100
vm> run
```

## 互換性

- **macOS**: 10.13+
- **Linux**: GCC 5+、任意の最新ディストリビューション
- **BSD**: FreeBSD 12+、OpenBSD 6.5+

C標準ライブラリ（ファイルI/O用POSIX）とGUI用SDL2のみが必要です。

両エミュレーターは自己完結しており、C標準ライブラリ以外の外部依存関係は不要です。

## 制限事項

- **8ビット VM**: 教育用、本番環境対応外
- **VM64**: Ubuntu などの完全なOSは実行不可
  - フルOS サポートには、QEMU、VirtualBox、KVMを使用
  - VM64はカーネルロードとシステムコールエミュレーションの概念を実演
- 動的コード生成またはJITコンパイルなし
- シングルスレッド実行モデル

## ライセンス

このプロジェクトはMITライセンスの下で提供されます。

```
MIT License

Copyright (c) 2025

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

## 関連プロジェクト

- **QEMU** - x86-64、ARM、RISC-Vなど複数アーキテクチャに対応したフルシステムエミュレーター
- **Bochs** - 広範なデバッグ機能を備えたx86/x86-64エミュレーター
- **TinyEMU** - RISC-VとX86用軽量エミュレーター
- **SimpleOS** - このようなエミュレーター向けの教育用OS

## 概要

このプロジェクトは2レベルの仮想マシン実装を実演しています：

1. **シンプル RISC エミュレーター** - VM基礎とCPU シミュレーション教育に最適
2. **拡張 x86-64 エミュレーター** - システムコール処理などの高度な機能を表示

どちらもUbuntu などの完全なOSを実行することはできません。その場合は上記のインダストリアルグレードエミュレーターを使用してください。
