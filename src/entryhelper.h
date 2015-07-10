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

#ifndef ENTRYHELPER_H
#define ENTRYHELPER_H

#include "qgithubreleaseapi_p.h"

template<class T>
class Q_DECL_HIDDEN EntryHelper {
	Q_DISABLE_COPY(EntryHelper)
public:
	explicit inline EntryHelper(const QGitHubReleaseAPIPrivate& a) : api(a) {}

	T operator()(int idx, const QString &id, const QString &subId = QString::null) const {

		if(api.dataAvailable()) {

			if(api.entries() > idx) {
				return subId.isEmpty() ? api.m_vdata[idx].toMap()[id].value<T>() :
										 api.m_vdata[idx].toMap()[subId].toMap()[id].value<T>();
			} else {
				emit api.error(QString(api.m_outOfBoundsError).arg(api.entries()).arg(idx));
			}

		} else {
			emit api.error(api.m_noDataAvailableError);
		}

		return T();
	}

private:
	const QGitHubReleaseAPIPrivate& api;
};

#ifdef QJSON_LEGACY_URL_WRAPPER
template<> inline QUrl EntryHelper<QUrl>::operator()(int idx, const QString &id,
													 const QString &subId) const {
	return QUrl(EntryHelper<QString>(api)(idx, id, subId));
}
#endif

#endif // ENTRYHELPER_H
