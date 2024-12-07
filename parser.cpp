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

// This is the start of a rewrite of a gradual, in-place rewrite of Terminal.cpp.
// The idea is to move as much of the parsing as possible out of there, and into stateless
// functions that can (and should!) be easily unit tested.
// Whether or not such a rewrite will ever be finished, who knows...

#include "parser.h"
#include <QDebug>

template<typename T>
struct Appender
{
    Appender(T& container)
        : m_size(0)
        , m_container(container)
    {
    }

    int size() { return m_size; }

    void append(const QColor& color)
    {
        m_container[m_size++] = color.rgb();
    }

private:
    int m_size;
    T& m_container;
};

struct ColourTable
{
public:
    ColourTable()
    {
        auto colours = Appender<decltype(m_colourData)>(m_colourData);

        //normal
        colours.append(QColor(0, 0, 0).rgb());
        colours.append(QColor(210, 0, 0).rgb());
        colours.append(QColor(0, 210, 0).rgb());
        colours.append(QColor(210, 210, 0).rgb());
        colours.append(QColor(0, 0, 240).rgb());
        colours.append(QColor(210, 0, 210).rgb());
        colours.append(QColor(0, 210, 210).rgb());
        colours.append(QColor(235, 235, 235).rgb());

        //bright
        colours.append(QColor(127, 127, 127).rgb());
        colours.append(QColor(255, 0, 0).rgb());
        colours.append(QColor(0, 255, 0).rgb());
        colours.append(QColor(255, 255, 0).rgb());
        colours.append(QColor(92, 92, 255).rgb());
        colours.append(QColor(255, 0, 255).rgb());
        colours.append(QColor(0, 255, 255).rgb());
        colours.append(QColor(255, 255, 255).rgb());

        //colour cube
        for (int r = 0x00; r < 0x100;) {
            for (int g = 0x00; g < 0x100;) {
                for (int b = 0x00; b < 0x100;) {
                    colours.append(QColor(r, g, b).rgb());
                    b += b ? 0x28 : 0x5f;
                }
                g += g ? 0x28 : 0x5f;
            }
            r += r ? 0x28 : 0x5f;
        }

        //greyscale ramp
        for (int i = 0, g = 8; i < 24; i++, g += 10)
            colours.append(QColor(g, g, g).rgb());

        assert(colours.size() == 256);
    }

    QRgb at(uint8_t pos)
    {
        return m_colourData[pos];
    }

private:
    std::array<QRgb, 256> m_colourData;
};
Q_GLOBAL_STATIC(ColourTable, colourTable);

QRgb Parser::fetchDefaultFgColor(bool backgroundWhite)
{
    return colourTable()->at(backgroundWhite ? 0 : 7);
}

QRgb Parser::fetchDefaultBgColor(bool backgroundWhite)
{
    return colourTable()->at(backgroundWhite ? 7 : 0);
}

