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
#include "feedparser.h"
#include <QDBusConnection>
#include <QDBusMessage>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QRegExp>

static QString CONFIG_FILE("/home/user/.local/share/data/rssevents/feeds");

static QString DEFAULT_ICON("general_rss");

static QString DBUS_SERVICE("org.hildon.eventfeed");
static QString DBUS_PATH("/org/hildon/eventfeed");
static QString DBUS_INTERFACE("org.hildon.eventfeed");
static QString DBUS_METHOD("addItem");

static qlonglong addItemToEventFeed(const QVariantMap &item) {
    QDBusMessage message = QDBusMessage::createMethodCall(DBUS_SERVICE, DBUS_PATH, DBUS_INTERFACE, DBUS_METHOD);
    message.setArguments(QVariantList() << item);
    const QVariantList args = QDBusConnection::sessionBus().call(message).arguments();    
    return args.isEmpty() ? -1 : args.first().toLongLong();
}

RssEvents::RssEvents(QObject *parent) :
    QObject(parent),
    m_parser(new FeedParser(this)),
    m_nam(new QNetworkAccessManager(this)),
    m_reply(0),
    m_index(-1)
{
}

RssEvents::~RssEvents() {
    if (m_reply) {
        delete m_reply;
        m_reply = 0;
    }
}

void RssEvents::getEvents() {
    if (!m_feeds.isEmpty()) {
        return;
    }
    
    m_index = -1;
    readFeeds();
    
    if (m_feeds.isEmpty()) {
        emit finished();
    }
    else {
        next();
    }
}

void RssEvents::next() {
    m_index++;
    
    if (m_index == m_feeds.size()) {
        writeFeeds();
        m_feeds.clear();
        m_index = -1;
        emit finished();
        return;
    }
    
    if (m_reply) {
        delete m_reply;
    }
    
    m_reply = m_nam->get(QNetworkRequest(m_feeds.at(m_index).first));
    connect(m_reply, SIGNAL(finished()), this, SLOT(parseFeed()));
}

void RssEvents::readFeeds() {
    QFile file(CONFIG_FILE);
    
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        while (!file.atEnd()) {
            QStringList split = QString::fromUtf8(file.readLine()).split("\t", QString::SkipEmptyParts);
            
            if (split.size() > 1) {
                m_feeds << QPair<QString, QDateTime>(split.at(0), QDateTime::fromString(split.at(1), Qt::ISODate));
            }
        }
        
        file.close();
    }
}

void RssEvents::writeFeeds() {
    QFile file(CONFIG_FILE);
    
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        for (int i = 0; i < m_feeds.size(); i++) {
            file.write(QString("%1\t%2\n").arg(m_feeds.at(i).first)
                                          .arg(m_feeds.at(i).second.toString(Qt::ISODate)).toUtf8());
        }
        
        file.close();
    }
}

void RssEvents::parseFeed() {
    if ((!m_parser->setContent(m_reply->readAll())) || (!m_parser->readChannel())) {
        m_reply->deleteLater();
        m_reply = 0;
        next();
        return;
    }
    
    const QString title = m_parser->title();
    const QString id = m_feeds.at(m_index).first;
    const QDateTime lastUpdated = m_feeds.at(m_index).second;
    
    while (m_parser->readNextArticle()) {
        if (m_parser->date() <= lastUpdated) {
            break;
        }
        
        QVariantMap item;
        item["icon"] = DEFAULT_ICON;
        item["title"] = m_parser->title();
        item["body"] = m_parser->description().remove(QRegExp("<[^>]*>"));
        item["footer"] = m_parser->author();
        item["timestamp"] = m_parser->date().toString(Qt::ISODate);
        item["url"] = m_parser->url();
        item["sourceName"] = QString("rss_events_%1").arg(id);
        item["sourceDisplayName"] = title;
        
        if (addItemToEventFeed(item) == -1) {
            break;
        }
    }
    
    m_feeds[m_index].second = QDateTime::currentDateTime();
    m_reply->deleteLater();
    m_reply = 0;
    next();
}
