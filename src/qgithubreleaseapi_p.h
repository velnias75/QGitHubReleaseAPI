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

QT_FORWARD_DECLARE_CLASS(QNetworkReply)
QT_FORWARD_DECLARE_CLASS(FileDownloader)

class QGITHUBRELEASEAPI_NO_EXPORT QGitHubReleaseAPIPrivate : public QObject {
	Q_OBJECT
	Q_DISABLE_COPY(QGitHubReleaseAPIPrivate)

	template<class> friend class EntryHelper;

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

	QUrl releaseUrl(int idx) const;
	QUrl assetsUrl(int idx) const;
	QUrl uploadUrl(int idx) const;
	QUrl releaseHtmlUrl(int idx) const;
	QUrl authorHtmlUrl(int idx) const;
	QUrl tarBallUrl(int idx) const;
	QUrl zipBallUrl(int idx) const;
	ulong releaseId(int idx) const;
	QString name(int idx) const;
	ulong authorId(int idx) const;
	QString login(int idx) const;
	QUrl avatarUrl(int idx) const;
	QString tagName(int idx) const;
	QDateTime publishedAt(int idx) const;
	QString targetCommitish(int idx) const;
	bool isDraft(int idx) const;
	bool isPreRelease(int idx) const;
	QDateTime createdAt(int idx) const;

	QByteArray tarBall(int idx) const;
	qint64 tarBall(QFile &of, int idx) const;

	QByteArray zipBall(int idx) const;
	qint64 zipBall(QFile &of, int idx) const;

	QString body(int idx) const;
	QImage avatar(int idx) const;

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

private:
	static const char *m_userAgent;
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
