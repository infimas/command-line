# ⚡ MyShell - Cyberpunk Terminal Interface

> **"Welcome Commander Irfan."**

**MyShell** is a custom C++ Linux shell designed for those who want the power of a terminal with the aesthetic of a sci-fi hacker movie. It combines robust shell functionality (pipes, redirection, history) with stunning visual effects like Matrix rain, hex dumps, and decryption animations.

---

## 🌟 Features

### 🎨 Visual Effects (The "Cool" Stuff)
* **Boot Sequence:** Starts with a high-speed scrolling Hex Dump memory check.
* **Security Protocol:** Password-protected entry system.
* **Decryption Animation:** Welcome messages decode character-by-character.
* **HUD Mode:** Live "Matrix Rain" falls in the header (top 10 lines) while keeping your typing area clean.
* **Custom Prompt:** `root@irfanOS path ⚡ λ` with dynamic coloring.

### 🛠 Core Functionality
* **Command Execution:** Runs standard Linux commands (`ls`, `grep`, `vim`, etc.).
* **Piping (`|`):** Chain commands together (e.g., `ls | grep cpp`).
* **Redirection:** Support for input (`<`) and output (`>`) redirection.
* **Background Processes (`&`):** Run tasks in the background.
* **History:**
    * `history`: View command log.
    * `!!`: Repeat last command.
    * `!n`: Repeat command number *n*.
* **Navigation:** Built-in `cd` support.
* **Signal Handling:** Ctrl+C (SIGINT) is caught and handled gracefully.

---

## 🚀 Installation & Usage

### 1. Compile
You can compile the shell using `g++`. Make sure all files (`main.cpp`, `Shell.cpp`, `Shell.h`) are in the same directory.

```bash
g++ main.cpp Shell.cpp -o myshell
