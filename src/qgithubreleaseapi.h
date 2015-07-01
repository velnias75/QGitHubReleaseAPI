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
#include <QImage>
#include <QDateTime>
#include <QVariantList>

#include "export.h"

class QFile;
class QGitHubReleaseAPIPrivate;

class QGITHUBRELEASEAPI_EXPORT QGitHubReleaseAPI : public QObject {
	Q_OBJECT
	Q_DISABLE_COPY(QGitHubReleaseAPI)
	Q_PROPERTY(QUrl apiUrl READ apiUrl)
	Q_PROPERTY(int entries READ entries)
	Q_PROPERTY(QString eTag READ eTag WRITE setETag)
	Q_PROPERTY(uint rateLimit READ rateLimit)
	Q_PROPERTY(uint rateLimitRemaining READ rateLimitRemaining)
	Q_PROPERTY(QDateTime rateLimitReset READ rateLimitReset)
	Q_PROPERTY(ulong releaseId READ releaseId)
	Q_PROPERTY(QUrl releaseUrl READ releaseUrl)
	Q_PROPERTY(QUrl assetsUrl READ assetsUrl)
	Q_PROPERTY(QUrl uploadUrl READ uploadUrl)
	Q_PROPERTY(QUrl releaseHtmlUrl READ releaseHtmlUrl)
	Q_PROPERTY(QString name READ name)
	Q_PROPERTY(QString body READ body)
	Q_PROPERTY(QString tagName READ tagName)
	Q_PROPERTY(QDateTime publishedAt READ publishedAt)
	Q_PROPERTY(QDateTime createdAt READ createdAt)
	Q_PROPERTY(QUrl avatarUrl READ avatarUrl)
	Q_PROPERTY(QImage avatar READ avatar)
	Q_PROPERTY(ulong authorId READ authorId)
	Q_PROPERTY(QUrl authorHtmlUrl READ authorHtmlUrl)
	Q_PROPERTY(QString login READ login)
	Q_PROPERTY(QUrl tarBallUrl READ tarBallUrl)
	Q_PROPERTY(QUrl zipBallUrl READ zipBallUrl)
	Q_PROPERTY(QByteArray tarBall READ tarBall)
	Q_PROPERTY(QByteArray zipBall READ zipBall)
	Q_PROPERTY(QString targetCommitish READ targetCommitish)
	Q_PROPERTY(bool draft READ isDraft)
	Q_PROPERTY(bool prerelease READ isPreRelease)

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

	QString eTag() const;
	void setETag(const QString &eTag);

	ulong releaseId(int idx = 0) const;
	QUrl releaseUrl(int idx = 0) const;
	QUrl assetsUrl(int idx = 0) const;
	QUrl uploadUrl(int idx = 0) const;
	QUrl releaseHtmlUrl(int idx = 0) const;
	QString name(int idx = 0) const;
	QString body(int idx = 0) const;
	QString tagName(int idx = 0) const;
	QDateTime publishedAt(int idx = 0) const;
	QDateTime createdAt(int idx = 0) const;
	QUrl avatarUrl(int idx = 0) const;
	QImage avatar(int idx = 0) const;
	ulong authorId(int idx = 0) const;
	QUrl authorHtmlUrl(int idx = 0) const;
	QString login(int idx = 0) const;
	QUrl tarBallUrl(int idx = 0) const;
	QUrl zipBallUrl(int idx = 0) const;
	QByteArray tarBall(int idx = 0) const;
	qint64 tarBall(QFile &of, int idx = 0) const;
	QByteArray zipBall(int idx = 0) const;
	qint64 zipBall(QFile &of, int idx = 0) const;
	QString targetCommitish(int idx = 0) const;
	bool isDraft(int idx = 0) const;
	bool isPreRelease(int idx = 0) const;

	QVariantList toVariantList() const;
	QByteArray asJsonData() const;

	uint rateLimit() const;
	uint rateLimitRemaining() const;
	QDateTime rateLimitReset() const;

signals:
	void available(const QGitHubReleaseAPI &);
	void error(const QString &);
	void canceled();
	void progress(qint64 bytesReceived, qint64 bytesTotal);

public slots:
	void cancel();

private slots:
	void apiAvailable();
	void apiCanceled();
	void apiError(const QString &);
	void apiDownloadProgress(qint64, qint64);

private:
	void init() const;

private:
	QGitHubReleaseAPIPrivate *const d_ptr;
	Q_DECLARE_PRIVATE(QGitHubReleaseAPI)
};

#endif // QGITHUBRELEASEAPI_H
