#include "Shell.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <csignal>
#include <fcntl.h>
#include <chrono>
#include <iomanip>
#include <cstdlib>
#include <thread>
#include <termios.h>
#include <sys/ioctl.h>

using namespace std;

// --- Colors ---
#define RESET        "\033[0m"
#define BOLD_RED     "\033[1;31m"
#define BOLD_GREEN   "\033[1;32m"
#define BOLD_YELLOW  "\033[1;33m"
#define BOLD_CYAN    "\033[1;36m"
#define BOLD_MAGENTA "\033[1;35m"
#define BOLD_WHITE   "\033[1;37m"
#define FAINT_GREEN  "\033[2;32m"

// --- Terminal Raw Mode Handling ---
struct termios orig_termios;

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

// --- EFFECT 1: SCROLLING HEX DUMP (Boot Sequence) ---
void scrollingHexDump() {
    cout << "\033[2J\033[H"; // Clear screen
    cout << FAINT_GREEN;
    for (int i = 0; i < 150; i++) {
        cout << "0x" << hex << uppercase << (rand() % 0xFFFF) << "  ";
        if (rand() % 2 == 0) cout << "00 ";
        else cout << dec << (rand() % 0xFF) << " ";
        cout << ":: MEM_ALLOC_" << (rand() % 9999);

        if (i % 4 == 0) cout << "\n";
        else cout << "  ";

        cout.flush();
        usleep(3000);
    }
    cout << RESET << endl;
    usleep(100000);
}

// --- EFFECT 2: DECRYPTION TEXT ANIMATION ---
void decryptText(const string& targetText) {
    string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789@#$%^&*";
    string current(targetText.size(), ' ');

    for (size_t i = 0; i < targetText.length(); ++i) {
        for (int k = 0; k < 5; k++) {
            cout << "\r" << BOLD_GREEN << " [ACCESS] " << BOLD_WHITE;
            for (size_t j = 0; j < i; j++) cout << targetText[j];
            cout << chars[rand() % chars.length()] << flush;
            usleep(20000);
        }
        current[i] = targetText[i];
    }

    cout << "\r" << BOLD_GREEN << " [ACCESS] " << BOLD_WHITE
         << targetText << RESET << endl;
}

// --- COOL NAME BANNER ---
void printSignature() {
    cout << "\033[2J\033[1;1H";
    cout << BOLD_CYAN << R"(

  _____  _____  ______       _   _ 
 |_   _||  __ \|  ____|     | \ | |
   | |  | |__) | |__   __ _ |  \| |
   | |  |  _  /|  __| / _` || . ` |
  _| |_ | | \ \| |   | (_| || |\  |
 |_____||_|  \_\_|    \__,_||_| \_|
                                   
)" << RESET << endl;

    cout << BOLD_WHITE << "         << SYSTEM ARCHITECT >>" << RESET << endl << endl;
    usleep(800000);
}

// --- Dynamic Matrix Rain (separate command) ---
void dynamicMatrixRain(int width = 80, int height = 20, int duration_sec = 4) {
    srand(time(nullptr));
    vector<int> drops(width, 0);

    auto start = chrono::steady_clock::now();

    while (chrono::duration_cast<chrono::seconds>(
               chrono::steady_clock::now() - start).count() < duration_sec) {

        cout << "\033[2J\033[H";

        for (int row = 0; row < height; ++row) {
            for (int col = 0; col < width; ++col) {
                if (drops[col] == row) {
                    cout << BOLD_WHITE << (char)(33 + rand() % 94) << RESET;
                } else if (drops[col] > row && drops[col] - row < 5) {
                    cout << BOLD_GREEN << (char)(33 + rand() % 94) << RESET;
                } else {
                    cout << " ";
                }
            }
            cout << "\n";
        }

        for (int i = 0; i < width; ++i) {
            if (drops[i] > height + (rand() % 10)) drops[i] = 0;
            else drops[i]++;
        }

        cout.flush();
        this_thread::sleep_for(chrono::milliseconds(100));
    }
    cout << "\033[2J\033[H";
}

