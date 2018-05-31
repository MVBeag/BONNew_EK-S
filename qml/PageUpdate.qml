import QtQuick 2.6
import QtQuick.Controls 1.5
import QtQuick.Controls.Styles 1.4

Item {
    width: 768
    height: 358

    property int progress: 0

    onProgressChanged: {
        progressBar.value = progress
    }

    ProgressBar {
        id: progressBar
        width: parent.width * 0.625
        height: parent.height * 0.0625
        anchors.centerIn: parent
        minimumValue: 0
        maximumValue: 100
        value: 0
        style: ProgressBarStyle {
            background: Rectangle {
                radius: 2
                color: "#000000"
                border.color: "#777777"
            }
            progress: Rectangle {
                anchors.fill: parent
                anchors.margins: 2
                color: "#5C99B5"
                border.color: "#000000"
            }
        }

        onValueChanged: {
            textUpdatePercentage.text = qsTr(value.toString().concat("%"))
        }

        Text {
            id: textUpdatePercentage
            width: parent.width * (1 - 0.625)
            height: parent.height
            anchors.centerIn: parent
            color: "#ffffff"
            text: qsTr("0%")
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font { family: fontDinotBold.name; pointSize: height * (1 - 0.625) }
        }
    }

    Text {
        id: textUpdate
        width: parent.width
        height: parent.height * (1 - 0.625) * 0.625
        color: "#ffffff"
        text: qsTr("UPDATE IN PROGRESS")
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font { family: fontDinotBold.name; pointSize: height * (1 - 0.625) }
    }

    Text {
        id: textUpdateNote
        y: 223
        width: parent.width
        height: textUpdate.height * (1 - 0.625)
        color: "#777777"
        text: qsTr("Please do not disconnect your device until the update is completed.")
        horizontalAlignment: Text.AlignHCenter
        font { family: fontDinotBold.name; pointSize: height * (1 - 0.625) }
    }
}

