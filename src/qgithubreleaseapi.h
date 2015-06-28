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

#include <QObject>

#include "export.h"

class QUrl;
class QDateTime;
class QGitHubReleaseAPIPrivate;

class QGITHUBRELEASEAPI_EXPORT QGitHubReleaseAPI : public QObject {
	Q_OBJECT
	Q_DISABLE_COPY(QGitHubReleaseAPI)

public:
	QGitHubReleaseAPI(const QUrl &apiUrl, QObject *parent = 0);

	int entries() const;

	QString name(int idx = 0) const;
	QString body(int idx = 0) const;
	QString tagName(int idx = 0) const;
	QDateTime publishedAt(int idx = 0) const;

signals:
	void available();
	void error(const QString &);

private slots:
	void apiAvailable();
	void apiError(const QString &);

private:
	QGitHubReleaseAPIPrivate *const d_ptr;
	Q_DECLARE_PRIVATE(QGitHubReleaseAPI)
};

#endif // QGITHUBRELEASEAPI_H
