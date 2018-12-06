#include "code_generator_wizard.h"
#include <QApplication>
#include <QDebug>
#include <QCommandLineParser>
#include <QFileInfo>
#include <code_generator.h>
#include <QSplashScreen>
#include <QTimer>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setOrganizationName("code_generator");
    QApplication::setApplicationName("Code Generator");
    QApplication::setApplicationVersion("1.0");

    auto *splash_screen = new QSplashScreen;
    splash_screen->setPixmap(QPixmap(R"(:/images/SplashScreen.png)").scaled(800,500,Qt::KeepAspectRatio));
    splash_screen->show();

    auto cont = false;
    auto result = 0;
    {
        CodeGeneratorWizard cg(cont);
        QTimer::singleShot(2500,
                           [splash_screen, &cg]()
                           {
                                splash_screen->close();
                                cg.show();
                           });
        result = app.exec();
    }
    while(cont)
    {
        CodeGeneratorWizard cg(cont);
        cg.show();
        result = app.exec();
    }
    return result;
}
