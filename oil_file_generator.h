#ifndef CODE_GENERATOR_OIL_FILE_GENERATOR_H
#define CODE_GENERATOR_OIL_FILE_GENERATOR_H

#include <string>
#include <utility>
#include "replacer.h"

namespace code_generator
{
    class OilFileGenerator
    {
    public:
        OilFileGenerator(std::string output_file_name, std::string timer_task_oil_file_name,
                         std::string can_send_task_oil_file_name, std::string pins_reader_task_oil_file_name,
                         std::string oil_ini_file_name,
                         std::string oil_fim_file_name, std::string trampoline_root_path,
                         std::string src_file_path, std::string exe_file_path)
                : output_file_name(std::move(output_file_name)), timer_task_oil_file_name(
                std::move(timer_task_oil_file_name)),
                  can_send_task_oil_file_name(std::move(can_send_task_oil_file_name)),
                  pins_reader_task_oil_file_name(std::move(pins_reader_task_oil_file_name)),
                  oil_ini_file_name(std::move(oil_ini_file_name)),
                  oil_fim_file_name(std::move(oil_fim_file_name)), trampoline_root_path(
                          std::move(trampoline_root_path)), src_file_path(std::move(src_file_path)),
                  exe_file_path(std::move(exe_file_path))
        {}

        void generate(const std::vector<code_generator::ast::TimerHandler>& timer_handlers,
                        unsigned int total_threads)
        {
            std::ofstream out_file{output_file_name};
            Replacer oil_ini_replacer{oil_ini_file_name};
            oil_ini_replacer.add_tag("TrampolineRootPath",trampoline_root_path)
                            .add_tag("ExeFileName",exe_file_path)
                            .add_tag("SrcFileName",src_file_path);
            oil_ini_replacer.replace_tags();
            out_file << oil_ini_replacer;
            Replacer can_send_task_replacer{can_send_task_oil_file_name};
            can_send_task_replacer.add_tag("CanSendTaskPriority",std::to_string(2U));
            can_send_task_replacer.replace_tags();
            out_file << can_send_task_replacer;
            Replacer pins_reader_task_replacer{pins_reader_task_oil_file_name};
            pins_reader_task_replacer.add_tag("PinsReaderTaskTime", std::to_string(static_cast<unsigned int>(120/1.024F)))
                                     .add_tag("PinsReaderTaskPriority", std::to_string(1U));
            pins_reader_task_replacer.replace_tags();
            out_file << pins_reader_task_replacer;
            unsigned int task_priority = total_threads;
            for(const code_generator::ast::TimerHandler& timer_handler : timer_handlers)
            {
                Replacer timer_task_oil_replacer{timer_task_oil_file_name};
                timer_task_oil_replacer.add_tag("TaskName", "OnTimer_" + timer_handler.name + "_" + std::to_string(timer_handler.milliseconds))
                        .add_tag("TaskTimerTime",std::to_string((unsigned int)(timer_handler.milliseconds/1.024F)))
                        .add_tag("TaskPriority",std::to_string(task_priority));
                timer_task_oil_replacer.replace_tags();
                out_file << timer_task_oil_replacer;
                task_priority--;
            }
            out_file << std::ifstream(oil_fim_file_name).rdbuf();
        }

    private:
        std::string output_file_name;
        std::string timer_task_oil_file_name;
        std::string can_send_task_oil_file_name;
        std::string pins_reader_task_oil_file_name;
        std::string oil_ini_file_name;
        std::string oil_fim_file_name;
        std::string trampoline_root_path;
        std::string src_file_path;
        std::string exe_file_path;
    };
}

#endif //CODE_GENERATOR_OIL_FILE_GENERATOR_H
