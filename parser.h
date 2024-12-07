/*
    Copyright (C) 2020 Crimson AS <info@crimson.no>

    Permission is hereby granted, free of charge, to any person obtaining a copy of
    this software and associated documentation files (the "Software"), to deal in
    the Software without restriction, including without limitation the rights to
    use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
    of the Software, and to permit persons to whom the Software is furnished to
    do so, subject to the following conditions:

        The above copyright notice and this permission notice shall be included
        in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
    FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
    COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
    IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
    WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once
#include <QColor>

#if !defined(TEST_MODE)

// Avoid catch2 trying to hijack our main
#    define CATCH_CONFIG_RUNNER

#endif

namespace Parser {
enum TextAttribute
{
    NoAttributes = 0x00,
    BoldAttribute = 0x01,
    ItalicAttribute = 0x02,
    UnderlineAttribute = 0x04,
    NegativeAttribute = 0x08,
    BlinkAttribute = 0x10
};
Q_DECLARE_FLAGS(TextAttributes, TextAttribute)

QRgb fetchDefaultFgColor(bool backgroundWhite);
QRgb fetchDefaultBgColor(bool backgroundWhite);

struct SGRParserState
{
    SGRParserState(QRgb& fg, QRgb& bg, QRgb defaultFg, QRgb defaultBg, TextAttributes& currentAttributes)
        : colours(Colours { fg, defaultFg, bg, defaultBg })
        , currentAttributes(currentAttributes)
    {
    }
    struct Colours
    {
        QRgb& fg;
        QRgb defaultFg;
        QRgb& bg;
        QRgb defaultBg;
    } colours;

    TextAttributes& currentAttributes;
};

bool handleSGR(SGRParserState& state, const QList<int>& params, QString& errorString);
}

Q_DECLARE_OPERATORS_FOR_FLAGS(Parser::TextAttributes)