bool Parser::handleSGR(Parser::SGRParserState& state, const QList<int>& params, QString& errorString)
{
    int pidx = 0;

    // NOTE: Previously, this code would try to deal with invalid input, but I don't think that's wise.
    // It will now discard anything that is malformed, and not try to look for subsequent correct messages.
    // If this is too strict, then it may need to be revisited, but I think this is saner.
    while (pidx < params.count()) {
        int p = params.at(pidx++);
        switch (p) {
        case 0:
            state.colours.fg = state.colours.defaultFg;
            state.colours.bg = state.colours.defaultBg;
            state.currentAttributes = Parser::NoAttributes;
            break;
        case 1:
            state.currentAttributes |= Parser::BoldAttribute;
            break;
        case 2:
            // TODO: Faint ("half bright")
            break;
        case 3:
            state.currentAttributes |= Parser::ItalicAttribute;
            break;
        case 4:
            state.currentAttributes |= Parser::UnderlineAttribute;
            break;
        case 5:
            state.currentAttributes |= Parser::BlinkAttribute;
            break;
        case 6:
            // This is supposed to be a "fast blink"? what is that?
            state.currentAttributes |= Parser::BlinkAttribute;
            break;
        case 7:
            state.currentAttributes |= Parser::NegativeAttribute;
            break;
        case 8:
            // TODO: Invisible..?
            break;
        case 9:
            // TODO: strikethrough
            break;
        case 21:
            // TODO: double underline..?
            break;
        case 22:
            state.currentAttributes &= ~Parser::BoldAttribute;
            break;
        case 23:
            state.currentAttributes &= ~Parser::ItalicAttribute;
            break;
        case 24:
            state.currentAttributes &= ~Parser::UnderlineAttribute;
            break;
        case 25:
            state.currentAttributes &= ~Parser::BlinkAttribute;
            break;
        case 26:
            // "fast blink" off...
            state.currentAttributes &= ~Parser::BlinkAttribute;
            break;
        case 27:
            state.currentAttributes &= ~Parser::NegativeAttribute;
            break;
        case 28:
            // TODO: visible..?
            break;
        case 29:
            // TODO: !strikethrough
            break;

        case 30: // fg black
        case 31:
        case 32:
        case 33:
        case 34:
        case 35:
        case 36:
        case 37:
            if (state.colours.fg & Parser::BoldAttribute)
                p += 8;
            state.colours.fg = colourTable()->at(p - 30);
            break;

        case 39: // fg default
            state.colours.fg = state.colours.defaultFg;
            break;

        case 40: // bg black
        case 41:
        case 42:
        case 43:
        case 44:
        case 45:
        case 46:
        case 47:
            state.colours.bg = colourTable()->at(p - 40);
            break;

        case 49: // bg default
            state.colours.bg = state.colours.defaultBg;
            break;

        case 90: // fg black, bold/bright, nonstandard
        case 91:
        case 92:
        case 93:
        case 94:
        case 95:
        case 96:
        case 97:
            state.colours.fg = colourTable()->at(p - 90 + 8);
            break;

        case 100: // fg black, bold/bright, nonstandard
        case 101:
        case 102:
        case 103:
        case 104:
        case 105:
        case 106:
        case 107:
            state.colours.fg = colourTable()->at(p - 100 + 8);
            break;

        case 38:
        case 48: {
            if (pidx >= params.count()) {
                errorString = "got invalid extended SGR (no type)";
                return false;
            }

            bool isForeground = p == 38;
            int ctype = params.at(pidx++);

            switch (ctype) {
            case 5: {
                // 5: 256 colours (xterm)
                if (pidx >= params.count()) {
                    errorString = "got invalid 256color SGR (no color)";
                    return false;
                }

                int colorIndex = params.at(pidx++);
                if (colorIndex < 0 || colorIndex >= 256) {
                    errorString = QString::fromLatin1("got invalid 256color SGR with out-of-range color: %1").arg(colorIndex);
                    return false;
                }

                if (isForeground) {
                    // Only apply bold attribute for standard 16-color; 256-color doesn't have bold
                    if (colorIndex < 9 && state.currentAttributes & Parser::BoldAttribute)
                        colorIndex += 8;
                    state.colours.fg = colourTable()->at(colorIndex);
                } else {
                    state.colours.bg = colourTable()->at(colorIndex);
                }
                break;
            }
            case 2: {
                // 2: 16-bit colours
                // r;g;b
                if (params.count() - pidx < 3) {
                    errorString = QString::fromLatin1("got invalid 16bit SGR with too few parameters: %1").arg(params.count() - pidx);
                    return false;
                }

                int r = params.at(pidx++);
                int g = params.at(pidx++);
                int b = params.at(pidx++);

                // Ignore any invalid component.
                if (r < 0 || r >= 256) {
                    errorString = QString::fromLatin1("got invalid 16bit SGR with out-of-range r: %1").arg(r);
                    return false;
                }
                if (g < 0 || g >= 256) {
                    errorString = QString::fromLatin1("got invalid 16bit SGR with out-of-range g: %1").arg(g);
                    return false;
                }
                if (b < 0 || b >= 256) {
                    errorString = QString::fromLatin1("got invalid 16bit SGR with out-of-range b: %1").arg(b);
                    return false;
                }

                if (isForeground)
                    state.colours.fg = QColor(r, g, b).rgb();
                else
                    state.colours.bg = QColor(r, g, b).rgb();
                break;
            }
            default:
                errorString = QString::fromLatin1("got unknown extended SGR: %1").arg(ctype);
                return false;
            }
            break;
        }
        default:
            errorString = QString::fromLatin1("got unknown SGR: %1").arg(p);
            return false;
        }
    }

    return true;
}

