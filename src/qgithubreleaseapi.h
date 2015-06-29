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

#ifndef QGITHUBRELEASEAPI_H
#define QGITHUBRELEASEAPI_H

#include <QUrl>
#include <QVariantList>

#include "export.h"

class QDateTime;
class QGitHubReleaseAPIPrivate;

class QGITHUBRELEASEAPI_EXPORT QGitHubReleaseAPI : public QObject {
	Q_OBJECT
	Q_DISABLE_COPY(QGitHubReleaseAPI)

public:
	QGitHubReleaseAPI(const QUrl &apiUrl, bool multi = true, QObject *parent = 0);
	QGitHubReleaseAPI(const QString &user, const QString &repo, bool latest = true,
					  QObject *parent = 0);
	QGitHubReleaseAPI(const QString &user, const QString &repo, const QString &tag,
					  QObject *parent = 0);
	QGitHubReleaseAPI(const QString &user, const QString &repo, const char *tag,
					  QObject *parent = 0);
	QGitHubReleaseAPI(const QString &user, const QString &repo, int perPage,
					  QObject *parent = 0);

	virtual ~QGitHubReleaseAPI();

	static void setUserAgent(const char *ua);

	QUrl apiUrl() const;

	int entries() const;

	ulong id(int idx = 0) const;
	QUrl url(int idx = 0) const;
	QUrl assetsUrl(int idx = 0) const;
	QUrl uploadUrl(int idx = 0) const;
	QUrl htmlUrl(int idx = 0) const;
	QString name(int idx = 0) const;
	QString body(int idx = 0) const;
	QString tagName(int idx = 0) const;
	QDateTime publishedAt(int idx = 0) const;
	QUrl avatarUrl(int idx = 0) const;
	QImage avatar(int idx = 0) const;
	QString login(int idx = 0) const;
	QUrl tarBallUrl(int idx = 0) const;
	QUrl zipBallUrl(int idx = 0) const;
	QByteArray tarBall(int idx = 0) const;
	QByteArray zipBall(int idx = 0) const;

	QVariantList toVariantList() const;
	QByteArray asJsonData() const;

	uint rateLimit() const;
	uint rateLimitRemaining() const;
	QDateTime rateLimitReset() const;

signals:
	void available(const QGitHubReleaseAPI &);
	void error(const QString &);
	void progress(qint64 bytesReceived, qint64 bytesTotal);

private slots:
	void apiAvailable();
	void apiError(const QString &);
	void apiDownloadProgress(qint64, qint64);

private:
	void init() const;

private:
	QGitHubReleaseAPIPrivate *const d_ptr;
	Q_DECLARE_PRIVATE(QGitHubReleaseAPI)
};

#endif // QGITHUBRELEASEAPI_H
