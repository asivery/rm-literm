/*
    Copyright 2011-2012 Heikki Holstila <heikki.holstila@gmail.com>

    This work is free software. you can redistribute it and/or modify
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

import QtQuick 2.0
import literm 1.0
import QtQuick.Window 2.0

Item {
    id: root

    width: 1620
    height: 2160

    signal requestQuit

    Item {
        id: page

        property int orientation: forceOrientation ? forcedOrientation : Qt.PortraitOrientation
        property bool forceOrientation: Util.orientationMode != Util.OrientationAuto
        property int forcedOrientation: Util.orientationMode == Util.OrientationLandscape ? Qt.LandscapeOrientation
                                                                                          : Qt.PortraitOrientation
        property bool portrait: rotation % 180 == 0

        width: portrait ? root.width : root.height
        height: portrait ? root.height : root.width
        anchors.centerIn: parent
        rotation: Screen.angleBetween(orientation, Screen.primaryOrientation)

        Rectangle {
            id: window

            property string fgcolor: Util.backgroundWhite ? "white" : "black"
            property string bgcolor: Util.backgroundWhite ? "white" : "#000000"
            property int fontSize: 14*pixelRatio

            property int fadeOutTime: 80
            property int fadeInTime: 350
            property real pixelRatio: root.width / 1620

            // layout constants
            property int buttonWidthSmall: 60*pixelRatio
            property int buttonWidthLarge: 180*pixelRatio
            property int buttonWidthHalf: 90*pixelRatio

            property int buttonHeightSmall: 48*pixelRatio
            property int buttonHeightLarge: 68*pixelRatio

            property int headerHeight: 20*pixelRatio

            property int radiusSmall: 5*pixelRatio
            property int radiusMedium: 10*pixelRatio
            property int radiusLarge: 15*pixelRatio

            property int paddingSmall: 5*pixelRatio
            property int paddingMedium: 10*pixelRatio

            property int fontSizeSmall: 14*pixelRatio
            property int fontSizeLarge: 24*pixelRatio

            property int uiFontSize: Util.uiFontSize * pixelRatio

            property int scrollBarWidth: 6*window.pixelRatio

            anchors.fill: parent
            color: bgcolor

            TextRender {
                id: textrender
                focus: true

                onTerminalQuit: () => root.requestQuit()
                onPanLeft: {
                    Util.notifyText(Util.panLeftTitle)
                    textrender.putString(Util.panLeftCommand)
                }
                onPanRight: {
                    Util.notifyText(Util.panRightTitle)
                    textrender.putString(Util.panRightCommand)
                }
                onPanUp: {
                    Util.notifyText(Util.panUpTitle)
                    textrender.putString(Util.panUpCommand)
                }
                onPanDown: {
                    Util.notifyText(Util.panDownTitle)
                    textrender.putString(Util.panDownCommand)
                }

                onDisplayBufferChanged: window.displayBufferChanged()
                dragMode: Util.dragMode
                onVisualBell: {
                    if (Util.visualBellEnabled)
                        bellTimer.start()
                }
                contentItem: Item {
                    width: parent.width
                    height: parent.height
                    opacity: (Util.keyboardMode == Util.KeyboardFade && vkb.active) ? 0.3
                                                                                    : 1.0

                    Behavior on opacity {
                        NumberAnimation { duration: textrender.duration; easing.type: Easing.InOutQuad }
                    }
                    Behavior on y {
                        NumberAnimation { duration: textrender.duration; easing.type: Easing.InOutQuad }
                    }
                }
                cellDelegate: Rectangle {
                }
                cellContentsDelegate: Text {
                    id: text
                    property bool blinking: false

                    textFormat: Text.PlainText
                    opacity: blinking ? 0.5 : 1.0
                    SequentialAnimation {
                        running: blinking
                        loops: Animation.Infinite
                        NumberAnimation {
                            target: text
                            property: "opacity"
                            to: 0.8
                            duration: 200
                        }
                        PauseAnimation {
                            duration: 400
                        }
                        NumberAnimation {
                            target: text
                            property: "opacity"
                            to: 0.5
                            duration: 200
                        }
                    }
                }
                cursorDelegate: Rectangle {
                    id: cursor
                    opacity: 0.5
                }
                selectionDelegate: Rectangle {
                    color: "blue"
                    opacity: 0.5
                }

                Rectangle {
                    id: bellTimerRect
                    visible: opacity > 0
                    opacity: bellTimer.running ? 0.5 : 0.0
                    anchors.fill: parent
                    color: "#ffffff"
                    Behavior on opacity {
                        NumberAnimation {
                            duration: 200
                        }
                    }
                }

                Connections {
                    target: Util
                    function onBackgroundWhiteChanged() {
                        textrender.setBackgroundWhite(Util.backgroundWhite);
                    }
                }

                property int duration
                property int cutAfter: height

                height: parent.height
                width: parent.width
                font.family: Util.fontFamily
                font.pointSize: Util.fontSize
                allowGestures: !vkb.active

                onCutAfterChanged: {
                    // this property is used in the paint function, so make sure that the element gets
                    // painted with the updated value (might not otherwise happen because of caching)
                    textrender.redraw();
                }

                Lineview {
                    id: lineView
                    opacity: ((Util.keyboardMode == Util.KeyboardFade) && vkb.active) ? 0.8
                                                                                      : 0.0
                    cursorWidth: textrender.cellSize.width
                    cursorHeight: textrender.cellSize.height
                }

                Keyboard {
                    id: vkb

                    property bool active
                    property bool keyboardEnabled: (Util.keyboardMode == Util.KeyboardMove)
                                                    || (Util.keyboardMode == Util.KeyboardFade)

                    y: parent.height - vkb.height
                    visible: keyboardEnabled

                    opacity: vkb.active ? 0.7 : 0.3
                    Behavior on opacity {
                        NumberAnimation { duration: textrender.duration; easing.type: Easing.InOutQuad }
                    }
                }

                // area to handle keyboard usage
                MultiPointTouchArea {
                    id: multiTouchArea

                    anchors.fill: parent

                    property int firstTouchId: -1
                    property var pressedKeys: ({})

                    onPressed: {
                        touchPoints.forEach(function (touchPoint) {
                            if (multiTouchArea.firstTouchId == -1) {
                                multiTouchArea.firstTouchId = touchPoint.pointId
                                //gestures c++ handler
                                textrender.mousePress(touchPoint.x, touchPoint.y)
                            }
                            var key = vkb.keyAt(touchPoint.x, touchPoint.y)
                            if (key != null) {
                                key.handlePress(multiTouchArea, touchPoint.x, touchPoint.y)
                            }
                            multiTouchArea.pressedKeys[touchPoint.pointId] = key
                        })
                    }
                    onUpdated: {
                        touchPoints.forEach(function (touchPoint) {
                            if (multiTouchArea.firstTouchId == touchPoint.pointId) {
                                //gestures c++ handler
                                textrender.mouseMove(touchPoint.x, touchPoint.y);
                            }
                            var key = multiTouchArea.pressedKeys[touchPoint.pointId]
                            if (key != null) {
                                if (!key.handleMove(multiTouchArea, touchPoint.x, touchPoint.y)) {
                                    delete multiTouchArea.pressedKeys[touchPoint.pointId];
                                }
                            }
                        })
                    }
                    onReleased: {
                        touchPoints.forEach(function (touchPoint) {
                            if (multiTouchArea.firstTouchId == touchPoint.pointId) {
                                // Toggle keyboard wake-up when tapping outside the keyboard, but:
                                //   - only when not scrolling (y-diff < 20 pixels)
                                //   - not in select mode, as it would be hard to select text
                                if (touchPoint.y < vkb.y && touchPoint.startY < vkb.y &&
                                        Math.abs(touchPoint.y - touchPoint.startY) < 20 &&
                                        Util.dragMode !== Util.DragSelect) {
                                    if (vkb.active) {
                                        window.sleepVKB()
                                    } else {
                                        window.wakeVKB()
                                    }
                                }

                                //gestures c++ handler
                                textrender.mouseRelease(touchPoint.x, touchPoint.y)
                                multiTouchArea.firstTouchId = -1
                            }
                            var key = multiTouchArea.pressedKeys[touchPoint.pointId]
                            if (key != null) {
                                key.handleRelease(multiTouchArea, touchPoint.x, touchPoint.y)
                            }
                            delete multiTouchArea.pressedKeys[touchPoint.pointId]
                        })
                    }
                }
            }

            MouseArea {
                //top right corner menu button
                x: window.width - width
                width: menuImg.width + 60*window.pixelRatio
                height: menuImg.height + 30*window.pixelRatio
                opacity: 0.5
                onClicked: menu.showing = true

                Image {
                    id: menuImg

                    anchors.centerIn: parent
                    source: "qrc:/literm/icons/menu.png"
                    scale: window.pixelRatio
                }
            }

            Image {
                // terminal buffer scroll indicator
                source: "qrc:/literm/icons/scroll-indicator.png"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                visible: textrender.showBufferScrollIndicator
                scale: window.pixelRatio
            }

            Timer {
                id: fadeTimer

                interval: Util.keyboardFadeOutDelay
                onTriggered: {
                    window.sleepVKB();
                }
            }

            Timer {
                id: bellTimer

                interval: 80
            }

            Connections {
                target: Util
                function onNotify(msg) {
                    textNotify.text = msg;
                    textNotifyAnim.enabled = false;
                    textNotify.opacity = 1.0;
                    textNotifyAnim.enabled = true;
                    textNotify.opacity = 0;
                }
            }

            MenuLiterm {
                id: menu
                onRequestQuit: () => root.requestQuit()
            }

            Text {
                // shows large text notification in the middle of the screen (for gestures)
                id: textNotify

                anchors.centerIn: parent
                color: "#ffffff"
                opacity: 0
                font.pointSize: 40*window.pixelRatio

                Behavior on opacity {
                    id: textNotifyAnim
                    NumberAnimation { duration: 500; }
                }
            }

            Rectangle {
                // visual key press feedback...
                // easier to work with the coordinates if it's here and not under keyboard element
                id: visualKeyFeedbackRect

                property string label

                visible: false
                radius: window.radiusSmall
                color: "#ffffff"

                Text {
                    color: "#000000"
                    font.pointSize: 34*window.pixelRatio
                    anchors.centerIn: parent
                    text: visualKeyFeedbackRect.label
                }
            }

            NotifyWin {
                id: aboutDialog

                text: {
                    var str = "<font size=\"+3\">rMPP literm " + Util.versionString() + "</font><br>\n" +
                            "<font size=\"+1\">" +
                            "<br>Source code:<br>\n<a href=\"https://github.com/asivery/rmpp-literm/\">https://github.com/asivery/rmpp-literm/</a>\n\n" +
                            "<br>Original source code:<br>\n<a href=\"https://github.com/rburchell/literm/\">https://github.com/rburchell/literm/</a>\n\n" +
                            "<br>Config files for adjusting settings are at:<br>\n" +
                            Util.configPath() + "/<br><br>\n"
                    if (textrender.terminalSize.width != 0 && textrender.terminalSize.height != 0) {
                        str += "<br>Current terminal size: <font color=\"gray\">" + textrender.terminalSize.width + "×" + textrender.terminalSize.height+ "</font>";
                        str += "<br>Charset: <font color=\"gray\">" + Util.charset + "</font>";
                    }
                    str += "</font>";
                    return str;
                }
            }

            NotifyWin {
                id: errorDialog
            }

            UrlWindow {
                id: urlWindow
            }

            LayoutWindow {
                id: layoutWindow
            }

            function vkbKeypress(key, modifiers) {
                wakeVKB();
                textrender.vkbKeyPress(key, modifiers);
            }

            function wakeVKB()
            {
                if (!vkb.keyboardEnabled)
                    return;

                textrender.duration = window.fadeOutTime;
                fadeTimer.restart();
                vkb.active = true;
                setTextRenderAttributes();
            }

            function sleepVKB()
            {
                textrender.duration = window.fadeInTime;
                vkb.active = false;
                setTextRenderAttributes();
            }

            function setTextRenderAttributes()
            {
                if (Util.keyboardMode == Util.KeyboardMove && vkb.active) {
                    var move = textrender.cursorPixelPos().y
                            + textrender.cellSize.height * (Util.extraLinesFromCursor + 0.5)
                    if (move < vkb.y) {
                        textrender.contentItem.y = 0
                        textrender.cutAfter = vkb.y
                    } else {
                        textrender.contentItem.y = 0 - move + vkb.y
                        textrender.cutAfter = move
                    }
                } else {
                    textrender.cutAfter = textrender.height
                    textrender.contentItem.y = 0
                }
            }

            function displayBufferChanged()
            {
                lineView.lines = textrender.printableLinesFromCursor(Util.extraLinesFromCursor);
                lineView.cursorX = textrender.cursorPixelPos().x;
                setTextRenderAttributes();
            }

            Component.onCompleted: {
                if (Util.startupErrorMessage != "") {
                    showErrorMessage(Util.startupErrorMessage)
                }
            }

            function showErrorMessage(string)
            {
                errorDialog.text = "<font size=\"+2\">" + string + "</font>";
                errorDialog.show = true
            }
        }
    }
}
