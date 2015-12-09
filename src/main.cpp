/*
 * Copyright (C) 2015 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "rssevents.h"
#include <QCoreApplication>

Q_DECL_EXPORT int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    app.setOrganizationName("RSS Events");
    app.setApplicationName("RSS Events");
    app.setApplicationVersion("0.1.0");
    
    RssEvents events;
    events.getEvents();
    
    QObject::connect(&events, SIGNAL(finished()), &app, SLOT(quit()));
    
    return app.exec();
}
