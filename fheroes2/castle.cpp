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

#include "difficulty.h"
#include "config.h"
#include "error.h"
#include "world.h"
#include "castle.h"

Castle::Castle(u32 gid, u16 mapindex, const void *ptr, bool rnd)
    : building(0), army_spread(false), army(CASTLEMAXARMY), uniq(gid), mp(mapindex % world.w(), mapindex / world.h())
{
    const u8  *byte8  = static_cast<const u8 *>(ptr);
    const u16 *byte16 = NULL;
    u16 swap16 = 0;

    switch(*byte8)
    {
	case 0x00: color = Color::BLUE;   break;
	case 0x01: color = Color::GREEN;  break;
        case 0x02: color = Color::RED;    break;
        case 0x03: color = Color::YELLOW; break;
        case 0x04: color = Color::ORANGE; break;
        case 0x05: color = Color::PURPLE; break;
        default:   color = Color::GRAY;   break;
    }
    ++byte8;
    
    // custom building
    if(*byte8)
    {
	++byte8;
	
	// building
	byte16 = reinterpret_cast<const u16 *>(byte8);
	swap16 = *byte16;
	SWAP16(swap16);
        if(0x0002 & swap16) building |= BUILD_THIEVESGUILD;
        if(0x0004 & swap16) building |= BUILD_TAVERN;
        if(0x0008 & swap16) building |= BUILD_SHIPYARD;
        if(0x0010 & swap16) building |= BUILD_WELL;
        if(0x0080 & swap16) building |= BUILD_STATUE;
        if(0x0100 & swap16) building |= BUILD_LEFTTURRET;
        if(0x0200 & swap16) building |= BUILD_RIGHTTURRET;
        if(0x0400 & swap16) building |= BUILD_MARKETPLACE;
        if(0x1000 & swap16) building |= BUILD_MOAT;
        if(0x0800 & swap16) building |= BUILD_WEL2;
        if(0x2000 & swap16) building |= BUILD_SPEC;
	++byte16;

	// dwelling
	swap16 = *byte16;
	SWAP16(swap16);
        if(0x0004 & swap16) building |= DWELLING_MONSTER1;
        if(0x0010 & swap16) building |= DWELLING_MONSTER2;
        if(0x0020 & swap16) building |= DWELLING_MONSTER3;
        if(0x0040 & swap16) building |= DWELLING_MONSTER4;
        if(0x0080 & swap16) building |= DWELLING_MONSTER5;
        if(0x0100 & swap16) building |= DWELLING_MONSTER6;
        if(0x0200 & swap16) building |= DWELLING_UPGRADE2;
        if(0x0400 & swap16) building |= DWELLING_UPGRADE3;
        if(0x0800 & swap16) building |= DWELLING_UPGRADE4;
        if(0x1000 & swap16) building |= DWELLING_UPGRADE5;
        if(0x2000 & swap16) building |= DWELLING_UPGRADE6;
	++byte16;

	
	// magic tower
	byte8 = reinterpret_cast<const u8 *>(byte16);
	if(0 < *byte8) building |= BUILD_MAGEGUILD1;
	if(1 < *byte8) building |= BUILD_MAGEGUILD2;
	if(2 < *byte8) building |= BUILD_MAGEGUILD3;
	if(3 < *byte8) building |= BUILD_MAGEGUILD4;
	if(4 < *byte8) building |= BUILD_MAGEGUILD5;
	++byte8;
    }
    else
    {
	byte8 += 6;

	// default building
        building |= BUILD_TAVERN;
        building |= DWELLING_MONSTER1;
        building |= DWELLING_MONSTER2;
	// default dwelling
	// default magic tower
    }

    // custom troops
    bool custom_troops = *byte8;
    if(custom_troops)
    {
	++byte8;
	
	// monster1
	army[0].SetMonster(Monster::Monster(*byte8));
	++byte8;

	// monster2
	army[1].SetMonster(Monster::Monster(*byte8));
	++byte8;

	// monster3
	army[2].SetMonster(Monster::Monster(*byte8));
	++byte8;

	// monster4
	army[3].SetMonster(Monster::Monster(*byte8));
	++byte8;

	// monster5
	army[4].SetMonster(Monster::Monster(*byte8));
	++byte8;

	// count1
	byte16 = reinterpret_cast<const u16 *>(byte8);
	swap16 = *byte16;
	SWAP16(swap16);
	army[0].SetCount(swap16);
	++byte16;

	// count2
	swap16 = *byte16;
	SWAP16(swap16);
	army[1].SetCount(swap16);
	++byte16;

	// count3
	swap16 = *byte16;
	SWAP16(swap16);
	army[2].SetCount(swap16);
	++byte16;

	// count4
	swap16 = *byte16;
	SWAP16(swap16);
	army[3].SetCount(swap16);
	++byte16;

	// count5
	swap16 = *byte16;
	SWAP16(swap16);
	army[4].SetCount(swap16);
	++byte16;

	byte8 = reinterpret_cast<const u8 *>(byte16);
    }
    else
    {
	byte8 += 16;
    }
    
    // captain
    captain = (*byte8 ? true : false);
    ++byte8;
    
    // custom name
    ++byte8;
    name = std::string(reinterpret_cast<const char *>(byte8));
    byte8 += 13;

    // race
    switch(*byte8)
    {
	case 0x00: race = Race::KNGT; break;
	case 0x01: race = Race::BARB; break;
	case 0x02: race = Race::SORC; break;
	case 0x03: race = Race::WRLK; break;
	case 0x04: race = Race::WZRD; break;
	case 0x05: race = Race::NECR; break;
	default: race = (Color::GRAY != color ? H2Config::GetKingdomRace(color) : Race::Rand()); break;
    }
    ++byte8;

    // castle
    if(*byte8) building |= BUILD_CASTLE;
    ++byte8;

    // allow upgrade to castle (0 - true, 1 - false)
    allow_castle = (*byte8 ? false : true);
    ++byte8;

    // unknown 29 byte
    //


    // troops auto pack
    if(!custom_troops)
    {
	const Monster::stats_t mon1 = Monster::GetStats(Monster::Monster(race, Castle::DWELLING_MONSTER1));
	const Monster::stats_t mon2 = Monster::GetStats(Monster::Monster(race, Castle::DWELLING_MONSTER2));

	switch(H2Config::GetGameDifficulty())
	{
    	    case Difficulty::EASY:
        	army[0].SetMonster(mon1.monster);
        	army[0].SetCount(mon1.grown * 2);
        	army[1].SetMonster(mon2.monster);
        	army[1].SetCount(mon2.grown * 2);
        	break;

    	    case Difficulty::NORMAL:
        	army[0].SetMonster(mon1.monster);
        	army[0].SetCount(mon1.grown);
        	break;

    	    case Difficulty::HARD:
    	    case Difficulty::EXPERT:
    	    case Difficulty::IMPOSSIBLE:
        	break;
	}
    }

    // modify RND sprites
    if(rnd) CorrectAreaMaps();

    // minimize area maps id
    MinimizeAreaMapsID();
    
    // end
    Error::Verbose((building & BUILD_CASTLE ? "add castle: " : "add town: ") + name + ", color: " + Color::String(color) + ", race: " + Race::String(race));
}

