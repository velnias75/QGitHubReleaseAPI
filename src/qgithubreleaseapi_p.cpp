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
#include <QRegExp>
#include <QEventLoop>

#if QT_VERSION >= QT_VERSION_CHECK(4, 5, 0)
#include <QBuffer>
#endif

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

#include "qgithubreleaseapi_p.h"
#include "filedownloader.h"

const char *QGitHubReleaseAPIPrivate::m_userAgent = "QGitHubReleaseAPI";
const char *QGitHubReleaseAPIPrivate::m_outOfBoundsError =
		QT_TRANSLATE_NOOP("QGitHubReleaseAPIPrivate", "Index %1 >= %2 (out of bounds)");
const char *QGitHubReleaseAPIPrivate::m_noDataAvailableError =
		QT_TRANSLATE_NOOP("QGitHubReleaseAPIPrivate", "No data available");

QGitHubReleaseAPIPrivate::QGitHubReleaseAPIPrivate(const QUrl &apiUrl, bool multi, QObject *p) :
	QObject(p), m_apiDownloader(new FileDownloader(apiUrl, m_userAgent)), m_jsonData(),
	m_errorString(), m_rateLimit(0), m_rateLimitRemaining(0), m_singleEntryRequested(!multi),
	m_avatars(), m_eTag(QString::null) {
	init();
}

