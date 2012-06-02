/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

/*
 * This file is based on WME Lite.
 * http://dead-code.org/redir.php?target=wmelite
 * Copyright (c) 2011 Jan Nedoma
 */

#ifndef WINTERMUTE_PATHUTILS_H
#define WINTERMUTE_PATHUTILS_H

#include "engines/wintermute/PlatformSDL.h"

namespace WinterMute {

class PathUtil {
public:
	static AnsiString UnifySeparators(const AnsiString &path);
	static AnsiString NormalizeFileName(const AnsiString &path);
	static AnsiString Combine(const AnsiString &path1, const AnsiString &path2);
	static AnsiString GetDirectoryName(const AnsiString &path);
	static AnsiString GetFileName(const AnsiString &path);
	static AnsiString GetFileNameWithoutExtension(const AnsiString &path);
	static AnsiString GetExtension(const AnsiString &path);
	static bool CreateDirectory(const AnsiString &path);
	static bool MatchesMask(const AnsiString &fileName, const AnsiString &mask);

	static bool FileExists(const AnsiString &fileName);

	static AnsiString GetSafeLogFileName();
	static AnsiString GetUserDirectory();
};

} // end of namespace WinterMute

#endif // WINTERMUTE_PATHUTILS_H