const Heroes * Castle::isHeroesPresent(void)
{
    return world.GetHeroes(mp.x, mp.y);
}

void Castle::ActionNewDay(void)
{
}

void Castle::ActionNewWeek(void)
{
}

void Castle::ActionNewMonth(void)
{
}

// change castle color
void Castle::ChangeColor(Color::color_t cl)
{
    color = cl;

    // modify left flag
    ModifyTIlesFlags(world.GetTiles(mp.x - 1, mp.y));

    // modify right flag
    ModifyTIlesFlags(world.GetTiles(mp.x + 1, mp.y));
}

/* correct sprites for RND castles */
void Castle::CorrectAreaMaps(void)
{
/* 
castle size: T and B - sprite, S - shadow, XX - center

              T0
      S1S1T1T1T1T1T1
    S2S2S2T2T2T2T2T2
      S3S3B1B1XXB1B1
        S4B2B2  B2B2
*/
    std::vector<u16> coords;

    // T0
    if(isCastle()) coords.push_back((mp.y - 3) * world.h() + mp.x);
    // T1
    coords.push_back((mp.y - 2) * world.h() + mp.x - 2);
    coords.push_back((mp.y - 2) * world.h() + mp.x - 1);
    coords.push_back((mp.y - 2) * world.h() + mp.x);
    coords.push_back((mp.y - 2) * world.h() + mp.x + 1);
    coords.push_back((mp.y - 2) * world.h() + mp.x + 2);
    // T2
    coords.push_back((mp.y - 1) * world.h() + mp.x - 2);
    coords.push_back((mp.y - 1) * world.h() + mp.x - 1);
    coords.push_back((mp.y - 1) * world.h() + mp.x);
    coords.push_back((mp.y - 1) * world.h() + mp.x + 1);
    coords.push_back((mp.y - 1) * world.h() + mp.x + 2);
    // B1
    coords.push_back(mp.y * world.h() + mp.x - 2);
    coords.push_back(mp.y * world.h() + mp.x - 1);
    coords.push_back(mp.y * world.h() + mp.x);
    coords.push_back(mp.y * world.h() + mp.x + 1);
    coords.push_back(mp.y * world.h() + mp.x + 2);
    // B2
    coords.push_back((mp.y + 1) * world.h() + mp.x - 2);
    coords.push_back((mp.y + 1) * world.h() + mp.x - 1);
    coords.push_back((mp.y + 1) * world.h() + mp.x);
    coords.push_back((mp.y + 1) * world.h() + mp.x + 1);
    coords.push_back((mp.y + 1) * world.h() + mp.x + 2);

    const u16 index_center = mp.y * world.h() + mp.x;
    
    Maps::Tiles & tile_center = world.GetTiles(index_center);

    // correct only RND town and castle
    switch(tile_center.GetObject())
    {
	case MP2::OBJ_RNDTOWN:
	case MP2::OBJ_RNDCASTLE:
	    break;
	
	default:
	    Error::Warning("Castle::CorrectAreaMaps: correct only RND town and castle. index: ", index_center);
	    return;
    }

    // modify all rnd sprites
    std::vector<u16>::const_iterator itc = coords.begin();
    for(; itc != coords.end(); ++itc) ModifyTIlesRNDSprite(world.GetTiles(*itc));
}

