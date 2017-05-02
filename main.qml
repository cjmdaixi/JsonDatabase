import QtQuick 2.6
import QtQuick.Window 2.2
import JsonFlux 1.0
import QtQuick.Controls 1.4

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")
    property var myarry: []

    JsonFluxModel{
        id: jsonModel
        source: "demo.json"
        name: "mymodel"
    }

    JsonFluxView{
        id: jsonQuery
        query: ["/widget/debug", "/widget/image"]
        model: jsonModel
        //modelName: "mymodel"
        onValuesChanged: {
            console.log(values["@0"], values["@1"]);
        }
    }

    JsonFluxModifier{
        id: jsonModifier
        model: jsonModel
    }

    Button{
        id: btn
        width: 100
        height: 50
        anchors.centerIn: parent
        text: "Click!"

        onClicked:{
            jsonModifier.modify("/widget/debug", [false, true, false]);
        }
    }
}
