/*
 * Copyright 2015 by Heiko Schäfer <heiko@rangun.de>
 *
 * This file is part of NetMauMau Qt Client.
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

#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H

#include <QNetworkAccessManager>

#include "export.h"

class QUrl;
class QNetworkReply;

class QGITHUBRELEASEAPI_NO_EXPORT FileDownloader : public QObject {
	Q_OBJECT
	Q_DISABLE_COPY(FileDownloader)
public:
	explicit FileDownloader(const QUrl &url, QObject *parent = 0L);
	virtual ~FileDownloader();

	const QByteArray &downloadedData() const;

signals:
	void downloaded();

private slots:
	void fileDownloaded(QNetworkReply *pReply);

private:
	QNetworkAccessManager m_WebCtrl;
	QByteArray m_DownloadedData;
};

#endif // FILEDOWNLOADER_H