void Castle::MinimizeAreaMapsID(void)
{

    // reset castle ID
    for(s8 yy = -3; yy < 2; ++yy)
	for(s8 xx = -2; xx < 3; ++xx)
    {
	Maps::Tiles & tile = world.GetTiles((mp.y + yy) * world.h() + mp.x + xx);

	if(MP2::OBJN_RNDCASTLE == tile.GetObject() ||
     	     MP2::OBJN_RNDTOWN == tile.GetObject() ||
	     MP2::OBJN_CASTLE  == tile.GetObject()) tile.SetObject(MP2::OBJ_ZERO);
    }

    // set minimum area castle ID
    for(s8 yy = -1; yy < 1; ++yy)
	for(s8 xx = -1; xx < 2; ++xx)
    {
	Maps::Tiles & tile = world.GetTiles((mp.y + yy) * world.h() + mp.x + xx);

	tile.SetObject(MP2::OBJN_CASTLE);
    }

    // restore center ID
    Maps::Tiles & tile_center = world.GetTiles(mp.y * world.h() + mp.x);

    tile_center.SetObject(MP2::OBJ_CASTLE);
}

/* modify RND sprites alghoritm */
void Castle::ModifyTIlesRNDSprite(Maps::Tiles & tile)
{
    const Maps::TilesAddon *addon = NULL;

    if( (addon = tile.FindAddon(0x98, 0, 31)) || (addon = tile.FindAddon(0x99, 0, 31)) ||
	(addon = tile.FindAddon(0x9A, 0, 31)) || (addon = tile.FindAddon(0x9B, 0, 31)))
	{
            Maps::TilesAddon & addon_w = *const_cast<Maps::TilesAddon *>(addon);

            addon_w.SetObject((*addon).GetObject() - 12);

            switch(race)
            {
        	case Race::KNGT: break; //addon_w.SetIndex((*addon).GetIndex()); break;
                case Race::BARB: addon_w.SetIndex((*addon).GetIndex() + 32); break;
                case Race::SORC: addon_w.SetIndex((*addon).GetIndex() + 64); break;
                case Race::WRLK: addon_w.SetIndex((*addon).GetIndex() + 96); break;
                case Race::WZRD: addon_w.SetIndex((*addon).GetIndex() + 128); break;
                case Race::NECR: addon_w.SetIndex((*addon).GetIndex() + 160); break;
                default: Error::Warning("Castle::ModifyTIlesRNDSprite: unknown race."); break;
	    }
	}
}

