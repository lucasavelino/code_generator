#ifndef CODE_GENERATOR_MSG_TYPES_FILE_GENERATOR_H
#define CODE_GENERATOR_MSG_TYPES_FILE_GENERATOR_H

#include <utility>
#include <fstream>
#include "ast.h"
#include "replacer.h"
#include "util.h"

namespace code_generator
{
    class MsgTypesFileGenerator
    {
    public:
        MsgTypesFileGenerator(std::string output_file_name, std::string msg_types_header_file_name,
                              std::string db_msg_class_declaration_file_name,
                              std::string db_msg_signal_construction_file_name,
                              std::string db_msg_signal_declaration_file_name)
                : output_file_name(std::move(output_file_name)), msg_types_header_file_name(
                std::move(msg_types_header_file_name)),
                  db_msg_class_declaration_file_name(std::move(db_msg_class_declaration_file_name)),
                  db_msg_signal_construction_file_name(std::move(db_msg_signal_construction_file_name)),
                  db_msg_signal_declaration_file_name(std::move(db_msg_signal_declaration_file_name))
        {}

        void generate(const std::vector<code_generator::ast::Message>& msgs)
        {
            std::stringstream db_msg_classes_declaration_list;
            for(const auto& msg : msgs)
            {
                std::stringstream db_msg_signals_constructor_initializer_list;
                std::stringstream db_msg_signals_declaration_list;
                auto first_time_signals_loop = true;
                for(const auto& sig : msg.signals)
                {
                    Replacer db_msg_signal_construction_replacer{db_msg_signal_construction_file_name};
                    db_msg_signal_construction_replacer.add_tag("SignalName", sig.name)
                            .add_tag("SignalOffset", std::to_string(sig.offset))
                            .add_tag("SignalFactor", std::to_string(sig.scale_factor));
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
                    db_msg_signal_declaration_replacer.add_tag("SignalType", util::get_appropriate_signal_type(sig))
                            .add_tag("SignalByteIndex", std::to_string(sig.byte_index-1U))
                            .add_tag("SignalStartBit", std::to_string(sig.bit_no))
                            .add_tag("SignalLength", std::to_string(sig.length))
                            .add_tag("SignalIntelBool", (sig.byte_order == 1U ? "true" : "false"))
                            .add_tag("SignalName", sig.name);
                    db_msg_signal_declaration_replacer.replace_tags();
                    db_msg_signals_declaration_list << db_msg_signal_declaration_replacer;
                }
                Replacer db_msg_class_declaration_replacer{db_msg_class_declaration_file_name};
                db_msg_class_declaration_replacer.add_tag("DBMsgName", msg.name)
                        .add_tag("DBMsgLength", std::to_string(msg.length))
                        .add_tag("DBMsgPGN", std::to_string(msg.pgn))
                        .add_tag("DBMsgSignalsConstructorInitializerList", db_msg_signals_constructor_initializer_list.str())
                        .add_tag("DBMsgSignalsDeclarationList", db_msg_signals_declaration_list.str());
                db_msg_class_declaration_replacer.replace_tags();
                db_msg_classes_declaration_list << db_msg_class_declaration_replacer;
            }
            Replacer msg_types_header_replacer{msg_types_header_file_name};
            msg_types_header_replacer.add_tag("DBMsgClassesDeclarationList", db_msg_classes_declaration_list.str());
            msg_types_header_replacer.replace_tags();
            std::ofstream out_file{output_file_name};
            out_file << msg_types_header_replacer;
        }
    private:
        std::string output_file_name;
        std::string msg_types_header_file_name;
        std::string db_msg_class_declaration_file_name;
        std::string db_msg_signal_construction_file_name;
        std::string db_msg_signal_declaration_file_name;
    };
}

#endif //CODE_GENERATOR_MSG_TYPES_FILE_GENERATOR_H
