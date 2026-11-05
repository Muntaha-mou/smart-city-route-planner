// Smart City Route & Infrastructure Planner
// Copyright © 2026 Quad Core. All rights reserved.
//
// ui.cpp - colours, borders and layout helpers (see ui.h).

#include "ui.h"

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN    // only the basic parts of windows.h
#endif
#ifndef NOMINMAX
#define NOMINMAX               // windows.h must not define min/max macros
#endif
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#else
#include <unistd.h>
#endif

using namespace std;

// ===== State (only used in this file) =====

static bool colorMode = false;     // colours, UTF-8 borders, screen clearing
static bool interactive = false;   // a person is typing at a console

// True while the console settings are changed and must be restored.
// Ctrl+C is handled on a different thread, so the main thread (ending the
// program) and the Ctrl+C thread may both try to restore at the same time.
// The mutex makes the second one wait until the first has completely
// finished, so the program never ends in the middle of restoring.
static bool consoleChanged = false;
static mutex restoreMutex;

#ifdef _WIN32
// The console settings before the program changed them.
static HANDLE outputHandle = nullptr;
static DWORD originalOutputMode = 0;
static UINT originalOutputCodePage = 0;
#endif

// ===== Colours =====
//
// "\x1b[" starts an ANSI code. "38;2;R;G;B" sets the text colour and
// "48;2;R;G;B" the background colour (red, green, blue from 0 to 255).

// The theme's normal style: light grey-blue text on a dark navy background.
// Every coloured piece of text switches back to this style afterwards, so the
// navy background is never lost.
static const string THEME_STYLE = "\x1b[0;38;2;205;214;235;48;2;12;20;46m";
static string BASE_STYLE = THEME_STYLE;   // becomes "\x1b[0m" after leaveTheme()

// The colours above only colour character cells. The terminal fills
// everything else (the window padding, the part-row at the bottom, areas
// that appear when the window is resized) with its own DEFAULT background.
// "OSC 11" (\x1b]11;...) changes that default background to the same navy,
// and "OSC 111" (\x1b]111) gives the terminal its own default back.
// Terminals that do not know these codes simply ignore them.
static const string SET_DEFAULT_BACKGROUND = "\x1b]11;rgb:0c/14/2e\x1b\\";
static const string RESET_DEFAULT_BACKGROUND = "\x1b]111\x1b\\";

// Everything needed to give the terminal its own colours back.
static const string RESET_ALL = RESET_DEFAULT_BACKGROUND + "\x1b[0m";

static string colorCode(Color color)
{
    switch (color) {
    case Color::Normal:  return "";
    case Color::Title:   return "\x1b[1;38;2;255;255;255m";
    case Color::Cyan:    return "\x1b[38;2;0;212;255m";
    case Color::Blue:    return "\x1b[38;2;100;160;255m";
    case Color::Purple:  return "\x1b[38;2;185;140;255m";
    case Color::Muted:   return "\x1b[38;2;110;125;165m";
    case Color::Success: return "\x1b[1;38;2;80;220;120m";
    case Color::Error:   return "\x1b[1;38;2;255;95;95m";
    case Color::Warning: return "\x1b[1;38;2;255;205;70m";
    case Color::Input:   return "\x1b[1;38;2;255;255;255m";
    }
    return "";
}

string paint(const string& text, Color color)
{
    if (!colorMode || color == Color::Normal) {
        return text;
    }
    return colorCode(color) + text + BASE_STYLE;
}

// ===== Border characters =====
// Box-drawing characters in colour mode, plain ASCII otherwise.

static string lineChar()     { return colorMode ? "─" : "-"; }
static string doubleChar()   { return colorMode ? "═" : "="; }
static string sideChar()     { return colorMode ? "║" : "|"; }
static string topLeft()      { return colorMode ? "╔" : "+"; }
static string topRight()     { return colorMode ? "╗" : "+"; }
static string bottomLeft()   { return colorMode ? "╚" : "+"; }
static string bottomRight()  { return colorMode ? "╝" : "+"; }

string arrow() { return colorMode ? "→" : "->"; }

// The same character (or short text) repeated `count` times.
static string repeat(const string& text, int count)
{
    string result;
    for (int i = 0; i < count; i++) {
        result += text;
    }
    return result;
}

// ===== Console setup =====

