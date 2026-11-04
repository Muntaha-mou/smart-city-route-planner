// Smart City Route & Infrastructure Planner
// Copyright © 2026 Quad Core. All rights reserved.

#ifndef SCP_UI_MAIN_WINDOW_H
#define SCP_UI_MAIN_WINDOW_H

#include <QMainWindow>

class QWidget;

// Top-level application window.
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    QWidget* createHeader();
    QWidget* createWelcomePanel();
};

#endif // SCP_UI_MAIN_WINDOW_H
