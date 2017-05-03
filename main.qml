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
        fileSync: true
    }

    JsonFluxView{
        id: jsonQuery
        query: ["/widget/debug", "/widget/image"]
        //model: jsonModel
        modelName: "mymodel"
        property var myVar
        onMyVarChanged: {
            console.log("new myVar:", myVar);
        }

        onValuesChanged: {
           // console.log(values["@0"], values["@1"]);
            myVar = values["@0"];
        }
    }

    JsonFluxModifier{
        id: jsonModifier
        //model: jsonModel
        modelName: "mymodel"
    }

    Button{
        id: btn
        width: 100
        height: 50
        anchors.centerIn: parent
        text: "Click!"

        onClicked:{
            var obj = {};
            obj.a = "jimmy";
            obj.b = "lucy";
            jsonModifier.modify("/widget/debug", /*false*/["Aa", "Bb", "Cc", "Dd"]/*[false, true, false]*/);
            //jsonModifier.modify("/widget/debug", obj);
        }
    }
}
