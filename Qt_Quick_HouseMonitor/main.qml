import QtQuick 2.3
import QtQuick.Window 2.2
import QtPositioning 5.5
import QtQuick.Layouts 1.2
import Qt.labs.settings 1.0
import QtQuick.Controls 1.3
import QtQuick.LocalStorage 2.0



//import "roomView.qml" as HouseViewModule

Window {
    width: 800
    height: 480
    visible: true

    Item {
        id: dataUpdateTimer
        Timer {
            interval: 5000; running: true; repeat: true
            onTriggered: {
                //time.text = Date().toString()
                //roomView.updateRoomData(20,20,20,1,1,1);
                //reader.readDataFromTS()


            }

        }

        Text { id: time }
    }



    Connections {
        target: reader
        onSendToQml: {
            roomView.updateRoomData(temp1,temp2,temp3,door1,door2,door3);
        }
    }


    MouseArea {
        id: mainApp
        width: 800
        height: 480
        hoverEnabled: true
        anchors.fill: parent


        Button {
            id: quit_btn
            x: 707
            y: 445
            text: qsTr("Quit")
            onClicked: {
                Qt.quit();
            }
        }

    }


    RoomView{
        id: roomView
        width: 800
        height: 400
        anchors.rightMargin: 0
        anchors.bottomMargin: 79
        anchors.fill: parent
        visible: true
    }
}
