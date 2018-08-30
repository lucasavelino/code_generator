#ifndef CODE_GENERATOR_WIZARD_H
#define CODE_GENERATOR_WIZARD_H

#include <QWizard>
#include <QtSerialPort/QSerialPortInfo>

QT_BEGIN_NAMESPACE
class QCheckBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QRadioButton;
class QPushButton;
class QFileDialog;
class QComboBox;
class QTextBrowser;
class QTimer;
QT_END_NAMESPACE

class CodeGeneratorWizard : public QWizard
{
    Q_OBJECT

public:
    explicit CodeGeneratorWizard(QWidget *parent = nullptr);
    void accept() override;
};

class IntroPage : public QWizardPage
{
    Q_OBJECT

public:
    IntroPage(QWidget *parent = nullptr);

private:
    QLabel *label;
};

class BusmasterGeneratedInputFilesPage : public QWizardPage
{
    Q_OBJECT

public:
    BusmasterGeneratedInputFilesPage(QWidget *parent = nullptr);
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

private:
    QLabel *output_folder_label;
    QLabel *oil_file_label;
    QLabel *cpp_file_label;
    QLabel *msg_types_header_file_label;
    QLineEdit *output_folder_line_edit;
    QLineEdit *oil_file_line_edit;
    QLineEdit *cpp_file_line_edit;
    QLineEdit *msg_types_header_file_line_edit;
    QPushButton *output_folder_open_button;
};

class PinsConfigPage : public QWizardPage
{
    Q_OBJECT

public:
    PinsConfigPage(QWidget *parent = nullptr);

private:
    QComboBox *arduino_select;
    QLabel *arduino_img_label;
    QLabel *arduino_pins_label[20];
    QCheckBox *arduino_pins_checkbox[20];
    QLabel *keys_label[20];
    QLineEdit *arduino_pins_line_edit[20];
};

class BuildPage : public QWizardPage
{
    Q_OBJECT

public:
    BuildPage(QWidget *parent = nullptr);

private:
    QTextBrowser *build_text_browser;
};

class LoadPage : public QWizardPage
{
    Q_OBJECT

public:
    LoadPage(QWidget *parent = nullptr);

private:
    QLabel *com_port_label;
    QComboBox *com_port_select;
    QTextBrowser *load_text_browser;
    QTimer *com_ports_check_timer;
};

class LastPage : public QWizardPage
{
    Q_OBJECT

public:
    LastPage(QWidget *parent = nullptr);

private:
    QLabel *label;
};

#endif // CODE_GENERATOR_WIZARD_H
