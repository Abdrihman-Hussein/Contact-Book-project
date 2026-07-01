# C++ Contact Book

A simple console-based contact manager written in C++17.

## Features

1. **Add contact** — name / phone / email / address (with empty-name validation)
2. **Search contact** — case-insensitive substring match on name
3. **Delete contact** — list-then-pick-by-index flow, with bounds checking
4. **Save contacts** — writes to `contacts.txt` (pipe-delimited, with escaping)
5. **Display all contacts**
6. **Exit** — auto-saves any unsaved changes on shutdown

- Auto-loads saved data on startup

## Files

- `main.cpp` — the full C++ source (single file, zero external dependencies)
- `contact_book` — pre-compiled Linux binary (rebuild on your platform)
- `README.md` — this file

## Build & Run

### Requirements

- A C++17 compiler: `g++`, `clang++`, or MSVC
- (Optional) VS Code with the **C/C++** extension by Microsoft

### Build from terminal

```
g++ -std=c++17 -O2 -Wall -o contact_book main.cpp
```

On Windows the output will be `contact_book.exe`. Use that name when running.

### Run

```
./contact_book          # macOS / Linux
.\contact_book.exe      # Windows PowerShell
contact_book.exe        # Windows CMD
```

The program creates `contacts.txt` in the current folder on first save — that is the persistence file. Delete it to start fresh.

## Build & Run in VS Code

1. Install a C++ compiler:
   - **Windows**: install [MinGW-w64](https://www.mingw-w64.org/) or [MSYS2](https://www.msys2.org/), ensure `g++` is on PATH
   - **macOS**: `xcode-select --install`
   - **Linux**: `sudo apt install build-essential`
2. Install the **C/C++** extension by Microsoft in VS Code.
3. Open this folder in VS Code (`File → Open Folder`).
4. Open the integrated terminal (`` Ctrl+` `` or `` Cmd+` ``) and run the build command above.
5. (Optional) Press `Ctrl+Shift+B` → "Configure Build Task" → "C/C++: g++ build active file" for one-click builds.
6. (Optional) Press `F5` to debug.

## Project Layout

```
contact_book/
├── main.cpp          # source code
├── contact_book      # prebuilt binary (Linux)
└── README.md         # this file
```

## Data File Format

`contacts.txt` uses pipe-delimited fields, one record per line:

```
Name|Phone|Email|Address
```

Pipes (`|`) and newlines inside fields are escaped as `\p` and `\n` so the format stays one-record-per-line.
