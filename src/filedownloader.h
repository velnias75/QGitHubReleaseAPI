/*
 * Copyright 2015 by Heiko Schäfer <heiko@rangun.de>
 *
 * This file is part of NetMauMau Qt Client.
 *
 * NetMauMau Qt Client is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * NetMauMau Qt Client is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with NetMauMau Qt Client.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H

#include <QUrl>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>

#include "export.h"

class QGITHUBRELEASEAPI_NO_EXPORT FileDownloader : public QObject {
	Q_OBJECT
	Q_DISABLE_COPY(FileDownloader)
public:
	FileDownloader(const QUrl &url, const char *userAgent, QObject *parent = 0L);
	virtual ~FileDownloader();

	void start() const;

	void setCacheLoadControlAttribute(QNetworkRequest::CacheLoadControl att);

	inline void setUserData(QVariant &ud) {
		m_userData = &ud;
	}

	inline QUrl url() const {
		return m_url;
	}

	const QByteArray &downloadedData() const;

	inline QList<QNetworkReply::RawHeaderPair> rawHeaderPairs() const {
		return m_rawHeaderPairs;
	}

signals:
	void downloaded(const FileDownloader &, QVariant *);
	void error(const QString &, QVariant *);
	void progress(qint64, qint64, QVariant *);

private slots:
	void fileDownloaded(QNetworkReply *pReply);
	void downloadProgress(qint64, qint64);

private:
	mutable QNetworkAccessManager m_WebCtrl;
	QByteArray m_DownloadedData;
	QUrl m_url;
	QList<QNetworkReply::RawHeaderPair> m_rawHeaderPairs;
	mutable QNetworkReply *m_reply;
	QVariant *m_userData;
	QNetworkRequest m_request;
};

#endif // FILEDOWNLOADER_H
