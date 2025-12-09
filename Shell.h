#ifndef SHELL_H
#define SHELL_H

#include <vector>
#include <string>

using namespace std;


class Shell{
    public:
    void run();

    private:
    vector<string> parseInput(string command);

    ////// ye execute karega
    void executeCommand(vector<string> &args);
    vector<string> history_log;
      
    ////// ye pipe ke liye hai code ok
    void executePipe(vector<string>& args);
};

#endif