/* modify flags sprite to origin color */
void Castle::ModifyTIlesFlags(Maps::Tiles & tile)
{
    const Maps::TilesAddon *addon = tile.FindAddon(0x38);

    if(addon)
    {
        Maps::TilesAddon & addon_w = *const_cast<Maps::TilesAddon *>(addon);

	// right flag - event index
	bool right_event = (*addon).GetIndex() % 2 ? true : false;

	u8 index_sprite = 0;

	switch(color)
	{
	    case Color::BLUE:	index_sprite = right_event ? 0 : 1; break;
	    case Color::GREEN:	index_sprite = right_event ? 2 : 3; break;
	    case Color::RED:	index_sprite = right_event ? 4 : 5; break;
	    case Color::YELLOW:	index_sprite = right_event ? 6 : 7; break;
	    case Color::ORANGE:	index_sprite = right_event ? 8 : 9; break;
	    case Color::PURPLE:	index_sprite = right_event ? 10 : 11; break;
	    case Color::GRAY:	index_sprite = right_event ? 12 : 13; break;
	    default: Error::Warning("Castle::ModifyTIlesFlags: unknown color."); break;
	}

	addon_w.SetIndex(index_sprite);
    }
}

// modify Town sprite to Castle
void Castle::ModifyTilesTownToCastle(Maps::Tiles & tile)
{
    const Maps::TilesAddon *addon = NULL;

    if( (addon = tile.FindAddon(0x8C)) || (addon = tile.FindAddon(0x8D)) ||
	(addon = tile.FindAddon(0x8E)) || (addon = tile.FindAddon(0x8F)))
	{
            Maps::TilesAddon & addon_w = *const_cast<Maps::TilesAddon *>(addon);

            addon_w.SetIndex((*addon).GetIndex() - 16);
	}
}

// return valid count army in castle
u8 Castle::GetCountArmy(void) const
{
    u8 result = 0;

    for(u8 ii = 0; ii < CASTLEMAXARMY; ++ii) if(army[ii].Valid()) ++result;
    
    return result;
}

// return mage guild level
u8 Castle::GetLevelMageGuild(void)
{
    if(BUILD_MAGEGUILD5 & building)	return 5;
    else
    if(BUILD_MAGEGUILD4 & building)	return 4;
    else
    if(BUILD_MAGEGUILD3 & building)	return 3;
    else
    if(BUILD_MAGEGUILD2 & building)	return 2;
    else
    if(BUILD_MAGEGUILD1 & building)	return 1;

    return 0;
}

