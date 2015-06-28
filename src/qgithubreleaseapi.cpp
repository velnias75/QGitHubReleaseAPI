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

#include "qgithubreleaseapi.h"

#include "qgithubreleaseapi_p.h"

QGitHubReleaseAPI::QGitHubReleaseAPI(const QUrl &apiUrl, QObject *p) : QObject(p),
	d_ptr(new QGitHubReleaseAPIPrivate(apiUrl, this)) {

	Q_D(const QGitHubReleaseAPI);
	QObject::connect(d, SIGNAL(available()), this, SLOT(apiAvailable()));
	QObject::connect(d, SIGNAL(error(QString)), this, SLOT(apiError(QString)));
}

void QGitHubReleaseAPI::apiAvailable() {
	emit available();
}

void QGitHubReleaseAPI::apiError(const QString &err) {
	emit error(err);
}

int  QGitHubReleaseAPI::entries() const {
	Q_D(const QGitHubReleaseAPI);
	return d->entries();
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
