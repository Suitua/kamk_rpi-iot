import QtQuick 2.0
import QtQuick.Layouts 1.2
import QtQuick.Window 2.2
import QtLocation 5.5
import QtQuick 2.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Extras 1.4


Item {
    id: houseView
    width: 800
    height: 400
    anchors.fill: parent

    function updateRoomData(room1Temp,room2Temp,room3Temp,room1Door,room2Door,room3Door)
    {

        if(room1Door === 1)
            roomDoor1.open();
        else
            roomDoor1.close();

        if(room2Door === 1)
            roomDoor2.open();
        else
            roomDoor2.close();

        if(room3Door === 1)
            roomDoor3.open();
        else
            roomDoor3.close();

        roomTempGauge1.setValue(room1Temp);
        roomTempGauge2.setValue(room2Temp);
        roomTempGauge3.setValue(room3Temp);
    }

    Rectangle {
        id: room1_rect
        x: 0
        y: 0
        width: 500
        height: 244
        color: "#e4e19b"
        z: -1
        clip: false
        visible: true
        opacity: 1
        border.width: height/20
        border.color: "#855e42"
        /*
        BorderImage {
            id: name
            source: "Room_border2.png"
            width: 180; height: 180
            border { left: 30; top: 30; right: 30; bottom: 30 }
            horizontalTileMode: BorderImage.Repeat
            verticalTileMode: BorderImage.Repeat

        }
*/

        MouseArea {    width: 800

            id: room1_mouseArea
            anchors.fill: parent

            onClicked: {
                houseView.state == 'Room1' ? houseView.state = "" : houseView.state = 'Room1';
            }
        }

        RoomDoor {
            id: roomDoor1
            x: 129
            y: 0
            width: 100
            height: 13
        }

        RoomTempGauge {
            id: roomTempGauge1
            x: 14
            y: 57
            width: 44
            height: 130
            opacity: 0.8
        }



    }

    Rectangle {
        id: room2_rect
        x: 500
        y: 0
        width: 300
        height: 244
        color: "#c27391"
        radius: 0
        opacity: 1
        border.width: height/20
        border.color: "#855e42"

        MouseArea {
            id: room2_mouseArea
            anchors.fill: parent

            onClicked: {
                houseView.state == 'Room2' ? houseView.state = "" : houseView.state = 'Room2';
            }
        }

        RoomDoor {
            id: roomDoor2
            x: 240
            y: 84
            width: 100
            height: 13
            anchors.topMargin: 49
            anchors.bottomMargin: 96
            anchors.leftMargin: 215
            anchors.rightMargin: 33
            rotation: 90
        }

        RoomTempGauge {
            id: roomTempGauge2
            x: 14
            y: 57
            width: 44
            height: 130
            opacity: 0.8
        }
    }



    Rectangle {
        id: room3_rect
        x: 0
        y: 244
        width: 800
        height: 156
        color: "#bca3ad"
        opacity: 1
        antialiasing: false
        border.width: height/20
        border.color: "#855e42"



        MouseArea {
            id: room3_mouseArea
            anchors.fill: parent

            onClicked: {
                houseView.state == 'Room3' ? houseView.state = "" : houseView.state = 'Room3';
            }

        }

        RoomDoor {
            id: roomDoor3
            x: 306
            y: 143
            width: 100
            height: 13
            rotation: 180
        }

       RoomTempGauge {
            id: roomTempGauge3
            x: 13
            y: 13
            width: 44
            height: 130
            opacity: 0.8
            z: 2
       }
    }



    states: [
        State {
            name: "Room1"

            PropertyChanges {
                target: room2_rect
                width: 0
                height: 0
                x: parent.width
                y: 0
            }

            PropertyChanges {
                target: room3_rect
                width: 0
                height: 0
                x: 0
                y: room1_rect.height + height

            }

            PropertyChanges {
                target: room1_rect
                width: parent.width
                height: parent.height
                x: 0
                y: 0
            }

            PropertyChanges {
                target: roomDoor1
                x: 203
                y: 0
                width: 177
                height: 19
            }

            PropertyChanges {
                target: roomTempGauge1
                x: 26
                y: 58
                width: 70
                height: 240
            }
        },
        State {
            name: "Room2"

            PropertyChanges {
                target: room1_rect
                width: 0
                height: 0
            }

            PropertyChanges {
                target: room3_rect
                width: 0
                height: 0
                x: 0
                y: room2_rect.height + height

            }

            PropertyChanges {
                target: room2_rect
                width: parent.width
                height: parent.height
                x: 0
                y: 0
            }

            PropertyChanges {
                target: roomDoor2
                x: 715
                y: 140
                width: 140
                height: 30
                anchors.rightMargin: 64
                anchors.bottomMargin: 93
                anchors.leftMargin: 184
                anchors.topMargin: 52
            }

            PropertyChanges {
                target: roomTempGauge2
                x: 25
                y: 63
                width: 70
                height: 240
            }
        },
        State {
            name: "Room3"

            PropertyChanges {
                target: room1_rect
                width: 0
                height: 0

            }

            PropertyChanges {
                target: room2_rect
                width: 0
                height: 0
                x: parent.width
                y: 0

            }

            PropertyChanges {
                target: room3_rect
                width: parent.width
                height: parent.height
                x: 0
                y: 0
            }

            PropertyChanges {
                target: roomDoor3
                x: 306
                y: 370
                width: 100
                height: 30
            }

            PropertyChanges {
                target: roomTempGauge1
                width: 44
                height: 130
            }

            PropertyChanges {
                target: roomTempGauge3
                x: 20
                y: 70
                width: 70
                height: 240
            }

        }
    ]

    transitions: [
        Transition {
            NumberAnimation {
                properties: "width, height, x, y"
                duration: 250
            }

        }

    ]

}

