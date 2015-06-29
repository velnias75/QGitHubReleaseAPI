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

#include <QSslConfiguration>

#include "filedownloader.h"

FileDownloader::FileDownloader(const QUrl &url, const char *userAgent,
							   const QString &eTag, QObject *p) : QObject(p), m_WebCtrl(),
	m_DownloadedData(), m_url(url), m_rawHeaderPairs(), m_reply(0L), m_userData(0L),
	m_request(url) {

	QObject::connect(&m_WebCtrl, SIGNAL(finished(QNetworkReply*)),
					 SLOT(fileDownloaded(QNetworkReply*)));

	QSslConfiguration cnf(m_request.sslConfiguration());

	cnf.setPeerVerifyMode(QSslSocket::VerifyNone);
	cnf.setProtocol(QSsl::AnyProtocol);

	m_request.setSslConfiguration(cnf);
	m_request.setRawHeader("User-Agent", QByteArray(userAgent));
	m_request.setRawHeader("Accept", QByteArray("application/vnd.github.v3.raw+json"));

	if(!eTag.isEmpty()) m_request.setRawHeader("If-None-Match", eTag.toLatin1());

	m_request.setAttribute(QNetworkRequest::CacheLoadControlAttribute,
						 QNetworkRequest::AlwaysNetwork);
}

FileDownloader::~FileDownloader() {}

void FileDownloader::start() const {

	m_reply = m_WebCtrl.get(m_request);

	QObject::connect(m_reply, SIGNAL(downloadProgress(qint64,qint64)),
					 this, SLOT(downloadProgress(qint64,qint64)));
}

void FileDownloader::setCacheLoadControlAttribute(QNetworkRequest::CacheLoadControl att) {
	m_request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, att);
}

void FileDownloader::downloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
	emit progress(bytesReceived, bytesTotal, m_userData);
}

void FileDownloader::fileDownloaded(QNetworkReply *pReply) {

	if(pReply->error() != QNetworkReply::NoError) {
		emit error(pReply->errorString(), m_userData);
	} else {

		QVariant redirectTarget =
				pReply->attribute(QNetworkRequest::RedirectionTargetAttribute);

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

		} else {
			m_rawHeaderPairs = pReply->rawHeaderPairs();
			m_DownloadedData = pReply->readAll();
			pReply->deleteLater();
			emit downloaded(*this, m_userData);
		}
	}
}

const QByteArray &FileDownloader::downloadedData() const {
	return m_DownloadedData;
}
