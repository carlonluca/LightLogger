/*
 * Author:  Luca Carlon
 * Company: Ennova Research
 * Date:    28.12.2013
 */

import QtQuick 2.0

Rectangle {
    width: 100
    height: 62

    Component.onCompleted: {
        var i = 1;

        logger.verbose("Some verbose message from QML!");
        logger.info("Some info message from QML: Qt is number " + i + ".");
        logger.warn("Some dangerous warning message from QML! >:-)");
    }
}
