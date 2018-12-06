#include <QtWidgets>
#include <QVector>
#include <QPair>
#include <QMap>
#include <QSettings>
#include <limits>
#include "code_generator_wizard.h"


static const auto digital_pin_used_by_mcp2515 =
[](unsigned int pin)
{
    constexpr std::array<unsigned int, 5> digital_pins_used_by_mcp2515 = { 2, 10, 11, 12, 13 };
    return std::find(std::begin(digital_pins_used_by_mcp2515),
                     std::end(digital_pins_used_by_mcp2515),
                     pin) != std::end(digital_pins_used_by_mcp2515);
};

CodeGeneratorWizard::CodeGeneratorWizard(bool& _configure_new, QWidget *parent) :
    QWizard(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
    configure_new(_configure_new)
{
    configure_new = false;
    setButtonText(WizardButton::NextButton,tr("&Avançar >"));
    setButtonText(WizardButton::CommitButton,tr("&Avançar >"));
    setButtonText(WizardButton::BackButton,tr("< &Voltar"));
    setButtonText(WizardButton::CancelButton,tr("&Cancelar"));
    setButtonText(WizardButton::FinishButton,tr("&Finalizar"));
    setWindowIcon(QIcon(":/images/cg_icon.png"));

    // AeroStyle's (default, on windows 10) close button is not working properly
    setWizardStyle(ModernStyle);

    auto *intro_page = new IntroPage;
    auto *load_configs_page = new LoadConfigsPage;
    auto *busmaster_generated_input_files_page = new BusmasterGeneratedInputFilesPage;
    auto *trampoline_rtos_configs_page = new TrampolineRTOSConfigsPage;
    auto *output_configs_page = new OutputConfigsPage;
    auto *pins_config_page = new PinsConfigPage;
    auto *code_config_page = new CodeConfigPage;
    auto *build_page = new BuildPage;
    auto *com_port_page = new ComPortPage;
    auto *load_page = new LoadPage;
    auto *last_page = new LastPage;
    addPage(intro_page);
    addPage(load_configs_page);
    addPage(busmaster_generated_input_files_page);
    addPage(trampoline_rtos_configs_page);
    addPage(output_configs_page);
    addPage(pins_config_page);
    addPage(code_config_page);
    addPage(build_page);
    addPage(com_port_page);
    addPage(load_page);
    addPage(last_page);

    setWindowTitle(tr("Code Generator"));
}

void CodeGeneratorWizard::accept()
{
    QWizard::accept();
}

IntroPage::IntroPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Bem vindo"));
    label = new QLabel(tr("Esse assistente serve para realizar as configurações do Code Generator"));
    label->setWordWrap(true);

    auto *layout = new QVBoxLayout;
    layout->addWidget(label);
    setLayout(layout);
}

LoadConfigsPage::LoadConfigsPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Configurações salvas"));
    setSubTitle(tr("Deseja carregar configurações salvas da última vez que utilizou o assistente?"));

    load_configs_label = new QLabel(tr("Carregar configurações salvas:"));
    load_configs_label->setWordWrap(true);
    load_configs_button = new QPushButton(tr("Carregar"));
    load_configs_label->setBuddy(load_configs_button);
    connect(load_configs_button,
            &QPushButton::clicked,
            this,
            [this]()
            {
                QSettings settings;
                setField("def_file", settings.value("def_file",""));
                setField("dbf_file", settings.value("dbf_file",""));
                setField("cpp_file", settings.value("cpp_file",""));
                setField("trampoline_root_dir", settings.value("trampoline_root_dir",""));
                setField("goil_exe_file", settings.value("goil_exe_file",""));
                setField("output_dir", settings.value("output_dir",""));
                setField("output_prefix_name", settings.value("output_prefix_name",""));
                setField("arduino_type", settings.value("arduino_type",""));
                setField("can_sender", settings.value("can_sender",""));
                setField("serial_user", settings.value("serial_user",""));
                setField("ecu_address", settings.value("ecu_address",""));
            }
    );

    auto *layout = new QGridLayout;
    layout->addWidget(load_configs_label,0,0);
    layout->addWidget(load_configs_button,0,1);
    setLayout(layout);
}

