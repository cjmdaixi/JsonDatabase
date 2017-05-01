import QtQuick 2.6
import QtQuick.Window 2.2
import JsonFlux 1.0

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
        onValuesChanged: {
            console.log(values["@0"], values["@1"]);
        }
    }
}
