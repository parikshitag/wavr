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

#include <QApplication>

#include "mainwindow.h"

#include <QtCore/QSettings>
#include <QtNetwork/QNetworkConfigurationManager>
#include <QtNetwork/QNetworkSession>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkRequired){
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

	    QNetworkSession *networkSession = new QNetworkSession(config, &app);
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

    MainWindow w;
    w.show();
    
    return a.exec();
}
