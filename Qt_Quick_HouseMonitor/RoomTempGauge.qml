
import QtQuick 2.0
import QtQuick.Layouts 1.2
import QtQuick.Window 2.2
import QtLocation 5.5
import QtQuick 2.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Extras 1.4


    Rectangle {
        id: rectangle1
        color: "#ffffff"
        radius: width/6
        implicitHeight: 130
        implicitWidth: 44

        function setValue(value)
        {
            gauge1.value = value
        }

        Gauge {

            id: gauge1
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            anchors.top: parent.top
            visible: parent.parent.width == 0 ? false : true
            //x: -parent.width/4

            minimumValue: -40
            maximumValue: 40
        /*
            Behavior on value {
                NumberAnimation {
                    duration: 300
                }
            }
        */
            style: GaugeStyle {
                    valueBar: Rectangle {
                        implicitWidth: 5
                        color: Qt.rgba(gauge1.value / gauge1.maximumValue, 0, 1 - gauge1.value / gauge1.maximumValue, 1)
                    }

                    tickmark: Item {
                        implicitWidth: 12
                        implicitHeight: 1

                        Rectangle {
                            color: "#000000"
                            anchors.fill: parent
                            anchors.leftMargin: 3
                            anchors.rightMargin: 3
                        }
                    }
                    minorTickmark: Item {
                        implicitWidth: 4
                        implicitHeight: 1

                        Rectangle {
                            color: "#000000"
                            anchors.fill: parent
                            anchors.leftMargin: 2
                            anchors.rightMargin: 4
                        }
                    }

                    tickmarkLabel:  Text {
                        font.pixelSize: gauge1.height / 17
                        text: styleData.value
                        color: "#000000"
                        antialiasing: true
                    }
                    background: Rectangle
                    {
                    color: "#888888"
            }


          }
        }
    }






