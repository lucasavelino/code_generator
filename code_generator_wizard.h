#ifndef CODE_GENERATOR_WIZARD_H
#define CODE_GENERATOR_WIZARD_H

#include <QWizard>
#include <QtSerialPort/QSerialPortInfo>
#include <QMessageBox>
#include <code_generator.h>

QT_BEGIN_NAMESPACE
class QCheckBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QRadioButton;
class QPushButton;
class QFileDialog;
class QComboBox;
class QTextEdit;
class QTimer;
class QCheckBox;
class QIntValidator;
QT_END_NAMESPACE

class CodeGeneratorWizard : public QWizard
{
    Q_OBJECT

public:
    explicit CodeGeneratorWizard(bool& _configure_new, QWidget *parent = nullptr);
    void accept() override;

    bool& configure_new;

protected:
    void closeEvent(QCloseEvent* event);
};

class IntroPage : public QWizardPage
{
    Q_OBJECT

public:
    IntroPage(QWidget *parent = nullptr);

private:
    QLabel *label;
};

class LoadConfigsPage : public QWizardPage
{
    Q_OBJECT

public:
    LoadConfigsPage(QWidget *parent = nullptr);

private:
    QLabel *load_configs_label;
    QPushButton *load_configs_button;
};

class BusmasterGeneratedInputFilesPage : public QWizardPage
{
    Q_OBJECT

public:
    BusmasterGeneratedInputFilesPage(QWidget *parent = nullptr);
    bool validatePage() override;
private:
    QLabel *def_file_label;
    QLabel *dbf_file_label;
    QLabel *cpp_file_label;
    QLineEdit *def_file_line_edit;
    QLineEdit *dbf_file_line_edit;
    QLineEdit *cpp_file_line_edit;
    QPushButton *def_file_open_button;
    QPushButton *dbf_file_open_button;
    QPushButton *cpp_file_open_button;
};

class TrampolineRTOSConfigsPage : public QWizardPage
{
    Q_OBJECT

public:
    TrampolineRTOSConfigsPage(QWidget *parent = nullptr);
    bool validatePage() override;
private:
    QLabel *trampoline_root_label;
    QLabel *goil_exe_label;
    QLineEdit *trampoline_root_line_edit;
    QLineEdit *goil_exe_line_edit;
    QPushButton *trampoline_root_open_button;
    QPushButton *goil_exe_open_button;
};

class OutputConfigsPage : public QWizardPage
{
    Q_OBJECT

public:
    OutputConfigsPage(QWidget *parent = nullptr);
    bool validatePage() override;
private:
    QLabel *output_folder_label;
    QLabel *output_prefix_label;
    QLineEdit *output_folder_line_edit;
    QLineEdit *output_prefix_line_edit;
    QPushButton *output_folder_open_button;
};

class PinsConfigPage : public QWizardPage
{
    Q_OBJECT

public:
    PinsConfigPage(QWidget *parent = nullptr);
    void initializePage() override;
    bool validatePage() override;
private:
    QComboBox *arduino_select;
    QLabel *arduino_img_label;
};

class CodeConfigPage : public QWizardPage
{
    Q_OBJECT

public:
    CodeConfigPage(QWidget *parent = nullptr);
    void initializePage() override;
    bool validatePage() override;
private:
    QCheckBox *can_sender_checkbox;
    QCheckBox *serial_user_checkbox;
    QLabel* ecu_address_label;
    QLineEdit* ecu_address_line_edit;
    code_generator::util::HexByteValidator *ecu_address_line_validator;
};

class BuildPage : public QWizardPage
{
    Q_OBJECT

public:
    BuildPage(QWidget *parent = nullptr);
    void initializePage() override;
    bool validatePage() override;
private:
    QTextEdit *build_text_edit;
};

class ComPortPage : public QWizardPage
{
    Q_OBJECT

public:
    ComPortPage(QWidget *parent = nullptr);

private:
    QLabel *com_port_label;
    QComboBox *com_port_select;
    QTimer *com_ports_check_timer;
};

class LoadPage : public QWizardPage
{
    Q_OBJECT

public:
    LoadPage(QWidget *parent = nullptr);
    void initializePage() override;
private:
    QTextEdit *load_text_edit;
};

class LastPage : public QWizardPage
{
    Q_OBJECT

public:
    LastPage(QWidget *parent = nullptr);
    int nextId() const;
private:
    QLabel *label;
    QPushButton *restart_button;
};

#endif // CODE_GENERATOR_WIZARD_H
