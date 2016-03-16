import QtQuick 2.0

Item {

    id:door_item
    implicitHeight: 20
    implicitWidth: 150

    visible: parent.width == 0 ? false : true

    function close()
    {
        state = ""
    }

    function open()
    {
        state = 'Open'
    }

    Rectangle {
        id: door_rect
        x: 0
        y: doorFrame_rect.height
        height: doorFrame_rect.width - doorFrame_rect.height / 1.5
        width: doorFrame_rect.height / 1.5
        color: "#5b5e1c"
        rotation: -90
        transformOrigin: Item.Top
        radius: width / 4

        Image {
            id: image1
            anchors.fill: parent
            source: "2wood_texture.jpg"
        }
    }

    Rectangle {
        id: doorFrame_rect
        x: 0
        y: 0
        width: parent.width
        height: parent.height
        color: "#3560f8"
        radius: width / 2

        Image {
            id: image2
            anchors.fill: parent
            source: "1wood_texture.jpg"
        }
    }

    states: [
        State {
            name: "Open"
            PropertyChanges {
                target: door_rect
                rotation: 0
                //transform: Rotation { origin.x: 25; origin.y: 25; angle: 45}

            }

            PropertyChanges {
                target: doorFrame_rect
                color: "#fa2a2a"
            }
        }
    ]

    transitions: [
        Transition {
            NumberAnimation {
                properties: "rotation, color"
                duration: 2000
            }

        }

    ]

}

