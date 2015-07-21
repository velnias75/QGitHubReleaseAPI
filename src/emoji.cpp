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

#include "emoji.h"
#include "entryhelper.h"

Emoji::Emoji(const QString &eTag) : QGitHubReleaseAPIPrivate(QUrl("https://api.github.com/emojis"),
															 false, QGitHubReleaseAPI::RAW, eTag) {}

Emoji::~Emoji() {}

QUrl Emoji::getUrl(const QString &key) const {
	return EntryHelper<QUrl>(*this)(0, key);
}
