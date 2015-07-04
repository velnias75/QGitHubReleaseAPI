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

#include <QSslConfiguration>

#include "filedownloader.h"

FileDownloader::FileDownloader(const QUrl &url, const char *userAgent,
							   const QString &eTag, QObject *p) : QObject(p), m_WebCtrl(),
	m_DownloadedData(), m_url(url), m_rawHeaderPairs(), m_reply(0L), m_request(url),
	m_userAgent(userAgent), m_generic(false) {

	QObject::connect(&m_WebCtrl, SIGNAL(finished(QNetworkReply*)),
					 SLOT(fileDownloaded(QNetworkReply*)));

	QSslConfiguration cnf(m_request.sslConfiguration());

	cnf.setPeerVerifyMode(QSslSocket::VerifyNone);
	cnf.setProtocol(QSsl::AnyProtocol);

	m_request.setSslConfiguration(cnf);
	m_request.setRawHeader("User-Agent", QByteArray(userAgent));

	if(!eTag.isEmpty()) m_request.setRawHeader("If-None-Match", eTag.toLatin1());

	m_request.setAttribute(QNetworkRequest::CacheLoadControlAttribute,
						   QNetworkRequest::AlwaysNetwork);
}

FileDownloader::~FileDownloader() {}

QNetworkReply *FileDownloader::start(QGitHubReleaseAPI::TYPE type) const {

	QString sType;

	switch(type) {
#ifdef HAVE_MKDIO_H
	case QGitHubReleaseAPI::RAW: sType = "raw"; break;
#else
	case QGitHubReleaseAPI::RAW:
#endif
	case QGitHubReleaseAPI::HTML: sType = "html"; break;
	case QGitHubReleaseAPI::TEXT: sType = "text"; break;
	}

	m_request.setRawHeader("Accept", !m_generic ?
							   QByteArray(QString("application/vnd.github.v3.%1+json").
										  arg(sType).toLatin1()) :
							   QByteArray("application/octet-stream"));

	m_reply = m_WebCtrl.get(m_request);

	QObject::connect(m_reply, SIGNAL(downloadProgress(qint64,qint64)),
					 this, SLOT(downloadProgress(qint64,qint64)));

	return m_reply;
}

void FileDownloader::setCacheLoadControlAttribute(QNetworkRequest::CacheLoadControl att) {
	m_request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, att);
}

void FileDownloader::downloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
	emit progress(bytesReceived, bytesTotal);
}

void FileDownloader::fileDownloaded(QNetworkReply *pReply) {

	if(m_reply->error() != QNetworkReply::NoError) {

		if(m_reply->error() != QNetworkReply::OperationCanceledError) {
			emit error(m_reply->errorString());
		} else {
			emit canceled();
		}

	} else {

		QVariant redirectTarget =
				m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

		if(!redirectTarget.isNull()) {

			m_url = m_url.resolved(redirectTarget.toUrl());
			qWarning("Redirect to: %s", m_url.toString().toLatin1().constData());

			QObject::disconnect(m_reply, SIGNAL(downloadProgress(qint64,qint64)),
								this, SLOT(downloadProgress(qint64,qint64)));

			m_reply->deleteLater();
			m_request.setUrl(m_url);
			m_reply = m_WebCtrl.get(m_request);

			QObject::connect(m_reply, SIGNAL(downloadProgress(qint64,qint64)),
							 this, SLOT(downloadProgress(qint64,qint64)));

			emit replyChanged(m_reply);

		} else {
#if QT_VERSION >= QT_VERSION_CHECK(4, 7, 0)
			m_rawHeaderPairs = m_reply->rawHeaderPairs();
#else
			m_rawHeaderPairs.clear();

			foreach(const QByteArray &rhk, m_reply->rawHeaderList()) {
				m_rawHeaderPairs.append(RAWHEADERPAIR(rhk, m_reply->rawHeader(rhk)));
			}
#endif

			m_DownloadedData = pReply->readAll();
			m_reply->deleteLater();
			emit downloaded(*this);
		}
	}
}

const QByteArray &FileDownloader::downloadedData() const {
	return m_DownloadedData;
}

void FileDownloader::abort() const {
	m_reply->abort();
}

void FileDownloader::cancel(const FileDownloader &fd) {
	fd.abort();
}
