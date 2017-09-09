/*
 *  Copyright (C) 2010, 2011 Tuomo Penttinen, all rights reserved.
 *
 *  Author: Tuomo Penttinen <tp@herqq.org>
 *
 *  This file is part of Herqq UPnP (HUPnP) library.
 *
 *  Herqq UPnP is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Herqq UPnP is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with Herqq UPnP. If not, see <http://www.gnu.org/licenses/>.
 */

#include "hcontrolpoint_dataretriever_p.h"

#include "hlogger_p.h"
#include "hupnp_global_p.h"

#include <QtCore/QUrl>
#include <QtCore/QTimerEvent>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

namespace Herqq
{

namespace Upnp
{

HDataRetriever::HDataRetriever(const QByteArray& loggingId) :
    m_loggingIdentifier(loggingId), m_nam(), m_reply(0), m_lastError(),
    m_success(false)
{
    bool ok = connect(
        &m_nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(finished()));
    Q_ASSERT(ok); Q_UNUSED(ok)
}

void HDataRetriever::finished()
{
    HLOG2(H_AT, H_FUN, (char*) (m_loggingIdentifier.data()));

    quit();

    if (m_reply->error() != QNetworkReply::NoError)
    {
        m_success = false;
        HLOG_WARN(QString(QLatin1String("Request failed: %1")).arg(m_reply->errorString()));
    }
    else
    {
        m_success = true;
    }
}

bool HDataRetriever::retrieveData(
    const QUrl& baseUrl, const QUrl& query, QByteArray* data)
{
    HLOG2(H_AT, H_FUN, (char*) (m_loggingIdentifier.data()));

    QString queryPart = extractRequestPart(query);

    QString request = queryPart.startsWith(QLatin1Char('/')) ?
                      extractHostPart(baseUrl) : baseUrl.toString();

    if (!query.isEmpty())
    {
        if (!request.endsWith(QLatin1Char('/'))) { request.append(QLatin1Char('/')); }
        if (queryPart.startsWith(QLatin1Char('/'))) { queryPart.remove(0, 1); }
        request.append(queryPart);
    }

    if (request.isEmpty())
    {
        request.append(QLatin1Char('/'));
    }

    QUrl _url(request);
    QNetworkRequest req(_url);
    m_reply = m_nam.get(req);

    int id = startTimer(3000);
    exec();
    killTimer(id);

    if (m_success)
    {
        *data = m_reply->readAll();
    }

    m_reply->deleteLater(); m_reply = 0;
    return m_success;
}

void HDataRetriever::timerEvent(QTimerEvent* event)
{
    HLOG2(H_AT, H_FUN, (char*) (m_loggingIdentifier.data()));

    HLOG_WARN(QString(QLatin1String("Request timed out.")));

    quit();
    killTimer(event->timerId());

    m_success = false;
}

bool HDataRetriever::retrieveServiceDescription(
    const QUrl& deviceLocation, const QUrl& scpdUrl, QString* data)
{
    HLOG2(H_AT, H_FUN, (char*) (m_loggingIdentifier.data()));

    HLOG_DBG(QString(QLatin1String(
        "Attempting to fetch a service description for [%1] from: [%2]")).arg(
            scpdUrl.toString(), deviceLocation.toString()));

    QByteArray tmp;
    if (!retrieveData(deviceLocation, scpdUrl, &tmp))
    {
        return false;
    }

    *data = QString::fromUtf8(tmp);
    return true;
}

bool HDataRetriever::retrieveIcon(
    const QUrl& deviceLocation, const QUrl& iconUrl, QByteArray* data)
{
    HLOG2(H_AT, H_FUN, (char*) (m_loggingIdentifier.data()));

    HLOG_DBG(QString(QLatin1String(
        "Attempting to retrieve icon [%1] from: [%2]")).arg(
            iconUrl.toString(), deviceLocation.toString()));

    return retrieveData(deviceLocation, iconUrl, data);
}

bool HDataRetriever::retrieveDeviceDescription(
    const QUrl& deviceLocation, QString* data)
{
    HLOG2(H_AT, H_FUN, (char*) (m_loggingIdentifier.data()));

    HLOG_DBG(QString(QLatin1String(
        "Attempting to fetch a device description from: [%1]")).arg(
            deviceLocation.toString()));

    QByteArray tmp;
    if (!retrieveData(deviceLocation, QUrl(), &tmp))
    {
        return false;
    }

    *data = QString::fromUtf8(tmp);
    return true;
}

}
}
