// Smart City Route & Infrastructure Planner
// Copyright © 2026 Quad Core. All rights reserved.
//
// ui.h - colours, borders and layout helpers for the console screens.
//
// Only main.cpp uses this file. The Graph, storage and algorithm code never
// print anything, so they do not depend on it.
//
// Colours are made with ANSI escape codes (special text such as "\x1b[31m"
// that the terminal turns into a colour). On Windows they work after
// "virtual terminal processing" is switched on, which initConsole() does.
// If that is not possible, or the output is redirected to a file, the program
// runs in PLAIN mode: no colours, no screen clearing, and ASCII borders.

#ifndef UI_H
#define UI_H

#include <string>
#include <vector>

// The colours of the theme. paint() turns text into coloured text.
enum class Color {
    Normal,     // light grey-blue: normal text
    Title,      // bright white, bold: titles and headings
    Cyan,       // main accent colour
    Blue,       // second accent colour (IDs, numbers)
    Purple,     // small accents
    Muted,      // grey: borders, hints
    Success,    // green
    Error,      // red
    Warning,    // yellow
    Input       // bright white: what the user types
};

// Width of the screen layout in characters.
const int SCREEN_WIDTH = 72;

// ----- Console setup -----

// Call once at the start of main(). Switches on colours and UTF-8 output if
// possible. `forcePlain` = true always uses plain mode (option --plain).
void initConsole(bool forcePlain);

bool colorsOn();          // true in colour mode, false in plain mode
bool isInteractive();     // true if a person is typing at a console

// ----- Text helpers -----

std::string paint(const std::string& text, Color color);

// Number of characters the text takes on screen (a UTF-8 character such as
// "─" is several bytes but one character wide).
int displayWidth(const std::string& text);

// Adds spaces on the right until the text is `width` characters wide.
// Text that is too long is shortened and ends with "...".
std::string fitText(const std::string& text, int width);

// A number with 2 decimals, e.g. 4.5 -> "4.50".
std::string formatNumber(double value);

// ----- Screen building blocks -----

void clearScreen();                      // only in colour mode with a person typing
void printBanner();                      // the big title box of the main menu
void printScreenHeader(const std::string& section, const std::string& title);
void printSectionTitle(const std::string& title, Color color);
void printLine();                        // a thin line across the screen
void printBlankLine();

// One table row: each cell is fitted to its width. `color` is used for all cells.
void printTableRow(const std::vector<std::string>& cells, const std::vector<int>& widths,
                   Color color);

// Prints "A -> B -> C", wrapping onto new lines when the line gets too long.
void printChain(const std::vector<std::string>& items, Color color);

// ----- Messages -----

void printSuccess(const std::string& message);   // [ OK ]   green
void printError(const std::string& message);     // [ERROR]  red
void printWarning(const std::string& message);   // [ WARN ] yellow
void printInfo(const std::string& message);      // [ INFO ] cyan
void printHint(const std::string& message);      // grey text

// ----- Input -----

// The text shown before the user types, e.g. "  > Location name: ".
// The typed text appears in the Input colour.
std::string promptText(const std::string& question);

// After reading a line: switches back from the Input colour.
void endInput();

// Shows `message` and waits for Enter (only when a person is typing).
void waitForEnter(const std::string& message = "Press Enter to return to the main menu...");

// The arrow used between locations: "→" in colour mode, "->" in plain mode.
std::string arrow();

// Before exiting: clears the navy screen and switches back to the terminal's
// own colours, so the terminal looks normal after the program ends.
void leaveTheme();

// Restores the normal console colours. Called automatically when the
// program ends.
void restoreConsole();

#endif // UI_H
