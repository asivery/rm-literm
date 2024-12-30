import QtQuick 2.5
import QtQuick.Controls 2.5

Rectangle {
    id: root
    anchors.fill: parent

    signal close
    function unloading() {

    }
    function gainedFocus() {

    }

    Loader {
        id: internalTerminal
        anchors.fill: parent
        source: "qrc:///literm/qml/mobile/Main.qml"
    }

    Connections {
        target: internalTerminal.item
        onRequestQuit: () => root.close()
    }
}
