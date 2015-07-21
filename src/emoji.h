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

#ifndef EMOJI_H
#define EMOJI_H

#include "qgithubreleaseapi_p.h"

class Q_DECL_HIDDEN Emoji : public QGitHubReleaseAPIPrivate {
	Q_OBJECT
	Q_DISABLE_COPY(Emoji)
public:
	explicit Emoji(const QString &eTag);
	virtual ~Emoji();

	QUrl getUrl(const QString &key) const;
};

#endif // EMOJI_H
