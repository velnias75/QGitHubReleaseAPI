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

	QUrl url() const;

	int entries() const;

	QString name(int idx) const;
	QString body(int idx) const;
	QString tagName(int idx) const;
	QDateTime publishedAt(int idx) const;

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
	void downloaded();
	void fdError(const QString &);
	void downloadProgress(qint64, qint64);

signals:
	void available();
	void error(const QString &) const;
	void progress(qint64, qint64);

private:
	void init() const;
	bool dataAvailable() const;

private:
	static const char *m_userAgent QGITHUBRELEASEAPI_EXPORT;

	const FileDownloader *m_downloader;
	QByteArray m_jsonData;
	QVariantList m_vdata;
	QString m_errorString;
	uint m_rateLimit;
	uint m_rateLimitRemaining;
	bool m_singleEntryRequested;
	QDateTime m_rateLimitReset;
};

#endif // QGITHUBRELEASEAPI_P_H
