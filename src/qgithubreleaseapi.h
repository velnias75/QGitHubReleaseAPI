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

/**
 * @file
 */

#ifndef QGITHUBRELEASEAPI_H
#define QGITHUBRELEASEAPI_H

#include <QUrl>
#include <QImage>
#include <QDateTime>
#include <QVariantList>

QT_FORWARD_DECLARE_CLASS(QFile)
QT_FORWARD_DECLARE_CLASS(QGitHubReleaseAPIPrivate)

/**
 * @brief The @c %QGitHubReleaseAPI class
 * @author Heiko Schaefer
 */
class Q_DECL_EXPORT QGitHubReleaseAPI : public QObject {
	Q_OBJECT
	Q_DISABLE_COPY(QGitHubReleaseAPI)
	Q_PROPERTY(QUrl apiUrl READ apiUrl) ///< the apiUrl
	Q_PROPERTY(int entries READ entries) ///< the number of entries
	Q_PROPERTY(QString eTag READ eTag WRITE setETag) ///< the eTag
	Q_PROPERTY(uint rateLimit READ rateLimit) ///< the current rateLimit
	Q_PROPERTY(uint rateLimitRemaining READ rateLimitRemaining) ///< the remaining rateLimit
	Q_PROPERTY(QDateTime rateLimitReset READ rateLimitReset) ///< the date the rate limit resets
	Q_PROPERTY(ulong releaseId READ releaseId) ///< the releaseId
	Q_PROPERTY(QUrl releaseUrl READ releaseUrl) ///< the releaseUrl
	Q_PROPERTY(QUrl assetsUrl READ assetsUrl) ///< the assetsUrl
	Q_PROPERTY(QUrl uploadUrl READ uploadUrl) ///< the uploadUrl
	Q_PROPERTY(QUrl releaseHtmlUrl READ releaseHtmlUrl) ///< the releaseHtmlUrl
	Q_PROPERTY(QString name READ name) ///< the release name
	Q_PROPERTY(QString body READ body) ///< the release body
	Q_PROPERTY(QString tagName READ tagName) ///< the release tag name
	Q_PROPERTY(QDateTime publishedAt READ publishedAt) ///< the date of publishing
	Q_PROPERTY(QDateTime createdAt READ createdAt) ///< the date of creation
	Q_PROPERTY(QUrl avatarUrl READ avatarUrl) ///< the avatarUrl
	Q_PROPERTY(QImage avatar READ avatar) ///< the avatatr as @c QImage
	Q_PROPERTY(ulong authorId READ authorId) ///< the authorId
	Q_PROPERTY(QUrl authorHtmlUrl READ authorHtmlUrl) ///< the authorHtmlUrl
	Q_PROPERTY(QString login READ login) ///< the author login
	Q_PROPERTY(QUrl tarBallUrl READ tarBallUrl) ///< the tarBallUrl
	Q_PROPERTY(QUrl zipBallUrl READ zipBallUrl) ///< the zipBallUrl
	Q_PROPERTY(QByteArray tarBall READ tarBall) ///< the tarBall as @c QByteArray
	Q_PROPERTY(QByteArray zipBall READ zipBall) ///< the zipBall as @c QByteArray
	Q_PROPERTY(QString targetCommitish READ targetCommitish) ///< the targetCommitish (branch)
	Q_PROPERTY(bool draft READ isDraft) ///< @c true if the release is a draft, @c false otherwise
	Q_PROPERTY(bool prerelease READ isPreRelease) ///< @c true if pre-release, @c false otherwise

public:
	/**
	 * @brief Type of body
	 */
	typedef enum { RAW, ///< receive a raw body and render it with @em libmarkdown
				   TEXT, ///< receive a text body
				   HTML ///< receive a html body (rendered by GitHub)
				 } TYPE;

