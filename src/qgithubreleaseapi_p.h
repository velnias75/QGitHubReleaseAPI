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

#ifndef QGITHUBRELEASEAPI_P_H
#define QGITHUBRELEASEAPI_P_H

#include <QFile>

#include "qgithubreleaseapi.h"

class QNetworkReply;
class FileDownloader;

class QGITHUBRELEASEAPI_NO_EXPORT QGitHubReleaseAPIPrivate : public QObject {
	Q_OBJECT
	Q_DISABLE_COPY(QGitHubReleaseAPIPrivate)

public:
	QGitHubReleaseAPIPrivate(const QUrl &apiUrl, bool multi, QGitHubReleaseAPI::TYPE type,
							 QObject *parent = 0);
	QGitHubReleaseAPIPrivate(const QString &user, const QString &repo, bool latest,
							 QGitHubReleaseAPI::TYPE type, QObject *p = 0);
	QGitHubReleaseAPIPrivate(const QString &user, const QString &repo, const QString &tag,
							 QGitHubReleaseAPI::TYPE type, QObject *parent = 0);
	QGitHubReleaseAPIPrivate(const QString &user, const QString &repo, int limit,
							 QGitHubReleaseAPI::TYPE type, QObject *parent = 0);

	virtual ~QGitHubReleaseAPIPrivate();

	inline static void setUserAgent(const char *ua) {
		m_userAgent = ua;
	}

	QByteArray downloadFile(const QUrl &u, bool generic = false) const;
	qint64 downloadFile(const QUrl &u, QIODevice *of, bool generic = false) const;

	QUrl apiUrl() const;
	int entries() const;

	inline QUrl releaseUrl(int idx) const {
		return entry<QUrl>(idx, "url");
	}

	inline QUrl assetsUrl(int idx) const {
		return entry<QUrl>(idx, "assets_url");
	}

	inline QUrl uploadUrl(int idx) const {
		return entry<QUrl>(idx, "upload_url");
	}

	inline QUrl releaseHtmlUrl(int idx) const {
		return entry<QUrl>(idx, "html_url");
	}

	inline QUrl authorHtmlUrl(int idx) const {
		return entry<QUrl>(idx, "html_url", "author");
	}

	inline QUrl tarBallUrl(int idx) const {
		return entry<QUrl>(idx, "tarball_url");
	}

	QByteArray tarBall(int idx) const;
	qint64 tarBall(QFile &of, int idx) const;

	QByteArray zipBall(int idx) const;
	qint64 zipBall(QFile &of, int idx) const;

	inline QUrl zipBallUrl(int idx) const {
		return entry<QUrl>(idx, "zipball_url");
	}

	inline ulong releaseId(int idx) const {
		return entry<ulong>(idx, "id");
	}

	inline QString name(int idx) const {
		return entry<QString>(idx, "name");
	}

	QString body(int idx) const;
	QImage avatar(int idx) const;

	inline ulong authorId(int idx) const {
		return entry<ulong>(idx, "id", "author");
	}

	inline QString login(int idx) const {
		return entry<QString>(idx, "login", "author");
	}

	inline QUrl avatarUrl(int idx) const {
		return entry<QUrl>(idx, "avatar_url", "author");
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

	inline QString targetCommitish(int idx) const {
		return entry<QString>(idx, "target_commitish");
	}

	inline bool isDraft(int idx) const {
		return entry<bool>(idx, "draft");
	}

	inline bool isPreRelease(int idx) const {
		return entry<bool>(idx, "prerelease");
	}

	inline QDateTime createdAt(int idx) const {
		return entry<QDateTime>(idx, "created_at");
	}

	inline QString eTag() const {
		return m_eTag;
	}

	inline void setETag(const QString &eTag) {
		m_eTag = eTag;
	}

public slots:
	void cancel();

private slots:
	void readChunk();
	void updateReply(QNetworkReply *);
	void downloaded(const FileDownloader &);
	void fdError(const QString &);
	void fdCanceled();
	void fileDownloadError(const QString &);
	void downloadProgress(qint64, qint64);
	void fileDownloadProgress(qint64, qint64);

signals:
	void available();
	void canceled();
	void error(const QString &) const;
	void progress(qint64, qint64);

private:
	void init() const;
	QVariant parseJSon(const QByteArray &ba, QString &err) const;
	QString embedImages(QString &b) const;
	bool dataAvailable() const;

	template<QUrl (QGitHubReleaseAPIPrivate::*T)(int) const>
	qint64 fileToFileDownload(QFile *of, int idx) const {

		int ok = 0;

		if(of && !of->isOpen()) ok = of->open(QFile::WriteOnly);

		if(ok) {
			return downloadFile((this->*T)(idx), of);
		} else if(of) {
			emit error(of->errorString());
		}

		return ok;
	}

	template<class T>
	T entry(int idx, const QString &id, const QString &subId = QString::null) const {

		if(dataAvailable()) {

			if(entries() > idx) {
				return subId.isEmpty() ?  m_vdata[idx].toMap()[id].value<T>() :
										  m_vdata[idx].toMap()[subId].toMap()[id].value<T>();
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
	QString m_eTag;
	mutable QIODevice *m_dlOutputFile;
	mutable qint64 m_readBytes;
	mutable QNetworkReply *m_readReply;
	mutable qint64 m_bytesAvail;
	QGitHubReleaseAPI::TYPE m_type;
};

#endif // QGITHUBRELEASEAPI_P_H
