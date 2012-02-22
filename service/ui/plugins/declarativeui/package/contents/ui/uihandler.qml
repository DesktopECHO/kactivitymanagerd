/*
 *   Copyright 2011 Marco Martin <mart@kde.org>
 *   Copyright 2012 Ivan Čukić <ivan.cukic@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 1.0
import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.mobilecomponents 0.1 as MobileComponents
import org.kde.qtextracomponents 0.1

Rectangle {
    id: main

    color: Qt.rgba(0, 0, 0, 0.35)

    width: 400
    height: 360

    property int mainIconSize: 64 + 32
    property int layoutPadding: 8

    MouseArea {
        onClicked: { uihandler.cancel() }

        anchors.fill: parent
    }

    PlasmaComponents.BusyIndicator {
        anchors.centerIn: parent
        running: visible
        visible: (!dialogNewPassword.active && !dialogPassword.active && !dialogMessage.active)
    }

    NewPasswordDialog {
        id: dialogNewPassword

        anchors {
            fill: parent
            leftMargin: 50
            topMargin: 32
            rightMargin: 50
        }

        property bool active: false
        transform: Translate {
            y: dialogNewPassword.active ? 0 : main.height - dialogNewPassword.y
            Behavior on y { NumberAnimation { duration: 300 } }
        }

        title:                  "Enter the password"
        passwordText1:          "Password:"
        passwordText2:          "Verify:"
        strengthText:           "Password strength meter:"
        passwordsMatchText:     "Passwords match"
        passwordsDontMatchText: "Passwords don't match"
        okText:                 "Ok"
        cancelText:             "Cancel"

        onPasswordChosen: uihandler.returnPassword(password)

        onCanceled: uihandler.cancel()

        // Just so that clicking inside this are doesn't call cancel
        MouseArea { anchors.fill: parent; z: -1; onClicked: {} }
    }

    PasswordDialog {
        id: dialogPassword

        anchors.centerIn: parent

        property bool active: false
        transform: Translate {
            y: dialogPassword.active ? 0 : main.height - dialogNewPassword.y
            Behavior on y { NumberAnimation { duration: 300 } }
        }

        title:      "Enter the password"
        okText:     "Unlock"
        cancelText: "Dismiss"

        onPasswordChosen: uihandler.returnPassword(password)
        onCanceled: uihandler.cancel()

        // Just so that clicking inside this are doesn't call cancel
        MouseArea { anchors.fill: parent; z: -1; onClicked: {} }
    }

    MessageDialog {
        id: dialogMessage

        property bool active: false
        transform: Translate {
            y: dialogMessage.active ? 0 : main.height - dialogNewPassword.y
            Behavior on y { NumberAnimation { duration: 300 } }
        }

        // Just so that clicking inside this are doesn't call cancel
        MouseArea { anchors.fill: parent; z: -1; onClicked: {} }
    }

    Connections {
        target: uihandler

        // void message(const QString & message);
        onMessage: {
            dialogMessage.text = message
            dialogMessage.active = true
        }

        // void askPassword(const QString & title, const QString & message, bool newPassword);
        onAskPassword: {
            if (newPassword) {
                dialogNewPassword.password = ""
                dialogNewPassword.passwordConfirmation = ""
                dialogNewPassword.active = true

            } else {
                dialogPassword.password = ""
                dialogPassword.active = true

            }
        }

        onHideAll: {
            dialogMessage.active = false
            dialogNewPassword.active = false
            dialogPassword.active = false
        }
    }
}