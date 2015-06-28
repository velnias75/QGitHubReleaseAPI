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

#include <QDateTime>

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

namespace {
const char *OOB = QT_TRANSLATE_NOOP("QGitHubReleaseAPIPrivate", "Index %1 >= %2 (out of bounds)");
const char *NDA = QT_TRANSLATE_NOOP("QGitHubReleaseAPIPrivate", "No data available");
}

QGitHubReleaseAPIPrivate::QGitHubReleaseAPIPrivate(const QUrl &apiUrl, QObject *p) : QObject(p),
	m_downloader(new FileDownloader(apiUrl)), m_errorString() {
	QObject::connect(m_downloader, SIGNAL(error(QString)), this, SLOT(fdError(QString)));
	QObject::connect(m_downloader, SIGNAL(downloaded()), this, SLOT(downloaded()));
}

QGitHubReleaseAPIPrivate::QGitHubReleaseAPIPrivate(const QString &user, const QString &repo,
												   QObject *p) : QObject(p),
	m_downloader(new FileDownloader(QUrl(QString("https://api.github.com/repos/%1/%2/releases")
										 .arg(QString(QUrl::toPercentEncoding(user)))
										 .arg(QString(QUrl::toPercentEncoding(repo)))))),
	m_errorString() {
	QObject::connect(m_downloader, SIGNAL(error(QString)), this, SLOT(fdError(QString)));
	QObject::connect(m_downloader, SIGNAL(downloaded()), this, SLOT(downloaded()));
}

QGitHubReleaseAPIPrivate::QGitHubReleaseAPIPrivate(const QString &user, const QString &repo,
												   int limit, QObject *p) : QObject(p),
	m_downloader(new FileDownloader(QUrl(QString("https://api.github.com/repos/%1/%2/" \
												 "releases?per_page=%3").
										 arg(QString(QUrl::toPercentEncoding(user))).
										 arg(QString(QUrl::toPercentEncoding(repo))).
										 arg(limit)))), m_errorString() {
	QObject::connect(m_downloader, SIGNAL(error(QString)), this, SLOT(fdError(QString)));
	QObject::connect(m_downloader, SIGNAL(downloaded()), this, SLOT(downloaded()));
}

QGitHubReleaseAPIPrivate::~QGitHubReleaseAPIPrivate() {
	delete m_downloader;
}

QString QGitHubReleaseAPIPrivate::name(int idx) const {

	if(dataAvailable()) {

		if(entries() > idx) {
			return QString::fromUtf8(m_vdata[idx].toMap()["name"].toByteArray().constData());
		} else {
			emit error(QString(OOB).arg(entries()).arg(idx));
		}

	} else {
		emit error(NDA);
	}

	return QString::null;
}

QString QGitHubReleaseAPIPrivate::tagName(int idx) const {

	if(dataAvailable()) {

		if(entries() > idx) {
			return QString::fromUtf8(m_vdata[idx].toMap()["tag_name"].toByteArray().constData());
		} else {
			emit error(QString(OOB).arg(entries()).arg(idx));
		}

	} else {
		emit error(NDA);
	}

	return QString::null;
}

QDateTime QGitHubReleaseAPIPrivate::publishedAt(int idx) const {

	if(dataAvailable()) {

		if(entries() > idx) {
			return m_vdata[idx].toMap()["published_at"].toDateTime();
		} else {
			emit error(QString(OOB).arg(entries()).arg(idx));
		}

	} else {
		emit error(NDA);
	}

	return QDateTime();
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
			emit error(QString(OOB).arg(entries()).arg(idx));
		}

	} else {
		emit error(NDA);
	}

#else
	emit error(tr("No libmarkdown installed, body not available"));
#endif

	return QString::null;
}

void QGitHubReleaseAPIPrivate::downloaded() {

	const QByteArray &dld(m_downloader->downloadedData());

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0) || defined(QJSON_FOUND)
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	QJson::Parser parser;
	bool ok = false;
#else
	QJsonParseError ok;
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
	m_vdata = QJsonDocument::fromJson(dld, &ok).toVariant().toList();
#else
	m_vdata = parser.parse(dld, &ok).toList();
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
	} else if(!dld.isEmpty()) {
		emit available();
	}
#endif
}

void QGitHubReleaseAPIPrivate::fdError(const QString &err) {
	emit error(QString("network error: %1").arg(err));
}

bool QGitHubReleaseAPIPrivate::dataAvailable() const {
	return !m_vdata.isEmpty();
}

QUrl QGitHubReleaseAPIPrivate::url() const {
	return m_downloader->url();
}

int QGitHubReleaseAPIPrivate::entries() const {
	return dataAvailable() ? m_vdata.count() : 0;
}
