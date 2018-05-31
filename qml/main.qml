import QtQuick 2.6
import QtQuick.Controls 1.5
import QtQuick.Controls.Styles 1.4
import Drivers 1.0

ApplicationWindow {
    id: mainWindow
    visible: true
    color: "#000000"
    minimumWidth: 768; minimumHeight: 512
    maximumWidth: 768; maximumHeight: 512
    title: qsTr("broncolor Update Center")

    readonly property int headerHeight: height * 0.148
    readonly property int footerHeight: headerHeight
    readonly property int bodyHeight: height - (headerHeight + footerHeight)
    readonly property int bodyY: headerHeight

    FontLoader { id: fontDinotBlack; source: "/font/DINOT-Black.otf" }
    FontLoader { id: fontDinotBold; source: "/font/DINOT-Bold.otf" }
    FontLoader { id: fontDinotLight; source: "/font/DINOT-Light.otf" }
    FontLoader { id: fontDinotMedium; source: "/font/DINOT-Medium.otf" }
    FontLoader { id: fontDinotRegular; source: "/font/DINOT-Regular.otf" }

    MessageDriverMissing {
        id: messageDriverMissing
        width: parent.width * 0.4
        height: parent.height * 0.2
        x: (parent.width * 0.6) / 2
        y: (parent.height * 0.8) / 2
        opacity: 0

        onShowDocumentationChanged: {
            if (showDocumentation) {
                updateDriver.openDocumentationNatively()
                mainFrame.opacity = 1
                opacity = 0
                showDocumentation = false
            }
        }
    }

    Image {
        id: mainFrame
        anchors.fill: parent
        source: "/img/main_frame.png"

        state: "WELCOME"

        property bool cancelPressed: false

        PageWelcome {
            id: pageWelcome
            width: parent.width
            height: bodyHeight
            y: bodyY
            opacity: 1

            Timer {
                id: timerWelcome
                running: pageWelcome.opacity
                interval: 100
                repeat: false
                onTriggered: {
                    updateDriver.search(false)
                }
            }
        }

        PageSearch {
            id: pageSearch
            width: parent.width
            height: bodyHeight
            y: bodyY
            opacity: 0

            Timer {
                id: timerSearchStartPostpone
                running: pageSearch.opacity
                interval: 500
                repeat: false
                onTriggered: {
                    updateDriver.search(true)
                }
            }

            Timer {
                id: timerSearchStatusPostpone
                running: false
                interval: 500
                repeat: false
                onTriggered: {
                    if ( updateDriver.result() ) {
                        mainFrame.state = "DEVICE_FOUND"

                        pageDeviceFound.logo = updateDriver.modelPicture
                        pageDeviceFound.name = updateDriver.modelName
                        pageDeviceFound.versionCurrent = updateDriver.versionCurrent
                        pageDeviceFound.versionNew = updateDriver.versionNew

                        if ( pageDeviceFound.logo == "/fmw/l/logo.jpg" ) {
                            pageStatus.isModelL = true
                            pageDeviceFound.isModelL = true
                        }

                        if (updateDriver.isUpdateAvailable()) {
                            pageDeviceFound.status = qsTr("Update available")
                        } else {
                            pageDeviceFound.status = qsTr("No update necessary")
                            mainButton.text = qsTr("INSTALL FIRMWARE")

                            if (updateDriver.versionCurrent != updateDriver.versionNew) {
                                pageDeviceFound.downgrade = true
                                mainButton.opacity = 0
                            }
                        }
                    } else {
                        if ( mainFrame.cancelPressed ) {
                            mainFrame.state = "WELCOME"
                        } else {
                            mainFrame.state = "SEARCH_ERROR"
                        }
                    }
                }
            }
        }

        PageDeviceFound {
            id: pageDeviceFound
            width: parent.width
            height: bodyHeight
            y: bodyY
            opacity: 0

            onChangelogClicked: {
                updateDriver.openChangelogNatively()
            }

            onPasswordVerified: {
                mainButton.opacity = 1
                mainButton.forceActiveFocus()
            }
        }

        PageUpdate {
            id: pageUpdate
            width: parent.width
            height: bodyHeight
            y: bodyY
            opacity: 0

            Timer {
                id: timerUpdateStartPostpone
                running: pageUpdate.opacity
                interval: 500
                repeat: false
                onTriggered: {
                    updateDriver.install()
                }
            }

            Timer {
                id: timerUpdateStatusPostpone
                running: false
                interval: 500
                repeat: false
                onTriggered: {
                    if (updateDriver.result()) {
                        mainFrame.state = "UPDATE_SUCCESS"
                    } else {
                        mainFrame.state = "UPDATE_ERROR"
                    }
                }
            }
        }

        PageStatus {
            id: pageStatus
            width: parent.width
            height: bodyHeight
            y: bodyY
            opacity: 0
        }

        Button {
            id: mainButton
            width: parent.width * 0.3
            height: footerHeight * 0.625
            x: parent.width - width - footerHeight * 0.2
            y: parent.height - footerHeight * 0.8

            text: qsTr("SCAN FOR DEVICES")

            onClicked: {
                if (mainFrame.state == "WELCOME") {
                    if (updateDriver.isDriverAvailable()) {
                        mainFrame.cancelPressed = false
                        mainFrame.state = "SEARCH"
                    } else {
                        mainFrame.opacity = 0
                        messageDriverMissing.opacity = 1
                    }
                } else if (mainFrame.state == "SEARCH") {
                    mainFrame.cancelPressed = true
                    updateDriver.search(false)
                } else if (mainFrame.state == "SEARCH_ERROR") {
                    mainFrame.state = "WELCOME"
                } else if (mainFrame.state == "DEVICE_FOUND") {
                    mainFrame.state = "UPDATE"
                } else if (mainFrame.state == "UPDATE_SUCCESS") {
                    mainFrame.state = "WELCOME"
                } else if (mainFrame.state == "UPDATE_ERROR") {
                    mainFrame.state = "WELCOME"
                }
            }

            style: ButtonStyle {
                background: Rectangle {
                    color: "#000000"
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
                    elide: Text.ElideRight
                    font { family: fontDinotBold.name; pointSize: height * (1 - 0.625) }
                }
            }
        }

        UpdateDriver {
            id: updateDriver
            onProgressChanged: {
                pageUpdate.progress = progress
            }
            onActiveChanged: {
                if (mainFrame.state == "SEARCH") {
                    if (!active) {
                        timerSearchStatusPostpone.running = true
                    }
                } else if (mainFrame.state == "UPDATE") {
                    if (!active) {
                        timerUpdateStatusPostpone.running = true
                    }
                }
            }
        }

        states: [
            State {
                name: "WELCOME"
                PropertyChanges {
                    target: mainButton
                    text: qsTr("SCAN FOR DEVICES")
                    opacity: 1
                }
                PropertyChanges {
                    target: pageWelcome
                    opacity: 1
                }
                PropertyChanges {
                    target: pageSearch
                    opacity: 0
                }
                PropertyChanges {
                    target: pageDeviceFound
                    opacity: 0
                }
                PropertyChanges {
                    target: pageUpdate
                    opacity: 0
                }
                PropertyChanges {
                    target: pageStatus
                    opacity: 0
                }
            },
            State {
                name: "SEARCH"
                PropertyChanges {
                    target: mainButton
                    text: qsTr("CANCEL")
                    opacity: 1
                }
                PropertyChanges {
                    target: pageWelcome
                    opacity: 0
                }
                PropertyChanges {
                    target: pageSearch
                    opacity: 1
                }
                PropertyChanges {
                    target: pageDeviceFound
                    isModelL: false
                    downgrade: false
                    opacity: 0
                }
                PropertyChanges {
                    target: pageUpdate
                    opacity: 0
                }
                PropertyChanges {
                    target: pageStatus
                    isModelL: false
                    opacity: 0
                }
            },
            State {
                name: "SEARCH_ERROR"
                PropertyChanges {
                    target: mainButton
                    text: qsTr("MAIN MENU")
                    opacity: 1
                }
                PropertyChanges {
                    target: pageWelcome
                    opacity: 0
                }
                PropertyChanges {
                    target: pageSearch
                    opacity: 0
                }
                PropertyChanges {
                    target: pageDeviceFound
                    opacity: 0
                }
                PropertyChanges {
                    target: pageUpdate
                    opacity: 0
                }
                PropertyChanges {
                    target: pageStatus
                    opacity: 1
                    state: qsTr("SEARCH_ERROR")
                }
            },
            State {
                name: "DEVICE_FOUND"
                PropertyChanges {
                    target: mainButton
                    text: qsTr("INSTALL UPDATE")
                    opacity: 1
                }
                PropertyChanges {
                    target: pageWelcome
                    opacity: 0
                }
                PropertyChanges {
                    target: pageSearch
                    opacity: 0
                }
                PropertyChanges {
                    target: pageDeviceFound
                    opacity: 1
                }
                PropertyChanges {
                    target: pageUpdate
                    opacity: 0
                }
                PropertyChanges {
                    target: pageStatus
                    opacity: 0
                }
            },
            State {
                name: "UPDATE"
                PropertyChanges {
                    target: mainButton
                    text: qsTr("NO BUTTON")
                    opacity: 0
                }
                PropertyChanges {
                    target: pageWelcome
                    opacity: 0
                }
                PropertyChanges {
                    target: pageSearch
                    opacity: 0
                }
                PropertyChanges {
                    target: pageDeviceFound
                    opacity: 0
                }
                PropertyChanges {
                    target: pageUpdate
                    opacity: 1
                    progress: 0
                }
                PropertyChanges {
                    target: pageStatus
                    opacity: 0
                }
            },
            State {
                name: "UPDATE_ERROR"
                PropertyChanges {
                    target: mainButton
                    text: qsTr("MAIN MENU")
                    opacity: 1
                }
                PropertyChanges {
                    target: pageWelcome
                    opacity: 0
                }
                PropertyChanges {
                    target: pageSearch
                    opacity: 0
                }
                PropertyChanges {
                    target: pageDeviceFound
                    opacity: 0
                }
                PropertyChanges {
                    target: pageUpdate
                    opacity: 0
                }
                PropertyChanges {
                    target: pageStatus
                    opacity: 1
                    state: qsTr("UPDATE_ERROR")
                }
            },
            State {
                name: "UPDATE_SUCCESS"
                PropertyChanges {
                    target: mainButton
                    text: qsTr("MAIN MENU")
                    opacity: 1
                }
                PropertyChanges {
                    target: pageWelcome
                    opacity: 0
                }
                PropertyChanges {
                    target: pageSearch
                    opacity: 0
                }
                PropertyChanges {
                    target: pageDeviceFound
                    opacity: 0
                }
                PropertyChanges {
                    target: pageUpdate
                    opacity: 0
                }
                PropertyChanges {
                    target: pageStatus
                    opacity: 1
                    state: qsTr("UPDATE_SUCCESS")
                }
            }
        ]
    }
}
