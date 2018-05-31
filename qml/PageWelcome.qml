import QtQuick 2.6

Item {
    width: 768
    height: 358

    readonly property int stepIconWidth: height * (1 - 0.625)
    readonly property int stepIconHeight: stepIconWidth
    readonly property int stepIconSpacer: (width - stepIconWidth * 2) / 3

    Rectangle {
        id: rectangleHeader
        width: parent.width
        height: parent.height * (1 - 0.625)
        color: "#000000"

        Text {
            id: textWelcomeHead1
            width: parent.width
            height: parent.height * 0.625
            color: "#5c99b5"
            text: qsTr("Welcome to the Siros Firmware Update")
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font { family: fontDinotBold.name; pixelSize: height * (1 - 0.625) }
        }

        Text {
            id: textWelcomeHead2
            width: parent.width
            height: parent.height - textWelcomeHead1.height
            y: textWelcomeHead1.height
            color: "#ffffff"
            text: qsTr("Please follow the steps below.")
            horizontalAlignment: Text.AlignHCenter
            font { family: fontDinotBold.name; pixelSize: height * (1 - 0.625) }
        }
		
		Text {
            id: textWelcomeHead3
            width: parent.width
            height: parent.height - textWelcomeHead2.height
            y: textWelcomeHead2.height
            color: "#ff0000"
            text: qsTr("SIROS S")
            horizontalAlignment: Text.AlignHCenter
            font { family: fontDinotBold.name; pixelSize: height * (1 - 0.625) }
        }
    }

    Rectangle {
        id: rectangleBody
        width: parent.width
        height: parent.height - rectangleHeader.height
        y: rectangleHeader.height
        color: "#000000"

        ItemStep {
            id: itemWelcomeStep1
            x: stepIconSpacer
            width: stepIconWidth
            icon: "/img/step_1.jpg"
            description: qsTr("Turn off your Siros and connect it via USB to your computer.")
        }

        ItemStep {
            id: itemWelcomeStep2
            x: stepIconSpacer * 2 + stepIconWidth
            width: stepIconWidth
            icon: "/img/step_2.jpg"
            description: qsTr("Turn on the Siros. When it's activated, click ''Scan for Devices''")
        }
    }
}
