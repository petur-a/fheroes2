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
#ifndef H2SPELL_H
#define H2SPELL_H

#include <string>
#include "gamedefs.h"
#include "icn.h"
#include "m82.h"

namespace Spell
{
    typedef enum
    {
	NONE,
	FIREBALL,
	FIREBLAST,
	LIGHTNINGBOLT,
	CHAINLIGHTNING,
	TELEPORT,
	CURE,
	MASSCURE,
	RESURRECT,
	RESURRECTTRUE,
	HASTE,
	MASSHASTE,
	SLOW,
	MASSSLOW,
	BLIND,
	BLESS,
	MASSBLESS,
	STONESKIN,
	STEELSKIN,
	CURSE,
	MASSCURSE,
	HOLYWORD,
	HOLYSHOUT,
	ANTIMAGIC,
	DISPEL,
	MASSDISPEL,
	ARROW,
	BERZERKER,
	ARMAGEDDON,
	ELEMENTALSTORM,
	METEORSHOWER,
	PARALYZE,
	HYPNOTIZE,
	COLDRAY,
	COLDRING,
	DISRUPTINGRAY,
	DEATHRIPPLE,
	DEATHWAVE,
	DRAGONSLAYER,
	BLOODLUST,
	ANIMATEDEAD,
	MIRRORIMAGE,
	SHIELD,
	MASSSHIELD,
	SUMMONEELEMENT,
	SUMMONAELEMENT,
	SUMMONFELEMENT,
	SUMMONWELEMENT,
	EARTHQUAKE,
	VIEWMINES,
	VIEWRESOURCES,
	VIEWARTIFACTS,
	VIEWTOWNS,
	VIEWHEROES,
	VIEWALL,
	IDENTIFYHERO,
	SUMMONBOAT,
	DIMENSIONDOOR,
	TOWNGATE,
	TOWNPORTAL,
	VISIONS,
	HAUNT,
	SETEGUARDIAN,
	SETAGUARDIAN,
	SETFGUARDIAN,
	SETWGUARDIAN,

	STONE,
    } spell_t;

    inline spell_t & operator++ (spell_t & spell){ return spell = (SETWGUARDIAN > spell ? spell_t(spell + 1) : SETWGUARDIAN); };
    inline spell_t & operator-- (spell_t & spell){ return spell = (NONE < spell ? spell_t(spell - 1) : NONE); };

    typedef enum
    {
	NOTARGET,
	ONEFRIEND,
	ONEENEMY,
	ALLFRIEND,
	ALLENEMY,
	ALLLIVE,
	ALLDEAD,
	ALL,
	//ANYCELL,
	FREECELL
    } target_t;

    spell_t Spell(u8);

    const std::string &String(spell_t spell);
    u8 Mana(spell_t spell);
    u8 Level(u8);
    bool isCombat(u8);
    bool isBad(u8);
    u16 GetInflictDamage(u8, u8);

    /* return index sprite spells.icn */
    u8 GetIndexSprite(u8);
    /* return index in spellinl.icn */
    u8 GetInlIndexSprite(u8);

    target_t Target(spell_t spell);
    u8 Power(spell_t spell);
    ICN::icn_t Icn(spell_t spell);
    M82::m82_t M82(spell_t spell);
    const std::string & Description(spell_t spell);

    spell_t RandCombat(const u8 lvl);
    spell_t RandAdventure(const u8 lvl);
};

#endif