BusmasterGeneratedInputFilesPage::BusmasterGeneratedInputFilesPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Arquivos do busmaster"));
    setSubTitle(tr("Arquivos de codigo gerados pelo busmaster "
                   "a serem utilizados como entrada"));
    def_file_label = new QLabel(tr("Arquivo de definições (.def):"));
    def_file_label->setMinimumWidth(150);
    def_file_label->setWordWrap(true);
    def_file_line_edit = new QLineEdit;
    def_file_line_edit->setFixedWidth(300);
    def_file_open_button = new QPushButton(tr("Procurar..."));
    def_file_label->setBuddy(def_file_line_edit);
    connect(def_file_open_button,
            &QPushButton::clicked,
            this,
            [&]()
            {
                QString def_file_name = QFileDialog::getOpenFileName(
                                            this,
                                            tr("Procurar Arquivo"),
                                            QString(),
                                            "Arquivos DEF (*.def)"
                                        );
                def_file_line_edit->setText(def_file_name);
            });

    dbf_file_label = new QLabel(tr("Arquivo do banco de dados de mensagens (.DBF):"));
    dbf_file_label->setMinimumWidth(150);
    dbf_file_label->setWordWrap(true);
    dbf_file_line_edit = new QLineEdit;
    dbf_file_line_edit->setFixedWidth(300);
    dbf_file_open_button = new QPushButton(tr("Procurar..."));
    dbf_file_label->setBuddy(dbf_file_line_edit);
    connect(dbf_file_open_button,
            &QPushButton::clicked,
            this,
            [&]()
            {
                QString dbf_file_name = QFileDialog::getOpenFileName(
                                            this,
                                            tr("Procurar Arquivo"),
                                            QString(),
                                            "Arquivos DEF (*.DBF)"
                                        );
                dbf_file_line_edit->setText(dbf_file_name);
            });

    cpp_file_label = new QLabel(tr("Arquivo de código fonte (.cpp):"));
    cpp_file_label->setMinimumWidth(150);
    cpp_file_label->setWordWrap(true);
    cpp_file_line_edit = new QLineEdit;
    cpp_file_line_edit->setFixedWidth(300);
    cpp_file_open_button = new QPushButton(tr("Procurar..."));
    cpp_file_label->setBuddy(cpp_file_line_edit);
    connect(cpp_file_open_button,
            &QPushButton::clicked,
            this,
            [&]()
            {
                QString cpp_file_name = QFileDialog::getOpenFileName(
                                            this,
                                            tr("Procurar Arquivo"),
                                            QString(),
                                            "Arquivos DEF (*.cpp)"
                                        );
                cpp_file_line_edit->setText(cpp_file_name);
            });

    registerField("def_file*", def_file_line_edit);
    registerField("dbf_file*", dbf_file_line_edit);
    registerField("cpp_file*", cpp_file_line_edit);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(def_file_label, 0, 0);
    layout->addWidget(def_file_line_edit, 0, 1);
    layout->addWidget(def_file_open_button, 0, 2);
    layout->addWidget(dbf_file_label, 1, 0);
    layout->addWidget(dbf_file_line_edit, 1, 1);
    layout->addWidget(dbf_file_open_button, 1, 2);
    layout->addWidget(cpp_file_label, 2, 0);
    layout->addWidget(cpp_file_line_edit, 2, 1);
    layout->addWidget(cpp_file_open_button, 2, 2);
    setLayout(layout);

}

bool BusmasterGeneratedInputFilesPage::validatePage()
{
    auto def_file = def_file_line_edit->text();
    if(!def_file.endsWith(".def"))
    {
        QMessageBox msg;
        msg.setText(tr("O arquivo de definições deve possuir extensão .def"));
        msg.exec();
        return false;
    } else if(!QFileInfo(def_file).exists())
    {
        QMessageBox msg;
        msg.setText(tr("O arquivo de definições: '%1' não foi encontrado").arg(def_file));
        msg.exec();
        return false;
    }

    auto dbf_file = dbf_file_line_edit->text();
    if(!dbf_file.endsWith(".DBF"))
    {
        QMessageBox msg;
        msg.setText(tr("O arquivo do banco de dados de mensagens deve possuir extensão .DBF"));
        msg.exec();
        return false;
    } else if(!QFileInfo(dbf_file).exists())
    {
        QMessageBox msg;
        msg.setText(tr("O arquivo do banco de dados de mensagens: '%1' não foi encontrado").arg(dbf_file));
        msg.exec();
        return false;
    }

    auto cpp_file = cpp_file_line_edit->text();
    if(!cpp_file.endsWith(".cpp"))
    {
        QMessageBox msg;
        msg.setText(tr("O arquivo de código fonte deve possuir extensão .cpp"));
        msg.exec();
        return false;
    } else if(!QFileInfo(cpp_file).exists())
    {
        QMessageBox msg;
        msg.setText(tr("O arquivo de código fonte: '%1' não foi encontrado").arg(cpp_file));
        msg.exec();
        return false;
    }

    QSettings settings;
    settings.setValue("dbf_file", dbf_file);
    settings.setValue("def_file", def_file);
    settings.setValue("cpp_file", cpp_file);
    return true;
}

TrampolineRTOSConfigsPage::TrampolineRTOSConfigsPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Configurações do TrampolineRTOS"));
    setSubTitle(tr("Indicar configurações do trampolineRTOS"));

    trampoline_root_label = new QLabel(tr("Diretório base do trampolineRTOS:"));
    trampoline_root_label->setMinimumWidth(150);
    trampoline_root_label->setWordWrap(true);
    trampoline_root_line_edit = new QLineEdit;
    trampoline_root_line_edit->setFixedWidth(300);
    trampoline_root_open_button = new QPushButton(tr("Procurar..."));
    trampoline_root_label->setBuddy(trampoline_root_line_edit);
    connect(trampoline_root_open_button,
            &QPushButton::clicked,
            this,
            [&]()
            {
                QString trampoline_root_dir_name = QFileDialog::getExistingDirectory(
                                            this,
                                            tr("Procurar Diretório"),
                                            QString(),
                                            QFileDialog::ShowDirsOnly
                                            | QFileDialog::DontResolveSymlinks
                                        );
                trampoline_root_line_edit->setText(trampoline_root_dir_name);
            });

    goil_exe_label = new QLabel(tr("Executável do GOIL:"));
    goil_exe_label->setMinimumWidth(150);
    goil_exe_label->setWordWrap(true);
    goil_exe_line_edit = new QLineEdit;
    goil_exe_line_edit->setFixedWidth(300);
    goil_exe_open_button = new QPushButton(tr("Procurar..."));
    goil_exe_label->setBuddy(goil_exe_line_edit);
    connect(goil_exe_open_button,
            &QPushButton::clicked,
            this,
            [&]()
            {
                QString goil_exe_dir_name = QFileDialog::getOpenFileName(
                                            this,
                                            tr("Procurar Arquivo"),
                                            QString(),
                                            tr("Arquivos executáveis (*.exe)")
                                        );
                goil_exe_line_edit->setText(goil_exe_dir_name);
            });

    registerField("trampoline_root_dir*", trampoline_root_line_edit);
    registerField("goil_exe_file*", goil_exe_line_edit);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(trampoline_root_label, 0, 0);
    layout->addWidget(trampoline_root_line_edit, 0, 1);
    layout->addWidget(trampoline_root_open_button, 0, 2);
    layout->addWidget(goil_exe_label, 1, 0);
    layout->addWidget(goil_exe_line_edit, 1, 1);
    layout->addWidget(goil_exe_open_button, 1, 2);
    setLayout(layout);
}

