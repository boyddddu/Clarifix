# ClariFix

ClariFix is a C-based **lexical and syntax analyzer** for `.clx` source files. It tokenizes ClariFix programs, validates their grammar with a recursive-descent parser, reports lexical and syntax errors, and saves formatted analysis reports to an output folder.

> **Why WSL?** The analyzer uses POSIX APIs (`mkdir` from `<sys/stat.h>`). On Windows, compile and run it inside **WSL (Windows Subsystem for Linux)**. On Linux or macOS, you can compile and run it natively with `gcc` and skip directly to [Compile and Run](#3-compile-and-run).

---

## Project Structure

```text
Clarifix/
├── README.md
└── src/
    ├── main.c                     # Lexical analyzer, CLI prompt, and report writer
    ├── parser.c                   # Recursive-descent syntax analyzer
    ├── parser.h                   # Shared parser declarations
    ├── test.clx                   # Scratchpad file for quick testing
    ├── examples/                  # Sample .clx programs
    ├── criteria-tests(Parser)/    # Parser criteria test cases
    └── regressionTests/           # Regression test cases and expected/actual outputs
```

---

## 1. Install WSL (Windows Only)

If you already have WSL 2 and `gcc` installed, skip to [Step 3: Compile and Run](#3-compile-and-run).

1. **Open PowerShell as Administrator:**
   Click **Start**, search for **PowerShell** (or **Terminal**), right-click it, and select **Run as administrator**.

2. **Install WSL:**
   ```powershell
   wsl --install
   ```

   > **Tip:** To pick a specific distribution, list available distros with `wsl --list --online` and install one with `wsl --install -d <DistroName>`. Add `--web-download` if the download stalls.

3. **Restart your PC:**
   Reboot Windows when prompted so virtualization and WSL components finish installing.

---

## 2. First-Time Linux User & Compiler Setup

1. **Install Ubuntu (if not installed automatically):**
   Open PowerShell and run:
   ```powershell
   wsl --install -d Ubuntu
   ```

2. **Launch WSL:**
   ```powershell
   wsl
   ```

3. **Create a UNIX username and password:**
   Enter a username and password when prompted (characters are hidden while typing the password). This becomes your default non-root account with `sudo` privileges.

4. **Verify WSL 2 (optional, in PowerShell):**
   ```powershell
   wsl --list --verbose
   ```
   Confirm your distribution is listed under `VERSION` `2`.

5. **Update packages and install GCC (inside WSL):**
   ```bash
   sudo apt update && sudo apt upgrade -y && sudo apt install -y build-essential
   ```

---

## 3. Compile and Run

### 1. Get the Repository

Clone the repository inside WSL (or enable long paths on Windows):

```bash
git clone https://github.com/boyddddu/Clarifix.git
```

> **Windows Git note:** If cloning from Windows PowerShell or CMD gives a `Filename too long` error, run:
> ```powershell
> git clone -c core.longpaths=true https://github.com/boyddddu/Clarifix.git
> ```

### 2. Navigate to `src/` in WSL

- **If you opened Terminal in the project folder:**
  ```powershell
  wsl
  ```
  Then inside WSL:
  ```bash
  cd src
  ```

- **If you opened WSL directly (starting at `~`):**
  Windows drives are mounted under `/mnt/` using **forward slashes (`/`)**, not Windows backslashes (`\`). For example, if your project is in `C:\PPL\Clarifix-main`:
  ```bash
  cd /mnt/c/PPL/Clarifix-main/src
  ```

### 3. Compile and Run

```bash
gcc main.c parser.c -o main && ./main
```

After compiling once, you can run `./main` directly without recompiling unless you edit `main.c`, `parser.c`, or `parser.h`.

### 4. Enter a `.clx` Filename

When prompted:

```text
Enter ClariFix source filename:
```

Type the path to a `.clx` file **relative to `src/`**, **without quotes** and **without spaces**:

| Location | What to type |
| --- | --- |
| Directly in `src/` | `test.clx` |
| Inside `examples/` | `examples/try.clx` |
| Inside `criteria-tests(Parser)/` | `criteria-tests(Parser)/05-input.clx` |
| Inside `regressionTests/testCases/` | `regressionTests/testCases/P01.clx` |

> **Tip:** For quick testing, you can paste any sample code into `src/test.clx` so you only have to type `test.clx` at the prompt. Press `Ctrl + C` to exit.

---

## 4. Output Reports

Each run automatically creates (or updates) an output folder named `<source-name>Output` alongside the input `.clx` file, containing two reports:

- **`<filename>_tokenTable.txt`** — Lexical errors and formatted token table.
- **`parserOutput.txt`** — Syntax analysis trace, syntax errors, and summary.

**Example:**
- Running `test.clx` creates:
  - `src/testOutput/test_tokenTable.txt`
  - `src/testOutput/parserOutput.txt`
- Running `criteria-tests(Parser)/05-input.clx` creates:
  - `src/criteria-tests(Parser)/05-inputOutput/05-input_tokenTable.txt`
  - `src/criteria-tests(Parser)/05-inputOutput/parserOutput.txt`

---

## Troubleshooting

| Issue | Cause & Fix |
| --- | --- |
| `-bash: cd: C:... No such file or directory` | You used a Windows path (`C:\...`) inside WSL. Use `/mnt/c/...` with forward slashes instead. |
| `ERROR - cannot open file. Please try again.` | The file isn't at that path relative to `src/`. Include any subfolder (e.g., `criteria-tests(Parser)/05-input.clx`) and check exact spelling and capitalization. |
| `Error E100: Invalid file extension. Use .clx files only.` | Make sure the filename ends with `.clx` and **do not** wrap it in quotes (`"..."`). |
| `Filename too long` when cloning on Windows | Re-clone with `git clone -c core.longpaths=true https://github.com/boyddddu/Clarifix.git` or clone inside WSL. |
