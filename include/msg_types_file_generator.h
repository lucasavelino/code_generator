#ifndef CODE_GENERATOR_MSG_TYPES_FILE_GENERATOR_H
#define CODE_GENERATOR_MSG_TYPES_FILE_GENERATOR_H

#include <utility>
#include "ast.h"
#include "replacer.h"
#include "util.h"
#include <QFile>
#include <QTextStream>
#include <QString>

namespace code_generator
{
    class MsgTypesFileGenerator
    {
    public:
        MsgTypesFileGenerator(QString output_file_name, QString msg_types_header_file_name,
                              QString db_msg_class_declaration_file_name,
                              QString db_msg_signal_construction_file_name,
                              QString db_msg_signal_declaration_file_name)
                : output_file_name(std::move(output_file_name)), msg_types_header_file_name(
                std::move(msg_types_header_file_name)),
                  db_msg_class_declaration_file_name(std::move(db_msg_class_declaration_file_name)),
                  db_msg_signal_construction_file_name(std::move(db_msg_signal_construction_file_name)),
                  db_msg_signal_declaration_file_name(std::move(db_msg_signal_declaration_file_name))
        {}

        void generate(const std::vector<code_generator::ast::Message>& msgs)
        {
            QString db_msg_classes_declaration_list_str;
            QTextStream db_msg_classes_declaration_list(&db_msg_classes_declaration_list_str);
            for(const auto& msg : msgs)
            {
                QString db_msg_signals_constructor_initializer_list_str;
                QTextStream db_msg_signals_constructor_initializer_list(&db_msg_signals_constructor_initializer_list_str);
                QString db_msg_signals_declaration_list_str;
                QTextStream db_msg_signals_declaration_list(&db_msg_signals_declaration_list_str);
                auto first_time_signals_loop = true;
                for(const auto& sig : msg.signals)
                {
                    Replacer db_msg_signal_construction_replacer{db_msg_signal_construction_file_name};
                    db_msg_signal_construction_replacer.add_tag("SignalName", sig.name.c_str())
                            .add_tag("SignalOffset", QString::number(sig.offset))
                            .add_tag("SignalFactor", QString::number(sig.scale_factor));
                    db_msg_signal_construction_replacer.replace_tags();
                    if(first_time_signals_loop)
                    {
                        first_time_signals_loop = false;
                    } else
                    {
                        db_msg_signals_constructor_initializer_list << ", ";
                        db_msg_signals_declaration_list << "\n\t";
                    }
                    db_msg_signals_constructor_initializer_list << db_msg_signal_construction_replacer;
                    Replacer db_msg_signal_declaration_replacer{db_msg_signal_declaration_file_name};
                    db_msg_signal_declaration_replacer.add_tag("SignalType", util::get_appropriate_signal_type(sig).c_str())
                            .add_tag("SignalByteIndex", QString::number(sig.byte_index-1U))
                            .add_tag("SignalStartBit", QString::number(sig.bit_no))
                            .add_tag("SignalLength", QString::number(sig.length))
                            .add_tag("SignalIntelBool", (sig.byte_order == 1U ? "true" : "false"))
                            .add_tag("SignalName", sig.name.c_str());
                    db_msg_signal_declaration_replacer.replace_tags();
                    db_msg_signals_declaration_list << db_msg_signal_declaration_replacer;
                }
                Replacer db_msg_class_declaration_replacer{db_msg_class_declaration_file_name};
                db_msg_class_declaration_replacer.add_tag("DBMsgName", msg.name.c_str())
                        .add_tag("DBMsgPGN", QString::number(msg.pgn))
                        .add_tag("DBMsgLength", QString::number(msg.length))
                        .add_tag("DBMsgSignalsConstructorInitializerList", db_msg_signals_constructor_initializer_list.readAll())
                        .add_tag("DBMsgSignalsDeclarationList", db_msg_signals_declaration_list.readAll());
                db_msg_class_declaration_replacer.replace_tags();
                db_msg_classes_declaration_list << db_msg_class_declaration_replacer;
            }
            Replacer msg_types_header_replacer{msg_types_header_file_name};
            msg_types_header_replacer.add_tag("DBMsgClassesDeclarationList", db_msg_classes_declaration_list.readAll());
            msg_types_header_replacer.replace_tags();
            QFile out_file{output_file_name};
            if(!out_file.open(QFile::WriteOnly | QFile::Text))
            {
                qDebug() << "Could not open file " << output_file_name;
                return;
            }
            QTextStream out_file_stream(&out_file);
            out_file_stream << msg_types_header_replacer;
        }
    private:
        QString output_file_name;
        QString msg_types_header_file_name;
        QString db_msg_class_declaration_file_name;
        QString db_msg_signal_construction_file_name;
        QString db_msg_signal_declaration_file_name;
    };
}

#endif //CODE_GENERATOR_MSG_TYPES_FILE_GENERATOR_H
