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

Item {
    id: menuWin

    property bool showing

    anchors.fill: parent
    visible: rect.x < menuWin.width

    signal requestQuit

    Rectangle {
        id: fader

        color: "#000000"
        opacity: menuWin.showing ? 0.5 : 0.0
        anchors.fill: parent

        Behavior on opacity { NumberAnimation { duration: 100; } }

        MouseArea {
            anchors.fill: parent
            onClicked: menuWin.showing = false
        }
    }
    Rectangle {
        id: rect

        color: "#e0e0e0"
        anchors.left: parent.right
        anchors.leftMargin: menuWin.showing ? -width : 1
        width: flickableContent.width + 22*window.pixelRatio;
        height: menuWin.height

        MouseArea {
            // event eater
            anchors.fill: parent
        }

        ScrollDecorator {
            x: parent.width-window.paddingMedium
            y: menuFlickArea.visibleArea.yPosition*menuFlickArea.height + window.scrollBarWidth
            height: menuFlickArea.visibleArea.heightRatio*menuFlickArea.height
            color: "#202020"
        }

        Flickable {
            id: menuFlickArea

            anchors.fill: parent
            anchors.topMargin: window.scrollBarWidth
            anchors.bottomMargin: window.scrollBarWidth
            anchors.leftMargin: window.scrollBarWidth
            anchors.rightMargin: 16*window.pixelRatio
            contentHeight: flickableContent.height

            Column {
                id: flickableContent

                spacing: 12*window.pixelRatio

                Row {
                    id: menuBlocksRow
                    spacing: 8*window.pixelRatio

                    Column {
                        spacing: 12*window.pixelRatio

                        Row {
                            Button {
                                text: "Copy"
                                onClicked: {
                                    menuWin.showing = false;
                                    textrender.copy();
                                }
                                width: window.buttonWidthHalf
                                height: window.buttonHeightLarge
                                enabled: textrender.selectedText.length
                            }
                            Button {
                                text: "Paste"
                                onClicked: {
                                    menuWin.showing = false;
                                    textrender.paste();
                                }
                                width: window.buttonWidthHalf
                                height: window.buttonHeightLarge
                                enabled: textrender.canPaste
                            }
                        }
                        Button {
                            text: "URL grabber"
                            width: window.buttonWidthLarge
                            height: window.buttonHeightLarge
                            onClicked: {
                                menuWin.showing = false;
                                urlWindow.urls = textrender.grabURLsFromBuffer();
                                urlWindow.show = true
                            }
                        }
                        Rectangle {
                            width: window.buttonWidthLarge
                            height: window.buttonHeightLarge
                            radius: window.radiusSmall
                            color: "#606060"
                            border.color: "#000000"
                            border.width: 1

                            Column {
                                Text {
                                    width: window.buttonWidthLarge
                                    height: window.headerHeight
                                    color: "#ffffff"
                                    font.pointSize: window.uiFontSize-1
                                    text: "Font size"
                                    horizontalAlignment: Text.AlignHCenter
                                }
                                Row {
                                    Button {
                                        text: "<font size=\"+3\">+</font>"
                                        onClicked: {
                                            Util.fontSize = Util.fontSize + window.pixelRatio
                                            Util.notifyText(textrender.terminalSize.width + "×" + textrender.terminalSize.height);
                                        }
                                        width: window.buttonWidthHalf
                                        height: window.buttonHeightSmall
                                    }
                                    Button {
                                        text: "<font size=\"+3\">-</font>"
                                        onClicked: {
                                            Util.fontSize = Util.fontSize - window.pixelRatio
                                            Util.notifyText(textrender.terminalSize.width + "×" + textrender.terminalSize.height);
                                        }
                                        width: window.buttonWidthHalf
                                        height: window.buttonHeightSmall
                                    }
                                }
                            }
                        }
                        Rectangle {
                            width: window.buttonWidthLarge
                            height: window.buttonHeightLarge
                            radius: window.radiusSmall
                            color: "#606060"
                            border.color: "#000000"
                            border.width: 1

                            Column {
                                Text {
                                    width: window.buttonWidthLarge
                                    height: window.headerHeight
                                    color: "#ffffff"
                                    font.pointSize: window.uiFontSize-1
                                    text: "UI Orientation"
                                    horizontalAlignment: Text.AlignHCenter
                                }
                                Row {
                                    Button {
                                        text: "<font size=\"-1\">Auto</font>"
                                        highlighted: Util.orientationMode == Util.OrientationAuto
                                        onClicked: Util.orientationMode = Util.OrientationAuto
                                        width: window.buttonWidthSmall
                                        height: window.buttonHeightSmall
                                    }
                                    Button {
                                        text: "<font size=\"-1\">L<font>"
                                        highlighted: Util.orientationMode == Util.OrientationLandscape
                                        onClicked: Util.orientationMode = Util.OrientationLandscape
                                        width: window.buttonWidthSmall
                                        height: window.buttonHeightSmall
                                    }
                                    Button {
                                        text: "<font size=\"-1\">P</font>"
                                        highlighted: Util.orientationMode == Util.OrientationPortrait
                                        onClicked: Util.orientationMode = Util.OrientationPortrait
                                        width: window.buttonWidthSmall
                                        height: window.buttonHeightSmall
                                    }
                                }
                            }
                        }
                        Rectangle {
                            width: window.buttonWidthLarge
                            height: window.buttonHeightLarge
                            radius: window.radiusSmall
                            color: "#606060"
                            border.color: "#000000"
                            border.width: 1

                            Column {
                                Text {
                                    width: window.buttonWidthLarge
                                    height: window.headerHeight
                                    color: "#ffffff"
                                    font.pointSize: window.uiFontSize-1
                                    text: "Drag mode"
                                    horizontalAlignment: Text.AlignHCenter
                                }
                                Row {
                                    Button {
                                        text: "<font size=\"-1\">Gesture</font>"
                                        highlighted: Util.dragMode == Util.DragGestures
                                        onClicked: {
                                            Util.dragMode = Util.DragGestures
                                            textrender.deselect();
                                            menuWin.showing = false;
                                        }
                                        width: window.buttonWidthSmall
                                        height: window.buttonHeightSmall
                                    }
                                    Button {
                                        text: "<font size=\"-1\">Scroll</font>"
                                        highlighted: Util.dragMode == Util.DragScroll
                                        onClicked: {
                                            Util.dragMode = Util.DragScroll
                                            textrender.deselect();
                                            menuWin.showing = false;
                                        }
                                        width: window.buttonWidthSmall
                                        height: window.buttonHeightSmall
                                    }
                                    Button {
                                        text: "<font size=\"-1\">Select</font>"
                                        highlighted: Util.dragMode == Util.DragSelect
                                        onClicked: {
                                            Util.dragMode = Util.DragSelect
                                            menuWin.showing = false;
                                        }
                                        width: window.buttonWidthSmall
                                        height: window.buttonHeightSmall
                                    }
                                }
                            }
                        }
                        Rectangle {
                            width: window.buttonWidthLarge
                            height: window.buttonHeightLarge
                            radius: window.radiusSmall
                            color: "#606060"
                            border.color: "#000000"
                            border.width: 1

                            Column {
                                Text {
                                    width: window.buttonWidthLarge
                                    height: window.headerHeight
                                    color: "#ffffff"
                                    font.pointSize: window.uiFontSize-1
                                    text: "VKB behavior"
                                    horizontalAlignment: Text.AlignHCenter
                                }
                                Row {
                                    Button {
                                        text: "Off"
                                        highlighted: Util.keyboardMode == Util.KeyboardOff
                                        onClicked: {
                                            Util.keyboardMode = Util.KeyboardOff
                                            window.setTextRenderAttributes();
                                            menuWin.showing = false;
                                        }
                                        width: window.buttonWidthSmall
                                        height: window.buttonHeightSmall
                                    }
                                    Button {
                                        text: "Fade"
                                        highlighted: Util.keyboardMode == Util.KeyboardFade
                                        onClicked: {
                                            Util.keyboardMode = Util.KeyboardFade
                                            window.setTextRenderAttributes();
                                            menuWin.showing = false;
                                        }
                                        width: window.buttonWidthSmall
                                        height: window.buttonHeightSmall
                                    }
                                    Button {
                                        text: "Move"
                                        highlighted: Util.keyboardMode == Util.KeyboardMove
                                        onClicked: {
                                            Util.keyboardMode = Util.KeyboardMove
                                            window.setTextRenderAttributes();
                                            menuWin.showing = false;
                                        }
                                        width: window.buttonWidthSmall
                                        height: window.buttonHeightSmall
                                    }
                                }
                            }
                        }
                        Button {
                            text: "VKB layout..."
                            onClicked: {
                                menuWin.showing = false;
                                layoutWindow.layouts = keyLoader.availableLayouts();
                                layoutWindow.show = true
                            }
                        }
                        Button {
                            text: Util.backgroundWhite ? "Dark Background" : "White Background"
                            onClicked: {
                                Util.backgroundWhite = !Util.backgroundWhite;
                            }
                        }
                        Button {
                            text: "About"
                            onClicked: {
                                menuWin.showing = false;
                                aboutDialog.show = true;
                            }
                        }
                        Button {
                            text: "Quit"
                            onClicked: {
                                menuWin.showing = false;
                                menuWin.requestQuit();
                            }
                        }
                    }
                }
            }
        }
    }
}