	/**
	 * @brief Creates an @c %QGitHubReleaseAPI instance
	 * @param apiUrl direct URL to retrieve
	 * @param type the type of the body
	 * @param multi @c true if multiple results are expected, @c false otherwise
	 */
	QGitHubReleaseAPI(const QUrl &apiUrl, TYPE type, bool multi = true, QObject *parent = 0);

	/**
	 * @brief Creates an @c %QGitHubReleaseAPI instance
	 * @param apiUrl direct URL to retrieve
	 * @param multi @c true if multiple results are expected, @c false otherwise
	 */
	explicit QGitHubReleaseAPI(const QUrl &apiUrl, bool multi = true, QObject *parent = 0);

	/**
	 * @brief Creates an @c %QGitHubReleaseAPI instance
	 * @param user the GitHub user (aka login)
	 * @param repo the repository to retrieve release information for
	 * @param type the type of the body
	 * @param latest @c true to only retrieve the latest release, @c false for all releases
	 */
	QGitHubReleaseAPI(const QString &user, const QString &repo, TYPE type, bool latest = true,
					  QObject *parent = 0);

	/**
	 * @brief Creates an @c %QGitHubReleaseAPI instance
	 * @param user the GitHub user (aka login)
	 * @param repo the repository to retrieve release information for
	 * @param latest @c true to only retrieve the latest release, @c false for all releases
	 */
	QGitHubReleaseAPI(const QString &user, const QString &repo, bool latest = true,
					  QObject *parent = 0);

	/**
	 * @brief Creates an @c %QGitHubReleaseAPI instance
	 * @param user the GitHub user (aka login)
	 * @param repo the repository to retrieve release information for
	 * @param tag the release tag to retrieve
	 * @param type the type of the body
	 */
	QGitHubReleaseAPI(const QString &user, const QString &repo, const QString &tag, TYPE type,
					  QObject *parent = 0);

	/**
	 * @brief Creates an @c %QGitHubReleaseAPI instance
	 * @param user the GitHub user (aka login)
	 * @param repo the repository to retrieve release information for
	 * @param tag the release tag to retrieve
	 */
	QGitHubReleaseAPI(const QString &user, const QString &repo, const QString &tag,
					  QObject *parent = 0);

	/**
	 * @brief Creates an @c %QGitHubReleaseAPI instance
	 * @param user the GitHub user (aka login)
	 * @param repo the repository to retrieve release information for
	 * @param tag the release tag to retrieve
	 * @param type the type of the body
	 */
	QGitHubReleaseAPI(const QString &user, const QString &repo, const char *tag, TYPE type,
					  QObject *parent = 0);

	/**
	 * @brief Creates an @c %QGitHubReleaseAPI instance
	 * @param user the GitHub user (aka login)
	 * @param repo the repository to retrieve release information for
	 * @param tag the release tag to retrieve
	 */
	QGitHubReleaseAPI(const QString &user, const QString &repo, const char *tag,
					  QObject *parent = 0);

	/**
	 * @brief Creates an @c %QGitHubReleaseAPI instance
	 * @param user the GitHub user (aka login)
	 * @param repo the repository to retrieve release information for
	 * @param perPage the amount of releases to retrieve
	 * @param type the type of the body
	 */
	QGitHubReleaseAPI(const QString &user, const QString &repo, int perPage, TYPE type,
					  QObject *parent = 0);

	/**
	 * @brief Creates an @c %QGitHubReleaseAPI instance
	 * @param user the GitHub user (aka login)
	 * @param repo the repository to retrieve release information for
	 * @param perPage the amount of releases to retrieve
	 */
	QGitHubReleaseAPI(const QString &user, const QString &repo, int perPage,
					  QObject *parent = 0);

	virtual ~QGitHubReleaseAPI();

	/**
	 * @brief Sets an UserAgent
	 * @note defaults to @em %QGitHubReleaseAPI
	 * @param userAgent an UserAgent
	 */
	static void setUserAgent(const char *userAgent);

