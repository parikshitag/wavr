/****************************************************************************
**
** This file is part of Wavr IM Application.
** 
** Copyright (c) 2013 Parikshit Agarwal.
** 
** Contact:  parikshit.ag@gmail.com
** 
** Wavr is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Wavr is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Wavr.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/


/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  containing the entry point of the project
 *
 *        Version:  1.0
 *        Created:  Friday 25 January 2013 06:03:09  IST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Parikshit Agarwal (), parikshit.ag@hotmail.com
 *   Organization:  Norps
 *
 * =====================================================================================
 */

#include "application.h"
#include "core.h"

#include <QtCore/QSettings>
#include <QtNetwork/QNetworkConfigurationManager>
#include <QtNetwork/QNetworkSession>

//  Define a unique application id. This is a combination of two GUIDs
const QString appId = "887df9b7-8131-4836-9004-1b1e458c9d9c-6d5a04e0-bab9-414a-8d72-2a8a80eaca47";

int main(int argc, char *argv[])
{
    Application application(appId, argc, argv);
    QDir::setCurrent(QApplication::applicationDirPath());

    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired){
	    // Get saved network configuration
	    QSettings settings (QSettings::UserScope, QLatin1String("Wavr"));
	    settings.beginGroup(QLatin1String("WavrSettings"));
	    const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
	    settings.endGroup();
	    
	    //If the saved network configuration is not currently discovered user the system default
	    QNetworkConfiguration config = manager.configurationFromIdentifier(id);
	    if ((config.state() & QNetworkConfiguration::Discovered) != QNetworkConfiguration::Discovered)
	    {
		    config = manager.defaultConfiguration();
            }

        QNetworkSession *networkSession = new QNetworkSession(config, &application);
	    networkSession->open();
	    networkSession->waitForOpened();

	    if (networkSession->isOpen()) {
		    //Save the used configuration
		    QNetworkConfiguration config = networkSession->configuration();
		    QString id;
		    if  (config.type() == QNetworkConfiguration::UserChoice) {
			    id = networkSession->sessionProperty(
			    			QLatin1String("UserChoiceConfiguration")).toString();
		    } else{
			    id = config.identifier();
		    }
		    
		    QSettings settings(QSettings::UserScope, QLatin1String("Wavr"));
		    settings.beginGroup(QLatin1String("WavrSettings"));
		    settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
		    settings.endGroup();
	    }
    }

    wavrCore core;
    
    core.init();

    return application.exec();
}
