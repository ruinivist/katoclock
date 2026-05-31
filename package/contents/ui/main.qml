import QtQuick
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.plasmoid

PlasmoidItem {
    id: root

    readonly property real clockWidth: 421
    readonly property real clockHeight: 600
    readonly property real clockCenterX: 300
    readonly property real clockCenterY: 286
    readonly property real handScale: 0.154
    property real hourAngle: 0
    property real minuteAngle: 0

    function refreshTime() {
        const now = new Date();
        const minute = now.getMinutes();
        const hour = now.getHours();
        root.minuteAngle = minute * 6;
        root.hourAngle = (hour % 12) * 30 + minute * 0.5;
        let nextTick = 60000 - (now.getSeconds() * 1000 + now.getMilliseconds());
        if (nextTick <= 0 || nextTick > 60000)
            nextTick = 60000;

        minuteTimer.interval = nextTick;
        minuteTimer.restart();
    }

    width: Kirigami.Units.gridUnit * 20
    height: Kirigami.Units.gridUnit * 20
    Plasmoid.backgroundHints: PlasmaCore.Types.NoBackground
    toolTipMainText: "katoclock"
    Component.onCompleted: refreshTime()

    Timer {
        id: minuteTimer

        repeat: false
        onTriggered: root.refreshTime()
    }

    component ClockHand: Item {
        required property url imageSource
        required property real sourceWidth
        required property real sourceHeight
        required property real pivotFractionX
        required property real pivotFractionY
        required property real clockAngle
        readonly property real pivotX: width * pivotFractionX
        readonly property real pivotY: height * pivotFractionY

        width: sourceWidth * root.handScale
        height: sourceHeight * root.handScale
        x: root.clockCenterX - pivotX
        y: root.clockCenterY - pivotY

        Image {
            id: handImage

            anchors.fill: parent
            source: imageSource
            smooth: true
            mipmap: true

            transform: Scale {
                origin.x: handImage.width / 2
                origin.y: handImage.height / 2
                yScale: -1
            }

        }

        transform: Rotation {
            origin.x: pivotX
            origin.y: pivotY
            angle: clockAngle - 90
        }

    }

    component ClockImageView: Item {
        implicitWidth: root.clockWidth
        implicitHeight: root.clockHeight

        Item {
            width: root.clockWidth
            height: root.clockHeight
            anchors.centerIn: parent
            scale: Math.min(parent.width / width, parent.height / height)

            Image {
                anchors.fill: parent
                source: Qt.resolvedUrl("assets/background.png")
                smooth: true
                mipmap: true
            }

            ClockHand {
                imageSource: Qt.resolvedUrl("assets/hand_hour.png")
                sourceWidth: 227
                sourceHeight: 76
                pivotFractionX: 0.126
                pivotFractionY: 0.502
                clockAngle: root.hourAngle
            }

            ClockHand {
                imageSource: Qt.resolvedUrl("assets/hand_minute.png")
                sourceWidth: 316
                sourceHeight: 78
                pivotFractionX: 0.101
                pivotFractionY: 0.488
                clockAngle: root.minuteAngle
            }

        }

    }

    fullRepresentation: Item {
        Layout.minimumWidth: Kirigami.Units.gridUnit * 12
        Layout.minimumHeight: Kirigami.Units.gridUnit * 12
        Layout.preferredWidth: root.width
        Layout.preferredHeight: root.height

        ClockImageView {
            anchors.fill: parent
            anchors.margins: Kirigami.Units.smallSpacing
        }

    }

    compactRepresentation: Item {
        Layout.minimumWidth: Kirigami.Units.gridUnit * 6
        Layout.minimumHeight: Kirigami.Units.gridUnit * 6

        ClockImageView {
            anchors.fill: parent
        }

        MouseArea {
            anchors.fill: parent
            onClicked: root.expanded = !root.expanded
        }

    }

}
