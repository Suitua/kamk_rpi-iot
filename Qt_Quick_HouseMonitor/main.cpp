 /*
 *  Qt_Quick_HouseMonitor
 *
 *  Description: "Qt_Quick_HouseMonitor is a Qt framework based UI project.
 *  It allows displaying data from ThingSpeak cloud service. 
 *  The project has been tested on Raspian (Wheezy) and on desktop PC (Linux Mint)"
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Version 1.0
 *  Author: Timo Leinonen 
 */

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "thingspeakreader.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    qRegisterMetaType<TS_CHANNEL_LAST>("TS_CHANNEL_LAST");


    ThingSpeakReader reader;
    QQmlContext* ctx = engine.rootContext();
    ctx->setContextProperty("reader", &reader);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();


}
