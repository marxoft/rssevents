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

#ifndef RSSEVENTS_H
#define RSSEVENTS_H

#include <QObject>
#include <QList>
#include <QPair>
#include <QDateTime>

class FeedParser;
class QNetworkAccessManager;
class QNetworkReply;

class RssEvents : public QObject
{
    Q_OBJECT
    
public:
    explicit RssEvents(QObject *parent = 0);
    ~RssEvents();
    
    void getEvents();

private Q_SLOTS:
    void parseFeed();

Q_SIGNALS:
    void finished();
    
private:    
    void next();
    
    void readFeeds();
    void writeFeeds();
    
    FeedParser *m_parser;
    QNetworkAccessManager *m_nam;
    QNetworkReply *m_reply;
    
    int m_index;
    QList< QPair<QString, QDateTime> > m_feeds;
};
    
#endif // RSSEVENTS_H
