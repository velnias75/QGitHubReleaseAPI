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
#include <QDateTime>

#include "qgithubreleaseapi.h"
#include "qgithubreleaseapi_p.h"

QGitHubReleaseAPI::QGitHubReleaseAPI(const QUrl &apiUrl, bool multi, QObject *p) : QObject(p),
	d_ptr(new QGitHubReleaseAPIPrivate(apiUrl, multi, this)) {
	init();
}

QGitHubReleaseAPI::QGitHubReleaseAPI(const QString &user, const QString &repo, bool latest,
									 QObject *p) : QObject(p),
	d_ptr(new QGitHubReleaseAPIPrivate(user, repo, latest, this)) {
	init();
}

QGitHubReleaseAPI::QGitHubReleaseAPI(const QString &user, const QString &repo, const QString &tag,
									 QObject *p) : QObject(p),
	d_ptr(new QGitHubReleaseAPIPrivate(user, repo, tag, this)) {
	init();
}

QGitHubReleaseAPI::QGitHubReleaseAPI(const QString &user, const QString &repo, const char *tag,
									 QObject *p) : QObject(p),
	d_ptr(new QGitHubReleaseAPIPrivate(user, repo, QString(tag), this)) {
	init();
}

QGitHubReleaseAPI::QGitHubReleaseAPI(const QString &user, const QString &repo, int perPage,
									 QObject *p) : QObject(p),
	d_ptr(new QGitHubReleaseAPIPrivate(user, repo, perPage, this)) {
	init();
}

QGitHubReleaseAPI::~QGitHubReleaseAPI() {}

void QGitHubReleaseAPI::init() const {
	Q_D(const QGitHubReleaseAPI);
	QObject::connect(d, SIGNAL(available()), this, SLOT(apiAvailable()));
	QObject::connect(d, SIGNAL(error(QString)), this, SLOT(apiError(QString)));
	QObject::connect(d, SIGNAL(progress(qint64,qint64)),
					 this, SLOT(apiDownloadProgress(qint64,qint64)));
}

void QGitHubReleaseAPI::setUserAgent(const char *ua) {
	QGitHubReleaseAPIPrivate::setUserAgent(ua);
}

void QGitHubReleaseAPI::apiAvailable() {
	emit available(*this);
}

void QGitHubReleaseAPI::apiError(const QString &err) {
	emit error(err);
}

void QGitHubReleaseAPI::apiDownloadProgress(qint64 br, qint64 bt) {
	emit progress(br, bt);
}

QUrl QGitHubReleaseAPI::apiUrl() const {
	Q_D(const QGitHubReleaseAPI);
	return d->apiUrl();
}

int  QGitHubReleaseAPI::entries() const {
	Q_D(const QGitHubReleaseAPI);
	return d->entries();
}

ulong QGitHubReleaseAPI::id(int idx) const {
	Q_D(const QGitHubReleaseAPI);
	return d->id(idx);
}

QUrl QGitHubReleaseAPI::avatarUrl(int idx) const {
	Q_D(const QGitHubReleaseAPI);
	return d->avatarUrl(idx);
}

QImage QGitHubReleaseAPI::avatar(int idx) const {
	Q_D(const QGitHubReleaseAPI);
	return d->avatar(idx);
}

QUrl QGitHubReleaseAPI::url(int idx) const {
	Q_D(const QGitHubReleaseAPI);
	return d->url(idx);
}

QUrl QGitHubReleaseAPI::assetsUrl(int idx) const {
	Q_D(const QGitHubReleaseAPI);
	return d->assetsUrl(idx);
}

QUrl QGitHubReleaseAPI::uploadUrl(int idx) const {
	Q_D(const QGitHubReleaseAPI);
	return d->uploadUrl(idx);
}

QUrl QGitHubReleaseAPI::htmlUrl(int idx) const {
	Q_D(const QGitHubReleaseAPI);
	return d->htmlUrl(idx);
}

QString QGitHubReleaseAPI::name(int idx) const {
	Q_D(const QGitHubReleaseAPI);
	return d->name(idx);
}

QString QGitHubReleaseAPI::body(int idx) const {
	Q_D(const QGitHubReleaseAPI);
	return d->body(idx);
}

QString QGitHubReleaseAPI::tagName(int idx) const {
	Q_D(const QGitHubReleaseAPI);
	return d->tagName(idx);
}

QDateTime QGitHubReleaseAPI::publishedAt(int idx) const {
	Q_D(const QGitHubReleaseAPI);
	return d->publishedAt(idx);
}

QVariantList QGitHubReleaseAPI::toVariantList() const {
	Q_D(const QGitHubReleaseAPI);
	return d->toVariantList();
}

QByteArray QGitHubReleaseAPI::asJsonData() const {
	Q_D(const QGitHubReleaseAPI);
	return d->asJsonData();
}

uint QGitHubReleaseAPI::rateLimit() const {
	Q_D(const QGitHubReleaseAPI);
	return d->rateLimit();
}

uint QGitHubReleaseAPI::rateLimitRemaining() const {
	Q_D(const QGitHubReleaseAPI);
	return d->rateLimitRemaining();
}

QDateTime QGitHubReleaseAPI::rateLimitReset() const {
	Q_D(const QGitHubReleaseAPI);
	return d->rateLimitReset();
}