bool TrampolineRTOSConfigsPage::validatePage()
{
    auto trampoline_root_dir = trampoline_root_line_edit->text();
    auto trampoline_root_dir_file_info = QFileInfo(trampoline_root_dir);
    if(!trampoline_root_dir_file_info.exists())
    {
        QMessageBox msg;
        msg.setText(tr("O diretório base do trampolineRTOS: '%1' não foi encontrado").arg(trampoline_root_dir));
        msg.exec();
        return false;
    } else if(!trampoline_root_dir_file_info.isDir())
    {
        QMessageBox msg;
        msg.setText(tr("O arquivo: '%1' não é um diretório").arg(trampoline_root_dir));
        msg.exec();
        return false;
    }

    auto goil_exe = goil_exe_line_edit->text();
    if(!goil_exe.endsWith(".exe"))
    {
        QMessageBox msg;
        msg.setText(tr("O executável do GOIL deve possuir extensão .exe"));
        msg.exec();
        return false;
    } else if(!QFileInfo(goil_exe).exists())
    {
        QMessageBox msg;
        msg.setText(tr("O executável do GOIL: '%1' não foi encontrado").arg(goil_exe));
        msg.exec();
        return false;
    }

    QSettings settings;
    settings.setValue("trampoline_root_dir", trampoline_root_dir);
    settings.setValue("goil_exe_file", goil_exe);
    return true;
}

OutputConfigsPage::OutputConfigsPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Configurações de saída"));
    setSubTitle(tr("Indicar configurações dos resultados produzidos pelo Code Generator"));

    output_folder_label = new QLabel(tr("Diretório de saída:"));
    output_folder_label->setMinimumWidth(150);
    output_folder_label->setWordWrap(true);
    output_folder_line_edit = new QLineEdit;
    output_folder_line_edit->setFixedWidth(300);
    output_folder_open_button = new QPushButton(tr("Procurar..."));
    output_folder_label->setBuddy(output_folder_line_edit);
    connect(output_folder_open_button,
            &QPushButton::clicked,
            this,
            [&]()
            {
                QString output_folder_dir_name = QFileDialog::getExistingDirectory(
                                            this,
                                            tr("Procurar Diretório"),
                                            QString(),
                                            QFileDialog::ShowDirsOnly
                                            | QFileDialog::DontResolveSymlinks
                                        );
                output_folder_line_edit->setText(output_folder_dir_name);
            });

    output_prefix_label = new QLabel(tr("Nome prefixo do arquivo de saida (sem extensão):"));
    output_prefix_label->setMinimumWidth(150);
    output_prefix_label->setWordWrap(true);
    output_prefix_line_edit = new QLineEdit;
    output_prefix_line_edit->setFixedWidth(300);
    output_prefix_label->setBuddy(output_prefix_line_edit);

    registerField("output_dir*", output_folder_line_edit);
    registerField("output_prefix_name*", output_prefix_line_edit);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(output_folder_label, 0, 0);
    layout->addWidget(output_folder_line_edit, 0, 1);
    layout->addWidget(output_folder_open_button, 0, 2);
    layout->addWidget(output_prefix_label, 1, 0);
    layout->addWidget(output_prefix_line_edit, 1, 1);
    setLayout(layout);

    setCommitPage(true);
}

bool OutputConfigsPage::validatePage()
{
    auto output_folder = output_folder_line_edit->text();
    auto output_folder_file_info = QFileInfo(output_folder);
    if(!output_folder_file_info.exists())
    {
        QMessageBox msg;
        msg.setText(tr("O diretório de saída: '%1' não foi encontrado").arg(output_folder));
        msg.exec();
        return false;
    } else if(!output_folder_file_info.isDir())
    {
        QMessageBox msg;
        msg.setText(tr("O arquivo: '%1' não é um diretório").arg(output_folder));
        msg.exec();
        return false;
    }

    auto output_prefix = output_prefix_line_edit->text();
    if(output_prefix.contains("."))
    {
        QMessageBox msg;
        msg.setText(tr("O nome prefixo do arquivo de saida não deve conter extensão"));
        msg.exec();
        return false;
    }

    QSettings settings;
    settings.setValue("output_dir", output_folder);
    settings.setValue("output_prefix_name", output_prefix);
    return true;
}

