import QtQuick 2.6
import QtQuick.Controls 1.5
import QtQuick.Controls.Styles 1.4

Rectangle {

    width: 256
    height: 64

    color: "#000000"
    border.width: 2
    border.color: "#5c99b5"
    radius: 2

    property bool showDocumentation: false

    Rectangle {

        x: 2
        y: 2
        width: parent.width - 4
        height: parent.height * 0.65 - 4

        color: "#000000"

        Text {
            id: textPleaseInstallDriver
            width: parent.width * 0.9
            height: parent.height * 0.2
            x: parent.width * 0.1 / 2
            y: parent.height * 0.8 / 2
            color: "#ffffff"
            text: qsTr("Please install the driver.")
            wrapMode: Text.WordWrap
            font { family: fontDinotRegular.name; pointSize: 11 }
        }
    }

    Rectangle {

        x: 0
        y: parent.height * 0.65
        width: parent.width
        height: parent.height * 0.35

        color: "#cacaca"
        border.width: 1
        border.color: "#5c99b5"
        radius: 2

        Button {
            id: buttonReadInstruction
            width: parent.width * 0.6
            height: parent.height * 0.9
            x: parent.width * 0.4 / 2
            y: parent.height * 0.1 / 2
            text: qsTr("READ INSTRUCTION")

            onClicked: {
                showDocumentation = true
            }

            style: ButtonStyle {
                background: Rectangle {
                    color: "#000000"
                    implicitWidth: 100
                    implicitHeight: 25
                    border.width: control.activeFocus ? 2 : 1
                    border.color: "#5C99B5"
                    radius: 2
                }
                label: Text {
                    renderType: Text.NativeRendering
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    color: "#FFFFFF"
                    text: control.text
                    font { family: fontDinotBold.name; pointSize: 12 }
                }
            }
        }
    }
}
