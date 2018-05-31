import QtQuick 2.6

Item {
    width: 768
    height: 358

    Image {
        id: imageSearch
        height: parent.height * (1 - 0.625)
        width: height
        anchors.centerIn: parent
        source: "/img/scanning.jpg"

        RotationAnimation on rotation {
            loops: Animation.Infinite
            from: 0
            to: 360
            duration: 1000
            running: pageSearch.opacity
        }
    }

    Text {
        id: textSearch
        y: imageSearch.y + imageSearch.height
        width: parent.width
        height: imageSearch.height * (1 - 0.625)
        color: "#5c99b5"
        text: qsTr("Searching connected devices...")
        horizontalAlignment: Text.AlignHCenter
        font { family: fontDinotBold.name; pointSize: height * (1 - 0.625) }
    }
}