PinsConfigPage::PinsConfigPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Configuração dos pinos"));
    setSubTitle(tr("Associar os pinos do arduino com as teclas relacionadas ao métodos OnKey gerados pelo busmaster"));

    arduino_select = new QComboBox;
    arduino_select->addItem(tr("Arduino UNO"));
    arduino_select->addItem(tr("Arduino NANO"));
    arduino_img_label = new QLabel;

    connect(arduino_select,
            QOverload<int>::of(&QComboBox::activated),
            [&](int index)
            {
                if(index == 0)
                {
                    arduino_img_label->setPixmap(QPixmap(":/images/arduino_uno.jpg")
                                                 .scaled(345,
                                                        448,
                                                        Qt::KeepAspectRatio
                                                        )
                                                 );
                } else
                {
                    arduino_img_label->setPixmap(QPixmap(":/images/arduino_nano.jpg")
                                                 .scaled(185,
                                                        358,
                                                        Qt::KeepAspectRatio
                                                        )
                                                 );
                }

            });

    registerField("arduino_type", arduino_select, "currentText", "currentTextChanged");

    auto *layout = new QGridLayout;
    layout->addWidget(arduino_select, 0, 0);
    layout->addWidget(arduino_img_label, 1, 0, 20, 1);
    auto *keys_label = new QLabel(tr("Tecla"));
    auto *pins_label = new QLabel(tr("Pino"));
    auto *input_type_label = new QLabel(tr("Tipo de entrada"));
    auto *active_state_label = new QLabel(tr("Ativo em nivel lógico"));
    input_type_label->setWordWrap(true);
    active_state_label->setWordWrap(true);
    pins_label->setMinimumWidth(50);
    layout->addWidget(keys_label, 0, 1);
    layout->addWidget(input_type_label, 0, 2);
    layout->addWidget(pins_label, 0, 3);
    layout->addWidget(active_state_label, 0, 4);
    setLayout(layout);
}

