/***************************************************************************
 *   Copyright (C) 2006 by Andrey Afletdinov                               *
 *   afletdinov@mail.dc.baikal.ru                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "agg.h"
#include "error.h"
#include "settings.h"
#include "sprite.h"

/* ICN Sprite constructor */
Sprite::Sprite(const ICN::Header & header, const char *data, const u32 size, const u8 modify)
    : Surface(header.Width(), header.Height(), Settings::Get().Shadow()), offsetX(header.OffsetX()), offsetY(header.OffsetY())
{
    SetColorKey();
    DrawICN(*this, data, size, modify);
    if(Settings::Get().Shadow()) SetDisplayFormat();
}

void Sprite::DrawICN(Surface & sf, const char *buf, const u32 size, const u8 flags)
{
    if(NULL == buf || 0 == size) return;

    const u8 *cur = (const u8 *) buf;
    const u8 *max = cur + size;
    const bool reflect = flags & REFLECT;

    u8  c = 0;
    u16 x = reflect ? sf.w() - 1 : 0;
    u16 y = 0;
    const u32 shadow = sf.alpha() ? sf.MapRGB(0, 0, 0, 0x40) : DEFAULT_COLOR_KEY16;

    // lock surface
    sf.Lock();

    while(1)
    {
	// 0x00 - end line
	if(0 == *cur)
	{
	    ++y;
	    x = reflect ? sf.w() - 1 : 0;
	    ++cur;
	}
	else
	// 0x7F - count data
	if(0x80 > *cur)
	{
	    c = *cur;
	    ++cur;
	    while(c-- && cur < max)
	    {
		sf.SetPixel2(x, y, AGG::GetColor(*cur, flags));
		reflect ? x-- : x++;
		++cur;
	    }
	}
	else
	// 0x80 - end data
	if(0x80 == *cur)
	{
	    break;
	}
	else
	// 0xBF - skip data
	if(0xC0 > *cur)
	{
	    reflect ? x -= *cur - 0x80 : x += *cur - 0x80;
	    ++cur;
	}
	else
	// 0xC0 - shadow
	if(0xC0 == *cur)
	{
	    ++cur;
	    c = *cur % 4 ? *cur % 4 : *(++cur);
	    while(c--){ sf.SetPixel2(x, y, shadow); reflect ? x-- : x++; }
	    ++cur;
	}
	else
	// 0xC1
	if(0xC1 == *cur)
	{
	    ++cur;
	    c = *cur;
	    ++cur;
	    while(c--){ sf.SetPixel2(x, y, AGG::GetColor(*cur, flags)); reflect ? x-- : x++; }
	    ++cur;
	}
	else
	{
	    c = *cur - 0xC0;
	    ++cur;
	    while(c--){ sf.SetPixel2(x, y, AGG::GetColor(*cur, flags)); reflect ? x-- : x++; }
	    ++cur;
	}

	if(cur >= max)
	{
	    if(Settings::Get().Debug()) Error::Warning("Sprite: index out of range");
	    break;
	}
    }

    // unlock surface
    sf.Unlock();
}
