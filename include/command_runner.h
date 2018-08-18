#ifndef CODE_GENERATOR_COMMAND_RUNNER_H
#define CODE_GENERATOR_COMMAND_RUNNER_H

#include <utility>
#include <vector>
#include <string>
#include <windows.h>
#include <sstream>
#include <cstdlib>

namespace code_generator
{
    class CommandRunner
    {
    public:
        CommandRunner(std::string command, std::string current_directory, std::vector<std::string> parameters)
                : command(std::move(command)), current_directory(std::move(current_directory)), parameters(std::move(parameters)),
                  pif(), si()
        {
            ZeroMemory( &si, sizeof( si ) );
            si.cb = sizeof( si );
        }

//        bool operator()()
//        {
//            std::stringstream cmd_ss;
//            //cmd_ss << command;
//            auto first = true;
//            for(const auto& param : parameters)
//            {
//                if(first)
//                {
//                    first = false;
//                } else
//                {
//                    cmd_ss << " ";
//                }
//                cmd_ss << param;
//            }
//            auto cmd = cmd_ss.str();
//            const auto cmd_lpstr = const_cast<LPSTR>(cmd.c_str());
//            auto to_return = (TRUE == CreateProcess(command.c_str(), cmd_lpstr, NULL, NULL, FALSE, 0, NULL, current_directory.c_str(), &si, &pif));
//            if(to_return)
//            {
//                CloseHandle(pif.hProcess);
//                CloseHandle(pif.hThread);
//            } else
//            {
//                LPVOID lpMsgBuf;
//                LPVOID lpDisplayBuf;
//                auto error = GetLastError();
//
//                FormatMessage(
//                        FORMAT_MESSAGE_ALLOCATE_BUFFER |
//                        FORMAT_MESSAGE_FROM_SYSTEM |
//                        FORMAT_MESSAGE_IGNORE_INSERTS,
//                        NULL,
//                        error,
//                        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
//                        (LPTSTR) &lpMsgBuf,
//                        0, NULL );
//                auto msg = static_cast<char *>(lpMsgBuf);
//                std::cout << "Error " << error << ": " << msg << "\n";
//                LocalFree(lpMsgBuf);
//            }
//            return to_return;
//        }
        void operator()()
        {
            std::stringstream cmd_ss;
            cmd_ss << "cd " << current_directory << " && "<< command;
            for(const auto& param : parameters)
            {
                cmd_ss << " " << param;
            }
            std::cout << cmd_ss.str() << "\n\n";
            system(cmd_ss.str().c_str());
        }
    private:
        std::string command;
        std::string current_directory;
        std::vector<std::string> parameters;
        PROCESS_INFORMATION pif;
        STARTUPINFO si;
    };
}

#endif //CODE_GENERATOR_COMMAND_RUNNER_H
