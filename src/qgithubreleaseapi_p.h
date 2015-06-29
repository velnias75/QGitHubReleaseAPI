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

#ifndef QGITHUBRELEASEAPI_P_H
#define QGITHUBRELEASEAPI_P_H

#include <QUrl>
#include <QDateTime>
#include <QVariantList>

#include "export.h"

class FileDownloader;

class QGITHUBRELEASEAPI_NO_EXPORT QGitHubReleaseAPIPrivate : public QObject {
	Q_OBJECT
	Q_DISABLE_COPY(QGitHubReleaseAPIPrivate)

public:
	QGitHubReleaseAPIPrivate(const QUrl &apiUrl, bool multi, QObject *parent = 0);
	QGitHubReleaseAPIPrivate(const QString &user, const QString &repo, bool latest, QObject *p = 0);
	QGitHubReleaseAPIPrivate(const QString &user, const QString &repo, const QString &tag,
							 QObject *parent = 0);
	QGitHubReleaseAPIPrivate(const QString &user, const QString &repo, int limit,
							 QObject *parent = 0);

	virtual ~QGitHubReleaseAPIPrivate();

	inline static void setUserAgent(const char *ua) {
		m_userAgent = ua;
	}

	QUrl apiUrl() const;

	int entries() const;

	inline QUrl url(int idx) const {
		return entry<QUrl>(idx, "url");
	}

	inline QUrl assetsUrl(int idx) const {
		return entry<QUrl>(idx, "assets_url");
	}

	inline QUrl uploadUrl(int idx) const {
		return entry<QUrl>(idx, "upload_url");
	}

	inline QUrl htmlUrl(int idx) const {
		return entry<QUrl>(idx, "html_url");
	}

	inline ulong id(int idx) const {
		return entry<ulong>(idx, "id");
	}

	inline QString name(int idx) const {
		return entry<QString>(idx, "name");
	}

	QString body(int idx) const;
	QImage avatar(int idx) const;

	inline QUrl avatarUrl(int idx) const {
		return entry<QUrl>(idx, "avatar_url", true);
	}

	inline QString tagName(int idx) const {
		return entry<QString>(idx, "tag_name");
	}

	inline QDateTime publishedAt(int idx) const {
		return entry<QDateTime>(idx, "published_at");
	}

	inline QVariantList toVariantList() const {
		return m_vdata;
	}

	inline QByteArray asJsonData() const {
		return m_jsonData;
	}

	inline uint rateLimit() const {
		return m_rateLimit;
	}

	inline uint rateLimitRemaining() const {
		return m_rateLimitRemaining;
	}

	inline QDateTime rateLimitReset() const {
		return m_rateLimitReset;
	}

private slots:
	void downloaded(const FileDownloader &, QVariant *);
	void imageDownloaded(const FileDownloader &, QVariant *);
	void fdError(const QString &, QVariant *);
	void imageError(const QString &, QVariant *);
	void downloadProgress(qint64, qint64, QVariant *);
	void imageProgress(qint64, qint64, QVariant *);

signals:
	void available();
	void error(const QString &) const;
	void progress(qint64, qint64);
	void avatarStopWait();

private:
	void init() const;
	bool dataAvailable() const;

	QImage downloadImage(const QUrl &u) const;

	template<class T>
	T entry(int idx, const QString &id, bool author = false) const {

		if(dataAvailable()) {

			if(entries() > idx) {
				return !author ?  m_vdata[idx].toMap()[id].value<T>() :
								  m_vdata[idx].toMap()["author"].toMap()[id].value<T>();
			} else {
				emit error(QString(m_outOfBoundsError).arg(entries()).arg(idx));
			}

		} else {
			emit error(m_noDataAvailableError);
		}

		return T();
	}

private:
	static const char *m_userAgent QGITHUBRELEASEAPI_EXPORT;
	static const char *m_outOfBoundsError;
	static const char *m_noDataAvailableError;

	const FileDownloader *m_apiDownloader;
	QByteArray m_jsonData;
	QVariantList m_vdata;
	QString m_errorString;
	uint m_rateLimit;
	uint m_rateLimitRemaining;
	bool m_singleEntryRequested;
	QDateTime m_rateLimitReset;
	mutable QMap<int, QImage> m_avatars;
};

#endif // QGITHUBRELEASEAPI_P_H
