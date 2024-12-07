/*
    Copyright 2011-2012 Heikki Holstila <heikki.holstila@gmail.com>

    This work is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This work is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this work.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TERMINAL_H
#define TERMINAL_H

#include <QObject>
#include <QRect>
#include <QRgb>
#include <QVector>

#include "ptyiface.h"

struct TermChar
{
    // TODO: Replace with the version in Parser.
    enum TextAttributes
    {
        NoAttributes = 0x00,
        BoldAttribute = 0x01,
        ItalicAttribute = 0x02,
        UnderlineAttribute = 0x04,
        NegativeAttribute = 0x08,
        BlinkAttribute = 0x10
    };

    QChar c;
    QRgb fgColor;
    QRgb bgColor;
    TextAttributes attrib;
};
inline TermChar::TextAttributes operator~(TermChar::TextAttributes a) { return (TermChar::TextAttributes) ~(int)a; }
inline TermChar::TextAttributes operator|(TermChar::TextAttributes a, TermChar::TextAttributes b) { return (TermChar::TextAttributes)((int)a | (int)b); }
inline TermChar::TextAttributes operator&(TermChar::TextAttributes a, TermChar::TextAttributes b) { return (TermChar::TextAttributes)((int)a & (int)b); }
inline TermChar::TextAttributes operator^(TermChar::TextAttributes a, TermChar::TextAttributes b) { return (TermChar::TextAttributes)((int)a ^ (int)b); }
inline TermChar::TextAttributes& operator|=(TermChar::TextAttributes& a, TermChar::TextAttributes b) { return (TermChar::TextAttributes&)((int&)a |= (int)b); }
inline TermChar::TextAttributes& operator&=(TermChar::TextAttributes& a, TermChar::TextAttributes b) { return (TermChar::TextAttributes&)((int&)a &= (int)b); }
inline TermChar::TextAttributes& operator^=(TermChar::TextAttributes& a, TermChar::TextAttributes b) { return (TermChar::TextAttributes&)((int&)a ^= (int)b); }

const QByteArray multiCharEscapes("().*+-/%#");

struct TermAttribs
{
    QPoint cursorPos;

    bool wrapAroundMode;
    bool originMode;

    QRgb currentFgColor;
    QRgb currentBgColor;
    TermChar::TextAttributes currentAttrib;
};

class TerminalLine
{
public:
    int size() const { return m_contents.size(); }
    void append(const TermChar& tc) { m_contents.append(tc); }
    void insert(int pos, const TermChar& tc) { m_contents.insert(pos, tc); }
    void removeAt(int pos) { m_contents.removeAt(pos); }
    void clear() { m_contents.clear(); }
    TermChar& operator[](int pos) { return m_contents[pos]; }
    const TermChar& operator[](int pos) const { return m_contents[pos]; }
    const TermChar& at(int pos) const { return m_contents.at(pos); }

private:
    QVector<TermChar> m_contents;
};

class TerminalBuffer
{
public:
    int size() const { return m_buffer.size(); }
    void append(const TerminalLine& l) { m_buffer.append(l); }
    void insert(int pos, const TerminalLine& l) { m_buffer.insert(pos, l); }
    void removeAt(int pos) { m_buffer.removeAt(pos); }
    TerminalLine takeAt(int pos) { return m_buffer.takeAt(pos); }
    void clear() { m_buffer.clear(); }
    TerminalLine& operator[](int pos) { return m_buffer[pos]; }
    const TerminalLine& operator[](int pos) const { return m_buffer[pos]; }
    const TerminalLine& at(int pos) const { return m_buffer.at(pos); }

private:
    QVector<TerminalLine> m_buffer;
};

class Terminal : public QObject
{
    Q_OBJECT

public:
    explicit Terminal(QObject* parent = 0);
    virtual ~Terminal() { }

    void init();

    QPoint cursorPos();
    void setCursorPos(QPoint pos);
    bool showCursor();

    QSize termSize() const { return iTermSize; }
    void setTermSize(QSize size);

    TerminalBuffer& buffer();
    const TerminalBuffer& buffer() const;
    TerminalBuffer& backBuffer() { return iBackBuffer; }
    const TerminalBuffer& backBuffer() const { return iBackBuffer; }

    TerminalLine& currentLine();

    bool inverseVideoMode() const { return m_inverseVideoMode; }

    void keyPress(int key, int modifiers, const QString& text = "");
    const QStringList printableLinesFromCursor(int lines);
    void putString(QString str);
    void paste(const QString& text);
    const QStringList grabURLsFromBuffer();

    void scrollBackBufferFwd(int lines);
    void scrollBackBufferBack(int lines);
    int backBufferScrollPos() const { return iBackBufferScrollPos; }
    void resetBackBufferScrollPos();

    QString selectedText() const;
    void setSelection(QPoint start, QPoint end, bool selectionOngoing);
    QRect selection() const;
    void clearSelection();

    void setBackgroundWhite(bool backgroundWhite);

    int rows() const;
    int columns() const;

    bool useAltScreenBuffer() const { return iUseAltScreenBuffer; }

    TermChar zeroChar;

signals:
    void cursorPosChanged(QPoint newPos);
    void termSizeChanged(int rows, int columns);
    void displayBufferChanged();
    void selectionChanged();
    void scrollBackBufferAdjusted(bool reset);
    void selectionFinished();
    void visualBell();
    void windowTitleChanged(const QString& windowTitle);
    void workingDirectoryChanged(const QString& workingDirectory);
    void hangupReceived();

protected:
    void timerEvent(QTimerEvent*) override;
    void insertInBuffer(const QString& chars);

private slots:
    void onDataAvailable();

private:
    Q_DISABLE_COPY(Terminal)

    void insertAtCursor(QChar c, bool overwriteMode = true, bool advanceCursor = true);
    void eraseLineAtCursor(int from = -1, int to = -1);
    void clearAll(bool wholeBuffer = false);
    void ansiSequence(const QString& seq);
    void handleMode(int mode, bool set, const QString& extra);
    bool handleIL(const QList<int>& params, const QString& extra);
    bool handleDL(const QList<int>& params, const QString& extra);
    bool handleDCH(const QList<int>& params, const QString& extra);
    bool handleICH(const QList<int>& params, const QString& extra);
    bool handleDECSED(const QList<int>& params, const QString& extra);
    bool handleEL(const QList<int>& params, const QString& extra);
    bool handleECH(const QList<int>& params, const QString& extra);
    void oscSequence(const QString& seq);
    void escControlChar(const QString& seq);
    void trimBackBuffer();
    void scrollBack(int lines, int insertAt = -1);
    void scrollFwd(int lines, int removeAt = -1);

    enum class ResetMode
    {
        Soft,
        Hard,
    };
    void resetTerminal(ResetMode);
    void resetTabs();
    void adjustSelectionPosition(int lines);
    void forwardTab();
    void backwardTab();

    // ### consider making this not a pointer
    PtyIFace* m_pty;

    TerminalBuffer iBuffer;
    TerminalBuffer iAltBuffer;
    TerminalBuffer iBackBuffer;
    QVector<QVector<int>> iTabStops;

    QSize iTermSize;
    bool iEmitCursorChangeSignal;

    bool iShowCursor;
    bool iUseAltScreenBuffer;
    bool iAppCursorKeys;
    bool iReplaceMode;
    bool iNewLineMode;
    bool m_inverseVideoMode;
    bool m_bracketedPasteMode;
    bool backgroundWhite;

    int iMarginTop;
    int iMarginBottom;

    int iBackBufferScrollPos;

    TermAttribs iTermAttribs;
    TermAttribs iTermAttribs_saved;
    TermAttribs iTermAttribs_saved_alt;

    QString escSeq;
    QString oscSeq;
    int escape;
    QRect iSelection;
    QVector<QRgb> iColorTable;
    int m_dispatch_timer;

    friend class TextRender;
};

#endif // TERMINAL_H
