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

#include "qgithubreleaseapi_p.h"

QGitHubReleaseAPI::QGitHubReleaseAPI(const QUrl &apiUrl, bool multi, QObject *p) : QObject(p),
	d_ptr(new QGitHubReleaseAPIPrivate(apiUrl, multi, RAW, this)) {
	init();
}

QGitHubReleaseAPI::QGitHubReleaseAPI(const QUrl &apiUrl, TYPE type, bool multi, QObject *p) :
	QObject(p), d_ptr(new QGitHubReleaseAPIPrivate(apiUrl, multi, type, this)) {
	init();
}

QGitHubReleaseAPI::QGitHubReleaseAPI(const QString &user, const QString &repo, bool latest,
									 QObject *p) : QObject(p),
	d_ptr(new QGitHubReleaseAPIPrivate(user, repo, latest, RAW, this)) {
	init();
}

QGitHubReleaseAPI::QGitHubReleaseAPI(const QString &user, const QString &repo, TYPE type,
									 bool latest, QObject *p) : QObject(p),
	d_ptr(new QGitHubReleaseAPIPrivate(user, repo, latest, type, this)) {
	init();
}

QGitHubReleaseAPI::QGitHubReleaseAPI(const QString &user, const QString &repo, const QString &tag,
									 QObject *p) : QObject(p),
	d_ptr(new QGitHubReleaseAPIPrivate(user, repo, tag, RAW, this)) {
	init();
}

QGitHubReleaseAPI::QGitHubReleaseAPI(const QString &user, const QString &repo, const QString &tag,
									 TYPE type, QObject *p) : QObject(p),
	d_ptr(new QGitHubReleaseAPIPrivate(user, repo, tag, type, this)) {
	init();
}

QGitHubReleaseAPI::QGitHubReleaseAPI(const QString &user, const QString &repo, const char *tag,
									 QObject *p) : QObject(p),
	d_ptr(new QGitHubReleaseAPIPrivate(user, repo, QString(tag), RAW, this)) {
	init();
}

QGitHubReleaseAPI::QGitHubReleaseAPI(const QString &user, const QString &repo, const char *tag,
									 TYPE type, QObject *p) : QObject(p),
	d_ptr(new QGitHubReleaseAPIPrivate(user, repo, QString(tag), type, this)) {
	init();
}

QGitHubReleaseAPI::QGitHubReleaseAPI(const QString &user, const QString &repo, int perPage,
									 QObject *p) : QObject(p),
	d_ptr(new QGitHubReleaseAPIPrivate(user, repo, perPage, RAW, this)) {
	init();
}

QGitHubReleaseAPI::QGitHubReleaseAPI(const QString &user, const QString &repo, int perPage,
									 TYPE type, QObject *p) : QObject(p),
	d_ptr(new QGitHubReleaseAPIPrivate(user, repo, perPage, type, this)) {
	init();
}

QGitHubReleaseAPI::~QGitHubReleaseAPI() {}

void QGitHubReleaseAPI::init() const {
	Q_D(const QGitHubReleaseAPI);
	QObject::connect(d, SIGNAL(canceled()), this, SLOT(apiCanceled()));
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

void QGitHubReleaseAPI::apiCanceled() {
	emit canceled();
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

ulong QGitHubReleaseAPI::releaseId(int idx) const {
	Q_D(const QGitHubReleaseAPI);
	return d->releaseId(idx);
}
ulong QGitHubReleaseAPI::authorId(int idx) const {
	Q_D(const QGitHubReleaseAPI);
	return d->authorId(idx);
}

QUrl QGitHubReleaseAPI::avatarUrl(int idx) const {
	Q_D(const QGitHubReleaseAPI);
	return d->avatarUrl(idx);
}

QImage QGitHubReleaseAPI::avatar(int idx) const {
	Q_D(const QGitHubReleaseAPI);
	return d->avatar(idx);
}

QUrl QGitHubReleaseAPI::releaseUrl(int idx) const {
	Q_D(const QGitHubReleaseAPI);
	return d->releaseUrl(idx);
}

QUrl QGitHubReleaseAPI::assetsUrl(int idx) const {
	Q_D(const QGitHubReleaseAPI);
	return d->assetsUrl(idx);
}

QUrl QGitHubReleaseAPI::uploadUrl(int idx) const {
	Q_D(const QGitHubReleaseAPI);
	return d->uploadUrl(idx);
}

QUrl QGitHubReleaseAPI::releaseHtmlUrl(int idx) const {
	Q_D(const QGitHubReleaseAPI);
	return d->releaseHtmlUrl(idx);
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

QString QGitHubReleaseAPI::login(int idx) const {
	Q_D(const QGitHubReleaseAPI);
	return d->login(idx);
}

QUrl QGitHubReleaseAPI::tarBallUrl(int idx) const {
	Q_D(const QGitHubReleaseAPI);
	return d->tarBallUrl(idx);
}

QUrl QGitHubReleaseAPI::zipBallUrl(int idx) const {
	Q_D(const QGitHubReleaseAPI);
	return d->zipBallUrl(idx);
}

QByteArray QGitHubReleaseAPI::tarBall(int idx) const {
	Q_D(const QGitHubReleaseAPI);
	return d->tarBall(idx);
}

qint64 QGitHubReleaseAPI::tarBall(QFile &of, int idx) const {
	Q_D(const QGitHubReleaseAPI);
	return d->tarBall(of, idx);
}

QByteArray QGitHubReleaseAPI::zipBall(int idx) const {
	Q_D(const QGitHubReleaseAPI);
	return d->zipBall(idx);
}

qint64 QGitHubReleaseAPI::zipBall(QFile &of, int idx) const {
	Q_D(const QGitHubReleaseAPI);
	return d->zipBall(of, idx);
}

QString QGitHubReleaseAPI::targetCommitish(int idx) const {
	Q_D(const QGitHubReleaseAPI);
	return d->targetCommitish(idx);
}

bool QGitHubReleaseAPI::isDraft(int idx) const {
	Q_D(const QGitHubReleaseAPI);
	return d->isDraft(idx);
}

bool QGitHubReleaseAPI::isPreRelease(int idx) const {
	Q_D(const QGitHubReleaseAPI);
	return d->isPreRelease(idx);
}

QString QGitHubReleaseAPI::eTag() const {
	Q_D(const QGitHubReleaseAPI);
	return d->eTag();
}

void QGitHubReleaseAPI::setETag(const QString &eTag) {
	Q_D(QGitHubReleaseAPI);
	d->setETag(eTag);
}

QUrl QGitHubReleaseAPI::authorHtmlUrl(int idx) const {
	Q_D(const QGitHubReleaseAPI);
	return d->authorHtmlUrl(idx);
}

QDateTime QGitHubReleaseAPI::createdAt(int idx) const {
	Q_D(const QGitHubReleaseAPI);
	return d->createdAt(idx);
}

void QGitHubReleaseAPI::cancel() {
	Q_D(QGitHubReleaseAPI);
	return d->cancel();
}

QByteArray QGitHubReleaseAPI::downloadToMemory(const QUrl &url) const {
	Q_D(const QGitHubReleaseAPI);
	return d->downloadFile(url);
}

qint64 QGitHubReleaseAPI::downloadToFile(const QUrl &url, QFile &of) const {
	Q_D(const QGitHubReleaseAPI);
	return d->downloadFile(url, &of);
}
