/*
 * Copyright 2015 by Heiko Schäfer <heiko@rangun.de>
 *
 * This file is part of QGitHubReleaseAPI.
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

#include <QImage>
#include <QEventLoop>

#include "qgithubreleaseapi_p.h"

#include "filedownloader.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0) || defined(QJSON_FOUND)
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QJsonDocument>
#else
#include <qjson/parser.h>
#endif
#ifdef HAVE_MKDIO_H
extern "C" {
#include <mkdio.h>
}
#endif
#endif

const char *QGitHubReleaseAPIPrivate::m_userAgent = "QGitHubReleaseAPI";
const char *QGitHubReleaseAPIPrivate::m_outOfBoundsError =
		QT_TRANSLATE_NOOP("QGitHubReleaseAPIPrivate", "Index %1 >= %2 (out of bounds)");
const char *QGitHubReleaseAPIPrivate::m_noDataAvailableError =
		QT_TRANSLATE_NOOP("QGitHubReleaseAPIPrivate", "No data available");

QGitHubReleaseAPIPrivate::QGitHubReleaseAPIPrivate(const QUrl &apiUrl, bool multi, QObject *p) :
	QObject(p), m_apiDownloader(new FileDownloader(apiUrl, m_userAgent)), m_jsonData(),
	m_errorString(), m_rateLimit(0), m_rateLimitRemaining(0), m_singleEntryRequested(!multi),
	m_avatars() {
	init();
}

QGitHubReleaseAPIPrivate::QGitHubReleaseAPIPrivate(const QString &user, const QString &repo,
												   bool latest, QObject *p) : QObject(p),
	m_apiDownloader(new FileDownloader(QUrl(QString("https://api.github.com/repos/%1/%2/releases%3")
											.arg(QString(QUrl::toPercentEncoding(user)))
											.arg(QString(QUrl::toPercentEncoding(repo)))
											.arg(latest ? "/latest" : "")), m_userAgent)),
	m_jsonData(), m_errorString(), m_rateLimit(0), m_rateLimitRemaining(0),
	m_singleEntryRequested(latest), m_avatars() {
	init();
}

QGitHubReleaseAPIPrivate::QGitHubReleaseAPIPrivate(const QString &user, const QString &repo,
												   const QString &tag, QObject *p) : QObject(p),
	m_apiDownloader(new FileDownloader(QUrl(QString("https://api.github.com/repos/%1/%2" \
													"/releases/tags/%3")
											.arg(QString(QUrl::toPercentEncoding(user)))
											.arg(QString(QUrl::toPercentEncoding(repo)))
											.arg(QString(QUrl::toPercentEncoding(tag)))),
									   m_userAgent)), m_jsonData(), m_errorString(), m_rateLimit(0),
	m_rateLimitRemaining(0), m_singleEntryRequested(true), m_avatars() {
	init();
}

QGitHubReleaseAPIPrivate::QGitHubReleaseAPIPrivate(const QString &user, const QString &repo,
												   int limit, QObject *p) : QObject(p),
	m_apiDownloader(new FileDownloader(QUrl(QString("https://api.github.com/repos/%1/%2/" \
													"releases?per_page=%3").
											arg(QString(QUrl::toPercentEncoding(user))).
											arg(QString(QUrl::toPercentEncoding(repo))).
											arg(limit)), m_userAgent)), m_jsonData(),
	m_errorString(), m_rateLimit(0), m_rateLimitRemaining(0), m_singleEntryRequested(false),
	m_avatars() {
	init();
}

QGitHubReleaseAPIPrivate::~QGitHubReleaseAPIPrivate() {
	delete m_apiDownloader;
	foreach(QImage *img, m_avatars) delete img;
}

void QGitHubReleaseAPIPrivate::init() const {
	QObject::connect(m_apiDownloader, SIGNAL(error(QString,QVariant)),
					 this, SLOT(fdError(QString,QVariant)));
	QObject::connect(m_apiDownloader, SIGNAL(downloaded(FileDownloader,QVariant)),
					 this, SLOT(downloaded(FileDownloader,QVariant)));
	QObject::connect(m_apiDownloader, SIGNAL(progress(qint64,qint64,QVariant)),
					 this, SLOT(downloadProgress(qint64,qint64,QVariant)));
}

QImage QGitHubReleaseAPIPrivate::avatar(int idx) const {

	if(m_avatars.contains(idx)) {
		return *m_avatars.value(idx);
	} else {

		QEventLoop wait;

		FileDownloader dl(avatarUrl(idx), m_userAgent);
		dl.setCacheLoadControlAttribute(QNetworkRequest::PreferNetwork);
		dl.setUserData(idx);

		QObject::connect(this, SIGNAL(avatarStopWait()), &wait, SLOT(quit()));
		QObject::connect(&dl, SIGNAL(downloaded(FileDownloader,QVariant)),
						 this, SLOT(avatarDownloaded(FileDownloader,QVariant)));
		QObject::connect(&dl, SIGNAL(error(QString,QVariant)),
						 this, SLOT(avatarError(QString,QVariant)));
		QObject::connect(&dl, SIGNAL(progress(qint64,qint64,QVariant)),
						 this, SLOT(avatarProgress(qint64,qint64,QVariant)));
		wait.exec();

		if(m_avatars.contains(idx)) return *m_avatars.value(idx);
	}

	return QImage();
}

void QGitHubReleaseAPIPrivate::avatarDownloaded(const FileDownloader &fd, const QVariant &ud) {
	m_avatars.insert(ud.toInt(), new QImage(QImage::fromData(fd.downloadedData())));
	emit avatarStopWait();
}

void QGitHubReleaseAPIPrivate::avatarError(const QString &err, const QVariant &) {
	emit error(err);
	emit avatarStopWait();
}

void QGitHubReleaseAPIPrivate::avatarProgress(qint64 br, qint64 bt, const QVariant &) {
	emit progress(br, bt);
}

QString QGitHubReleaseAPIPrivate::body(int idx) const {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0) || defined(QJSON_FOUND)) && defined(HAVE_MKDIO_H)

	if(dataAvailable()) {

		if(entries() > idx) {

			const QString bMD(m_vdata[idx].toMap()["body"].toString());

			const mkd_flag_t f = MKD_TOC|MKD_AUTOLINK|MKD_NOEXT|MKD_NOHEADER|MKD_NOIMAGE;
			MMIOT *doc = 0L;
			char *html = 0L;
			int dlen   = EOF;

			if((doc = mkd_string(bMD.toStdString().c_str(), bMD.length(), f)) &&
					mkd_compile(doc, f) != EOF && (dlen = mkd_document(doc, &html)) != EOF) {

				const QString b(QString::fromUtf8((QByteArray(html,
															  dlen).append('\0')).constData()));
				mkd_cleanup(doc);

				return b;

			} else {
				emit error(tr("libmarkdown: parsing failed"));
			}
		} else {
			emit error(QString(m_outOfBoundsError).arg(entries()).arg(idx));
		}

	} else {
		emit error(m_noDataAvailableError);
	}

#else
	emit error(tr("No libmarkdown installed, body not available"));
#endif

	return QString::null;
}

void QGitHubReleaseAPIPrivate::downloadProgress(qint64 br, qint64 bt, const QVariant &) {
	emit progress(br, bt);
}

void QGitHubReleaseAPIPrivate::downloaded(const FileDownloader &fd, const QVariant &) {

	m_jsonData = fd.downloadedData();

	foreach(const QNetworkReply::RawHeaderPair &pair, fd.rawHeaderPairs()) {

		if(pair.first == "X-RateLimit-Reset") {
			m_rateLimitReset = QDateTime::fromTime_t(QString(pair.second).toUInt());
		}

		if(pair.first == "X-RateLimit-Limit") m_rateLimit = QString(pair.second).toUInt();

		if(pair.first == "X-RateLimit-Remaining") {
			m_rateLimitRemaining = QString(pair.second).toUInt();
		}
	}

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0) || defined(QJSON_FOUND)
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	QJson::Parser parser;
	bool ok = false;
#else
	QJsonParseError ok;
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
	if(m_singleEntryRequested) {
		m_vdata.append(QJsonDocument::fromJson(m_jsonData, &ok).toVariant());
	} else {
		m_vdata = QJsonDocument::fromJson(m_jsonData, &ok).toVariant().toList();
	}
#else
	if(m_singleEntryRequested) {
		m_vdata.append(parser.parse(m_jsonData, &ok));
	} else {
		m_vdata = parser.parse(m_jsonData, &ok).toList();
	}
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)

	m_errorString = ok.errorString();

	if(ok.error != QJsonParseError::NoError) {
		qWarning("QJson: %s", m_errorString.toStdString().c_str());
#else

	m_errorString = parser.errorString();

	if(!ok) {
		qWarning("QJson: %s", m_errorString.toStdString().c_str());
#endif
		emit error(m_errorString);
	} else if(!m_jsonData.isEmpty()) {
		emit available();
	}
#endif
}

void QGitHubReleaseAPIPrivate::fdError(const QString &err, const QVariant &) {
	emit error(QString("network error: %1").arg(err));
}

bool QGitHubReleaseAPIPrivate::dataAvailable() const {
	return !m_vdata.isEmpty();
}

QUrl QGitHubReleaseAPIPrivate::apiUrl() const {
	return m_apiDownloader->url();
}

int QGitHubReleaseAPIPrivate::entries() const {
	return dataAvailable() ? m_vdata.count() : 0;
}