	/**
	 * @brief The api URL
	 * @return the api URL
	 */
	QUrl apiUrl() const;

	/**
	 * @brief The number of entries received
	 * @return the number of entries received
	 */
	int entries() const;

	/**
	 * @brief Downloads the file at @c QUrl into a @c QByteArray
	 * @param url the URL to download from
	 * @return the @c QByteArray containing the downloaded file
	 */
	QByteArray downloadToMemory(const QUrl &url) const;

	/**
	 * @brief Downloads the file at @c QUrl into a file
	 * @param url the URL to download from
	 * @param outputFile the file to download to
	 * @return the number of received bytes
	 */
	qint64 downloadToFile(const QUrl &url, QFile &outputFile) const;

	/**
	 * @brief the eTag
	 *
	 * An @em eTag, if known, can get used to avoid using the rate limit
	 *
	 * @return the ETag
	 */
	QString eTag() const;

	/**
	 * @brief Sets the eTag
	 *
	 * An @em eTag, if known, can get used to avoid using the rate limit
	 *
	 * @param eTag the eTag
	 */
	void setETag(const QString &eTag);

	/**
	 * @brief Gets the release information as @c QVariantList
	 * @return
	 */
	QVariantList toVariantList() const;

	/**
	 * @brief Gets the release information as raw Json data
	 * @return
	 */
	QByteArray asJsonData() const;

	/**
	 * @name Accessing the release information
	 * @{
	 */

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
	QUrl tarBallUrl(int idx = 0) const;
	QUrl zipBallUrl(int idx = 0) const;
	QByteArray tarBall(int idx = 0) const;
	qint64 tarBall(QFile &outputFile, int idx = 0) const;
	QByteArray zipBall(int idx = 0) const;
	qint64 zipBall(QFile &outputFile, int idx = 0) const;
	QString targetCommitish(int idx = 0) const;
	bool isDraft(int idx = 0) const;
	bool isPreRelease(int idx = 0) const;

	/// @}

	/**
	 * @name Accessing the user information
	 * @{
	 */

	/**
	 * @brief The URL to the avatar
	 * @param idx the entry index
	 * @return url to the avatar
	 */
	QUrl avatarUrl(int idx = 0) const;

	/**
	 * @brief Downloads the avatar and creates a @c QImage
	 * @param idx the entry index
	 * @return the avatar image of a @c NULL QImage on failure
	 */
	QImage avatar(int idx = 0) const;

	/**
	 * @brief The URL to the author HTML site
	 * @param idx the entry index
	 * @return url to the author HTML site
	 */
	QUrl authorHtmlUrl(int idx = 0) const;

	/**
	 * @brief the authorId
	 * @param idx the entry index
	 * @return the authorId
	 */
	ulong authorId(int idx = 0) const;

	/**
	 * @brief The login name of the author
	 * @param idx the entry index
	 * @return the login name of the author
	 */
	QString login(int idx = 0) const;

	/// @}

	uint rateLimit() const;
	uint rateLimitRemaining() const;
	QDateTime rateLimitReset() const;

signals:
	/**
	 * @brief Emitted if the release data is available
	 * @param api reference to the @c %QGitHubReleaseAPI
	 */
	void available(const QGitHubReleaseAPI &api);

	/**
	 * @brief Emitted on any error
	 * @param error the error string
	 */
	void error(const QString &error);

	/**
	 * @brief Emitted if a operation has canceled
	 */
	void canceled();

	/**
	 * @brief Emitted while downloading
	 *
	 * @c bytesTotal is @c -1 if the number of total bytes is not available\n
	 * @c bytesReceived is equal to @c bytesTotal if the download has finished
	 *
	 * @param bytesReceived the number of bytes received
	 * @param bytesTotal the number of total bytes
	 */
	void progress(qint64 bytesReceived, qint64 bytesTotal);

public slots:
	/**
	 * @brief Cancels the current operation
	 */
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
