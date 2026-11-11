// Smart City Route & Infrastructure Planner
// Copyright © 2026 Quad Core. All rights reserved.

#include "ui/main_window.h"

#include "core/project_info.h"

#include <QFrame>
#include <QLabel>
#include <QStatusBar>
#include <QVBoxLayout>

namespace {

QString toQString(const std::string& text)
{
    return QString::fromStdString(text);
}

} // namespace

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle(toQString(scp::applicationName()));
    setMinimumSize(1024, 680);
    resize(1280, 800);

    auto* central = new QWidget(this);
    auto* layout = new QVBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(createHeader());
    layout->addWidget(createWelcomePanel(), 1);
    setCentralWidget(central);

    statusBar()->addPermanentWidget(new QLabel(toQString(scp::copyrightNotice())));
}

QWidget* MainWindow::createHeader()
{
    auto* header = new QFrame;
    header->setObjectName("appHeader");

    auto* title = new QLabel(toQString(scp::applicationName()));
    title->setObjectName("appTitle");

    auto* subtitle = new QLabel("Developed by Team " + toQString(scp::teamName()));
    subtitle->setObjectName("appSubtitle");

    auto* layout = new QVBoxLayout(header);
    layout->setContentsMargins(24, 16, 24, 16);
    layout->setSpacing(2);
    layout->addWidget(title);
    layout->addWidget(subtitle);
    return header;
}

QWidget* MainWindow::createWelcomePanel()
{
    auto* panel = new QFrame;
    panel->setObjectName("contentArea");

    auto* heading = new QLabel("Welcome");
    heading->setObjectName("pageHeading");

    auto* description = new QLabel(toQString(scp::projectDescription()));
    description->setObjectName("bodyText");
    description->setWordWrap(true);
    description->setMaximumWidth(720);

    auto* versionLabel = new QLabel("Version " + toQString(scp::version()));
    versionLabel->setObjectName("mutedText");

    auto* layout = new QVBoxLayout(panel);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(12);
    layout->addWidget(heading);
    layout->addWidget(description);
    layout->addWidget(versionLabel);
    layout->addStretch();
    return panel;
}
