import QtQuick
import QtQuick.Layouts
import "cppbridge" as CppBridge
import org.kde.kirigami as Kirigami
import org.kde.plasma.components as PlasmaComponents
import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.plasmoid

PlasmoidItem {
    id: root

    // Widget setup
    width: Kirigami.Units.gridUnit * 20
    height: Kirigami.Units.gridUnit * 20
    Plasmoid.backgroundHints: PlasmaCore.Types.NoBackground
    toolTipMainText: "katoclock"
    Component.onCompleted: {
        const initialized = clockBridge.initialize(Qt.resolvedUrl("assets/background.png"), Qt.resolvedUrl("assets/hand_hour.png"), Qt.resolvedUrl("assets/hand_minute.png"));
        if (!initialized)
            console.warn(clockBridge.errorString);

    }

    CppBridge.ClockBridge {
        id: clockBridge
    }

    component ClockImageView: Item {
        // asset image size -> 421x600, 1 is cropping artifact but I'll live with it
        implicitWidth: 421
        implicitHeight: 600

        Image {
            anchors.fill: parent
            visible: clockBridge.ready
            source: clockBridge.imageSource
            fillMode: Image.PreserveAspectFit
            asynchronous: true
            cache: false
        }

        PlasmaComponents.Label {
            anchors.fill: parent
            visible: !clockBridge.ready
            text: clockBridge.errorString.length > 0 ? clockBridge.errorString : "Loading..."
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            wrapMode: Text.Wrap
        }

    }

    // full representation (popup or desktop widget)
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

    // compact representation (icon in panel)
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
