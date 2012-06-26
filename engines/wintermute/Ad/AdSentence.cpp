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

#include "engines/wintermute/dcgf.h"
#include "engines/wintermute/Ad/AdSentence.h"
#include "engines/wintermute/Ad/AdTalkDef.h"
#include "engines/wintermute/Ad/AdTalkNode.h"
#include "engines/wintermute/Ad/AdGame.h"
#include "engines/wintermute/utils/PathUtil.h"
#include "engines/wintermute/Base/BGame.h"
#include "engines/wintermute/Base/BSound.h"
#include "engines/wintermute/Ad/AdScene.h"
#include "engines/wintermute/Base/BFont.h"
#include "engines/wintermute/Base/BSprite.h"
#include "engines/wintermute/Base/BFileManager.h"

namespace WinterMute {

IMPLEMENT_PERSISTENT(CAdSentence, false)

//////////////////////////////////////////////////////////////////////////
CAdSentence::CAdSentence(CBGame *inGame): CBBase(inGame) {
	_text = NULL;
	_stances = NULL;
	_tempStance = NULL;

	_duration = 0;
	_startTime = 0;
	_currentStance = 0;

	_font = NULL;

	_pos.x = _pos.y = 0;
	_width = Game->_renderer->_width;

	_align = (TTextAlign)TAL_CENTER;

	_sound = NULL;
	_soundStarted = false;

	_talkDef = NULL;
	_currentSprite = NULL;
	_currentSkelAnim = NULL;
	_fixedPos = false;
	_freezable = true;
}


//////////////////////////////////////////////////////////////////////////
CAdSentence::~CAdSentence() {
	delete _sound;
	delete[] _text;
	delete[] _stances;
	delete[] _tempStance;
	delete _talkDef;
	_sound = NULL;
	_text = NULL;
	_stances = NULL;
	_tempStance = NULL;
	_talkDef = NULL;

	_currentSprite = NULL; // ref only
	_currentSkelAnim = NULL;
	_font = NULL; // ref only
}


//////////////////////////////////////////////////////////////////////////
void CAdSentence::setText(const char *text) {
	if (_text) delete [] _text;
	_text = new char[strlen(text) + 1];
	if (_text) strcpy(_text, text);
}


//////////////////////////////////////////////////////////////////////////
void CAdSentence::setStances(const char *stances) {
	if (_stances) delete [] _stances;
	if (stances) {
		_stances = new char[strlen(stances) + 1];
		if (_stances) strcpy(_stances, stances);
	} else _stances = NULL;
}


//////////////////////////////////////////////////////////////////////////
char *CAdSentence::getCurrentStance() {
	return getStance(_currentStance);
}


//////////////////////////////////////////////////////////////////////////
char *CAdSentence::getNextStance() {
	_currentStance++;
	return getStance(_currentStance);
}


//////////////////////////////////////////////////////////////////////////
char *CAdSentence::getStance(int Stance) {
	if (_stances == NULL) return NULL;

	if (_tempStance) delete [] _tempStance;
	_tempStance = NULL;

	char *start;
	char *curr;
	int pos;

	if (Stance == 0) start = _stances;
	else {
		pos = 0;
		start = NULL;
		curr = _stances;
		while (pos < Stance) {
			if (*curr == '\0') break;
			if (*curr == ',') pos++;
			curr++;
		}
		if (pos == Stance) start = curr;
	}

	if (start == NULL) return NULL;

	while (*start == ' ' && *start != ',' && *start != '\0') start++;

	curr = start;
	while (*curr != '\0' && *curr != ',') curr++;

	while (curr > start && *(curr - 1) == ' ') curr--;

	_tempStance = new char [curr - start + 1];
	if (_tempStance) {
		_tempStance[curr - start] = '\0';
		strncpy(_tempStance, start, curr - start);
	}

	return _tempStance;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CAdSentence::display() {
	if (!_font || !_text) return E_FAIL;

	if (_sound && !_soundStarted) {
		_sound->play();
		_soundStarted = true;
	}

	if (Game->_subtitles) {
		int x = _pos.x;
		int y = _pos.y;

		if (!_fixedPos) {
			x = x - ((CAdGame *)Game)->_scene->GetOffsetLeft();
			y = y - ((CAdGame *)Game)->_scene->GetOffsetTop();
		}


		x = MAX(x, 0);
		x = MIN(x, Game->_renderer->_width - _width);
		y = MAX(y, 0);

		_font->DrawText((byte *)_text, x, y, _width, _align);
	}

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
void CAdSentence::setSound(CBSound *sound) {
	if (!sound) return;
	delete _sound;
	_sound = sound;
	_soundStarted = false;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CAdSentence::finish() {
	if (_sound) _sound->stop();
	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CAdSentence::persist(CBPersistMgr *persistMgr) {

	persistMgr->transfer(TMEMBER(Game));

	persistMgr->transfer(TMEMBER_INT(_align));
	persistMgr->transfer(TMEMBER(_currentStance));
	persistMgr->transfer(TMEMBER(_currentSprite));
	persistMgr->transfer(TMEMBER(_currentSkelAnim));
	persistMgr->transfer(TMEMBER(_duration));
	persistMgr->transfer(TMEMBER(_font));
	persistMgr->transfer(TMEMBER(_pos));
	persistMgr->transfer(TMEMBER(_sound));
	persistMgr->transfer(TMEMBER(_soundStarted));
	persistMgr->transfer(TMEMBER(_stances));
	persistMgr->transfer(TMEMBER(_startTime));
	persistMgr->transfer(TMEMBER(_talkDef));
	persistMgr->transfer(TMEMBER(_tempStance));
	persistMgr->transfer(TMEMBER(_text));
	persistMgr->transfer(TMEMBER(_width));
	persistMgr->transfer(TMEMBER(_fixedPos));
	persistMgr->transfer(TMEMBER(_freezable));

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CAdSentence::setupTalkFile(const char *soundFilename) {
	delete _talkDef;
	_talkDef = NULL;
	_currentSprite = NULL;

	if (!soundFilename) return S_OK;


	AnsiString path = PathUtil::GetDirectoryName(soundFilename);
	AnsiString name = PathUtil::GetFileNameWithoutExtension(soundFilename);

	AnsiString talkDefFileName = PathUtil::Combine(path, name + ".talk");

	Common::SeekableReadStream *file = Game->_fileManager->openFile(talkDefFileName.c_str());
	if (file) {
		Game->_fileManager->closeFile(file);
	} else return S_OK; // no talk def file found


	_talkDef = new CAdTalkDef(Game);
	if (!_talkDef || FAILED(_talkDef->loadFile(talkDefFileName.c_str()))) {
		delete _talkDef;
		_talkDef = NULL;
		return E_FAIL;
	}
	//Game->LOG(0, "Using .talk file: %s", TalkDefFile);

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CAdSentence::update(TDirection dir) {
	if (!_talkDef) return S_OK;

	uint32 CurrentTime;
	// if sound is available, synchronize with sound, otherwise use timer

	/*
	if (_sound) CurrentTime = _sound->GetPositionTime();
	else CurrentTime = Game->_timer - _startTime;
	*/
	CurrentTime = Game->_timer - _startTime;

	bool TalkNodeFound = false;
	for (int i = 0; i < _talkDef->_nodes.GetSize(); i++) {
		if (_talkDef->_nodes[i]->isInTimeInterval(CurrentTime, dir)) {
			TalkNodeFound = true;

			CBSprite *NewSprite = _talkDef->_nodes[i]->getSprite(dir);
			if (NewSprite != _currentSprite) NewSprite->Reset();
			_currentSprite = NewSprite;

			if (!_talkDef->_nodes[i]->_playToEnd) break;
		}
	}


	// no talk node, try to use default sprite instead (if any)
	if (!TalkNodeFound) {
		CBSprite *NewSprite = _talkDef->getDefaultSprite(dir);
		if (NewSprite) {
			if (NewSprite != _currentSprite) NewSprite->Reset();
			_currentSprite = NewSprite;
		} else _currentSprite = NULL;
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
bool CAdSentence::CanSkip() {
	// prevent accidental sentence skipping (TODO make configurable)
	return (Game->_timer - _startTime) > 300;
}

} // end of namespace WinterMute