// --- Live Matrix Rain Background Input (top 10 lines only) ---
string getCommandWithMatrixBackground() {
    string input;
    char c;
    enableRawMode();

    struct winsize w{};
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int rows = (w.ws_row > 0) ? w.ws_row : 24;
    int cols = (w.ws_col > 0) ? w.ws_col : 80;

    int inputRow   = rows;      // last line for prompt+input
    int rainMaxRow = min(10, rows - 1); // sirf top 10 lines ya jitne available

    vector<int> drops(cols, -1);
    vector<int> dropSpeeds(cols);
    for (int i = 0; i < cols; i++) dropSpeeds[i] = 1 + rand() % 2;

    auto last_update = chrono::steady_clock::now();

    while (true) {
        auto now = chrono::steady_clock::now();
        if (chrono::duration_cast<chrono::milliseconds>(now - last_update).count() > 50) {

            cout << "\0337";  // save cursor (prompt line)

            for (int j = 0; j < cols; ++j) {
                if (drops[j] == -1) {
                    if (rand() % 100 < 2) drops[j] = 0;
                } else {
                    if (drops[j] < rainMaxRow) {
                        cout << "\033[" << (drops[j] + 1) << ";" << (j + 1) << "H" << " ";
                    }

                    drops[j] += dropSpeeds[j];

                    if (drops[j] < rainMaxRow) {
                        cout << "\033[" << (drops[j] + 1) << ";" << (j + 1) << "H";
                        cout << FAINT_GREEN << (rand() % 2) << RESET;
                    } else {
                        drops[j] = -1;
                    }
                }
            }

            cout << "\0338";  // restore cursor (input line)
            cout.flush();
            last_update = now;
        }

        if (read(STDIN_FILENO, &c, 1) == 1) {
            if (c == '\n') {
                cout << endl;
                break;
            } else if (c == 127) {
                if (!input.empty()) {
                    input.pop_back();
                    cout << "\b \b" << flush;
                }
            } else if (c >= 32 && c <= 126) {
                input.push_back(c);
                cout << c << flush;
            }
        }
    }

    disableRawMode();
    return input;
}

// --- Typewriter text ---
void typeText(const string& text, int delay_ms = 30) {
    for (char c : text) {
        cout << c << flush;
        usleep(delay_ms * 1000);
    }
    cout << endl;
}

// --- Signal handler ---
void handleSignal(int signal) {
    if (signal == SIGINT) {
        char cwd[1024];
        cout << "\n";
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            string path = cwd;
            char* homeEnv = getenv("HOME");
            string home = homeEnv ? string(homeEnv) : "";
            if (!home.empty() && path.find(home) == 0) path.replace(0, home.length(), "~");

            cout << BOLD_GREEN << "root@irfanOS "
                 << BOLD_YELLOW << path
                 << BOLD_RED   << " ⚡ "
                 << BOLD_CYAN  << "λ " << RESET;
        } else {
            cout << "myshell> ";
        }
        cout.flush();
    }
}

// === Shell::run ===
void Shell::run() {
    signal(SIGINT, handleSignal);
    srand(time(nullptr));

    // 1. Boot scroll
    scrollingHexDump();

    // 2. Password
    cout << "\033[2J\033[1;1H";
    cout << BOLD_YELLOW << "ENTER PASSWORD: " << RESET;
    string password;
    getline(cin, password);
    if (password != "irfan") {
        cout << BOLD_RED << "\n [!!!] ACCESS DENIED. INTRUDER ALERT." << RESET << endl;
        exit(1);
    }

    // 3. Banner + boot
    printSignature();
    typeText(" [*] Initializing Kernel Core...", 20);
    typeText(" [*] Mounting File System...", 20);
    typeText(" [*] Establishing Secure Uplink...", 20);

    cout << BOLD_MAGENTA << " [*] SYSTEM LOAD: [";
    for (int i = 0; i < 30; i++) {
        if (i % 3 == 0) cout << "#";
        else cout << "=";
        cout.flush();
        usleep(30000);
    }
    cout << "] 100%" << RESET << endl;
    usleep(200000);

    cout << endl;
    decryptText("WELCOME COMMANDER IRFAN.");
    cout << "------------------------------------------------" << endl << endl;

    char cwd[1024];
    vector<string> history_log;

    while (true) {
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            string path = cwd;
            char* homeEnv = getenv("HOME");
            string home = homeEnv ? string(homeEnv) : "";
            if (!home.empty() && path.find(home) == 0) path.replace(0, home.length(), "~");
            cout << BOLD_GREEN << "root@irfanOS "
                 << BOLD_YELLOW << path
                 << BOLD_RED   << " ⚡ "
                 << BOLD_CYAN  << "λ " << RESET;
        } else {
            cout << "myshell> ";
        }
        cout.flush();

        string command = getCommandWithMatrixBackground();
        if (command.empty()) continue;

        if (command == "!!") {
            if (history_log.empty()) {
                cout << "No history" << endl;
                continue;
            }
            command = history_log.back();
            cout << "Echo: " << command << endl;
        }
        else if (command[0] == '!') {
            try {
                int n = stoi(command.substr(1));
                if (n > 0 && (size_t)n <= history_log.size()) {
                    command = history_log[n-1];
                    cout << "Echo: " << command << endl;
                } else {
                    cout << "Error: Invalid index" << endl;
                    continue;
                }
            } catch (...) {
                cout << "Error: Invalid number" << endl;
                continue;
            }
        }

        history_log.push_back(command);
        auto start_time = chrono::high_resolution_clock::now();

        if (command == "exit") break;
        else if (command == "history") {
            for (size_t i = 0; i < history_log.size(); i++)
                cout << i+1 << " " << history_log[i] << endl;
        }
        else if (command == "matrix") {
            dynamicMatrixRain(80, 20, 4);
        }
        else {
            vector<string> args = parseInput(command);
            if (args.empty()) continue;

            if (args[0] == "cd") {
                if (args.size() > 1) {
                    if (chdir(args[1].c_str()) != 0) perror("cd failed");
                } else {
                    cout << "Error: Provide path" << endl;
                }
            } else {
                bool hasPipe = false;
                for (const auto& arg : args) {
                    if (arg == "|") { hasPipe = true; break; }
                }
                if (hasPipe) executePipe(args);
                else executeCommand(args);
            }
        }

        auto end_time = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = end_time - start_time;
        if (elapsed.count() > 0.05) {
            cout << BOLD_CYAN << "  [Time: "
                 << fixed << setprecision(3) << elapsed.count()
                 << "s]" << RESET << endl;
        }
    }
}

