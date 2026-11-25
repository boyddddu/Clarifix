# Install muna WSL kasi PUTANGINA WINDOWS

## 1. Install WSL and a Linux Distribution

1. **Open PowerShell as Administrator:**
   Click **Start**, search for **PowerShell** (or **Terminal**), right-click it, and choose **Run as administrator**.

2. **Run the install command:**

   ```powershell
   wsl --install
   ```

   > **Tip:** If WSL is already installed and you want to choose a specific distribution, list available distributions with `wsl --list --online` and install one using `wsl --install -d <DistroName>`. Add `--web-download` if a download stalls.

3. **Restart your PC:**
   Reboot Windows when prompted so the required virtualization and WSL components finish enabling.

## 2. First-Time Linux User & Package Setup

1. **Install Ubuntu:**
   If Ubuntu was not automatically installed during step 1, open PowerShell and run:

   ```
   wsl --install -d Ubuntu
   
   ```
2. **Launch your Linux distro:**
   run `wsl` in PowerShell.

3. **Create a UNIX username and password:**
   Enter a username and password when prompted (nothing shows on screen while typing the password). This becomes your default non-root account with `sudo` privileges.

4. **Verify WSL 2:**
   In PowerShell, verify that your distribution is running on WSL version `2`:

   ```
   wsl --list --verbose
   
   ```

5. **Update Linux packages & install GCC:**
   Inside your Linux terminal, update your package list, upgrade installed packages, and install `build-essential` (which includes `gcc`):

   ```
   sudo apt update && sudo apt upgrade && sudo apt install build-essential
   
   ```

## 3. Compile and Run `main.c` in WSL

1. **Enter WSL:**
   Open your terminal in your project folder and start WSL:

   ```powershell
   wsl
   ```
   
   Navigate to src
   ```bash
   cd src
   ```
   

3. **Compile `main.c`:**
   
   ```bash
   gcc main.c -o main
   ```

4. **Run the compiled program:**
   ```bash
   ./main

   "filename.clx"
   ```
