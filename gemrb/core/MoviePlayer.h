/* GemRB - Infinity Engine Emulator
 * Copyright (C) 2003 The GemRB Project
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
 *
 */

/**
 * @file MoviePlayer.h
 * Declares MoviePlayer class, abstract loader and player for videos
 * @author The GemRB Project
 */


#ifndef MOVIEPLAYER_H
#define MOVIEPLAYER_H

#include "globals.h"

#include "Resource.h"

#include "GUI/TextSystem/Font.h"
#include "GUI/Window.h"
#include "System/String.h"
#include "Video/Video.h"

namespace GemRB {

/**
 * @class MoviePlayer
 * Abstract loader and player for videos
 */

class GEM_EXPORT MoviePlayer : public Resource {
public:
	static const TypeID ID;

	class SubtitleSet {
		Color col;
		Font* font;
	
	public:
		explicit SubtitleSet(Font* fnt, Color col = ColorWhite)
		: col(col) {
			font = fnt;
			assert(font);
		}
		
		virtual ~SubtitleSet() = default;

		virtual size_t NextSubtitleFrame() const = 0;
		virtual const String& SubtitleAtFrame(size_t) const = 0;
		
		void RenderInBuffer(VideoBuffer& buffer, size_t frame) const {
			if (frame >= NextSubtitleFrame()) {
				buffer.Clear();
				const String& str = SubtitleAtFrame(frame);
				Region rect(Point(), buffer.Size());
				font->Print(rect, str, IE_FONT_ALIGN_CENTER|IE_FONT_ALIGN_MIDDLE, {col, ColorBlack});
			}
		}
	};

private:
	bool isPlaying;
	bool showSubtitles;
	SubtitleSet* subtitles;

protected:
	// NOTE: make sure any new movie plugins set these!
	Video::BufferFormat movieFormat;
	Size movieSize;
	size_t framePos;

	long timer_last_sec = 0;
	long timer_last_usec = 0;
	unsigned int frame_wait = 0;
	unsigned int video_frameskip = 0;
	unsigned int video_skippedframes = 0;

protected:
	void DisplaySubtitle(const String& sub);
	void PresentMovie(const Region&, Video::BufferFormat fmt);

	void get_current_time(long &sec, long &usec) const;
	void timer_start();
	void timer_wait(unsigned int frame_wait);

	virtual bool DecodeFrame(VideoBuffer&) = 0;

public:
	MoviePlayer(void);
	~MoviePlayer(void) override;

	Size Dimensions() const { return movieSize; }
	void Play(Window* win);
	void Stop();

	void SetSubtitles(SubtitleSet* subs);
	void EnableSubtitles(bool set);
	bool SubtitlesEnabled() const;
};

class MoviePlayerControls : public View {
	MoviePlayer& player;

private:
	// currently dont have any real controls
	void DrawSelf(Region /*drawFrame*/, const Region& /*clip*/) override {}
	
	bool OnKeyPress(const KeyboardEvent& Key, unsigned short /*Mod*/) override {
		KeyboardKey keycode = Key.keycode;
		switch (keycode) {
			case 's':
				player.EnableSubtitles(!player.SubtitlesEnabled());
				break;
			default:
				player.Stop();
				break;
		}

		return true;
	}
	
	bool OnMouseDown(const MouseEvent& /*me*/, unsigned short /*Mod*/) override {
		player.Stop();
		return true;
	}

public:
	explicit MoviePlayerControls(MoviePlayer& player)
	: View(Region(Point(), player.Dimensions())), player(player) {}
};

}

#endif
