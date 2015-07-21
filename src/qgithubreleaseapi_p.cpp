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

#include <QBuffer>
#include <QRegExp>
#include <QEventLoop>

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
#include "entryhelper.h"
#include "emoji.h"

const char *QGitHubReleaseAPIPrivate::m_userAgent = "QGitHubReleaseAPI";
const char *QGitHubReleaseAPIPrivate::m_outOfBoundsError =
		QT_TRANSLATE_NOOP("QGitHubReleaseAPIPrivate", "Index %1 >= %2 (out of bounds)");
const char *QGitHubReleaseAPIPrivate::m_noDataAvailableError =
		QT_TRANSLATE_NOOP("QGitHubReleaseAPIPrivate", "No data available");

QGitHubReleaseAPIPrivate::QGitHubReleaseAPIPrivate(const QUrl &apiUrl, bool multi,
												   QGitHubReleaseAPI::TYPE type, QObject *p) :
	QObject(p), m_apiDownloader(new FileDownloader(apiUrl, m_userAgent)), m_jsonData(), m_vdata(),
	m_errorString(), m_rateLimit(0), m_rateLimitRemaining(0), m_singleEntryRequested(!multi),
	m_rateLimitReset(), m_avatars(), m_eTag(QString::null), m_dlOutputFile(0L),
	m_readBytes(Q_INT64_C(-1)), m_readReply(0L), m_bytesAvail(Q_INT64_C(-1)), m_type(type) {
	init();
}

QGitHubReleaseAPIPrivate::QGitHubReleaseAPIPrivate(const QUrl &apiUrl, bool multi,
												   QGitHubReleaseAPI::TYPE type,
												   const QString &etag, QObject *p) :
	QObject(p), m_apiDownloader(new FileDownloader(apiUrl, m_userAgent)), m_jsonData(), m_vdata(),
	m_errorString(), m_rateLimit(0), m_rateLimitRemaining(0), m_singleEntryRequested(!multi),
	m_rateLimitReset(), m_avatars(), m_eTag(etag), m_dlOutputFile(0L), m_readBytes(Q_INT64_C(-1)),
	m_readReply(0L), m_bytesAvail(Q_INT64_C(-1)), m_type(type) {
	init();
}

QGitHubReleaseAPIPrivate::QGitHubReleaseAPIPrivate(const QString &user, const QString &repo,
												   bool latest, QGitHubReleaseAPI::TYPE type,
												   QObject *p) : QObject(p),
	m_apiDownloader(new FileDownloader(QUrl(QString("https://api.github.com/repos/%1/%2/releases%3")
											.arg(QString(QUrl::toPercentEncoding(user)))
											.arg(QString(QUrl::toPercentEncoding(repo)))
											.arg(latest ? "/latest" : "")), m_userAgent)),
	m_jsonData(), m_vdata(), m_errorString(), m_rateLimit(0), m_rateLimitRemaining(0),
	m_singleEntryRequested(latest), m_rateLimitReset(), m_avatars(), m_eTag(QString::null),
	m_dlOutputFile(0L), m_readBytes(Q_INT64_C(-1)), m_readReply(0L), m_bytesAvail(Q_INT64_C(-1)),
	m_type(type) {
	init();
}

QGitHubReleaseAPIPrivate::QGitHubReleaseAPIPrivate(const QString &user, const QString &repo,
												   const QString &tag, QGitHubReleaseAPI::TYPE type,
												   QObject *p) : QObject(p),
	m_apiDownloader(new FileDownloader(QUrl(QString("https://api.github.com/repos/%1/%2" \
													"/releases/tags/%3")
											.arg(QString(QUrl::toPercentEncoding(user)))
											.arg(QString(QUrl::toPercentEncoding(repo)))
											.arg(QString(QUrl::toPercentEncoding(tag)))),
									   m_userAgent)), m_jsonData(), m_errorString(),
	m_rateLimit(0), m_rateLimitRemaining(0), m_singleEntryRequested(true), m_avatars(),
	m_eTag(QString::null), m_type(type) {
	init();
}