// Gives the terminal its own colours and settings back. Runs only once, even
// if it is called from the normal exit and from Ctrl+C at the same time.
// `fromCtrlC` = true: called on the Ctrl+C thread, where cout must not be
// used (the main thread may be using it), so the text is written directly.
static void restoreNow(bool fromCtrlC)
{
    lock_guard<mutex> lock(restoreMutex);   // waits if the other thread is restoring
    if (!consoleChanged) {
        return;   // already restored (or never changed)
    }
    consoleChanged = false;

#ifdef _WIN32
    if (!fromCtrlC) {
        cout << flush;   // write everything still waiting in cout first
    }
    // The reset codes (default background and text colours) must be written
    // while ANSI codes are still switched on.
    DWORD written = 0;
    WriteConsoleA(outputHandle, RESET_ALL.c_str(), static_cast<DWORD>(RESET_ALL.size()),
                  &written, nullptr);
    SetConsoleMode(outputHandle, originalOutputMode);   // the original output mode
    SetConsoleOutputCP(originalOutputCodePage);          // the original code page
#else
    (void)fromCtrlC;
    cout << RESET_ALL << flush;
#endif
}

#ifdef _WIN32
// Called by Windows when Ctrl+C (or Ctrl+Break, or closing the window) happens:
// restore the console, then let Windows close the program as usual (FALSE).
static BOOL WINAPI onCtrlC(DWORD)
{
    restoreNow(true);
    return FALSE;
}
#endif

