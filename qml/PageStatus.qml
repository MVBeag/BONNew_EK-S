import QtQuick 2.6

Item {
    width: 768
    height: 358

    property string state: "WELCOME"
    property bool isModelL: false

    onStateChanged: {

        if ( state === "SEARCH_ERROR" ) {
            imageStatus.source = "/img/status_error.jpg"
            textStatusHead.text = qsTr("Sorry, we did not detect Siros")
            textStatusNote.text = qsTr("Please use the SirosUpdater_4901 first.")
        }
        else if ( state === "UPDATE_ERROR" ) {
            imageStatus.source = "/img/status_error.jpg"
            textStatusHead.text = qsTr("Error: Update failed")
            textStatusNote.text = qsTr("Please unpower Siros, disconnect the cable and contact your local service station or distributor for further help.")
        }
        else if ( state === "UPDATE_SUCCESS" ) {
            imageStatus.source = "/img/status_success.jpg"
            textStatusHead.text = qsTr("Update successful!")
            if ( isModelL )
            {
              textStatusNote.text = qsTr("Please unplug the USB cable\nthen the battery and restart Siros L.")
            }
            else
            {
              textStatusNote.text = qsTr("Please unplug the USB cable\nand restart Siros.")
            }
        }
    }

    Image {
        id: imageStatus
        height: parent.height * (1 - 0.625)
        width: height
        anchors.centerIn: parent
        source: "/img/status_success.jpg"
    }

    Text {
        id: textStatusHead
        y: imageStatus.y + imageStatus.height
        width: parent.width
        height: imageStatus.height * (1 - 0.625)
        color: "#5c99b5"
        text: qsTr("Update status unknown!")
        horizontalAlignment: Text.AlignHCenter
        font { family: fontDinotBold.name; pointSize: height * (1 - 0.625) }
    }

    Text {
        id: textStatusNote
        y: imageStatus.y + imageStatus.height + textStatusHead.height
        width: parent.width
        height: textStatusHead.height * 0.625
        color: "#777777"
        text: qsTr("...")
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        font { family: fontDinotBold.name; pointSize: height * (1 - 0.625) }
    }
}