const std::string & Castle::GetStringBuilding(const building_t & build, const Race::race_t & race)
{
    static const std::string str_build[] = { "Thieves' Guild", "Tavern", "Shipyard", "Well", "Statue", "Left Turret",
	"Right Turret", "Marketplace", "Moat", "Castle", "Tent", "Captain", "Mage Guild", "Unknown" };

    static const std::string str_wel2[] = { "Farm", "Garbage Heap", "Crystal Garden", "Orchard", "Waterfall", "Skull Pile" };

    static const std::string str_spec[] = { "Fortifications", "Coliseum", "Rainbow", "Library", "Dungeon", "Storm" };

    static const std::string str_dwelling[] = {
	"Thatched Hut", "Hut", "Treehouse", "Cave", "Habitat", "Excavation",
	"Archery Range", "Stick Hut", "Cottage", "Crypt", "Pen", "Graveyard",
	"Blacksmith", "Den", "Archery Range", "Nest", "Foundry", "Pyramid",
	"Armory", "Adobe", "Stonehenge", "Maze", "Cliff Nest", "Mansion",
	"Jousting Arena", "Bridge", "Fenced Meadow", "Swamp", "Ivory Tower", "Mausoleum",
	"Cathedral", "Pyramid", "Red Tower", "Green Tower", "Cloud Castle", "Laboratory" };

    static const std::string str_upgrade[] = {
	"Upg. Archery Range", "Upg. Stick Hut", "Upg. Cottage", "Crypt", "Pen", "Upg. Graveyard",
	"Upg. Blacksmith", "Den", "Upg. Archery Range", "Nest", "Upg. Foundry", "Upg. Pyramid",
	"Upg. Armory", "Upg. Adobe", "Upg. Stonehenge", "Upg. Maze", "Cliff Nest", "Upg. Mansion",
	"Upg. Jousting Arena", "Upg. Bridge", "Fenced Meadow", "Swamp", "Upg. Ivory Tower", "Upg. Mausoleum",
	"Upg. Cathedral", "Pyramid", "Red Tower", "Red Tower", "Upg. Cloud Castle", "Laboratory",
	"Upg. Cathedral", "Pyramid", "Red Tower", "Black Tower", "Upg. Cloud Castle", "Laboratory" };

    u8 offset = 0;

    switch(race)
    {
	case Race::KNGT: offset = 0; break;
	case Race::BARB: offset = 1; break;
	case Race::SORC: offset = 2; break;
	case Race::WRLK: offset = 3; break;
	case Race::WZRD: offset = 4; break;
	case Race::NECR: offset = 5; break;
	default: break;
    }

    switch(build)
    {
        case BUILD_THIEVESGUILD:return str_build[0];
        case BUILD_TAVERN:	return str_build[1];
        case BUILD_SHIPYARD:	return str_build[2];
        case BUILD_WELL:	return str_build[3];
        case BUILD_STATUE:	return str_build[4];
        case BUILD_LEFTTURRET:	return str_build[5];
        case BUILD_RIGHTTURRET:	return str_build[6];
        case BUILD_MARKETPLACE:	return str_build[7];
        case BUILD_MOAT:	return str_build[8];
        case BUILD_CASTLE:	return str_build[9];
        case BUILD_TENT:	return str_build[10];
        case BUILD_CAPTAIN:	return str_build[11];
        case BUILD_MAGEGUILD1:
        case BUILD_MAGEGUILD2:
        case BUILD_MAGEGUILD3:
        case BUILD_MAGEGUILD4:
        case BUILD_MAGEGUILD5:	return str_build[12];

        case BUILD_SPEC:	return str_spec[offset];
        case BUILD_WEL2:	return str_wel2[offset];

        case DWELLING_MONSTER1:	return str_dwelling[offset];
        case DWELLING_MONSTER2:	return str_dwelling[6 + offset];
        case DWELLING_MONSTER3:	return str_dwelling[12 + offset];
        case DWELLING_MONSTER4:	return str_dwelling[18 + offset];
        case DWELLING_MONSTER5:	return str_dwelling[24 + offset];
        case DWELLING_MONSTER6:	return str_dwelling[30 + offset];

        case DWELLING_UPGRADE2: return str_upgrade[offset];
        case DWELLING_UPGRADE3: return str_upgrade[6 + offset];
        case DWELLING_UPGRADE4: return str_upgrade[12 + offset];
        case DWELLING_UPGRADE5: return str_upgrade[18 + offset];
        case DWELLING_UPGRADE6: return str_upgrade[24 + offset];
        case DWELLING_UPGRADE7: return str_upgrade[30 + offset];

	default: break;
    }
    
    return str_build[13];
}

