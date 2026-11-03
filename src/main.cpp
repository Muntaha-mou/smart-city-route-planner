// Smart City Route & Infrastructure Planner
// Copyright © 2026 Quad Core. All rights reserved.

#include "core/project_info.h"
#include "ui/main_window.h"

#include <QApplication>
#include <QFile>
#include <QFont>

namespace {

// Reads the application stylesheet from the Qt resource system.
QString loadStyleSheet()
{
    QFile file(":/style.qss");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return {};
    }
    return QString::fromUtf8(file.readAll());
}

} // namespace

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(QString::fromStdString(scp::applicationName()));
    QApplication::setApplicationVersion(QString::fromStdString(scp::version()));
    QApplication::setOrganizationName(QString::fromStdString(scp::teamName()));

    // Inter if installed, otherwise the Windows system font Segoe UI.
    QFont font;
    font.setFamilies({"Inter", "Segoe UI"});
    font.setPointSize(10);
    QApplication::setFont(font);

    app.setStyleSheet(loadStyleSheet());

    MainWindow window;
    window.show();
    return app.exec();
}
