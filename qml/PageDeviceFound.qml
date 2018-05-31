import QtQuick 2.6

Item {
    width: 768
    height: 358

    property string logo: "/img/siros.jpg"
    property string name: qsTr("unknown")
    property string status: qsTr("Update available")
    property string versionCurrent: qsTr("Currently Installed: ")
    property string versionNew: qsTr("New Version: ")
    property bool isModelL: false
    property bool downgrade: false

    signal changelogClicked()
    signal passwordVerified()

    onLogoChanged: {
        imageDeviceFoundLogo.source = logo
    }

    onNameChanged: {
        textDeviceFoundName.text = name
    }

    onStatusChanged: {
        textDeviceFoundStatus.text = status
    }

    onVersionCurrentChanged: {
        textDeviceFoundCurrent.text = qsTr("Currently Installed: ") + versionCurrent
    }

    onVersionNewChanged: {
        textDeviceFoundNew.text = qsTr("New Version: ") + versionNew
    }

    onIsModelLChanged: {
        if (isModelL) {
            textModelLTip.opacity = 1
        } else {
            textModelLTip.opacity = 0
        }
    }

    onDowngradeChanged: {
        if (downgrade) {
            textDowngradeTip.opacity = 1
            textInputDowngradePassword.opacity = 1
            textInputDowngradePassword.enabled = true
            textInputDowngradePassword.forceActiveFocus()
        } else {
            textDowngradeTip.opacity = 0
            textInputDowngradePassword.opacity = 0
            textInputDowngradePassword.enabled = false
            textInputDowngradePassword.text = ""
        }
    }

    Text {
        id: textDeviceFound
        x: parent.width / 2 * (1 - 0.625)
        y: parent.height * 0.1
        width: parent.width / 2 * 0.625
        height: parent.height * 0.05
        color: "#ffffff"
        text: qsTr("DEVICE FOUND")
        font { family: fontDinotBold.name; pointSize: height }
    }

    Image {
        id: imageDeviceFoundLogo
        x: parent.width / 2 * (1 - 0.625)
        y: parent.height * 0.3
        width: parent.width / 2 * 0.625
        height: parent.height / 2 * 0.8
        source: logo
    }

    Text {
        id: textDeviceFoundName
        x: parent.width * 0.525
        y: parent.height * 0.30
        width: parent.width * 0.3
        height: parent.height * 0.05
        color: "#ffffff"
        text: name
        font { family: fontDinotBold.name; pointSize: height }
    }

    Text {
        id: textDeviceFoundStatus
        x: parent.width * 0.525
        y: parent.height * 0.40
        width: parent.width * 0.3
        height: parent.height * 0.05
        color: "#ffffff"
        text: status
        font { family: fontDinotBold.name; pointSize: height * 0.625 }
    }

    Text {
        id: textDeviceFoundCurrent
        x: parent.width * 0.525
        y: parent.height * 0.45
        width: parent.width * 0.3
        height: parent.height * 0.05
        color: "#777777"
        text: versionCurrent
        font { family: fontDinotRegular.name; pointSize: height * 0.625 }
    }

    Text {
        id: textDeviceFoundNew
        x: parent.width * 0.525
        y: parent.height * 0.50
        width: parent.width * 0.3
        height: parent.height * 0.05
        color: "#ffffff"
        text: versionNew
        font { family: fontDinotRegular.name; pointSize: height * 0.625 }
    }

    Text {
        id: textDeviceFoundChangelog
        x: parent.width * 0.525
        y: parent.height * 0.55
        width: parent.width * 0.3
        height: parent.height * 0.05
        color: "#5c99b5"
        linkColor: "#5c99b5"
        text: "<a href=\"www.broncolor.com\">Changelog</a>"
        font { family: fontDinotRegular.name; pointSize: height * 0.625 }

        onLinkActivated: {
            changelogClicked()
        }
    }

    Text {
        id: textModelLTip
        x: parent.width * 0.525
        y: parent.height * 0.65
        width: parent.width * 0.3
        height: parent.height * 0.1
        color: "#ffffff"
        text: qsTr("Please make sure that your battery is half full before continuing.")
        wrapMode: Text.WordWrap
        font { family: fontDinotRegular.name; pointSize: height * 0.3 }
        opacity: 0
    }

    Text {
        id: textDowngradeTip
        x: parent.width / 2 * (1 - 0.625)
        y: parent.height * 1.0625
        width: parent.width / 2 * 0.625
        height: parent.height * 0.06
        color: "#ffffff"
        text: qsTr("Downgrade password:")
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignRight
        font { family: fontDinotRegular.name; pointSize: height * 0.625 }
        opacity: 0
    }

    TextInput {
        id: textInputDowngradePassword
        x: parent.width * 0.525
        y: parent.height * 1.0625
        width: parent.width * 0.3
        height: parent.height * 0.06
        color: "#ffffff"
        text: qsTr("")
        echoMode: TextInput.Password
        font { family: fontDinotRegular.name; pointSize: height * 0.625 }
        opacity: 0

        onTextChanged: {
            if (text == "Siros") {
                downgrade = false
                passwordVerified()
            }
        }
    }
}

