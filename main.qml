import QtQuick 2.6
import QtQuick.Window 2.2
import JsonDatabase 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    JsonModel{
        id: jsonModel
        source: "demo.json"
        name: "mymodel"
    }

    JsonQuery{
        id: jsonQuery
        query: ["widget.debug", "widget.image.src"]
        model: "mymodel"
        onValuesChanged: {
            console.log(values["@0"], values["@1"]);
        }
    }
}
