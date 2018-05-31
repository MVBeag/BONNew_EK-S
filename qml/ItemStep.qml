import QtQuick 2.6

Rectangle {
    width: 156
    height: width / 0.625
    color: "#000000"

    property string icon: ""
    property string description: qsTr("")

    onIconChanged: {
        imageIcon.source = icon
    }

    onDescriptionChanged: {
        textDescription.text = description
    }

    Image {
        id: imageIcon
        width: parent.width
        height: parent.width
    }

    Rectangle {
        id: rectangleSpacer
        width: parent.width
        height: parent.width * 0.0625
        y: imageIcon.height
        color: parent.color
    }

    Text {
        id: textDescription
        width: parent.width
        height: parent.height - imageIcon.height - rectangleSpacer.height
        y: imageIcon.height + rectangleSpacer.height
        color: "#777777"
        wrapMode: Text.WordWrap
        font { family: fontDinotBold.name; pixelSize: height / 6 }
    }
}