const std::string & Castle::GetDescriptionBuilding(const building_t & build, const Race::race_t & race)
{
    static const std::string desc_build[] = {
	"The Thieves' Guild provides information on enemy players. Thieves' Guilds can also provide scouting information on enemy towns.",
	"The Tavern increases morale for troops defending the castle.",
	"The Shipyard allows ships to be built.",
	"The Well increases the growth rate of all dwellings by 2 creatures per week.",
	"The Statue increases your town's income by 250 per day.",
	"The Left Turret provides extra firepower during castle combat.",
	"The Right Turret provides extra firepower during castle combat.",
	"The Marketplace can be used to convert one type of resource into another. The more marketplaces you control, the better the exchange rate.",
	"The Moat slows attacking units. Any unit entering the moat must end its turn there and becomes more vulnerable to attack.",
	"The Castle improves town defense and increases income to 1000 gold per day.",
	"The Tent provides workers to build a castle, provided the materials and the gold are available.",
	"The Captain's Quarters provides a captain to assist in the castle's defense when no hero is present.",
	"The Mage Guild allows heroes to learn spells and replenish their spell points.", "Unknown" };

    static const std::string desc_wel2[] = {
	"The Farm increases production of Peasants by 8 per week.",
	"The Garbage Heap increases production of Goblins by 8 per week.",
	"The Crystal Garden increases production of Sprites by 8 per week.",
	"The Waterfall increases production of Centaurs by 8 per week.",
	"The Orchard increases production of Halflings by 8 per week.",
	"The Skull Pile increases production of Skeletons by 8 per week." };

    static const std::string desc_spec[] = {
	"The Fortifications increase the toughness of the walls, increasing the number of turns it takes to knock them down.",
	"The Coliseum provides inspiring spectacles to defending troops, raising their morale by two during combat.",
	"The Rainbow increases the luck of the defending units by two.",
	"The Dungeon increases the income of the town by 500 / day.",
	"The Library increases the number of spells in the Guild by one for each level of the guild.",
	"The Storm adds +2 to the power of spells of a defending spell caster." };

    u8 offset = 0;

    switch(race)
    {
	case Race::KNGT: offset = 0; break;
	case Race::BARB: offset = 1; break;
	case Race::SORC: offset = 2; break;
	case Race::WRLK: offset = 3; break;
	case Race::WZRD: offset = 4; break;
	case Race::NECR: offset = 5; break;
	default: break;
    }

    switch(build)
    {
        case BUILD_THIEVESGUILD:return desc_build[0];
        case BUILD_TAVERN:	return desc_build[1];
        case BUILD_SHIPYARD:	return desc_build[2];
        case BUILD_WELL:	return desc_build[3];
        case BUILD_STATUE:	return desc_build[4];
        case BUILD_LEFTTURRET:	return desc_build[5];
        case BUILD_RIGHTTURRET:	return desc_build[6];
        case BUILD_MARKETPLACE:	return desc_build[7];
        case BUILD_MOAT:	return desc_build[8];
        case BUILD_CASTLE:	return desc_build[9];
        case BUILD_TENT:	return desc_build[10];
        case BUILD_CAPTAIN:	return desc_build[11];
        case BUILD_MAGEGUILD1:
        case BUILD_MAGEGUILD2:
        case BUILD_MAGEGUILD3:
        case BUILD_MAGEGUILD4:
        case BUILD_MAGEGUILD5:	return desc_build[12];

        case BUILD_SPEC:	return desc_spec[offset];
        case BUILD_WEL2:	return desc_wel2[offset];

	default: break;
    }

    return desc_build[13];
}