void initConsole(bool forcePlain)
{
    // NO_COLOR is a common convention for "please do not use colours".
    bool noColorRequested = forcePlain || getenv("NO_COLOR") != nullptr;

#ifdef _WIN32
    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE input = GetStdHandle(STD_INPUT_HANDLE);
    DWORD outputMode = 0;
    DWORD inputMode = 0;
    bool outputIsConsole = GetConsoleMode(output, &outputMode) != 0;
    bool inputIsConsole = GetConsoleMode(input, &inputMode) != 0;
    interactive = outputIsConsole && inputIsConsole;

    if (outputIsConsole && !noColorRequested) {
        // Remember the settings BEFORE changing them, so they can be restored.
        outputHandle = output;
        originalOutputMode = outputMode;
        originalOutputCodePage = GetConsoleOutputCP();

        // Switch on ANSI colour codes. This fails on very old Windows versions;
        // the program then simply stays in plain mode (nothing was changed).
        if (SetConsoleMode(output, outputMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
            SetConsoleOutputCP(CP_UTF8);   // needed for the box-drawing characters
            colorMode = true;
            consoleChanged = true;
            SetConsoleCtrlHandler(onCtrlC, TRUE);
        }
    }
#else
    interactive = isatty(STDOUT_FILENO) && isatty(STDIN_FILENO);
    colorMode = isatty(STDOUT_FILENO) && !noColorRequested;
    consoleChanged = colorMode;
#endif

    if (colorMode) {
        atexit(restoreConsole);   // also runs after exit(0) and at the end of main()
        // Navy for the whole window (default background), then the theme style.
        cout << SET_DEFAULT_BACKGROUND << BASE_STYLE;
    }
}

void restoreConsole()
{
    restoreNow(false);
}

void leaveTheme()
{
    if (!colorMode) {
        return;
    }
    // From now on, coloured text goes back to the terminal's own colours
    // instead of the navy background.
    BASE_STYLE = "\x1b[0m";
    cout << RESET_ALL;   // the terminal's own default background and colours
    if (interactive) {
        cout << "\x1b[2J\x1b[H" << flush;   // clear the visible navy screen
    }
}

bool colorsOn() { return colorMode; }
bool isInteractive() { return interactive; }

// ===== Text helpers =====

int displayWidth(const string& text)
{
    // In UTF-8, every byte of the form 10xxxxxx continues the previous
    // character, so only the other bytes start a new character.
    int width = 0;
    for (unsigned char c : text) {
        if ((c & 0xC0) != 0x80) {
            width++;
        }
    }
    return width;
}

string fitText(const string& text, int width)
{
    int length = displayWidth(text);
    if (length <= width) {
        return text + string(width - length, ' ');
    }
    // Too long: keep the first characters and add "...".
    // (Names are plain text, so cutting by bytes is safe for them.)
    if (width <= 3) {
        return text.substr(0, width);
    }
    return text.substr(0, width - 3) + "...";
}

string formatNumber(double value)
{
    ostringstream text;
    text << fixed << setprecision(2) << value;
    return text.str();
}

// Puts text in the middle of a field `width` characters wide.
static string centered(const string& text, int width)
{
    int space = width - displayWidth(text);
    if (space <= 0) {
        return text;
    }
    int left = space / 2;
    return string(left, ' ') + text + string(space - left, ' ');
}

// ===== Screen building blocks =====

void clearScreen()
{
    if (colorMode && interactive) {
        // \x1b[2J clears the visible screen and \x1b[H moves the cursor to the
        // top-left corner. The scroll-back history is kept, so earlier
        // screens can still be read by scrolling up. The cleared rows take
        // the current (navy) background from BASE_STYLE; the area outside the
        // rows is navy because of SET_DEFAULT_BACKGROUND (see initConsole).
        cout << BASE_STYLE << "\x1b[2J\x1b[H" << flush;
    } else {
        cout << "\n";
    }
}

// One line inside the banner box, centred.
static void bannerLine(const string& text, Color color)
{
    int inner = SCREEN_WIDTH - 4;
    cout << "  " << paint(sideChar(), Color::Cyan) << paint(centered(text, inner), color)
         << paint(sideChar(), Color::Cyan) << "\n";
}

void printBanner()
{
    int inner = SCREEN_WIDTH - 4;
    cout << "\n  " << paint(topLeft() + repeat(doubleChar(), inner) + topRight(), Color::Cyan) << "\n";
    bannerLine("", Color::Normal);
    bannerLine("SMART CITY ROUTE & INFRASTRUCTURE PLANNER", Color::Title);
    bannerLine(colorMode ? "Algorithms Lab – CSE 232" : "Algorithms Lab - CSE 232", Color::Cyan);
    bannerLine("Team Quad Core", Color::Purple);
    bannerLine("", Color::Normal);
    cout << "  " << paint(bottomLeft() + repeat(doubleChar(), inner) + bottomRight(), Color::Cyan)
         << "\n";
}

void printScreenHeader(const string& section, const string& title)
{
    string separator = colorMode ? "  ›  " : "  >  ";
    cout << "\n  " << paint("SMART CITY PLANNER", Color::Cyan) << paint(separator, Color::Muted)
         << paint(section, Color::Purple) << paint(separator, Color::Muted)
         << paint(title, Color::Title) << "\n";
    cout << "  " << paint(repeat(doubleChar(), SCREEN_WIDTH - 2), Color::Blue) << "\n\n";
}

void printSectionTitle(const string& title, Color color)
{
    cout << "  " << paint(title, color) << "\n";
    cout << "  " << paint(repeat(lineChar(), displayWidth(title) + 4), Color::Muted) << "\n";
}

void printLine()
{
    cout << "  " << paint(repeat(lineChar(), SCREEN_WIDTH - 2), Color::Muted) << "\n";
}

void printBlankLine()
{
    cout << "\n";
}

void printTableRow(const vector<string>& cells, const vector<int>& widths, Color color)
{
    string row;
    for (size_t i = 0; i < cells.size() && i < widths.size(); i++) {
        row += fitText(cells[i], widths[i]);
    }
    cout << "  " << paint(row, color) << "\n";
}

void printChain(const vector<string>& items, Color color)
{
    const int maxWidth = SCREEN_WIDTH - 4;
    string separator = " " + arrow() + " ";
    int lineWidth = 0;

    cout << "  ";
    for (size_t i = 0; i < items.size(); i++) {
        string piece = items[i];
        int pieceWidth = displayWidth(piece) + (i > 0 ? displayWidth(separator) : 0);
        // Start a new line if this location does not fit any more.
        if (i > 0 && lineWidth + pieceWidth > maxWidth) {
            cout << paint(separator, Color::Muted) << "\n  ";
            lineWidth = 0;
            pieceWidth = displayWidth(piece);
        } else if (i > 0) {
            cout << paint(separator, Color::Muted);
        }
        cout << paint(piece, color);
        lineWidth += pieceWidth;
    }
    cout << "\n";
}

// ===== Messages =====

void printSuccess(const string& message)
{
    cout << "  " << paint("[ OK ]", Color::Success) << " " << paint(message, Color::Success) << "\n";
}

void printError(const string& message)
{
    cout << "  " << paint("[ERROR]", Color::Error) << " " << paint(message, Color::Error) << "\n";
}

void printWarning(const string& message)
{
    cout << "  " << paint("[ WARN ]", Color::Warning) << " " << message << "\n";
}

void printInfo(const string& message)
{
    cout << "  " << paint("[ INFO ]", Color::Cyan) << " " << message << "\n";
}

void printHint(const string& message)
{
    cout << "  " << paint(message, Color::Muted) << "\n";
}

// ===== Input =====

string promptText(const string& question)
{
    string pointer = colorMode ? "▸ " : "> ";
    if (!colorMode) {
        return "  " + pointer + question;
    }
    // The typed text keeps the Input colour until endInput() is called.
    return "  " + paint(pointer, Color::Cyan) + question + colorCode(Color::Input);
}

void endInput()
{
    if (colorMode) {
        cout << BASE_STYLE;
    }
}

void waitForEnter(const string& message)
{
    if (!interactive) {
        return;
    }
    cout << "\n  " << paint(message, Color::Muted);
    string line;
    if (!getline(cin, line)) {
        exit(0);   // input ended; restoreConsole() runs automatically
    }
}
