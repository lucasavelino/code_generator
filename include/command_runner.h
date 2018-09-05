#ifndef CODE_GENERATOR_COMMAND_RUNNER_H
#define CODE_GENERATOR_COMMAND_RUNNER_H

#include <utility>
#include <vector>
#include <string>
#include <windows.h>
#include <sstream>
#include <cstdlib>
#include <iostream>
#include <QProcess>
#include <QVector>
#include <QList>
#include <QDebug>

namespace code_generator
{
    class CommandRunner
    {
    public:
        CommandRunner(QString command, QList<QString> parameters = {})
                : command(command), parameters(std::move(parameters)),
                  env(QProcessEnvironment::systemEnvironment()),output_str()
        {
        }

        CommandRunner& set_directory(QString current_directory)
        {
            cmd.setWorkingDirectory(current_directory);
            return *this;
        }

        CommandRunner& add_environment_variable(QString var_name, QString var_value)
        {
            env.insert(var_name, var_value);
            return *this;
        }

        CommandRunner& append_environment_variable(QString var_name, QString var_value)
        {
            auto old_value = env.value(var_name);
            env.remove(var_name);
            env.insert(var_name,old_value + ";" + var_value);
            return *this;
        }

        CommandRunner& remove_environment_variable(QString var_name)
        {
            env.remove(var_name);
            return *this;
        }

        CommandRunner& add_parameter(QString parameter)
        {
            parameters.push_back(parameter);
            return *this;
        }

        CommandRunner& remove_parameter(QString parameter)
        {
            parameters.removeOne(parameter);
            return *this;
        }

        QString get_output()
        {
            return output_str;
        }

        void operator()()
        {
            QTextStream output(&output_str);
            cmd.setProcessEnvironment(env);
            cmd.start(command, parameters);
            if(!cmd.waitForStarted())
            {
                output << "Error while waiting for command " << command << "to start\n"
                       << "Error: " << cmd.errorString();
                return;
            }
            if(!cmd.waitForFinished())
            {
                output << "Error while waiting for command " << command << "completion\n"
                       << "Error: " << cmd.errorString();
                return;
            }
            output << "$ " << command << " "
                   << parameters.join(" ") << "\n\n"
                   << cmd.readAll() << "\n\n"
                   << cmd.readAllStandardOutput() << "\n\n"
                   << cmd.readAllStandardError() <<  "\n\n" ;
        }
    private:
        QString command;
        QString current_directory;
        QList<QString> parameters;
        QProcessEnvironment env;
        QProcess cmd;
        QString output_str;
    };
}

#endif //CODE_GENERATOR_COMMAND_RUNNER_H
