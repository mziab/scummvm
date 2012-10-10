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

#ifndef NEVERHOOD_GAMEVARS_H
#define NEVERHOOD_GAMEVARS_H

#include "common/array.h"
#include "neverhood/neverhood.h"

namespace Neverhood {

enum {
	// Misc
	V_MODULE_NAME			= 0x91080831,			// Currently active module name hash
	V_DEBUG					= 0xA4014072,			// Original debug-flag, can probably be removed
	V_SMACKER_CAN_ABORT		= 0x06C02850,			// Not set anywhere (yet), seems like a debug flag
	V_KEY3_LOCATION			= 0x13382860,			// Location of the third key
	V_TEXT_FLAG1			= 0x8440001F,
	V_TEXT_INDEX			= 0x01830201,
	V_TEXT_COUNTING_INDEX1	= 0x29408F00,
	V_TEXT_COUNTING_INDEX2	= 0x8A140C21,
	V_TALK_COUNTING_INDEX	= 0xA0808898,
	V_FRUIT_COUNTING_INDEX	= 0x40040831,
	V_NOISY_SYMBOL_INDEX	= 0x2414C2F2,
	V_COLUMN_BACK_NAME		= 0x4CE79018,
	V_COLUMN_TEXT_NAME		= 0xC8C28808,
	V_CLICKED_COLUMN_INDEX	= 0x48A68852,
	V_CLICKED_COLUMN_ROW	= 0x49C40058,
	V_MUSIC_NAME			= 0x89A82A15,
	// Klayman
	V_KLAYMAN_SMALL			= 0x1860C990,			// Is Klayman small?
	V_KLAYMAN_FRAMEINDEX	= 0x18288913,
	V_KLAYMAN_IS_DELTA_X	= 0xC0418A02,
	V_KLAYMAN_SAVED_X		= 0x00D30138,
	V_CAR_DELTA_X			= 0x21E60190,
	// Flags
	V_CRYSTAL_COLORS_INIT	= 0xDE2EC914,
	V_TV_JOKE_TOLD			= 0x92603A79,
	V_NOTES_DOOR_UNLOCKED	= 0x0045D021,
	V_WATER_RUNNING			= 0x4E0BE910,
	V_CREATURE_ANGRY		= 0x0A310817,			// After having played with the music box
	V_BEEN_SHRINKING_ROOM	= 0x1C1B8A9A,
	V_BEEN_STATUE_ROOM		= 0xCB45DE03,
	V_MOUSE_PUZZLE_SOLVED	= 0x70A1189C,
	V_NOTES_PUZZLE_SOLVED	= 0x86615030,
	V_TILE_PUZZLE_SOLVED	= 0x404290D5,
	V_STAIRS_PUZZLE_SOLVED	= 0xA9035F60,
	V_SPIKES_RETRACTED		= 0x18890C91,
	V_LARGE_DOOR_NUMBER		= 0x9A500914,			// Number of the currently "large" door
	V_LIGHTS_ON				= 0x4D080E54,
	V_SHRINK_LIGHTS_ON		= 0x190A1D18,			// Lights on in the room with the shrinking device
	V_STAIRS_DOWN			= 0x09221A62,
	V_LADDER_DOWN			= 0x0018CA22,			// Is the ladder in the statue room down?
	V_LADDER_DOWN_ACTION	= 0x00188211,
	V_WALL_BROKEN			= 0x10938830,
	V_BOLT_DOOR_OPEN		= 0x01BA1A52,
	V_BOLT_DOOR_UNLOCKED	= 0x00040153,
	V_SEEN_SYMBOLS_NO_LIGHT	= 0x81890D14,
	V_FELL_DOWN_HOLE		= 0xE7498218,
	V_DOOR_PASSED			= 0x2090590C,			// Auto-closing door was passed
	V_ENTRANCE_OPEN			= 0xD0A14D10,			// Is the entrance to Module1300 open (after the robot got his teddy)
	V_WINDOW_OPEN			= 0x03C698DA,
	V_DOOR_STATUS			= 0x52371C95,
	V_DOOR_BUSTED			= 0xD217189D,
	V_WORLDS_JOINED			= 0x98109F12,			// Are the worlds joined?
	V_KEYDOOR_UNLOCKED		= 0x80455A41,			// Is the keyboard-door unlocked?
	V_MOUSE_SUCKED_IN		= 0x01023818,			// Are mouse/cheese in Scene1308?
	V_BALLOON_POPPED		= 0xAC00C0D0,			// Has the balloon with the key been popped?
	V_TNT_DUMMY_BUILT		= 0x000CF819,			// Are all TNT parts on the dummy?
	V_TNT_DUMMY_FUSE_LIT	= 0x20A0C516,
	V_RING5_PULLED			= 0x4DE80AC0,
	V_CREATURE_EXPLODED		= 0x2A02C07B,
	// Match
	V_MATCH_STATUS			= 0x0112090A,
	// Venus fly trap
	V_FLYTRAP_RING_EATEN	= 0x2B514304,
	V_FLYTRAP_RING_DOOR		= 0x8306F218,
	V_FLYTRAP_RING_FENCE	= 0x80101B1E,
	V_FLYTRAP_RING_BRIDGE	= 0x13206309,
	V_FLYTRAP_POSITION_1	= 0x1B144052,
	V_FLYTRAP_POSITION_2	= 0x86341E88,
	// Navigation
	V_NAVIGATION_INDEX		= 0x4200189E,			// Navigation scene: Current navigation index
	// Cannon
	V_CANNON_RAISED			= 0x000809C2,			// Is the cannon raised?
	V_CANNON_TURNED			= 0x9040018A,			// Is the cannon turned?
	V_ROBOT_HIT				= 0x0C0288F4,			// Was the robot hit by the cannon?
	V_ROBOT_TARGET			= 0x610210B7,			// Is the robot at the cannon target position? (teddy)
	V_CANNON_SMACKER_NAME	= 0xF0402B0A,
	V_CANNON_TARGET_STATUS	= 0x20580A86,
	// Projector
	V_PROJECTOR_SLOT		= 0x04A10F33,			// Projector x slot index
	V_PROJECTOR_LOCATION	= 0x04A105B3,			// Projector scene location
	V_PROJECTOR_ACTIVE		= 0x12A10DB3,			// Is the projecor projecting?
	// Inventory
	V_HAS_NEEDLE			= 0x31C63C51,			// Has Klayman the needle?
	V_HAS_FINAL_KEY			= 0xC0780812,			// Has Klayman the key from the diskplayer?
	V_HAS_TEST_TUBE			= 0x45080C38,
#if 0
Arrays:
0x0800547C		Water pipes water level (index equals pipe number; 0 to 4)
0x0090EA95		Has Klayman the key (index equals the key number; 0 to 2)
0x08D0AB11		Has Klayman inserted the key (index equals the key number; 0 to 2)
#endif
	V_END_	
};

struct GameVar {
	uint32 nameHash;
	uint32 value;
	int16 firstIndex, nextIndex;
};

class GameVars {
public:
	GameVars();
	~GameVars();
	// TODO void load(???);
	// TODO void save(???);
	uint32 getGlobalVar(uint32 nameHash);
	void setGlobalVar(uint32 nameHash, uint32 value);
	uint32 getSubVar(uint32 nameHash, uint32 subNameHash);
	void setSubVar(uint32 nameHash, uint32 subNameHash, uint32 value);
protected:
	Common::Array<GameVar> _vars;
	int16 addVar(uint32 nameHash, uint32 value);
	int16 findSubVarIndex(int16 varIndex, uint32 subNameHash);
	int16 addSubVar(int16 varIndex, uint32 subNameHash, uint32 value);
	int16 getSubVarIndex(int16 varIndex, uint32 subNameHash);
};

} // End of namespace Neverhood

#endif /* NEVERHOOD_GAMEVARS_H */
