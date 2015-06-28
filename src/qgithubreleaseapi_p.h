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

#include <QVariantList>

#include "export.h"

class QUrl;
class QDateTime;
class FileDownloader;

class QGITHUBRELEASEAPI_NO_EXPORT QGitHubReleaseAPIPrivate : public QObject {
	Q_OBJECT
	Q_DISABLE_COPY(QGitHubReleaseAPIPrivate)

public:
	explicit QGitHubReleaseAPIPrivate(const QUrl &apiUrl, QObject *parent = 0);
	~QGitHubReleaseAPIPrivate();

	int entries() const;

	QString name(int idx) const;
	QString body(int idx) const;
	QString tagName(int idx) const;
	QDateTime publishedAt(int idx) const;

private slots:
	void downloaded();

signals:
	void available();
	void error(const QString &) const;

private:
	bool dataAvailable() const;

private:
	const FileDownloader *m_downloader;
	QVariantList m_vdata;
	QString m_errorString;
};

#endif // QGITHUBRELEASEAPI_P_H