QGitHubReleaseAPIPrivate::QGitHubReleaseAPIPrivate(const QString &user, const QString &repo,
												   int limit, QGitHubReleaseAPI::TYPE type,
												   QObject *p) : QObject(p),
	m_apiDownloader(new FileDownloader(QUrl(QString("https://api.github.com/repos/%1/%2/" \
													"releases?per_page=%3").
											arg(QString(QUrl::toPercentEncoding(user))).
											arg(QString(QUrl::toPercentEncoding(repo))).
											arg(limit)), m_userAgent)), m_jsonData(),
	m_errorString(), m_rateLimit(0), m_rateLimitRemaining(0), m_singleEntryRequested(false),
	m_avatars(), m_eTag(QString::null), m_type(type) {
	init();
}

QGitHubReleaseAPIPrivate::~QGitHubReleaseAPIPrivate() {
	cancel();
	delete m_apiDownloader;
}

void QGitHubReleaseAPIPrivate::init() const {

	QObject::connect(m_apiDownloader, SIGNAL(error(QString)), this, SLOT(fdError(QString)));
	QObject::connect(m_apiDownloader, SIGNAL(downloaded(FileDownloader)),
					 this, SLOT(downloaded(FileDownloader)));
	QObject::connect(m_apiDownloader, SIGNAL(progress(qint64,qint64)),
					 this, SLOT(downloadProgress(qint64,qint64)));

	m_apiDownloader->start(m_type);
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

QByteArray QGitHubReleaseAPIPrivate::downloadFile(const QUrl &u, bool generic) const {

	QByteArray ba;
	QBuffer buf(&ba);

	if(!(buf.open(QIODevice::WriteOnly) && downloadFile(u, &buf, generic) != Q_INT64_C(-1))) {
		emit error(buf.errorString());
	}

	ba.squeeze();

	return ba;
}

qint64 QGitHubReleaseAPIPrivate::downloadFile(const QUrl &u, QIODevice *of, bool generic) const {

	m_readBytes = Q_INT64_C(-1);

	if(of) {

		QEventLoop wait;
		FileDownloader dl(u, m_userAgent, m_eTag);

		m_dlOutputFile = of;

		dl.setCacheLoadControlAttribute(QNetworkRequest::PreferCache);
		dl.setGeneric(generic);

		QObject::connect(&dl, SIGNAL(canceled()), &wait, SLOT(quit()));
		QObject::connect(&dl, SIGNAL(error(QString)), &wait, SLOT(quit()));
		QObject::connect(&dl, SIGNAL(downloaded(FileDownloader)), &wait, SLOT(quit()));
		QObject::connect(&dl, SIGNAL(replyChanged(QNetworkReply*)),
						 this, SLOT(updateReply(QNetworkReply*)));
		QObject::connect(&dl, SIGNAL(canceled()), this, SLOT(fdCanceled()));
		QObject::connect(&dl, SIGNAL(error(QString)), this, SLOT(fileDownloadError(QString)));
		QObject::connect(&dl, SIGNAL(progress(qint64,qint64)),
						 this, SLOT(fileDownloadProgress(qint64,qint64)));

		m_readReply = dl.start(QGitHubReleaseAPI::RAW);

		QObject::connect(m_readReply, SIGNAL(readyRead()), this, SLOT(readChunk()));
		wait.exec();

		m_dlOutputFile = 0L;
	}

	m_readReply = 0L;

	return m_readBytes;
}

void QGitHubReleaseAPIPrivate::updateReply(QNetworkReply *r) {

	QObject::disconnect(m_readReply, SIGNAL(readyRead()), this, SLOT(readChunk()));
	QObject::connect(r, SIGNAL(readyRead()), this, SLOT(readChunk()));
	m_readReply = r;
}

void QGitHubReleaseAPIPrivate::readChunk() {
	m_readBytes += m_dlOutputFile->write(m_readReply->readAll());
}

void QGitHubReleaseAPIPrivate::fileDownloadError(const QString &err) {
	emit error(err);
}

void QGitHubReleaseAPIPrivate::fileDownloadProgress(qint64 br, qint64 bt) {
	emit progress(br, bt);
}

void QGitHubReleaseAPIPrivate::fdError(const QString &err) {
	emit error(QString("network error: %1").arg(err));
}

void QGitHubReleaseAPIPrivate::fdCanceled() {
	qWarning("Download canceled");
	m_readBytes = Q_INT64_C(-1);
	emit canceled();
}

QString QGitHubReleaseAPIPrivate::body(int idx) const {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0) || defined(QJSON_FOUND))

	if(dataAvailable()) {

		if(entries() > idx) {
#ifdef HAVE_MKDIO_H
			const QString bMD(m_vdata[idx].toMap()["body"].toString());

#if QT_VERSION >= QT_VERSION_CHECK(4, 5, 0)
			const mkd_flag_t f = MKD_TOC|MKD_AUTOLINK|MKD_NOEXT|MKD_NOHEADER;
#else
			const mkd_flag_t f = MKD_TOC|MKD_AUTOLINK|MKD_NOEXT|MKD_NOHEADER|MKD_NOIMAGE;
#endif
#endif

			switch(m_type) {
#ifdef HAVE_MKDIO_H
			case QGitHubReleaseAPI::RAW: {
					MMIOT *doc = 0L;
					char *html = 0L;
					int dlen   = EOF;

					if((doc = mkd_string(bMD.toStdString().c_str(), bMD.length(), f)) &&
							mkd_compile(doc, f) != EOF && (dlen = mkd_document(doc, &html)) != EOF) {

						QString b(QString::fromUtf8((QByteArray(html,
																dlen).append('\0')).constData()));
						mkd_cleanup(doc);

						return embedImages(b);

					} else {
						emit error(tr("libmarkdown: parsing failed"));
					}

				} break;
#else
			case QGitHubReleaseAPI::RAW:
#endif
			case QGitHubReleaseAPI::HTML: {
					QString b(m_vdata[idx].toMap()["body_html"].toString());
					return embedImages(b);
				} break;
			default:
				return m_vdata[idx].toMap()["body_text"].toString().
						append(QString::fromUtf8("\n\n--\nRelease information provided by " \
						"QGitHubReleaseAPI "
						PROJECTVERSION
						" \u00a9 2015 Heiko Sch\u00e4fer <heiko@rangun.de>"));
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

QString QGitHubReleaseAPIPrivate::embedImages(QString &b) const {
#if QT_VERSION >= QT_VERSION_CHECK(4, 5, 0)

	QRegExp emjRex(":([_a-zA-Z0-9]+):");
	QRegExp imgRex("<[^<]*img[^>]*src\\s*=\\s*\"([^\"]*)\"[^>]*>");

	Emoji *emoji = 0L;

	int idx = -1;

	while((idx = b.indexOf(emjRex, idx + 1)) != -1) {

		const QString emjKey(emjRex.cap(1));

		if(!emoji) {
			QEventLoop emjLoop;
			QObject::connect((emoji = new Emoji(eTag())), SIGNAL(available()),
							 &emjLoop, SLOT(quit()));

			if(!emoji->entries()) emjLoop.exec();
		}

		const QUrl &emjUrl(emoji->getUrl(emjKey));

		if(emjUrl.isValid()) {
			b.replace(idx, emjKey.length() + 2, "<img width=\"16\" height=\"16\" alt=\"" + emjKey +
					  "\" src=\"" + emjUrl.toString() + "\">");
		}

		idx += emjKey.length() + 1;
	}

	delete emoji;

	idx = -1;

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

	b.append("<hr /><p>Release information provided by " \
			 "<em>QGitHubReleaseAPI "
			 PROJECTVERSION
			 "</em> &copy; 2015 " \
			 "Heiko Sch&auml;fer &lt;<a href=\"mailto:heiko@rangun.de?" \
			 "subject=QGitHubReleaseAPI%20"
			 PROJECTVERSION
			 "\">heiko@rangun.de</a>&gt;");

#ifdef HAVE_MKDIO_H
	if(m_type == QGitHubReleaseAPI::RAW) {
		b.append(QString("<br />Markdown rendered with <em>libmarkdown %1</em>").
				 arg(markdown_version));
	}
#endif

	return b.append("</p>");
}

void QGitHubReleaseAPIPrivate::downloadProgress(qint64 br, qint64 bt) {
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

void QGitHubReleaseAPIPrivate::downloaded(const FileDownloader &fd) {

	QObject::connect(m_apiDownloader, SIGNAL(error(QString)), this, SLOT(fdError(QString)));
	QObject::connect(m_apiDownloader, SIGNAL(downloaded(FileDownloader)),
					 this, SLOT(downloaded(FileDownloader)));
	QObject::connect(m_apiDownloader, SIGNAL(progress(qint64,qint64)),
					 this, SLOT(downloadProgress(qint64,qint64)));

	m_jsonData = fd.downloadedData();

	foreach(const FileDownloader::RAWHEADERPAIR &pair, fd.rawHeaderPairs()) {

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

qint64 QGitHubReleaseAPIPrivate::tarBall(QFile &of, int idx) const {
	return fileToFileDownload<&QGitHubReleaseAPIPrivate::tarBallUrl>(&of, idx);;
}

QByteArray QGitHubReleaseAPIPrivate::zipBall(int idx) const {
	return downloadFile(zipBallUrl(idx));
}

qint64 QGitHubReleaseAPIPrivate::zipBall(QFile &of, int idx) const {
	return fileToFileDownload<&QGitHubReleaseAPIPrivate::zipBallUrl>(&of, idx);;
}

void QGitHubReleaseAPIPrivate::cancel() {

#if QT_VERSION >= QT_VERSION_CHECK(4, 5, 0)
	if(m_readReply && m_readReply->isRunning()) {
#else
	if(m_readReply) {
#endif
		m_readReply->abort();
	}
}

QUrl QGitHubReleaseAPIPrivate::releaseUrl(int idx) const {
	return EntryHelper<QUrl>(*this)(idx, "url");
}

QUrl QGitHubReleaseAPIPrivate::assetsUrl(int idx) const {
	return EntryHelper<QUrl>(*this)(idx, "assets_url");
}

QUrl QGitHubReleaseAPIPrivate::uploadUrl(int idx) const {
	return EntryHelper<QUrl>(*this)(idx, "upload_url");
}

QUrl QGitHubReleaseAPIPrivate::releaseHtmlUrl(int idx) const {
	return EntryHelper<QUrl>(*this)(idx, "html_url");
}

QUrl QGitHubReleaseAPIPrivate::authorHtmlUrl(int idx) const {
	return EntryHelper<QUrl>(*this)(idx, "html_url", "author");
}

QUrl QGitHubReleaseAPIPrivate::tarBallUrl(int idx) const {
	return EntryHelper<QUrl>(*this)(idx, "tarball_url");
}

QUrl QGitHubReleaseAPIPrivate::zipBallUrl(int idx) const {
	return EntryHelper<QUrl>(*this)(idx, "zipball_url");
}

ulong QGitHubReleaseAPIPrivate::releaseId(int idx) const {
	return EntryHelper<ulong>(*this)(idx, "id");
}

QString QGitHubReleaseAPIPrivate::name(int idx) const {
	return EntryHelper<QString>(*this)(idx, "name");
}

ulong QGitHubReleaseAPIPrivate::authorId(int idx) const {
	return EntryHelper<ulong>(*this)(idx, "id", "author");
}

QString QGitHubReleaseAPIPrivate::login(int idx) const {
	return EntryHelper<QString>(*this)(idx, "login", "author");
}

QUrl QGitHubReleaseAPIPrivate::avatarUrl(int idx) const {
	return EntryHelper<QUrl>(*this)(idx, "avatar_url", "author");
}

QString QGitHubReleaseAPIPrivate::tagName(int idx) const {
	return EntryHelper<QString>(*this)(idx, "tag_name");
}

QDateTime QGitHubReleaseAPIPrivate::publishedAt(int idx) const {
	return EntryHelper<QDateTime>(*this)(idx, "published_at");
}

QString QGitHubReleaseAPIPrivate::targetCommitish(int idx) const {
	return EntryHelper<QString>(*this)(idx, "target_commitish");
}

bool QGitHubReleaseAPIPrivate::isDraft(int idx) const {
	return EntryHelper<bool>(*this)(idx, "draft");
}

bool QGitHubReleaseAPIPrivate::isPreRelease(int idx) const {
	return EntryHelper<bool>(*this)(idx, "prerelease");
}

QDateTime QGitHubReleaseAPIPrivate::createdAt(int idx) const {
	return EntryHelper<QDateTime>(*this)(idx, "created_at");
}