QGitHubReleaseAPIPrivate::QGitHubReleaseAPIPrivate(const QString &user, const QString &repo,
												   bool latest, QObject *p) : QObject(p),
	m_apiDownloader(new FileDownloader(QUrl(QString("https://api.github.com/repos/%1/%2/releases%3")
											.arg(QString(QUrl::toPercentEncoding(user)))
											.arg(QString(QUrl::toPercentEncoding(repo)))
											.arg(latest ? "/latest" : "")), m_userAgent)),
	m_jsonData(), m_errorString(), m_rateLimit(0), m_rateLimitRemaining(0),
	m_singleEntryRequested(latest), m_avatars(), m_eTag(QString::null) {
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
	m_rateLimitRemaining(0), m_singleEntryRequested(true), m_avatars(), m_eTag(QString::null) {
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
	m_avatars(), m_eTag(QString::null) {
	init();
}

QGitHubReleaseAPIPrivate::~QGitHubReleaseAPIPrivate() {
	delete m_apiDownloader;
}

void QGitHubReleaseAPIPrivate::init() const {
	QObject::connect(m_apiDownloader, SIGNAL(error(QString,QVariant*)),
					 this, SLOT(fdError(QString,QVariant*)));
	QObject::connect(m_apiDownloader, SIGNAL(downloaded(FileDownloader,QVariant*)),
					 this, SLOT(downloaded(FileDownloader,QVariant*)));
	QObject::connect(m_apiDownloader, SIGNAL(progress(qint64,qint64,QVariant*)),
					 this, SLOT(downloadProgress(qint64,qint64,QVariant*)));

	m_apiDownloader->start();
}

QImage QGitHubReleaseAPIPrivate::avatar(int idx) const {

	if(m_avatars.contains(idx)) {
		return m_avatars.value(idx);
	} else {
		QImage img = QImage::fromData(downloadFile(avatarUrl(idx)));
		if(!img.isNull()) return *m_avatars.insert(idx, img);
	}

	return QImage();
}

QByteArray QGitHubReleaseAPIPrivate::downloadFile(const QUrl &u) const {

	QEventLoop wait;
	QVariant file(QVariant::ByteArray);

	FileDownloader dl(u, m_userAgent, m_eTag);

	dl.setCacheLoadControlAttribute(QNetworkRequest::PreferCache);
	dl.setUserData(file);

	QObject::connect(this, SIGNAL(avatarStopWait()), &wait, SLOT(quit()));
	QObject::connect(&dl, SIGNAL(downloaded(FileDownloader,QVariant*)),
					 this, SLOT(fileDownloaded(FileDownloader,QVariant*)));
	QObject::connect(&dl, SIGNAL(error(QString,QVariant*)),
					 this, SLOT(fileDownloadError(QString,QVariant*)));
	QObject::connect(&dl, SIGNAL(progress(qint64,qint64,QVariant*)),
					 this, SLOT(fileDownloadProgress(qint64,qint64,QVariant*)));
	dl.start();

	wait.exec();

	return file.toByteArray();
}

void QGitHubReleaseAPIPrivate::fileDownloaded(const FileDownloader &fd, QVariant *ud) {
	if(ud) ud->setValue(fd.downloadedData());
	emit avatarStopWait();
}

void QGitHubReleaseAPIPrivate::fileDownloadError(const QString &err, QVariant *) {
	emit error(err);
	emit avatarStopWait();
}

void QGitHubReleaseAPIPrivate::fileDownloadProgress(qint64 br, qint64 bt, QVariant *) {
	emit progress(br, bt);
}

QString QGitHubReleaseAPIPrivate::body(int idx) const {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0) || defined(QJSON_FOUND)) && defined(HAVE_MKDIO_H)

	if(dataAvailable()) {

		if(entries() > idx) {

			const QString bMD(m_vdata[idx].toMap()["body"].toString());

#if QT_VERSION >= QT_VERSION_CHECK(4, 5, 0)
			const mkd_flag_t f = MKD_TOC|MKD_AUTOLINK|MKD_NOEXT|MKD_NOHEADER;
#else
			const mkd_flag_t f = MKD_TOC|MKD_AUTOLINK|MKD_NOEXT|MKD_NOHEADER|MKD_NOIMAGE;
#endif

			MMIOT *doc = 0L;
			char *html = 0L;
			int dlen   = EOF;

			if((doc = mkd_string(bMD.toStdString().c_str(), bMD.length(), f)) &&
					mkd_compile(doc, f) != EOF && (dlen = mkd_document(doc, &html)) != EOF) {

				QString b(QString::fromUtf8((QByteArray(html,
														dlen).append('\0')).constData()));
				mkd_cleanup(doc);

#if QT_VERSION >= QT_VERSION_CHECK(4, 5, 0)

				QRegExp imgRex("<[^<]*img[^>]*src\\s*=\\s*\"([^\"]*)\"[^>]*>");
				int idx = -1;

				while((idx = b.indexOf(imgRex, idx + 1)) != -1) {

					const QUrl url = QUrl(imgRex.cap(1));

					if(url.isValid()) {

						const QImage img = QImage::fromData(downloadFile(url));

						if(!img.isNull()) {

							QByteArray ba;
							QBuffer buf(&ba);
							buf.open(QIODevice::WriteOnly);
							img.save(&buf, "PNG");
							ba.squeeze();

							b.replace(imgRex.pos(1), imgRex.cap(1).length(),
									  QString("data:image/png;base64,%1").
									  arg(ba.toBase64().constData()));
						}
					}
				}
#endif
				return b.append("<hr /><p>Release information provided by " \
								"<em>QGitHubReleaseAPI "
								PROJECTVERSION
								"</em> &copy; 2015 " \
								"Heiko Sch&auml;fer &lt;<a href=\"mailto:heiko@rangun.de?" \
								"subject=QGitHubReleaseAPI%20"
								PROJECTVERSION
								"\">heiko@rangun.de</a>&gt;<br />").
						append(QString("Markdown rendered with <em>libmarkdown %1</em></p>").
							   arg(markdown_version));

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

void QGitHubReleaseAPIPrivate::downloadProgress(qint64 br, qint64 bt, QVariant *) {
	emit progress(br, bt);
}

QVariant QGitHubReleaseAPIPrivate::parseJSon(const QByteArray &ba, QString &err) const {
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0) || defined(QJSON_FOUND)
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	QJson::Parser parser;
	bool ok = false;
#else
	QJsonParseError ok;
#endif

	err = QString::null;

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)

	QVariant v(QJsonDocument::fromJson(ba, &ok).toVariant());

	if(ok.error == QJsonParseError::NoError) {
		return v;
	} else {
		err = ok.errorString();
	}

#else

	QVariant v(parser.parse(m_jsonData, &ok));

	if(ok) {
		return v;
	} else {
		err = parser.errorString();
	}

#endif
#else
	err = tr("No JSon parser found");
#endif

	return QVariant();
}

void QGitHubReleaseAPIPrivate::downloaded(const FileDownloader &fd, QVariant *) {

	m_jsonData = fd.downloadedData();

	foreach(const QNetworkReply::RawHeaderPair &pair, fd.rawHeaderPairs()) {

		if(pair.first == "ETag") m_eTag = pair.second.mid(2);

		if(pair.first == "X-RateLimit-Reset") {
			m_rateLimitReset = QDateTime::fromTime_t(QString(pair.second).toUInt());
		}

		if(pair.first == "X-RateLimit-Limit") m_rateLimit = QString(pair.second).toUInt();

		if(pair.first == "X-RateLimit-Remaining") {
			m_rateLimitRemaining = QString(pair.second).toUInt();
		}
	}

	QVariant va(parseJSon(m_jsonData, m_errorString));

	if(m_errorString.isNull()) {

		if(m_singleEntryRequested) {
			m_vdata.append(va);
		} else if((m_vdata = va.toList()).isEmpty()) {
			m_errorString = va.toMap()["message"].toString();
			emit error(m_errorString);
			return;
		}

		emit available();

	} else {
		emit error(m_errorString);
	}
}

void QGitHubReleaseAPIPrivate::fdError(const QString &err, QVariant *) {
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

QByteArray QGitHubReleaseAPIPrivate::tarBall(int idx) const {
	return downloadFile(tarBallUrl(idx));
}

QByteArray QGitHubReleaseAPIPrivate::zipBall(int idx) const {
	return downloadFile(zipBallUrl(idx));
}
