/*
 * Copyright 2015 by Heiko Schäfer <heiko@rangun.de>
 *
 * This file is part of QGitHubReleaseAPI.
 *
 * QGitHubReleaseAPI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * QGitHubReleaseAPI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with QGitHubReleaseAPI.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H

#include <QUrl>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>

#include "export.h"
#include "qgithubreleaseapi.h"

class QGITHUBRELEASEAPI_NO_EXPORT FileDownloader : public QObject {
	Q_OBJECT
	Q_DISABLE_COPY(FileDownloader)
public:
#if QT_VERSION >= QT_VERSION_CHECK(4, 7, 0)
	typedef QNetworkReply::RawHeaderPair RAWHEADERPAIR;
#else
	typedef QPair<QByteArray, QByteArray> RAWHEADERPAIR;
#endif
	typedef QList<RAWHEADERPAIR> RAWHEADERPAIRLIST;

	FileDownloader(const QUrl &url, const char *userAgent, const QString &eTag = QString::null,
				   QObject *parent = 0L);
	virtual ~FileDownloader();

	QNetworkReply *start(QGitHubReleaseAPI::TYPE type) const;

	inline QString userAgent() const {
		return m_userAgent;
	}

	inline void setGeneric(bool b) {
		m_generic = b;
	}

	void setCacheLoadControlAttribute(QNetworkRequest::CacheLoadControl att);

	inline QUrl url() const {
		return m_url;
	}

	const QByteArray &downloadedData() const;

	inline RAWHEADERPAIRLIST rawHeaderPairs() const {
		return m_rawHeaderPairs;
	}

signals:
	void canceled();
	void downloaded(const FileDownloader &);
	void error(const QString &);
	void progress(qint64, qint64);
	void replyChanged(QNetworkReply *);

public slots:
	void cancel(const FileDownloader &);
	void abort() const;

private slots:
	void fileDownloaded(QNetworkReply *pReply);
	void downloadProgress(qint64, qint64);

private:
	mutable QNetworkAccessManager m_WebCtrl;
	QByteArray m_DownloadedData;
	QUrl m_url;
	RAWHEADERPAIRLIST m_rawHeaderPairs;
	mutable QNetworkReply *m_reply;
	mutable QNetworkRequest m_request;
	QString m_userAgent;
	bool m_generic;
};

#endif // FILEDOWNLOADER_H