void PinsConfigPage::initializePage()
{
    if(arduino_select->currentIndex() == 0)
    {
        arduino_img_label->setPixmap(QPixmap(":/images/arduino_uno.jpg")
                                     .scaled(345,
                                            448,
                                            Qt::KeepAspectRatio
                                            )
                                     );
    } else
    {
        arduino_img_label->setPixmap(QPixmap(":/images/arduino_nano.jpg")
                                     .scaled(185,
                                            358,
                                            Qt::KeepAspectRatio
                                            )
                                     );
    }
    using size_type = std::vector<code_generator::ast::KeyHandler>::size_type;
    auto def_file = field("def_file").toString();
    std::vector<code_generator::ast::TimerHandler> timer_handlers;
    std::vector<code_generator::ast::KeyHandler> key_handlers;
    std::vector<code_generator::ast::MessageHandlerPgnName> pgn_name_msgs;
    code_generator::ast::MessageHandlerPgnAll msg_handler_pgn_all;
    code_generator::ast::OnDllLoadHandler dll_load_handler;
    std::tie(timer_handlers, key_handlers, pgn_name_msgs, msg_handler_pgn_all, dll_load_handler) = code_generator::util::read_def(def_file.toStdString());
    auto *layout = dynamic_cast<QGridLayout *>(this->layout());
    for(size_type i = 0; i < key_handlers.size(); ++i)
    {
        const auto& key_handler = key_handlers[i];
        if(! field(tr("key_%1_pin").arg(QString::fromStdString(key_handler.key))).isValid())
        {
            auto *key_label = new QLabel(tr("<%1>:").arg(QString::fromStdString(key_handler.key)));
            key_label->setTextFormat(Qt::PlainText);
            auto *pin_type_select = new QComboBox;
            pin_type_select->addItem(tr("digital"));
            pin_type_select->addItem(tr("analógico"));
            auto *key_select = new QComboBox;
            for(unsigned int j = 0; j < 14; ++j)
            {
                if(!digital_pin_used_by_mcp2515(j))
                {
                    key_select->addItem(QString::number(j));
                }
            }
            key_label->setBuddy(key_select);
            auto *digital_pin_active_state_select = new QComboBox;
            digital_pin_active_state_select->addItem(tr("1 (alto)"));
            digital_pin_active_state_select->addItem(tr("0 (baixo)"));
            connect(pin_type_select,
                    QOverload<int>::of(&QComboBox::activated),
                    [this, i](int index)
                    {
                        auto* layout = dynamic_cast<QGridLayout *>(this->layout());
                        if(layout != nullptr)
                        {
                            auto* digital_pin_active_state_item = layout->itemAtPosition(static_cast<int>(i) + 1, 4);
                            if(digital_pin_active_state_item != nullptr)
                            {
                                auto* digital_pin_active_state_widget = digital_pin_active_state_item->widget();
                                if(digital_pin_active_state_widget != nullptr)
                                {
                                    digital_pin_active_state_widget->setEnabled(index == 0);
                                }
                            }
                            auto* key_select_item = layout->itemAtPosition(static_cast<int>(i) + 1, 3);
                            if(key_select_item != nullptr)
                            {
                                auto* key_select_widget = key_select_item->widget();
                                if(key_select_widget != nullptr)
                                {
                                    auto* key_select = dynamic_cast<QComboBox *>(key_select_widget);
                                    if(key_select != nullptr)
                                    {
                                        key_select->clear();
                                        if(index == 0)
                                        {
                                            for(unsigned int j = 0; j < 14; ++j)
                                            {
                                                if(!digital_pin_used_by_mcp2515(j))
                                                {
                                                    key_select->addItem(QString::number(j));
                                                }
                                            }
                                        } else
                                        {
                                            for(unsigned int j = 14; j < 20; ++j)
                                            {
                                                key_select->addItem(QString::number(j));
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    });
            layout->addWidget(key_label, static_cast<int>(i) + 1, 1);
            layout->addWidget(pin_type_select, static_cast<int>(i) + 1, 2);
            layout->addWidget(key_select, static_cast<int>(i) + 1, 3);
            layout->addWidget(digital_pin_active_state_select, static_cast<int>(i) + 1, 4);
            registerField(tr("key_%1_pin").arg(QString::fromStdString(key_handler.key)), key_select, "currentText", "currentTextChanged");
            registerField(tr("key_%1_pin_type").arg(QString::fromStdString(key_handler.key)), pin_type_select, "currentText", "currentTextChanged");
            registerField(tr("key_%1_digital_pin_active_state").arg(QString::fromStdString(key_handler.key)), digital_pin_active_state_select, "currentText", "currentTextChanged");
        }
    }
    setLayout(layout);
}

bool PinsConfigPage::validatePage()
{
    using size_type = std::vector<code_generator::ast::KeyHandler>::size_type;
    auto def_file = field("def_file").toString();
    std::vector<code_generator::ast::TimerHandler> timer_handlers;
    std::vector<code_generator::ast::KeyHandler> key_handlers;
    std::vector<code_generator::ast::MessageHandlerPgnName> pgn_name_msgs;
    code_generator::ast::MessageHandlerPgnAll msg_handler_pgn_all;
    code_generator::ast::OnDllLoadHandler on_dll_load;
    std::tie(timer_handlers, key_handlers, pgn_name_msgs, msg_handler_pgn_all, on_dll_load) = code_generator::util::read_def(def_file.toStdString());
    QMap<QString,int> contador_pinos_repetidos;
    QStringList pinos;
    for(size_type i = 0; i < key_handlers.size(); ++i)
    {
        const auto& key_handler = key_handlers[i];
        pinos.push_back(field(tr("key_%1_pin").arg(QString::fromStdString(key_handler.key))).toString());
    }

    for(const auto& pino : pinos)
    {
        contador_pinos_repetidos[pino]++;
    }
    const auto& contagem_pinos = contador_pinos_repetidos.values();
    if(std::find_if(contagem_pinos.begin(),
                    contagem_pinos.end(),
                    [](int a){ return a > 1;})
            != contagem_pinos.end())
    {
        QMessageBox msg;
        msg.setText(tr("Um mesmo pino não deve ser associado a mais de uma tecla."));
        msg.exec();
        return false;
    }

    for(auto&& key_handler : key_handlers)
    {
        auto pin_type = field(tr("key_%1_pin_type").arg(QString::fromStdString(key_handler.key))).toString();
        unsigned int pin = field(tr("key_%1_pin").arg(QString::fromStdString(key_handler.key))).toString().toUInt();
        if(pin_type == "digital" && (digital_pin_used_by_mcp2515(pin) || pin > 13))
        {
            QMessageBox msg;
            msg.setText(tr("O pino %1 não é um pino digital válido ou é utilizado para comunicação com o módulo CAN mcp2515").arg(QString::number(pin)));
            msg.exec();
            return false;
        } else if(pin_type == "analógico" && pin < 14)
        {
            QMessageBox msg;
            msg.setText(tr("O pino %1 não é um pino analógico válido").arg(QString::number(pin)));
            msg.exec();
            return false;
        }
    }

    QSettings settings;
    settings.setValue("arduino_type", arduino_select->currentText());
    return true;
}

CodeConfigPage::CodeConfigPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Configurações do código gerado para plataforma"));
    setSubTitle(tr("Configurações gerais relacionadas ao código fonte produzido"));
    can_sender_checkbox = new QCheckBox(tr("A ECU envia mensagens pelo barremento CAN"));
    serial_user_checkbox = new QCheckBox(tr("A ECU faz uso da interface serial"));
    ecu_address_label = new QLabel(tr("Endereço da ECU"));
    ecu_address_label->setMinimumWidth(150);
    ecu_address_label->setWordWrap(true);
    ecu_address_line_edit = new QLineEdit;
    ecu_address_line_edit->setFixedWidth(50);
    ecu_address_label->setBuddy(ecu_address_line_edit);
    ecu_address_line_validator = new QIntValidator(0, 0xFF);
    ecu_address_line_edit->setValidator(ecu_address_line_validator);

    registerField("can_sender", can_sender_checkbox);
    registerField("serial_user", serial_user_checkbox);
    registerField("ecu_address*", ecu_address_line_edit);

    auto *layout = new QGridLayout;
    layout->addWidget(can_sender_checkbox, 0, 0);
    layout->addWidget(serial_user_checkbox, 1, 0);
    layout->addWidget(ecu_address_label, 0, 1);
    layout->addWidget(ecu_address_line_edit, 1, 1);
    setLayout(layout);

    setButtonText(QWizard::NextButton, tr("&Gerar"));
}

void CodeConfigPage::initializePage()
{
    auto handlers = code_generator::util::read_def(field("def_file").toString().toStdString());
    code_generator::util::SystemTasksInfo tasks_info(std::get<0>(handlers).size(), std::get<1>(handlers).size() != 0,
                                                     true, std::get<3>(handlers).declared,
                                                     std::get<2>(handlers).size() != 0, std::get<4>(handlers).declared,
                                                     true);

    auto* layout = dynamic_cast<QGridLayout*>(this->layout());
    auto grid_line_counter = 2;
    if(tasks_info.can_send_task_used && !field("can_send_task_stack_size").isValid())
    {
        auto* can_send_task_stack_size_label = new QLabel(tr("Tamanho da stack de can_send_task"));
        can_send_task_stack_size_label->setWordWrap(true);
        auto* can_send_task_stack_size_line_edit = new QLineEdit;
        can_send_task_stack_size_label->setBuddy(can_send_task_stack_size_line_edit);
        auto* can_send_task_stack_size_validator = new QIntValidator(0, 512);
        can_send_task_stack_size_line_edit->setValidator(can_send_task_stack_size_validator);
        registerField("can_send_task_stack_size", can_send_task_stack_size_line_edit);
        layout->addWidget(can_send_task_stack_size_label, grid_line_counter, 0);
        layout->addWidget(can_send_task_stack_size_line_edit, grid_line_counter, 1);
        grid_line_counter++;
    }
    if(tasks_info.can_recv_task_used && !field("can_recv_task_stack_size").isValid())
    {
        auto* can_recv_task_stack_size_label = new QLabel(tr("Tamanho da stack de can_recv_task"));
        can_recv_task_stack_size_label->setWordWrap(true);
        auto* can_recv_task_stack_size_line_edit = new QLineEdit;
        can_recv_task_stack_size_label->setBuddy(can_recv_task_stack_size_line_edit);
        auto* can_recv_task_stack_size_validator = new QIntValidator(0, 512);
        can_recv_task_stack_size_line_edit->setValidator(can_recv_task_stack_size_validator);
        registerField("can_recv_task_stack_size", can_recv_task_stack_size_line_edit);
        layout->addWidget(can_recv_task_stack_size_label, grid_line_counter, 0);
        layout->addWidget(can_recv_task_stack_size_line_edit, grid_line_counter, 1);
        grid_line_counter++;
    }
    if(tasks_info.pins_reader_task_used && !field("pins_reader_task_stack_size").isValid())
    {
        auto* pins_reader_task_stack_size_label = new QLabel(tr("Tamanho da stack de pins_reader_task"));
        pins_reader_task_stack_size_label->setWordWrap(true);
        auto* pins_reader_task_stack_size_line_edit = new QLineEdit;
        pins_reader_task_stack_size_label->setBuddy(pins_reader_task_stack_size_line_edit);
        auto* pins_reader_task_stack_size_validator = new QIntValidator(0, 512);
        pins_reader_task_stack_size_line_edit->setValidator(pins_reader_task_stack_size_validator);

        auto* pins_reader_task_period_label = new QLabel(tr("Ciclo de varredura de pins_reader_task (ms)"));
        pins_reader_task_period_label->setWordWrap(true);
        auto* pins_reader_task_period_line_edit = new QLineEdit;
        pins_reader_task_period_label->setBuddy(pins_reader_task_period_line_edit);
        auto* pins_reader_task_period_validator = new QIntValidator(0, std::numeric_limits<int>::max());
        pins_reader_task_period_line_edit->setValidator(pins_reader_task_period_validator);

        registerField("pins_reader_task_stack_size", pins_reader_task_stack_size_line_edit);
        registerField("pins_reader_task_period", pins_reader_task_period_line_edit);

        layout->addWidget(pins_reader_task_stack_size_label, grid_line_counter, 0);
        layout->addWidget(pins_reader_task_stack_size_line_edit, grid_line_counter, 1);
        layout->addWidget(pins_reader_task_period_label, grid_line_counter + 1, 0);
        layout->addWidget(pins_reader_task_period_line_edit, grid_line_counter + 1, 1);

        grid_line_counter += 2;
    }
    if(tasks_info.timer_task_used)
    {
        for(const auto& timer_task : std::get<0>(handlers))
        {
            const auto timer_task_str = tr("timer_task_%1").arg(timer_task.name.c_str());
            const auto timer_task_stack_size_field_name = tr("timer_task_%1_stack_size").arg(timer_task.name.c_str());
            if(!field(timer_task_stack_size_field_name).isValid())
            {
                auto* timer_task_stack_size_label = new QLabel(tr("Tamanho da stack de %1").arg(timer_task_str));
                timer_task_stack_size_label->setWordWrap(true);
                auto* timer_task_stack_size_line_edit = new QLineEdit;
                timer_task_stack_size_label->setBuddy(timer_task_stack_size_line_edit);
                auto* timer_task_stack_size_validator = new QIntValidator(0, 512);
                timer_task_stack_size_line_edit->setValidator(timer_task_stack_size_validator);
                registerField(timer_task_stack_size_field_name, timer_task_stack_size_line_edit);
                layout->addWidget(timer_task_stack_size_label, grid_line_counter, 0);
                layout->addWidget(timer_task_stack_size_line_edit, grid_line_counter, 1);
                grid_line_counter++;
            }
        }
    }

}

bool CodeConfigPage::validatePage()
{
    auto handlers = code_generator::util::read_def(field("def_file").toString().toStdString());
    code_generator::util::SystemTasksInfo tasks_info(std::get<0>(handlers).size(), std::get<1>(handlers).size() != 0,
                                                     true, std::get<3>(handlers).declared,
                                                     std::get<2>(handlers).size() != 0, std::get<4>(handlers).declared,
                                                     true);

    auto is_power_of_2 = [](int n){ return (n != 0) && ((n & (n-1)) == 0); };
    if(tasks_info.can_send_task_used)
    {
        auto can_send_task_stack_size_str = field("can_send_task_stack_size").toString();
        if(!can_send_task_stack_size_str.isEmpty())
        {
            auto can_send_task_stack_size = can_send_task_stack_size_str.toInt();
            if(!is_power_of_2(can_send_task_stack_size))
            {
                QMessageBox msg;
                msg.setText(tr("O tamanho da stack de can_send_task não é uma potência de 2"));
                msg.exec();
                return false;
            }
        }
    }
    if(tasks_info.can_recv_task_used)
    {
        auto can_recv_task_stack_size_str = field("can_recv_task_stack_size").toString();
        if(!can_recv_task_stack_size_str.isEmpty())
        {
            auto can_recv_task_stack_size = can_recv_task_stack_size_str.toInt();
            if(!is_power_of_2(can_recv_task_stack_size))
            {
                QMessageBox msg;
                msg.setText(tr("O tamanho da stack de can_recv_task não é uma potência de 2"));
                msg.exec();
                return false;
            }
        }
    }
    if(tasks_info.pins_reader_task_used)
    {
        auto pins_reader_task_stack_size_str = field("pins_reader_task_stack_size").toString();
        if(!pins_reader_task_stack_size_str.isEmpty())
        {
            auto pins_reader_task_stack_size = pins_reader_task_stack_size_str.toInt();
            if(!is_power_of_2(pins_reader_task_stack_size))
            {
                QMessageBox msg;
                msg.setText(tr("O tamanho da stack de pins_reader_task não é uma potência de 2"));
                msg.exec();
                return false;
            }
        }
    }
    if(tasks_info.timer_task_used)
    {
        for(const auto& timer_task : std::get<0>(handlers))
        {
            const auto timer_task_str = tr("timer_task_%1").arg(timer_task.name.c_str());
            const auto timer_task_stack_size_field_name = tr("timer_task_%1_stack_size").arg(timer_task.name.c_str());
            auto timer_task_stack_size_str = field(timer_task_stack_size_field_name).toString();
            if(!timer_task_stack_size_str.isEmpty())
            {
                auto timer_task_stack_size = timer_task_stack_size_str.toInt();
                if(!is_power_of_2(timer_task_stack_size))
                {
                    QMessageBox msg;
                    msg.setText(tr("O tamanho da stack de %1 não é uma potência de 2").arg(timer_task_str));
                    msg.exec();
                    return false;
                }
            }
        }
    }
    QSettings settings;
    settings.setValue("ecu_address", ecu_address_line_edit->text());
    settings.setValue("can_sender", can_sender_checkbox->isChecked());
    settings.setValue("serial_user", serial_user_checkbox->isChecked());
    return true;
}

BuildPage::BuildPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Geração e compilação do código fonte"));
    setSubTitle(tr("Veja o resultado do processo de geração do código e compilação"));

    build_text_edit = new QTextEdit;
    build_text_edit->setReadOnly(true);
    registerField("build_text_edit",build_text_edit,"plainText","textChanged");
    auto *layout = new QVBoxLayout;
    layout->addWidget(build_text_edit);
    setLayout(layout);
}

void BuildPage::initializePage()
{
    auto def_file = field("def_file").toString();
    auto dbf_file = field("dbf_file").toString();
    auto cpp_file = field("cpp_file").toString();

    auto trampoline_root_dir = field("trampoline_root_dir").toString();
    auto goil_exe_file = field("goil_exe_file").toString();

    auto output_dir = field("output_dir").toString();
    auto output_prefix_name = field("output_prefix_name").toString();

    auto can_sender = field("can_sender").toBool();
    auto serial_user = field("serial_user").toBool();

    auto ecu_address = field("ecu_address").toString().toUInt();

    std::map<std::string, int> stack_sizes;
    int pins_reader_period = -1;
    auto handlers = code_generator::util::read_def(def_file.toStdString());
    code_generator::util::SystemTasksInfo tasks_info(std::get<0>(handlers).size(), std::get<1>(handlers).size() != 0,
                                                     true, std::get<3>(handlers).declared,
                                                     std::get<2>(handlers).size() != 0, std::get<4>(handlers).declared,
                                                     true);
    if(tasks_info.can_send_task_used)
    {
        auto can_send_task_stack_size_str = field("can_send_task_stack_size").toString();
        stack_sizes["can_send_task"] = !can_send_task_stack_size_str.isEmpty() ? can_send_task_stack_size_str.toInt() : -1;
    }
    if(tasks_info.can_recv_task_used)
    {
        auto can_recv_task_stack_size_str = field("can_recv_task_stack_size").toString();
        stack_sizes["can_recv_task"] = !can_recv_task_stack_size_str.isEmpty() ? can_recv_task_stack_size_str.toInt() : -1;
    }
    if(tasks_info.pins_reader_task_used)
    {
        auto pins_reader_task_stack_size_str = field("pins_reader_task_stack_size").toString();
        stack_sizes["pins_reader_task"] = !pins_reader_task_stack_size_str.isEmpty() ? pins_reader_task_stack_size_str.toInt() : -1;
        auto pins_reader_task_period_str = field("pins_reader_task_period").toString();
        pins_reader_period = !pins_reader_task_period_str.isEmpty() ? pins_reader_task_period_str.toInt() : -1;
    }
    if(tasks_info.timer_task_used)
    {
        for(const auto& timer_task : std::get<0>(handlers))
        {
            const auto timer_task_str = tr("timer_task_%1").arg(timer_task.name.c_str());
            const auto timer_task_stack_size_field_name = tr("timer_task_%1_stack_size").arg(timer_task.name.c_str());
            auto timer_task_stack_size_str = field(timer_task_stack_size_field_name).toString();
            stack_sizes[timer_task_str.toStdString()] = !timer_task_stack_size_str.isEmpty() ? timer_task_stack_size_str.toInt() : -1;
        }
    }


    std::vector<code_generator::ast::TimerHandler> timer_handlers;
    std::vector<code_generator::ast::KeyHandler> key_handlers;
    std::vector<code_generator::ast::MessageHandlerPgnName> pgn_name_msgs;
    code_generator::ast::MessageHandlerPgnAll msg_handler_pgn_all;
    code_generator::ast::OnDllLoadHandler on_dll_load;
    std::tie(timer_handlers, key_handlers, pgn_name_msgs, msg_handler_pgn_all, on_dll_load) = code_generator::util::read_def(def_file.toStdString());

    QString pins_associated_to_keys = "{";
    bool first_pin = true;
    for(const auto& key_handler : key_handlers)
    {
        if(first_pin)
        {
            first_pin = false;
        } else
        {
            pins_associated_to_keys += ",";
        }
        auto key = QString::fromStdString(key_handler.key);
        auto pin_number = field(tr("key_%1_pin").arg(key)).toString();
        auto pin_type = field(tr("key_%1_pin_type").arg(key)).toString() == tr("digital") ? QString("d") : QString("a");
        auto pin_active_state = field(tr("key_%1_digital_pin_active_state").arg(key)).toString() == tr("1 (alto)") ? QString("h") : QString("l");
        pins_associated_to_keys +=
                "OnKey_"
                + key + ":"
                + "{"
                + "type:" + pin_type
                + ","
                + "number:" + pin_number
                + (pin_type == QString("d") ?
                    (QString(",active_state:") + pin_active_state) :
                    QString(""))
                + "}";
    }
    pins_associated_to_keys += "}";

    code_generator::CodeGenerator cd;
    code_generator::CodeGeneratorPropertiesManager(cd)
            .set_def_file_path(def_file)
            .set_dbf_file_path(dbf_file)
            .set_cpp_src_file_path(cpp_file)
            .set_trampoline_root_path(trampoline_root_dir)
            .set_goil_exe_path(goil_exe_file)
            .set_output_folder_path(output_dir)
            .set_output_cpp_file_name(output_prefix_name + ".cpp")
            .set_output_oil_file_name(output_prefix_name + ".oil")
            .set_output_msg_types_header_file_name("msg_types.h")
            .set_output_exe_file_name(output_prefix_name + "_bin")
            .set_key_mapping(pins_associated_to_keys.toStdString())
            .is_can_sender(can_sender)
            .use_serial_interface(serial_user)
            .set_ecu_address(ecu_address)
            .set_stack_sizes(stack_sizes)
            .set_pins_reader_task_period(pins_reader_period)
            .configure();
    QString build_output_str = cd.execute_build();
    setField("build_text_edit",build_output_str);
}

bool BuildPage::validatePage()
{
    auto output_dir = field("output_dir").toString();
    auto output_prefix_name = field("output_prefix_name").toString();
    QFileInfo cpp_file_info(QDir(output_dir).absoluteFilePath(output_prefix_name + ".cpp"));
    if(!cpp_file_info.exists())
    {
        QMessageBox msg;
        msg.setText(tr("Houveram erros na geração/compilação de código para a plataforma"));
        msg.exec();
        return false;
    }
    QFileInfo output_file_info(QDir(output_dir).absoluteFilePath(output_prefix_name + "_bin"));
    if(!output_file_info.exists() || output_file_info.lastModified() < cpp_file_info.lastModified())
    {
        QMessageBox msg;
        msg.setText(tr("Houveram erros na geração/compilação de código para a plataforma"));
        msg.exec();
        return false;
    }
    return true;
}

ComPortPage::ComPortPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Carregar executável"));
    setSubTitle(tr("Configuração para carregar executável na placa arduino"));

    com_port_label = new QLabel(tr("Seleciona a porta serial conectada a placa arduino"));
    com_port_label->setMinimumWidth(150);
    com_port_label->setWordWrap(true);
    com_port_select = new QComboBox;
    com_port_label->setBuddy(com_port_select);

    auto available_ports = QSerialPortInfo::availablePorts();
    for(const auto& port : available_ports)
    {
        com_port_select->addItem(port.portName());
    }

    com_ports_check_timer = new QTimer(this);
    connect(com_ports_check_timer,
            &QTimer::timeout,
            this,
            [this]()
            {
                auto current_text = com_port_select->currentText();
                com_port_select->clear();
                auto available_ports = QSerialPortInfo::availablePorts();
                for(const auto& port : available_ports)
                {
                    com_port_select->addItem(port.portName());
                }
                auto new_com_port_selected_index = com_port_select->findText(current_text);
                if(new_com_port_selected_index != -1)
                {
                    com_port_select->setCurrentIndex(new_com_port_selected_index);
                }
            });
    com_ports_check_timer->start(3000);

    registerField("com_port", com_port_select, "currentText", "currentTextChanged");

    setButtonText(QWizard::NextButton, tr("Carregar &executável"));

    auto *layout = new QGridLayout;
    layout->addWidget(com_port_label, 0, 0);
    layout->addWidget(com_port_select, 0, 1);
    setLayout(layout);
}

LoadPage::LoadPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Carregamento para o hardware"));
    setSubTitle(tr("Veja o resultado do processo de carregamento para a plataforma de hardware"));

    load_text_edit = new QTextEdit;
    load_text_edit->setReadOnly(true);
    registerField("load_text_edit",load_text_edit,"plainText","textChanged");
    auto *layout = new QVBoxLayout;
    layout->addWidget(load_text_edit);
    setLayout(layout);
}

void LoadPage::initializePage()
{
    auto output_dir = field("output_dir").toString();
    auto output_prefix_name = field("output_prefix_name").toString();
    auto arduino_type = field("arduino_type").toString();
    auto com_port = field("com_port").toString();

    code_generator::CodeGenerator cd;
    code_generator::CodeGeneratorPropertiesManager(cd)
            .set_output_folder_path(output_dir)
            .set_output_exe_file_name(output_prefix_name + "_bin")
            .set_com_port(com_port)
            .is_arduino_nano(arduino_type == "Arduino NANO");
    QString load_output_str = cd.execute_flash();
    setField("load_text_edit",load_output_str);
}

LastPage::LastPage(QWidget *parent)
    : QWizardPage(parent)
{

    setTitle(tr("Fim"));
    label = new QLabel(tr("O assistente terminou o serviço!"));
    label->setWordWrap(true);

    restart_button = new QPushButton(tr("Configurar nova ECU"));
    connect(restart_button, &QPushButton::clicked, [this](){
        dynamic_cast<CodeGeneratorWizard*>(this->wizard())->configure_new = true;
        this->wizard()->close();
    });

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    layout->addWidget(restart_button,0,Qt::AlignLeft);
    setLayout(layout);
}

int LastPage::nextId() const
{
    return -1;
}
