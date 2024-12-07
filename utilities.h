/*
    Copyright 2021 Crimson AS <info@crimson.no>
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

#ifndef UTIL_H
#define UTIL_H

#include <QtCore>

#include "textrender.h"

class Terminal;
class TextRender;
class QQuickView;

class Util : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool visualBellEnabled READ visualBellEnabled CONSTANT)
    Q_PROPERTY(int cursorAnimationStartPauseDuration READ cursorAnimationStartPauseDuration CONSTANT)
    Q_PROPERTY(int cursorAnimationFadeInDuration READ cursorAnimationFadeInDuration CONSTANT)
    Q_PROPERTY(int cursorAnimationMiddlePauseDuration READ cursorAnimationMiddlePauseDuration CONSTANT)
    Q_PROPERTY(int cursorAnimationFadeOutDuration READ cursorAnimationFadeOutDuration CONSTANT)
    Q_PROPERTY(int cursorAnimationEndPauseDuration READ cursorAnimationEndPauseDuration CONSTANT)
    Q_PROPERTY(QString fontFamily READ fontFamily CONSTANT)
    Q_PROPERTY(int uiFontSize READ uiFontSize CONSTANT)
    Q_PROPERTY(int fontSize READ fontSize WRITE setFontSize NOTIFY fontSizeChanged)
    Q_PROPERTY(TextRender::DragMode dragMode READ dragMode WRITE setDragMode NOTIFY dragModeChanged)
    Q_PROPERTY(int keyboardMode READ keyboardMode WRITE setKeyboardMode NOTIFY keyboardModeChanged)
    Q_PROPERTY(int keyboardFadeOutDelay READ keyboardFadeOutDelay WRITE setKeyboardFadeOutDelay NOTIFY keyboardFadeOutDelayChanged)
    Q_PROPERTY(QString keyboardLayout READ keyboardLayout WRITE setKeyboardLayout NOTIFY keyboardLayoutChanged)
    Q_PROPERTY(int extraLinesFromCursor READ extraLinesFromCursor CONSTANT)
    Q_PROPERTY(bool backgroundWhite READ backgroundWhite WRITE setBackgroundWhite NOTIFY backgroundWhiteChanged)
    Q_PROPERTY(QString charset READ charset CONSTANT)
    Q_PROPERTY(int keyboardMargins READ keyboardMargins CONSTANT)
    Q_PROPERTY(int orientationMode READ orientationMode WRITE setOrientationMode NOTIFY orientationModeChanged)
    Q_PROPERTY(QByteArray terminalEmulator READ terminalEmulator CONSTANT)
    Q_PROPERTY(QString terminalCommand READ terminalCommand CONSTANT)
    Q_PROPERTY(int terminalScrollbackSize READ terminalScrollbackSize CONSTANT)
    Q_PROPERTY(QString panLeftTitle READ panLeftTitle CONSTANT)
    Q_PROPERTY(QString panLeftCommand READ panLeftCommand CONSTANT)
    Q_PROPERTY(QString panRightTitle READ panRightTitle CONSTANT)
    Q_PROPERTY(QString panRightCommand READ panRightCommand CONSTANT)
    Q_PROPERTY(QString panDownTitle READ panDownTitle CONSTANT)
    Q_PROPERTY(QString panDownCommand READ panDownCommand CONSTANT)
    Q_PROPERTY(QString panUpTitle READ panUpTitle CONSTANT)
    Q_PROPERTY(QString panUpCommand READ panUpCommand CONSTANT)
    Q_PROPERTY(QString startupErrorMessage READ startupErrorMessage CONSTANT)

    Q_ENUMS(KeyboardMode)
    Q_ENUMS(DragMode)
    Q_ENUMS(OrientationMode)

public:
    enum KeyboardMode
    {
        KeyboardOff,
        KeyboardFade,
        KeyboardMove
    };

    enum OrientationMode
    {
        OrientationAuto,
        OrientationLandscape,
        OrientationPortrait
    };

    explicit Util(const QString& settingsFile, QObject* parent = 0);
    virtual ~Util();

    static Util* instance();
    QString panLeftTitle() const;
    QString panLeftCommand() const;
    QString panRightTitle() const;
    QString panRightCommand() const;
    QString panDownTitle() const;
    QString panDownCommand() const;
    QString panUpTitle() const;
    QString panUpCommand() const;

    QString startupErrorMessage() const;
    void setStartupErrorMessage(const QString&);

    QByteArray terminalEmulator() const;
    QString terminalCommand() const;
    int terminalScrollbackSize() const;

    // Q_INVOKABLE QString getUserMenuXml();

    Q_INVOKABLE QString versionString();
    Q_INVOKABLE QString configPath();
    QVariant settingsValue(QString key, const QVariant& defaultValue = QVariant()) const;
    void setSettingsValue(QString key, QVariant value);

    int uiFontSize();

    int fontSize();
    void setFontSize(int size);

    bool backgroundWhite();
    void setBackgroundWhite(bool backgroundWhite);

    Q_INVOKABLE void keyPressFeedback();
    Q_INVOKABLE void keyReleaseFeedback();
    Q_INVOKABLE void notifyText(QString text);

    Q_INVOKABLE void copyTextToClipboard(QString str);

    bool visualBellEnabled() const;

    int cursorAnimationStartPauseDuration() const;
    int cursorAnimationFadeInDuration() const;
    int cursorAnimationMiddlePauseDuration() const;
    int cursorAnimationFadeOutDuration() const;
    int cursorAnimationEndPauseDuration() const;

    QString fontFamily();

    TextRender::DragMode dragMode();
    void setDragMode(TextRender::DragMode mode);

    int keyboardMode();
    void setKeyboardMode(int mode);

    int keyboardFadeOutDelay();
    void setKeyboardFadeOutDelay(int delay);

    QString keyboardLayout();
    void setKeyboardLayout(const QString& layout);

    int extraLinesFromCursor();
    QString charset();
    int keyboardMargins();

    int orientationMode();
    void setOrientationMode(int mode);

signals:
    void notify(QString msg);
    void fontSizeChanged();
    void dragModeChanged();
    void backgroundWhiteChanged();
    void keyboardModeChanged();
    void keyboardFadeOutDelayChanged();
    void keyboardLayoutChanged();
    void orientationModeChanged();

private:
    Q_DISABLE_COPY(Util)

    QSettings m_settings;
    QString m_startupErrorMessage;
    QString monospaceFontFamily;
};

#endif // UTIL_H
