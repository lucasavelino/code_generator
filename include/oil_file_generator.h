#ifndef CODE_GENERATOR_OIL_FILE_GENERATOR_H
#define CODE_GENERATOR_OIL_FILE_GENERATOR_H

#include <string>
#include <utility>
#include "replacer.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QString>

namespace code_generator
{
    class OilFileGenerator
    {
    public:
        OilFileGenerator(QString output_file_name, QString timer_task_oil_file_name,
                         QString can_send_task_oil_file_name, QString pins_reader_task_oil_file_name,
                         QString can_recv_task_oil_file_name, QString oil_ini_file_name,
                         QString oil_fim_file_name, QString trampoline_root_path,
                         QString src_file_path, QString exe_file_path)
                : output_file_name(std::move(output_file_name)), timer_task_oil_file_name(
                std::move(timer_task_oil_file_name)),
                  can_send_task_oil_file_name(std::move(can_send_task_oil_file_name)),
                  pins_reader_task_oil_file_name(std::move(pins_reader_task_oil_file_name)),
                  can_recv_task_oil_file_name(std::move(can_recv_task_oil_file_name)),
                  oil_ini_file_name(std::move(oil_ini_file_name)),
                  oil_fim_file_name(std::move(oil_fim_file_name)), trampoline_root_path(
                          std::move(trampoline_root_path)), src_file_path(std::move(src_file_path)),
                  exe_file_path(std::move(exe_file_path))
        {}

        void generate(const std::vector<code_generator::ast::TimerHandler>& timer_handlers,
                        const util::SystemTasksInfo& tasks_info)
        {
            QFile out_file(output_file_name);
            if(!out_file.open(QFile::WriteOnly | QFile::Text))
            {
                qDebug() << "Could not open file " << output_file_name;
                return;
            }
            QTextStream out_file_stream{&out_file};
            Replacer oil_ini_replacer{oil_ini_file_name};
            oil_ini_replacer.add_tag("TrampolineRootPath",trampoline_root_path)
                            .add_tag("ExeFileName",exe_file_path)
                            .add_tag("SrcFileName",src_file_path);
            oil_ini_replacer.replace_tags();
            out_file_stream << oil_ini_replacer;
            if(tasks_info.can_send_task_used)
            {
                Replacer can_send_task_replacer{can_send_task_oil_file_name};
                can_send_task_replacer.add_tag("CanSendTaskPriority",
                                               QString::number(1U
                                                               + (tasks_info.pins_reader_task_used ? 1U : 0U)))
                                      .add_tag("CanSendTaskStackSize", QString::number(128U));
                can_send_task_replacer.replace_tags();
                out_file_stream << can_send_task_replacer;
            }

            if(tasks_info.pins_reader_task_used)
            {
                Replacer pins_reader_task_replacer{pins_reader_task_oil_file_name};
                pins_reader_task_replacer.add_tag("PinsReaderTaskTime", QString::number(static_cast<unsigned int>(300/1.024F)))
                                         .add_tag("PinsReaderTaskPriority",
                                                  QString::number(1U))
                                         .add_tag("PinsReaderTaskStackSize", QString::number(128U))
                                         .add_tag("CanSendResourcesAndEvents",
                                                  tasks_info.can_send_task_used ?
                                                      "    RESOURCE = can_send_msg_queue_resource;\n"
                                                      "    EVENT = can_send_msg_queue_full;" :
                                                      "");
                pins_reader_task_replacer.replace_tags();
                out_file_stream << pins_reader_task_replacer;
            }
            if(tasks_info.can_recv_task_used)
            {
                Replacer can_recv_task_replacer{can_recv_task_oil_file_name};
                can_recv_task_replacer.add_tag("CanRecvIsrPriority", QString::number(1U))
                                      .add_tag("CanRecvIsrStackSize", QString::number(128U))
                                      .add_tag("CanRecvTaskPriority", QString::number(tasks_info.number_of_tasks()))
                                      .add_tag("CanRecvTaskStackSize", QString::number(128U))
                                      .add_tag("CanSendResourcesAndEvents",
                                               tasks_info.can_send_task_used ?
                                                   "    RESOURCE = can_send_msg_queue_resource;\n"
                                                   "    EVENT = can_send_msg_queue_full;" :
                                                   "");
                can_recv_task_replacer.replace_tags();
                out_file_stream << can_recv_task_replacer;
            }
            if(tasks_info.timer_task_used)
            {
                unsigned int task_priority = 1U +
                                             (tasks_info.can_send_task_used ? 1U : 0U) +
                                             (tasks_info.pins_reader_task_used ? 1U : 0U);
                for(const code_generator::ast::TimerHandler& timer_handler : timer_handlers)
                {
                    Replacer timer_task_oil_replacer{timer_task_oil_file_name};
                    timer_task_oil_replacer.add_tag("TaskName", QString("OnTimer_") + timer_handler.name.c_str() + "_" + QString::number(timer_handler.milliseconds))
                            .add_tag("TaskTimerTime",QString::number(static_cast<unsigned int>(timer_handler.milliseconds/1.024F)))
                            .add_tag("TaskPriority",QString::number(task_priority))
                            .add_tag("TimerTaskStackSize", QString::number(128U))
                            .add_tag("CanSendResourcesAndEvents",
                                     tasks_info.can_send_task_used ?
                                         "    RESOURCE = can_send_msg_queue_resource;\n"
                                         "    EVENT = can_send_msg_queue_full;" :
                                         "");
                    timer_task_oil_replacer.replace_tags();
                    out_file_stream << timer_task_oil_replacer;
                    task_priority++;
                }
            }
            QFile oil_fim_file(oil_fim_file_name);
            if(!oil_fim_file.open(QFile::ReadOnly | QFile::Text))
            {
                qDebug() << "Could not open file " << oil_fim_file_name;
                return;
            }
            out_file_stream << oil_fim_file.readAll();
        }

    private:
        QString output_file_name;
        QString timer_task_oil_file_name;
        QString can_send_task_oil_file_name;
        QString pins_reader_task_oil_file_name;
        QString can_recv_task_oil_file_name;
        QString oil_ini_file_name;
        QString oil_fim_file_name;
        QString trampoline_root_path;
        QString src_file_path;
        QString exe_file_path;
    };
}

#endif //CODE_GENERATOR_OIL_FILE_GENERATOR_H