// === Helper functions ===
void Shell::executeCommand(vector<string>& args) {
    if (args.empty()) return;

    string outputFile, inputFile;
    vector<string> finalArgs;
    bool background = false;

    for (size_t i = 0; i < args.size(); i++) {
        if (args[i] == ">") {
            if (i+1 < args.size()) { outputFile = args[i+1]; i++; }
            else { cout << "Error: Output file?" << endl; return; }
        }
        else if (args[i] == "<") {
            if (i+1 < args.size()) { inputFile = args[i+1]; i++; }
            else { cout << "Error: Input file?" << endl; return; }
        }
        else if (args[i] == "&") {
            background = true;
        }
        else {
            finalArgs.push_back(args[i]);
        }
    }

    vector<char*> c_args;
    for (auto& arg : finalArgs) c_args.push_back(&arg[0]);
    c_args.push_back(nullptr);

    pid_t pid = fork();
    if (pid == -1) {
        perror("Fork failed");
    } else if (pid == 0) {
        if (background) {
            int null_fd = open("/dev/null", O_RDONLY);
            dup2(null_fd, STDIN_FILENO);
            close(null_fd);
        }
        if (!outputFile.empty()) {
            int fd = open(outputFile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
            dup2(fd, STDOUT_FILENO); close(fd);
        }
        if (!inputFile.empty()) {
            int fd = open(inputFile.c_str(), O_RDONLY);
            dup2(fd, STDIN_FILENO); close(fd);
        }
        if (execvp(c_args[0], c_args.data()) == -1) {
            perror("Command failed");
            exit(1);
        }
    } else {
        if (background) cout << "[Process Started] PID: " << pid << endl;
        else waitpid(pid, nullptr, 0);
    }
}

vector<string> Shell::parseInput(string command) {
    vector<string> tokens;
    stringstream ss(command);
    string word;
    while (ss >> word) tokens.push_back(word);
    return tokens;
}

void Shell::executePipe(vector<string>& args) {
    vector<string> left_cmd, right_cmd;
    bool pipeFound = false;

    for (const auto& arg : args) {
        if (arg == "|") { pipeFound = true; continue; }
        if (!pipeFound) left_cmd.push_back(arg);
        else right_cmd.push_back(arg);
    }

    if (left_cmd.empty() || right_cmd.empty()) {
        cout << "Pipe Error" << endl;
        return;
    }

    int pipefd[2];
    if (pipe(pipefd) == -1) return;

    if (fork() == 0) {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        vector<char*> c_args;
        for (auto& a : left_cmd) c_args.push_back(&a[0]);
        c_args.push_back(nullptr);
        execvp(c_args[0], c_args.data());
        exit(1);
    }

    if (fork() == 0) {
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        vector<char*> c_args;
        for (auto& a : right_cmd) c_args.push_back(&a[0]);
        c_args.push_back(nullptr);
        execvp(c_args[0], c_args.data());
        exit(1);
    }

    close(pipefd[0]);
    close(pipefd[1]);
    wait(nullptr);
    wait(nullptr);
}
