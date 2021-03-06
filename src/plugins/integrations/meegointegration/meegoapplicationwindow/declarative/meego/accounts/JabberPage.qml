// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import org.qutim 0.3
import "../constants.js" as CONST
import ".."

SettingsItemPage {
    id: root
    anchors.margins: 10
    property alias account: root.controller
    impl: QtObject {
        function save() {
            config.setValue("general/passwd", passwordField.text, Config.Crypted);
            config.setValue("general/resource", resourceField.text);
        }
        function load() {
            passwordField.text = config.value("general/passwd", "", Config.Crypted);
            resourceField.text = config.value("general/resource", "qutIM");
        }
    }
    Config {
        id: config
        object: root.account
    }
    FlickableColumn {
		spacing: 10
        anchors.fill: parent
        Label {
            width: root.width
            text: qsTr("Jabber ID:") + " <i>" + (root.account === null ? "" : root.account.id) + "</i>"
        }
        Label {
            text: qsTr("Password:")
        }
        TextField {
            id: passwordField
            width: root.width
	    echoMode: TextInput.Password
        }
        Label {
            text: qsTr("Resource:")
        }
        TextField {
            id: resourceField
            width: root.width
        }
	Text {
		id:privacy
		anchors.horizontalCenter: parent.horizontalCenter
		text: "<a href=\"%1\">Privacy Policy</a>".arg(CONST.PRIVACY_POLICY_LINK);
		onLinkActivated: {
			Qt.openUrlExternally(link);
		}
	}
    }
    
    tools: SettingsToolBarLayout {
		id: toolBarLayout
        page: root
	}
}
