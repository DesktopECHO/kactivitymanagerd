/*   vim:set foldenable foldmethod=marker:
 *
 *   Copyright (C) 2015 Ivan Cukic <ivan.cukic(at)kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2,
 *   or (at your option) any later version, as published by the Free
 *   Software Foundation
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.0
import org.kde.kquickcontrolsaddons 2.0
import QtQuick.Controls 1.0 as QtControls

import org.kde.activities 0.1 as Activities
import org.kde.plasma.core 2.0 as PlasmaCore

import "static.js" as S

Item {
    id: root

    anchors.fill: parent

    QtControls.Button {
        id: buttonCreateActivity

        text: i18n("Create activity...")
        iconName: "list-add"

        anchors {
            top: parent.top
            left: parent.left
        }

        onClicked: S.openActivityCreationDialog(
                        dialogCreateActivityLoader,
                        {
                            kactivities: kactivities,
                            kactivitiesExtras: kactivitiesExtras,
                            readyStatus: Loader.Ready
                        }
                    )

        enabled: !dialogCreateActivityLoader.itemVisible
    }

    Loader {
        id: dialogCreateActivityLoader

        property bool itemVisible: status == Loader.Ready && item.visible

        z: 1

        anchors {
            top: buttonCreateActivity.bottom
            left: buttonCreateActivity.left
        }
    }

    QtControls.ScrollView {
        anchors {
            top: buttonCreateActivity.bottom
            topMargin: units.smallSpacing
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        enabled: !dialogCreateActivityLoader.itemVisible

        ListView {
            width: parent.width
            // anchors.fill: parent

            model: Activities.ActivityModel {
                id: kactivities
            }

            SystemPalette {
                id: palette
                colorGroup: SystemPalette.Active
            }

            ///////////////////////////////////////////////////////////////////
            delegate: Rectangle {
                width: parent.width

                Behavior on height { PropertyAnimation { duration: units.shortDuration } }
                height: icon.height + units.smallSpacing * 2 +
                            (dialogConfigureLoader.itemVisible ? dialogConfigureLoader.height : 0) +
                            (dialogDeleteLoader.itemVisible ? dialogDeleteLoader.height : 0)

                color: (model.index % 2 == 0) ? palette.base : palette.alternateBase

                Item {
                    id: header

                    height: units.iconSizes.medium

                    anchors {
                        left: parent.left
                        right: parent.right
                        top: parent.top
                    }

                    QIconItem {
                        id: icon
                        icon: model.icon

                        width:  height
                        height: parent.height

                        anchors {
                            left:   parent.left
                            top:    parent.top

                            topMargin: units.smallSpacing
                            bottomMargin: units.smallSpacing
                        }
                    }

                    QtControls.Label {
                        text: model.name

                        anchors {
                            left: icon.right
                            right: buttons.left
                            leftMargin: units.largeSpacing
                            verticalCenter: icon.verticalCenter
                        }
                    }

                    Row {
                        id: buttons

                        spacing: units.smallSpacing

                        anchors {
                            right: parent.right

                            rightMargin: units.smallSpacing
                            verticalCenter: parent.verticalCenter
                        }

                        QtControls.Button {
                            id: buttonConfigure

                            iconName: "configure"

                            onClicked: S.openActivityConfigurationDialog(
                                            dialogConfigureLoader,
                                            model.id,
                                            model.name,
                                            model.iconSource,
                                            {
                                                kactivities: kactivities,
                                                kactivitiesExtras: kactivitiesExtras,
                                                readyStatus: Loader.Ready,
                                                i18nd:       i18nd
                                            }
                                        );
                        }

                        QtControls.Button {
                            id: buttonDelete

                            iconName: "edit-delete"

                            onClicked: S.openActivityDeletionDialog(
                                            dialogDeleteLoader,
                                            model.id,
                                            {
                                                kactivities: kactivities,
                                                readyStatus: Loader.Ready,
                                                i18nd:       i18nd
                                            }
                                        );
                        }

                        visible: !dialogDeleteLoader.itemVisible
                    }

                    visible: !dialogConfigureLoader.itemVisible
                }

                Loader {
                    id: dialogConfigureLoader

                    property bool itemVisible: status == Loader.Ready && item.visible

                    anchors {
                        left: parent.left
                        top: parent.top
                    }
                }

                Loader {
                    id: dialogDeleteLoader

                    property bool itemVisible: status == Loader.Ready && item.visible

                    anchors {
                        left: parent.left
                        top: header.bottom
                    }
                }
            }
            ///////////////////////////////////////////////////////////////////
        }
    }
}
