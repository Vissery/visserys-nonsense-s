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
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef ULTIMA_SHARED_ENGINE_RESOURCES_H
#define ULTIMA_SHARED_ENGINE_RESOURCES_H

#include "common/algorithm.h"
#include "common/str.h"
#include "common/serializer.h"

#define STRING_BUFFER_SIZE 32768

namespace Ultima {
namespace Shared {

/**
 * Base class for classes that exposes a set of strings, arrays, and other data from a resource
 */
class ResourceFile {
private:
	Common::ReadStream *_inStream;
	char _buffer[STRING_BUFFER_SIZE];
	char *_bufferP;
protected:
	/**
	 * Constructor
	 */
	ResourceFile(Common::ReadStream *in);

	/**
	 * Destructor
	 */
	virtual ~ResourceFile() {}

	virtual void syncString(const char *&str);
	virtual void syncStrings(const char **str, size_t count);
	virtual void syncStrings2D(const char **str, size_t count1, size_t count2);
	virtual void syncNumber(int &val);
	virtual void syncNumbers(int *vals, size_t count);
	virtual void syncNumbers2D(int *vals, size_t count1, size_t count2);
	virtual void syncNumbers3D(int *vals, size_t count1, size_t count2, size_t count3);
	virtual void syncBytes(byte *vals, size_t count);
	virtual void syncBytes2D(byte *vals, size_t count1, size_t count2);
};

/**
 * Derived base class for resources that have their contents within the executable rather than a data file.
 * This will allow the data for a particular Ultima game to be gradually built up without repeatedly
 * regenerating a data file. Once a game has been properly tested, then it can be moved out.
 */
class LocalResourceFile : protected ResourceFile {
private:
	Common::WriteStream *_outStream;
protected:
	/**
	 * Constructor
	 */
	LocalResourceFile(Common::ReadStream *in) : ResourceFile(in) {}

	/**
	 * Constructor
	 */
	LocalResourceFile(Common::WriteStream *out);

	virtual void syncString(const char *&str);
	virtual void syncStrings(const char **str, size_t count);
	virtual void syncStrings2D(const char **str, size_t count1, size_t count2);
	virtual void syncNumber(int &val);
	virtual void syncNumbers(int *vals, size_t count);
	virtual void syncNumbers2D(int *vals, size_t count1, size_t count2);
	virtual void syncNumbers3D(int *vals, size_t count1, size_t count2, size_t count3);
	virtual void syncBytes(byte *vals, size_t count);
	virtual void syncBytes2D(byte *vals, size_t count1, size_t count2);
};

/**
 * Resources manager
 */
class Resources {
public:
	/**
	 * Constructor
	 */
	Resources() {}

	/**
	 * Sets up the resources for the engine
	 * @returns		False if setup failed
	 */
	bool setup();
};

} // End of namespace Shared
} // End of namespace Ultima

#endif
