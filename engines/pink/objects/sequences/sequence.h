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

#ifndef PINK_SEQUENCE_H
#define PINK_SEQUENCE_H

#include "common/array.h"

#include "pink/objects/object.h"

namespace Pink {

class Sequencer;
class SequenceItem;
class SequenceContext;

class Sequence : public NamedObject {
public:
    Sequence();
    virtual ~Sequence();
    virtual void deserialize(Archive &archive);

    virtual void toConsole();

    Common::Array<SequenceItem*> &getItems();

    void setContext(SequenceContext *context);
    virtual void init(int unk);
    virtual void start(int unk);
    virtual void end();

    virtual void update();
    virtual void restart();
    virtual void skipSubSequence();
    virtual void skipToLastSubSequence();
    void skipItemsTo(int index);


public:
    SequenceContext *_context;
    Sequencer *_sequencer;
    Common::Array<SequenceItem*> _items;
    int _unk;
};

class Sound;

class SequenceAudio : public Sequence {
public:
    virtual void deserialize(Archive &archive);
    virtual void toConsole();

    virtual void init(int unk);
    virtual void start(int unk);
    virtual void end();

    virtual void update();
    virtual void restart();

    virtual void skipSubSequence() {};
    virtual void skipToLastSubSequence();

private:
    Common::String _soundName;
    Sound *_sound;
    uint _sample;
};

} // End of namespace Pink

#endif
