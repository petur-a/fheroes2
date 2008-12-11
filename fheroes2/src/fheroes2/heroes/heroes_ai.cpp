/***************************************************************************
 *   Copyright (C) 2008 by Andrey Afletdinov                               *
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

#include "settings.h"
#include "kingdom.h"
#include "world.h"
#include "heroes.h"

void AIToPickupResource(Heroes &hero, const u16 dst_index);
void AIToTreasureChest(Heroes &hero, const u16 dst_index);
void AIToResource(Heroes &hero, const u16 dst_index);
void AIToWagon(Heroes &hero, const u16 dst_index);
void AIToSkeleton(Heroes &hero, const u16 dst_index);
void AIToCaptureObject(Heroes &hero, const u16 dst_index);
void AIToFlotSam(Heroes &hero, const u16 dst_index);

void Heroes::AIUpdateRoles(void)
{
    SetModes(SCOUTER);
    SetModes(HUNTER);
}

void Heroes::AIAction(const u16 dst_index)
{
    const MP2::object_t & object = world.GetTiles(dst_index).GetObject();

    switch(object)
    {
        // pickup object
        case MP2::OBJ_RESOURCE:
        case MP2::OBJ_BOTTLE:
        case MP2::OBJ_CAMPFIRE:         AIToPickupResource(*this, dst_index); break;

        case MP2::OBJ_WATERCHEST:
        case MP2::OBJ_TREASURECHEST:	AIToTreasureChest(*this, dst_index); break;

        case MP2::OBJ_MAGICGARDEN:
        case MP2::OBJ_LEANTO:
        case MP2::OBJ_WINDMILL:
        case MP2::OBJ_WATERWHEEL:	AIToResource(*this, dst_index); break;

        case MP2::OBJ_WAGON:		AIToWagon(*this, dst_index); break;
        case MP2::OBJ_SKELETON:		AIToSkeleton(*this, dst_index); break;
        case MP2::OBJ_FLOTSAM:		AIToFlotSam(*this, dst_index); break;

	case MP2::OBJ_ALCHEMYLAB:
        case MP2::OBJ_MINES:
	case MP2::OBJ_SAWMILL:
        case MP2::OBJ_LIGHTHOUSE:	AIToCaptureObject(*this, dst_index); break;

	default:
	    Error::Verbose("AI::Action: Hero " + GetName() + " say: I'm stupid, help my please..");
	    break;
    }
}

void AIToPickupResource(Heroes &hero, const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);
    Resource::funds_t resource;
    const u8 count = tile.GetQuantity2();

    switch(tile.GetQuantity1())
    {
        case Resource::WOOD: resource.wood += count; break;
        case Resource::MERCURY: resource.mercury += count; break;
        case Resource::ORE: resource.ore += count; break;
        case Resource::SULFUR: resource.sulfur += count; break;
        case Resource::CRYSTAL: resource.crystal += count; break;
        case Resource::GEMS: resource.gems += count; break;
        case Resource::GOLD: resource.gold += 100 * count; break;

        default: break;
    }

    world.GetKingdom(hero.GetColor()).AddFundsResource(resource);
    tile.RemoveObjectSprite();

    tile.SetObject(MP2::OBJ_ZERO);
    if(Settings::Get().Debug()) Error::Verbose("AIToPickupResource: " + hero.GetName() + " pickup small resource");
}

void AIToTreasureChest(Heroes &hero, const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);
    Resource::funds_t resource;
    resource.gold = tile.GetQuantity2() * 100;

    if(Maps::Ground::WATER == tile.GetGround())
    {
	if(tile.GetQuantity1())
	{
	    const Artifact::artifact_t art = Artifact::Artifact(tile.GetQuantity1());
	    if(!hero.PickupArtifact(art))
		    resource.gold = 1500;	// it is from FAQ
	}
	world.GetKingdom(hero.GetColor()).AddFundsResource(resource);
    }
    else
    {
	if(tile.GetQuantity1())
	{
	    const Artifact::artifact_t art = Artifact::Artifact(tile.GetQuantity1());
	
	    if(!hero.PickupArtifact(art))
		    resource.gold = 1000;	// it is from FAQ
	    world.GetKingdom(hero.GetColor()).AddFundsResource(resource);
	}
	else
	{
	    const u16 expr = resource.gold - 500;

	    // select gold or exp
	    if(Rand::Get(1))
		world.GetKingdom(hero.GetColor()).AddFundsResource(resource);
	    else
		hero.IncreaseExperience(expr);
	}
    }

    tile.RemoveObjectSprite();
    tile.SetObject(MP2::OBJ_ZERO);

    if(Settings::Get().Debug()) Error::Verbose("AIToTreasureChest: " + hero.GetName() + " pickup chest");
}

void AIToResource(Heroes &hero, const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);
    const u8 count = tile.GetQuantity2();
    Resource::funds_t resource;

    switch(tile.GetQuantity1())
    {
	case Resource::WOOD: resource.wood += count; break;
    	case Resource::MERCURY: resource.mercury += count; break;
    	case Resource::ORE: resource.ore += count; break;
    	case Resource::SULFUR: resource.sulfur += count; break;
    	case Resource::CRYSTAL: resource.crystal += count; break;
    	case Resource::GEMS: resource.gems += count; break;
    	case Resource::GOLD: resource.gold += 100 * count; break;

	default: break;
    }

    if(resource.GetValidItems())
	world.GetKingdom(hero.GetColor()).AddFundsResource(resource);

    tile.SetQuantity1(0);
    tile.SetQuantity2(0);

    if(H2Config::Debug()) Error::Verbose("AIToResource: " + hero.GetName() + " pickup small resource");
}

void AIToSkeleton(Heroes &hero, const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);

    // artifact
    if(tile.GetQuantity1() && 0 == tile.GetQuantity2())
    {
	const Artifact::artifact_t art = Artifact::Artifact(tile.GetQuantity1());

	if(hero.PickupArtifact(art))
	{
	    tile.SetQuantity1(0);
	    tile.SetQuantity2(0);
	}
    }

    if(H2Config::Debug()) Error::Verbose("AIToSkeleton: " + hero.GetName());
}

void AIToWagon(Heroes &hero, const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);

    // artifact
    if(tile.GetQuantity1() && 0 == tile.GetQuantity2())
    {
	const Artifact::artifact_t art = Artifact::Artifact(tile.GetQuantity1());
	if(hero.PickupArtifact(art))
	    tile.SetQuantity1(0);
    }
    else
    if(tile.GetQuantity1() && tile.GetQuantity2())
    {
	const u8 count = tile.GetQuantity2();
	Resource::funds_t resource;

	switch(tile.GetQuantity1())
	{
	    case Resource::WOOD:	resource.wood += count; break;
    	    case Resource::MERCURY:	resource.mercury += count; break;
    	    case Resource::ORE:		resource.ore += count; break;
    	    case Resource::SULFUR:	resource.sulfur += count; break;
    	    case Resource::CRYSTAL:	resource.crystal += count; break;
    	    case Resource::GEMS:	resource.gems += count; break;
    	    case Resource::GOLD:	resource.gold += 100 * count; break;

	    default: break;
	}

	world.GetKingdom(hero.GetColor()).AddFundsResource(resource);

	tile.SetQuantity1(0);
	tile.SetQuantity2(0);
    }

    if(H2Config::Debug()) Error::Verbose("AIToWagon: " + hero.GetName());
}

void AIToCaptureObject(Heroes &hero, const u16 dst_index)
{
    const Maps::Tiles & tile = world.GetTiles(dst_index);
    const MP2::object_t obj = tile.GetObject();
    Resource::resource_t res = Resource::UNKNOWN;

    switch(obj)
    {
	case MP2::OBJ_ALCHEMYLAB:	res = Resource::MERCURY; break;
	case MP2::OBJ_SAWMILL:		res = Resource::WOOD; break;
        case MP2::OBJ_MINES:
    	{
    	    const Maps::TilesAddon * taddon = world.GetTiles(dst_index).FindMines();
            // ore
            if(0 == taddon->index)	res = Resource::ORE;
            else
            // sulfur
            if(1 == taddon->index)	res = Resource::SULFUR;
            else
            // crystal
            if(2 == taddon->index)	res = Resource::CRYSTAL;
            else
            // gems
            if(3 == taddon->index)	res = Resource::GEMS;
            else
            // gold
            if(4 == taddon->index)	res = Resource::GOLD;
    	}
    	break;
        default: break;
    }

    // capture object
    if(hero.GetColor() != world.ColorCapturedObject(dst_index))
    {
	world.CaptureObject(dst_index, hero.GetColor());
	world.GetTiles(dst_index).CaptureFlags32(obj, hero.GetColor());
    }
    if(H2Config::Debug()) Error::Verbose("AIToCaptureObject: " + hero.GetName() + " captured: " + std::string(MP2::StringObject(obj)));
}

void AIToFlotSam(Heroes &hero, const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);
    Resource::funds_t resource;

    resource.gold += 100 * tile.GetQuantity1();
    resource.wood += tile.GetQuantity2();

    if(resource.GetValidItems()) world.GetKingdom(hero.GetColor()).AddFundsResource(resource);

    tile.RemoveObjectSprite();
    tile.SetObject(MP2::OBJ_ZERO);

    if(H2Config::Debug()) Error::Verbose("AIToFlotSam: " + hero.GetName() + " pickup small resource");
}

/*
// action to next cell
void Heroes::Action(const u16 dst_index)
{
    if(H2Config::MyColor() != GetColor()) return AIAction(*this, dst_index);

    const MP2::object_t & object = world.GetTiles(dst_index).GetObject();

    switch(object)
    {
	case MP2::OBJ_MONSTER:	ActionToMonster(*this, dst_index); break;

        case MP2::OBJ_CASTLE:	ActionToCastle(*this, dst_index); break;
        case MP2::OBJ_HEROES:	ActionToHeroes(*this, dst_index); break;

        case MP2::OBJ_BOAT:	ActionToBoat(*this, dst_index); break;
	case MP2::OBJ_COAST:	ActionToCoast(*this, dst_index); break;

        // resource object

        // pickup object
        case MP2::OBJ_RESOURCE:
        case MP2::OBJ_BOTTLE:
        case MP2::OBJ_CAMPFIRE:		ActionToPickupResource(*this, dst_index); break;

        case MP2::OBJ_ANCIENTLAMP:	ActionToAncientLamp(*this, dst_index); break;

        case MP2::OBJ_SHIPWRECKSURVIROR:
        case MP2::OBJ_ARTIFACT: 	ActionToArtifact(*this, dst_index); break;

        // shrine circle
	case MP2::OBJ_SHRINE1:
	case MP2::OBJ_SHRINE2:
        case MP2::OBJ_SHRINE3:		ActionToShrine(*this, dst_index); break;

        // witchs hut
        case MP2::OBJ_WITCHSHUT: 	ActionToWitchsHut(*this, dst_index); break;

        // info message
        case MP2::OBJ_SIGN:		ActionToSign(*this, dst_index); break;

        // luck modification
        case MP2::OBJ_FOUNTAIN:
        case MP2::OBJ_FAERIERING:
        case MP2::OBJ_IDOL:		ActionToGoodLuckObject(*this, dst_index); break;

	case MP2::OBJ_PYRAMID:		ActionToPoorLuckObject(*this, dst_index); break;

        case MP2::OBJ_MAGICWELL: 	ActionToMagicWell(*this, dst_index); break;
        case MP2::OBJ_TRADINGPOST:	ActionToTradingPost(*this); break;

        // primary skill modification
        case MP2::OBJ_FORT:
        case MP2::OBJ_MERCENARYCAMP:
        case MP2::OBJ_DOCTORHUT:
        case MP2::OBJ_STANDINGSTONES:	ActionToPrimarySkillObject(*this, dst_index); break;

        // morale modification
        case MP2::OBJ_OASIS:
        case MP2::OBJ_TEMPLE:
        case MP2::OBJ_WATERINGHOLE:
        case MP2::OBJ_BUOY:		ActionToGoodMoraleObject(*this, dst_index); break;

	case MP2::OBJ_SHIPWRECK:
        case MP2::OBJ_GRAVEYARD:
	case MP2::OBJ_DERELICTSHIP:	ActionToPoorMoraleObject(*this, dst_index); break;

        // experience modification
        case MP2::OBJ_GAZEBO:		ActionToExperienceObject(*this, dst_index); break;

        // teleports
	case MP2::OBJ_STONELIGHTS:	ActionToTeleports(*this, dst_index); break;
	case MP2::OBJ_WHIRLPOOL:	ActionToWhirlpools(*this, dst_index); break;

	// obsv tower
	case MP2::OBJ_OBSERVATIONTOWER:	ActionToObservationTower(*this, dst_index); break;
        case MP2::OBJ_MAGELLANMAPS:	ActionToMagellanMaps(*this, dst_index); break;

	// capture color object

        case MP2::OBJ_ABANDONEDMINE:	ActionToAbandoneMine(*this, dst_index); break;

	// accept army
        case MP2::OBJ_WATCHTOWER:
        case MP2::OBJ_EXCAVATION:
        case MP2::OBJ_CAVE:
        case MP2::OBJ_TREEHOUSE:
	case MP2::OBJ_ARCHERHOUSE:
        case MP2::OBJ_GOBLINHUT:
        case MP2::OBJ_DWARFCOTT:
        case MP2::OBJ_HALFLINGHOLE:
	case MP2::OBJ_PEASANTHUT:
        case MP2::OBJ_THATCHEDHUT:	ActionToDwellingJoinMonster(*this, dst_index); break;

	// recruit army
        case MP2::OBJ_RUINS:
        case MP2::OBJ_TREECITY:
        case MP2::OBJ_WAGONCAMP:
	case MP2::OBJ_DESERTTENT:	ActionToDwellingRecruitMonster(*this, dst_index); break;

        // battle and recruit army
        case MP2::OBJ_DRAGONCITY:
        case MP2::OBJ_CITYDEAD:
        case MP2::OBJ_TROLLBRIDGE:	ActionToDwellingBattleMonster(*this, dst_index); break;

        case MP2::OBJ_ARTESIANSPRING:	ActionToArtesianSpring(*this, dst_index); break;

        case MP2::OBJ_XANADU:		ActionToXanadu(*this, dst_index); break;

        case MP2::OBJ_HILLFORT:
        case MP2::OBJ_FREEMANFOUNDRY:	ActionToUpgradeArmyObject(*this, dst_index); break;

	case MP2::OBJ_EVENT:		ActionToEvent(*this, dst_index); break;

        // object
        case MP2::OBJ_DAEMONCAVE:
        case MP2::OBJ_OBELISK:
	case MP2::OBJ_ORACLE:
        case MP2::OBJ_TREEKNOWLEDGE:
        case MP2::OBJ_SPHINX:

        case MP2::OBJ_JAIL:
        case MP2::OBJ_WATERALTAR:
        case MP2::OBJ_AIRALTAR:
        case MP2::OBJ_FIREALTAR:
        case MP2::OBJ_EARTHALTAR:
        case MP2::OBJ_BARROWMOUNDS:
        case MP2::OBJ_ARENA:
        case MP2::OBJ_STABLES:
        case MP2::OBJ_ALCHEMYTOWER:
        case MP2::OBJ_HUTMAGI:
        case MP2::OBJ_EYEMAGI:
        case MP2::OBJ_SIRENS:
        case MP2::OBJ_MERMAID:
	    if(H2Config::Debug()) Error::Verbose("Heroes::Action: FIXME: " + std::string(MP2::StringObject(object)));
	    break;

	default: break;
    }
}

void ActionToMonster(Heroes &hero, const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);
    const Monster::monster_t monster = Monster::Monster(tile);
    const u16 count = tile.GetCountMonster();
    Army::army_t army;
    Army::ArrangeTroopsForBattle(army, monster, count);

    if(H2Config::Debug()) Error::Verbose("ActionToMonster: " + hero.GetName() + " attack monster " + Monster::String(monster));

    const u32 exp = army.CalculateExperience();
    const Army::battle_t b = Army::Battle(hero, army, tile);

    switch(b)
    {
	case Army::WIN:
	{
	    hero.IncreaseExperience(exp);
	    Maps::TilesAddon *addon = tile.FindMonster();
	    if(addon)
	    {
		AGG::PlaySound(M82::KILLFADE);
		const u32 uniq = addon->uniq;
		AnimationRemoveObject(tile);
		tile.Remove(uniq);
		tile.SetObject(MP2::OBJ_ZERO);

		// remove shadow from left cell
		if(Maps::isValidDirection(dst_index, Direction::LEFT))
		    world.GetTiles(Maps::GetDirectionIndex(dst_index, Direction::LEFT)).Remove(uniq);
	    }
	    hero.ActionAfterBattle();
	    break;
	}

	case Army::RETREAT:
	case Army::SURRENDER:
	case Army::LOSE:
	    BattleLose(hero, b);
	    if(!Settings::Get().Original())
	    {
	        tile.SetCountMonster(army.GetCountMonsters(monster));
	    }
	    break;
        
        default: break;
    }
}

void ActionToHeroes(Heroes &hero, const u16 dst_index)
{
    const Heroes *other_hero = world.GetHeroes(dst_index);

    if(! other_hero) return;

    if(hero.GetColor() == other_hero->GetColor())
    {
	if(H2Config::Debug()) Error::Verbose("ActionToHeroes: " + hero.GetName() + " meeting " + other_hero->GetName());

	hero.MeetingDialog(const_cast<Heroes &>(*other_hero));
    }
    else
    {
	if(H2Config::Debug()) Error::Verbose("ActionToHeroes: " + hero.GetName() + " attack enemy hero " + other_hero->GetName());
	const Army::battle_t b = Army::Battle(hero, const_cast<Heroes &>(*other_hero), world.GetTiles(dst_index));
	const u32 exp = other_hero->GetArmy().CalculateExperience();

	switch(b)
	{
	    case Army::WIN:
		hero.IncreaseExperience(exp);
		AGG::PlaySound(M82::KILLFADE);
		(*other_hero).FadeOut();
    		world.GetKingdom((*other_hero).GetColor()).RemoveHeroes(other_hero);
    		const_cast<Heroes &>(*other_hero).SetFreeman(b);
		hero.TakeArtifacts(const_cast<Heroes &>(*other_hero));
		hero.ActionAfterBattle();
		break;

	    case Army::LOSE:
	    case Army::RETREAT:
	    case Army::SURRENDER:
		BattleLose(hero, b);
		break;

            default: break;
	}
    }
}

void ActionToCastle(Heroes &hero, const u16 dst_index)
{
    const Castle *castle = world.GetCastle(dst_index);

    if(! castle) return;

    if(hero.GetColor() == castle->GetColor())
    {
	if(H2Config::Debug()) Error::Verbose("ActionToCastle: " + hero.GetName() + " goto castle " + castle->GetName());

	Mixer::Reduce();

	const_cast<Castle *>(castle)->OpenDialog();

	Mixer::Enhance();
    }
    else
    {
	if(H2Config::Debug()) Error::Verbose("ActionToCastle: " + hero.GetName() + " attack enemy castle " + castle->GetName());
	const Army::battle_t b = Army::Battle(hero, const_cast<Castle &>(*castle), world.GetTiles(dst_index));
	const u32 exp = castle->GetArmy().CalculateExperience();

	switch(b)
	{
	    case Army::WIN:
		hero.IncreaseExperience(exp);
		world.GetKingdom(castle->GetColor()).RemoveCastle(castle);
		world.GetKingdom(hero.GetColor()).AddCastle(castle);
		const_cast<Castle *>(castle)->ChangeColor(hero.GetColor());
		Game::SelectBarCastle::Get().Redraw();
		hero.ActionAfterBattle();
		break;

	    case Army::LOSE:
	    case Army::RETREAT:
	    case Army::SURRENDER:
		BattleLose(hero, b);
		break;

            default: break;
	}
    }
}

void ActionToBoat(Heroes &hero, const u16 dst_index)
{
    if(hero.isShipMaster()) return;

    const u16 from_index = Maps::GetIndexFromAbsPoint(hero.GetCenter());

    Maps::Tiles & tiles_from = world.GetTiles(from_index);
    Maps::Tiles & tiles_to = world.GetTiles(dst_index);

    AGG::PlaySound(M82::KILLFADE);
    hero.GetPath().Hide();
    hero.FadeOut();
    hero.ResetMovePoints();
    tiles_from.SetObject(MP2::OBJ_COAST);
    hero.SetCenter(dst_index);
    hero.SetShipMaster(true);
    tiles_to.SetObject(MP2::OBJ_HEROES);
    hero.SaveUnderObject(MP2::OBJ_ZERO);

    if(H2Config::Debug()) Error::Verbose("ActionToBoat: " + hero.GetName() + " to boat");
}

void ActionToCoast(Heroes &hero, const u16 dst_index)
{
    if(! hero.isShipMaster()) return;

    const u16 from_index = Maps::GetIndexFromAbsPoint(hero.GetCenter());

    Maps::Tiles & tiles_from = world.GetTiles(from_index);
    Maps::Tiles & tiles_to = world.GetTiles(dst_index);

    hero.ResetMovePoints();
    tiles_from.SetObject(MP2::OBJ_BOAT);
    hero.SetCenter(dst_index);
    hero.SetShipMaster(false);
    tiles_to.SetObject(MP2::OBJ_HEROES);
    hero.SaveUnderObject(MP2::OBJ_ZERO);
    AGG::PlaySound(M82::KILLFADE);
    hero.GetPath().Hide();
    hero.FadeIn();

    if(H2Config::Debug()) Error::Verbose("ActionToCoast: " + hero.GetName() + " to coast");
}




void ActionToShrine(Heroes &hero, const u16 dst_index)
{
    const Spell::spell_t spell = world.SpellFromShrine(dst_index);
    const std::string & spell_name = Spell::String(spell);
    const u8 spell_level = Spell::Level(spell);

    std::string head;
    std::string body("You come across");

    switch(spell_level)
    {
	case 1:
	    head = "Shrine of the 1st Circle";
	    body += " a small shrine attended by a group of novice acolytes. In exchange for your protection, they agree to teach you a simple spell -";
	    break;
	case 2:
	    head = "Shrine of the 2st Circle";
	    body += " an ornate shrine attended by a group of rotund friars. In exchange for your protection, they agree to teach you a spell -";
	    break;
	case 3:
	    head = "Shrine of the 3st Circle";
	    body += " a lavish shrine attended by a group of high priests. In exchange for your protection, they agree to teach you a sophisticated spell -";
	    break;
	default: return;
    }
    
    body += "'" + spell_name + "'.";

    // check spell book
    if(!hero.HasArtifact(Artifact::MAGIC_BOOK))
    {
	PlaySoundFailure;
	body += " Unfortunately, you have no Magic Book to record the spell with.";
	Dialog::Message(head, body, Font::BIG, Dialog::OK);
	return;
    }

    // check valid level spell and wisdom skill
    if(3 == spell_level && Skill::Level::NONE == hero.GetLevelSkill(Skill::Secondary::WISDOM))
    {
	PlaySoundFailure;
	body += " Unfortunately, you do not have the wisdom to understand the spell, and you are unable to learn it.";
	Dialog::Message(head, body, Font::BIG, Dialog::OK);
	return;
    }

    PlaySoundSuccess;
    hero.AppendSpellToBook(spell);
    hero.SetVisited(dst_index, Visit::GLOBAL);
    Dialog::SpellInfo(spell_name, body, spell);

    if(H2Config::Debug()) Error::Verbose("ActionToShrine: " + hero.GetName());
}

void ActionToWitchsHut(Heroes &hero, const u16 dst_index)
{
    const Skill::Secondary::skill_t skill = world.SkillFromWitchsHut(dst_index);
    const std::string & skill_name = Skill::Secondary::String(skill);
    const std::string head("Witch's Hut");

    // check full
    if(HEROESMAXSKILL == hero.CountSecondarySkill())
    {
	PlaySoundFailure;
	Dialog::Message("You approach the hut and observe a witch inside studying an ancient tome on " + skill_name + ".", "As you approach, she turns and focuses her one glass eye on you. \"You already know everything you deserve to learn!\" the witch screeches. \"NOW GET OUT OF MY HOUSE!\"", Font::BIG, Dialog::OK);
	return;
    }

    // check present skill
    if(hero.HasSecondarySkill(skill))
    {
	PlaySoundVisited;
	const std::string body();
	Dialog::Message("You approach the hut and observe a witch inside studying an ancient tome on " + skill_name + ".", "As you approach, she turns and speaks. \"You already know that which I would teach you. I can help you no further.\"", Font::BIG, Dialog::OK);
	return;
    }

    PlaySoundSuccess;
    hero.LearnBasicSkill(skill);
    hero.SetVisited(dst_index, Visit::GLOBAL);

    Dialog::SkillInfo(skill_name, "An ancient and immortal witch living in a hut with bird's legs for stilts teaches you " + skill_name + " for her own inscrutable purposes.", skill, Skill::Level::BASIC);

    if(H2Config::Debug()) Error::Verbose("ActionToWitchsHut: " + hero.GetName());
}

void ActionToGoodLuckObject(Heroes &hero, const u16 dst_index)
{
    const Maps::Tiles & tile = world.GetTiles(dst_index);
    const MP2::object_t obj = tile.GetObject();

    std::string body_true;
    std::string body_false;

    switch(obj)
    {
        case MP2::OBJ_FOUNTAIN:
    	    body_false = "You drink from the enchanted fountain, but nothing happens.";
    	    body_true = "As you drink the sweet water, you gain luck for your next battle.";
    	    break;

        case MP2::OBJ_FAERIERING:
    	    body_false = "You enter the faerie ring, but nothing happens.";
    	    body_true = "Upon entering the mystical faerie ring, your army gains luck for its next battle.";
    	    break;

        case MP2::OBJ_IDOL:
	    body_false = "You've found an ancient and weathered stone idol. It is supposed to grant luck to visitors, but since the stars are already smiling upon you, it does nothing.";
	    body_true = "You've found an ancient and weathered stone idol. Kissing it is supposed to be lucky, so you do. The stone is very cold to the touch.";
    	    break;

    	default: return;
    }

    // check already visited
    if(hero.isVisited(obj))
    {
	PlaySoundVisited;
	Dialog::Message(MP2::StringObject(obj), body_false, Font::BIG, Dialog::OK);
    }
    else
    {
	// modify luck
        hero.SetVisited(dst_index);
	AGG::PlaySound(M82::GOODLUCK);
	DialogLuck(MP2::StringObject(obj), body_true, true, 1);
    }

    if(H2Config::Debug()) Error::Verbose("ActionToGoodLuckObject: " + hero.GetName());
}

void ActionToPoorLuckObject(Heroes &hero, const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);
    const MP2::object_t obj = tile.GetObject();
    const bool battle = tile.GetQuantity1();
    bool complete = false;
    std::string body;

    switch(obj)
    {
        case MP2::OBJ_PYRAMID:
    	    if(battle)
    	    {
		PlaySoundWarning;
		if(Dialog::YES == Dialog::Message("You come upon the pyramid of a great and ancient king.", "You are tempted to search it for treasure, but all the old stories warn of fearful curses and undead guardians. Will you search?", Font::BIG, Dialog::OK))
		{
		    // battle
		    Army::army_t army;
		    army.At(0).Set(Monster::ROYAL_MUMMY, 10);
		    army.At(1).Set(Monster::LORD_VAMPIRE, 10);
		    army.At(2).Set(Monster::ROYAL_MUMMY, 10);
		    army.At(3).Set(Monster::LORD_VAMPIRE, 10);
		    army.At(4).Set(Monster::ROYAL_MUMMY, 10);

		    // battle
		    const u32 exp = army.CalculateExperience();
		    const Army::battle_t b = Army::Battle(hero, army, tile);
		    switch(b)
		    {
			case Army::WIN:
			{
			    PlaySoundSuccess;
			    hero.IncreaseExperience(exp);
			    complete = true;
			    const Spell::spell_t spell(static_cast<Spell::spell_t>(tile.GetQuantity1()));
			    // check magick book
			    if(!hero.HasArtifact(Artifact::MAGIC_BOOK))
				Dialog::Message(MP2::StringObject(obj), "Unfortunately, you have no Magic Book to record the spell with.", Font::BIG, Dialog::OK);
			    else
			    // check skill level for wisdom
			    if(Skill::Level::EXPERT > hero.GetLevelSkill(Skill::Secondary::WISDOM))
			    	Dialog::Message(MP2::StringObject(obj), "Unfortunately, you do not have the wisdom to understand the spell, and you are unable to learn it.", Font::BIG, Dialog::OK);
			    else
			    {
				Dialog::SpellInfo(Spell::String(spell), "Upon defeating the monsters, you decipher an ancient glyph on the wall, telling the secret of the spell.", spell, true);
				hero.AppendSpellToBook(spell);
			    }
			    hero.ActionAfterBattle();
			    break;
			}
			case Army::RETREAT:
			case Army::SURRENDER:
			case Army::LOSE:
			    BattleLose(hero, b);
			    break;
        
    			default: break;
		    }
		}
    	    }
    	    else
    		body = "You come upon the pyramid of a great and ancient king. Routine exploration reveals that the pyramid is completely empty.";
    	    break;

    	default: return;
    }

    if(complete)
    {
	tile.SetQuantity1(0);
	tile.SetQuantity2(0);
    }
    else
    if(!battle && !hero.isVisited(obj))
    {
	// modify luck
        hero.SetVisited(dst_index);
	AGG::PlaySound(M82::BADLUCK);
	DialogLuck(MP2::StringObject(obj), body, false, 2);
    }

    if(H2Config::Debug()) Error::Verbose("ActionToPoorLuckObject: " + hero.GetName());
}

void ActionToSign(Heroes &hero, const u16 dst_index)
{
    PlaySoundWarning;
    Dialog::Message("Sign", world.MessageSign(dst_index), Font::BIG, Dialog::OK);
    if(H2Config::Debug()) Error::Verbose("ActionToSign: " + hero.GetName());
}

void ActionToMagicWell(Heroes &hero, const u16 dst_index)
{
    const u16 max = hero.GetMaxSpellPoints();

    if(hero.GetSpellPoints() == max)
    {
	PlaySoundFailure;
	Dialog::Message(MP2::StringObject(MP2::OBJ_MAGICWELL), "A drink at the well is supposed to restore your spell points, but you are already at maximum.", Font::BIG, Dialog::OK);
    }
    else
    // check already visited
    if(hero.isVisited(MP2::OBJ_MAGICWELL))
    {
	PlaySoundVisited;
	Dialog::Message(MP2::StringObject(MP2::OBJ_MAGICWELL), "A second drink at the well in one day will not help you.", Font::BIG, Dialog::OK);
    }
    else
    {
	PlaySoundSuccess;
	hero.SetVisited(dst_index);
	hero.SetSpellPoints(max);
	Dialog::Message(MP2::StringObject(MP2::OBJ_MAGICWELL), "A drink from the well has restored your spell points to maximum.", Font::BIG, Dialog::OK);
    }

    if(H2Config::Debug()) Error::Verbose("ActionToMagicWell: " + hero.GetName());
}

void ActionToTradingPost(Heroes &hero)
{
    PlaySoundSuccess;
    Dialog::Marketplace(true);
    if(H2Config::Debug()) Error::Verbose("ActionToTradingPost: " + hero.GetName());
}

void ActionToPrimarySkillObject(Heroes &hero, const u16 dst_index)
{
    const Maps::Tiles & tile = world.GetTiles(dst_index);
    const MP2::object_t obj = tile.GetObject();

    std::string body_false;
    std::string body_true;
    Skill::Primary::skill_t skill = Skill::Primary::ATTACK;

    switch(obj)
    {
        case MP2::OBJ_FORT:
    	    skill = Skill::Primary::DEFENCE;
    	    body_false = "\"I'm sorry sir,\" The leader of the soldiers says, \"but you already know everything we have to teach.\"";
    	    body_true = "The soldiers living in the fort teach you a few new defensive tricks.";
    	    break;

        case MP2::OBJ_MERCENARYCAMP:
    	    skill = Skill::Primary::ATTACK;
    	    body_false = "You've come upon a mercenary camp practicing their tactics. \"You're too advanced for us,\" the mercenary captain says. \"We can teach nothing more.\"";
    	    body_true = "You've come upon a mercenary camp practicing their tactics. The mercenaries welcome you and your troops and invite you to train with them.";
    	    break;

        case MP2::OBJ_DOCTORHUT:
    	    skill = Skill::Primary::KNOWLEDGE;
    	    body_false = "\"Go 'way!\", the witch doctor barks, \"you know all I know.\"";
    	    body_true = "An Orcish witch doctor living in the hut deepens your knowledge of magic by showing you how to cast stones, read portents, and decipher the intricacies of chicken entrails.";
    	    break;

        case MP2::OBJ_STANDINGSTONES:
    	    skill = Skill::Primary::POWER;
    	    body_false = "You've found a group of Druids worshipping at one of their strange stone edifices. Silently, the Druids turn you away, indicating they have nothing new to teach you.";
    	    body_true = "You've found a group of Druids worshipping at one of their strange stone edifices. Silently, they teach you new ways to cast spells.";
    	    break;

    	default: return;
    }

    // check already visited
    if(hero.isVisited(tile))
    {
	PlaySoundVisited;
	Dialog::Message(MP2::StringObject(obj), body_false, Font::BIG, Dialog::OK);
    }
    else
    {
	PlaySoundSuccess;
	// increase skill
	hero.IncreasePrimarySkill(skill);
	hero.SetVisited(dst_index);
	Dialog::SkillInfo(MP2::StringObject(obj), body_true, skill);
    }

    if(H2Config::Debug()) Error::Verbose("ActionToPrimarySkillObject: " + hero.GetName());
}

void ActionToPoorMoraleObject(Heroes &hero, const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);
    const MP2::object_t obj = tile.GetObject();

    const bool battle = (tile.GetQuantity1() || tile.GetQuantity2());
    bool complete = false;
    std::string body;
    
    switch(obj)
    {
        case MP2::OBJ_GRAVEYARD:
    	    if(battle)
    	    {
    		PlaySoundWarning;
		if(Dialog::YES == Dialog::Message("You tentatively approach the burial ground of ancient warriors.", "Do you want to search the graves?", Font::BIG, Dialog::YES | Dialog::NO))
    		{
		    Army::army_t army;
		    Army::ArrangeTroopsForBattle(army, Monster::MUTANT_ZOMBIE, 100);

		    // battle
		    const u32 exp = army.CalculateExperience();
		    const Army::battle_t b = Army::Battle(hero, army, tile);
		    switch(b)
		    {
			case Army::WIN:
			{
			    hero.IncreaseExperience(exp);
			    complete = true;
			    const Artifact::artifact_t art = Artifact::Artifact(tile.GetQuantity1());
			    Resource::funds_t resource;
			    resource.gold = tile.GetQuantity2() * 100;
			    PlaySoundSuccess;
			    DialogWithArtifactAndGold(MP2::StringObject(obj), "Upon defeating the zomies you search the graves and find something!", art, resource.gold);
			    hero.PickupArtifact(art);
			    world.GetKingdom(hero.GetColor()).AddFundsResource(resource);
			    hero.ActionAfterBattle();
			    break;
			}
			case Army::RETREAT:
			case Army::SURRENDER:
			case Army::LOSE:
			    BattleLose(hero, b);
			    break;
        
    			default: break;
		    }
    		}
    	    }
    	    else
    		body = "Upon defeating the Zombies you spend several hours searching the graves and find nothing. Such a despicable act reduces your army's morale.";
    	    break;

        case MP2::OBJ_SHIPWRECK:
    	    if(battle)
    	    {
    		PlaySoundWarning;
    		if(Dialog::YES == Dialog::Message("The rotting hulk of a great pirate ship creaks eerily as it is pushed against the rocks.", "Do you wish to search the shipwreck?", Font::BIG, Dialog::YES | Dialog::NO))
    		{
		    Army::army_t army;
		    Resource::funds_t resource;
		    Artifact::artifact_t art = Artifact::UNKNOWN;
		    u8 c = 1;
                    switch(tile.GetQuantity2())
                    {
                	case 1:
                	    resource.gold = 1000;
                	    c = 2;
                	    break;
                	case 2:
                	    resource.gold = 2000;
                	    c = 3;
                	    break;
                	case 3:
                	    resource.gold = 5000;
                	    c = 5;
                	    break;
                	case 4:
                	    resource.gold = 2000;
			    art = Artifact::Artifact(tile.GetQuantity1());
                	    c = 10;
                	    break;
                	default: Error::Warning("ActionToPoorMoraleObject: unknown variant for ShipWreck, index: ", dst_index); break;
                    }
                    ArrangeTroopsForBattle(army, Monster::GHOST, c * ARMYMAXTROOPS);

		    // battle
		    const u32 exp = army.CalculateExperience();
		    const Army::battle_t b = Army::Battle(hero, army, tile);
		    switch(b)
		    {
			case Army::WIN:
			{
			    hero.IncreaseExperience(exp);
			    complete = true;
			    PlaySoundSuccess;
			    if(art == Artifact::UNKNOWN)
				DialogWithGold(MP2::StringObject(obj), "Upon defeating the Ghosts you sift through the debris and find something!", resource.gold);
			    else
			    {
				DialogWithArtifactAndGold(MP2::StringObject(obj), "Upon defeating the Ghosts you sift through the debris and find something!", art, resource.gold);
				hero.PickupArtifact(art);
			    }
			    world.GetKingdom(hero.GetColor()).AddFundsResource(resource);
			    hero.ActionAfterBattle();
			    break;
			}
			case Army::RETREAT:
			case Army::SURRENDER:
			case Army::LOSE:
			    BattleLose(hero, b);
			    break;
        
    			default: break;
		    }
    		}
    	    }
    	    else
    		body = "Upon defeating the Ghosts you spend several hours sifting through the debris and find nothing. Such a despicable act reduces your army's morale.";
    	    break;

        case MP2::OBJ_DERELICTSHIP:
    	    if(battle)
    	    {
    		PlaySoundWarning;
    		if(Dialog::YES == Dialog::Message("The rotting hulk of a great pirate ship creaks eerily as it is pushed against the rocks.", "Do you wish to search the ship?", Font::BIG, Dialog::YES | Dialog::NO))
    		{
		    Army::army_t army;
		    ArrangeTroopsForBattle(army, Monster::SKELETON, 200);

		    // battle
		    const u32 exp = army.CalculateExperience();
		    const Army::battle_t b = Army::Battle(hero, army, tile);
		    switch(b)
		    {
			case Army::WIN:
			{
			    hero.IncreaseExperience(exp);
			    complete = true;
			    Resource::funds_t resource;
			    resource.gold = tile.GetQuantity2() * 100;
			    PlaySoundSuccess;
			    DialogWithGold(MP2::StringObject(obj), "Upon defeating the Skeletons you sift through the debris and find something!", resource.gold);
			    world.GetKingdom(hero.GetColor()).AddFundsResource(resource);
			    hero.ActionAfterBattle();
			    break;
			}
			case Army::RETREAT:
			case Army::SURRENDER:
			case Army::LOSE:
			    BattleLose(hero, b);
			    break;
        
    			default: break;
		    }
    		}
    	    }
    	    else
    		body = "Upon defeating the Skeletons you spend several hours sifting through the debris and find nothing. Such a despicable act reduces your army's morale.";
    	    break;

    	default: return;
    }

    if(complete)
    {
	tile.SetQuantity1(0);
	tile.SetQuantity2(0);
    }
    else
    if(!battle && !hero.isVisited(obj))
    {
	// modify morale
	hero.SetVisited(dst_index);
	AGG::PlaySound(M82::BADMRLE);
	DialogMorale(MP2::StringObject(obj), body, false, 1);
    }

    if(H2Config::Debug()) Error::Verbose("ActionToPoorMoraleObject: " + hero.GetName());
}

void ActionToGoodMoraleObject(Heroes &hero, const u16 dst_index)
{
    const Maps::Tiles & tile = world.GetTiles(dst_index);
    const MP2::object_t obj = tile.GetObject();

    std::string body_true;
    std::string body_false;
    u16 move = 0;

    switch(obj)
    {
        case MP2::OBJ_BUOY:
    	    body_false = "Your men spot a navigational buoy, confirming that you are on course.";
    	    body_true = "Your men spot a navigational buoy, confirming that you are on course and increasing their morale.";
    	    break;

        case MP2::OBJ_OASIS:
    	    body_false = "The drink at the oasis is refreshing, but offers no further benefit. The oasis might help again if you fought a battle first.";
    	    body_true = "A drink at the oasis fills your troops with strength and lifts their spirits.  You can travel a bit further today.";
    	    move = 800;	// + 8TP, from FAQ
    	    break;

        case MP2::OBJ_WATERINGHOLE:
    	    body_false = "The drink at the watering hole is refreshing, but offers no further benefit. The watering hole might help again if you fought a battle first.";
	    body_true = "A drink at the watering hole fills your troops with strength and lifts their spirits. You can travel a bit further today.";
    	    move = 400;	// + 4TP, from FAQ
    	    break;

        case MP2::OBJ_TEMPLE:
    	    body_false = "It doesn't help to pray twice before a battle. Come back after you've fought.";
    	    body_true = "A visit and a prayer at the temple raises the morale of your troops.";
    	    break;

    	default: return;
    }

    // check already visited
    if(hero.isVisited(obj))
    {
	PlaySoundVisited;
    	Dialog::Message(MP2::StringObject(obj), body_false, Font::BIG, Dialog::OK);
    }
    else
    {
	// modify morale
	hero.SetVisited(dst_index);
	AGG::PlaySound(M82::GOODMRLE);
	DialogMorale(MP2::StringObject(obj), body_true, true, (obj == MP2::OBJ_TEMPLE ? 2 : 1));
        hero.IncreaseMovePoints(move);
    }

    if(H2Config::Debug()) Error::Verbose("ActionToGoodMoraleObject: " + hero.GetName());
}


void ActionToExperienceObject(Heroes &hero, const u16 dst_index)
{
    const Maps::Tiles & tile = world.GetTiles(dst_index);
    const MP2::object_t obj = tile.GetObject();

    std::string body_true;
    std::string body_false;
    
    u32 exp = 0;

    switch(obj)
    {
        case MP2::OBJ_GAZEBO:
    	    body_false = "An old Knight appears on the steps of the gazebo. \"I am sorry, my liege, I have taught you all I can.\"";
    	    body_true = "An old Knight appears on the steps of the gazebo. \"My liege, I will teach you all that I know to aid you in your travels.\"";
    	    exp = 1000;
    	    break;

    	default: return;
    }

    // check already visited
    if(hero.isVisited(tile))
    {
	PlaySoundVisited;
	Dialog::Message(MP2::StringObject(obj), body_false, Font::BIG, Dialog::OK);
    }
    else
    {
	std::string count;
	String::AddInt(count, exp);
	const Sprite & sprite = AGG::GetICN(ICN::EXPMRL, 4);
	Surface image(sprite.w(), sprite.h() + 12);
	image.SetColorKey();
	image.Blit(sprite);
	Text text(count, Font::SMALL);
	text.Blit((sprite.w() - Text::width(count, Font::SMALL)) / 2, sprite.h() + 2, image);

	PlaySoundWarning;
	Dialog::SpriteInfo(MP2::StringObject(obj), body_true, image);

	// visit
	hero.SetVisited(dst_index);
	hero.IncreaseExperience(exp);
    }

    if(H2Config::Debug()) Error::Verbose("ActionToExperienceObject: " + hero.GetName());
}

void ActionToArtifact(Heroes &hero, const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);

    const Artifact::artifact_t art = Artifact::Artifact(tile.GetQuantity1());

    bool conditions = false;
    const u8 c = tile.GetQuantity2();

    switch(tile.GetObject())
    {
        case MP2::OBJ_SHIPWRECKSURVIROR:
	    conditions = true;
	    PlaySoundSuccess;
	    DialogWithArtifact(MP2::StringObject(tile.GetObject()), "You've pulled a shipwreck survivor from certain death in an unforgiving ocean.  Grateful, he rewards you for your act of kindness by giving you the " + Artifact::String(art), art);        break;

	case MP2::OBJ_ARTIFACT:
	    switch(c)
	    {
		// 1,2,3 - 2000g, 2500g+3res,3000g+5res
		case 1:
		case 2:
		case 3:
		{
		    const Resource::resource_t r = Resource::Rand();
		    std::string header = "A leprechaun offers you the " + Artifact::String(art) + " for the small price of ";
		    Resource::funds_t payment;
		    if(1 == c)
		    {
			header += "2000 Gold.";
			payment += Resource::funds_t(Resource::GOLD, 2000);
		    }
		    else
		    if(2 == c)
		    {
			header += "2500 Gold and 3 " + Resource::String(r) + ".";
			payment += Resource::funds_t(Resource::GOLD, 2500);
			payment += Resource::funds_t(r, 3);
		    }
		    else
		    {
			header += "3000 Gold and 3 " + Resource::String(r) + ".";
			payment += Resource::funds_t(Resource::GOLD, 3000);
			payment += Resource::funds_t(r, 5);
		    }

		    PlaySoundWarning;
		    if(Dialog::YES == DialogWithArtifact(header, "Do you wish to buy this artifact?", art, Dialog::YES | Dialog::NO))
		    {
			if(world.GetKingdom(hero.GetColor()).AllowPayment(payment))
			{
		    	    conditions = true;
		    	    world.GetKingdom(hero.GetColor()).OddFundsResource(payment);
			}
			else
			{
			    PlaySoundFailure;
			    Dialog::Message("You try to pay the leprechaun, but realize that you can't afford it.", "The leprechaun stamps his foot and ignores you.", Font::BIG, Dialog::OK);
			}
		    }
		    else
			Dialog::Message("", "Insulted by your refusal of his generous offer, the leprechaun stamps his foot and ignores you.", Font::BIG, Dialog::OK);
		    break;
		}

		// 4,5 - need have skill wisard or leadership,
		case 4:
		case 5:
		{
		    if(4 == c)
		    {
			if(hero.HasSecondarySkill(Skill::Secondary::WISDOM))
			{
			    PlaySoundSuccess;
			    DialogWithArtifact(MP2::StringObject(tile.GetObject()), "You've found the artifact: " + Artifact::String(art), art, Dialog::OK);
			    conditions = true;
			}
			else
			{
			    PlaySoundFailure;
			    Dialog::Message("You've found the humble dwelling of a withered hermit.", "The hermit tells you that he is willing to give the " + Artifact::String(art) + " to the first wise person he meets.", Font::BIG, Dialog::OK);
			}
		    }
		    else
		    {
			if(hero.HasSecondarySkill(Skill::Secondary::LEADERSHIP))
			{
			    PlaySoundSuccess;
			    DialogWithArtifact(MP2::StringObject(tile.GetObject()), "You've found the artifact: " + Artifact::String(art), art, Dialog::OK);
			    conditions = true;
			}
			else
			{
			    PlaySoundFailure;
			    Dialog::Message("You've come across the spartan quarters of a retired soldier.", "The soldier tells you that he is willing to pass on the " + Artifact::String(art) + " to the first true leader he meets.", Font::BIG, Dialog::OK);
			}
		    }
		    break;
		}

		// 6 - 50 rogues, 7 - 1 gin, 8,9,10,11,12,13 - 1 monster level4
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		{
		    bool battle = true;
		    const u8 count = (c == 6 ? 50 : 1);
		    const Monster::monster_t mons = (6 == c ? Monster::ROGUE :
						    (7 == c ? Monster::GENIE :
						    (8 == c ? Monster::PALADIN :
						    (9 == c ? Monster::CYCLOPS :
						    (10== c ? Monster::PHOENIX :
						    (11== c ? Monster::GREEN_DRAGON :
						    (12== c ? Monster::TITAN : Monster::BONE_DRAGON )))))));
		    Army::army_t army;
		    Army::ArrangeTroopsForBattle(army, mons, count);

		    PlaySoundWarning;
		    if(6 == c)
			Dialog::Message("You come upon an ancient artifact.", "As you reach for it, a pack of Rogues leap out of the brush to guard their stolen loot.", Font::BIG, Dialog::OK);
		    else
			battle = (Dialog::YES == Dialog::Message("Through a clearing you observe an ancient artifact.",
						"Unfortunately, it's guarded by a nearby " + Monster::String(mons) + ". Do you want to fight the " + Monster::String(mons) + " for the artifact?",
						Font::BIG, Dialog::YES | Dialog::NO));

		    if(battle)
		    {
			//Display::Fade(); need move to start battle code
			const u32 exp = army.CalculateExperience();
			const Army::battle_t b = Army::Battle(hero, army, tile);
			switch(b)
			{
			    case Army::WIN:
			    hero.IncreaseExperience(exp);
			    conditions = true;
			    PlaySoundSuccess;
			    DialogWithArtifact("Victorious, you take your prize, the ", Artifact::String(art), art);
			    hero.ActionAfterBattle();
			    break;

			    case Army::RETREAT:
			    case Army::SURRENDER:
			    case Army::LOSE:
				BattleLose(hero, b);
				break;
        
    			default: break;
			}
		    }
		    else
		    {
			PlaySoundFailure;
			Dialog::Message("", "Discretion is the better part of valor, and you decide to avoid this fight for today.", Font::BIG, Dialog::OK);
		    }
		    break;
		}

		default:
		    PlaySoundSuccess;
		    DialogWithArtifact(MP2::StringObject(tile.GetObject()), "You've found the artifact: " + Artifact::String(art), art);
		    conditions = true;
		    break;
	    }
	break;

	default: break;
    }

    if(conditions && hero.PickupArtifact(art))
    {
	PlayPickupSound();
	AnimationRemoveObject(tile);

	tile.RemoveObjectSprite();
	tile.SetQuantity1(0);
	tile.SetQuantity2(0);
	tile.SetObject(MP2::OBJ_ZERO);
    }

    if(H2Config::Debug()) Error::Verbose("ActionToArtifact: " + hero.GetName() + " pickup artifact");
}


void ActionToAncientLamp(Heroes &hero, const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);
    const u32 count = tile.GetCountMonster();

    PlaySoundSuccess;
    if(Dialog::YES == Dialog::Message("You stumble upon a dented and tarnished lamp lodged deep in the earth.", "Do you wish to rub the lamp?", Font::BIG, Dialog::YES|Dialog::NO))
    {
	PlayPickupSound();

	const u16 recruit = Dialog::RecruitMonster(Monster::GENIE, count);
	if(!hero.GetArmy().JoinTroop(Monster::GENIE, recruit))
	    Dialog::Message(Monster::String(Monster::GENIE), "You are unable to recruit at this time, your ranks are full.", Font::BIG, Dialog::OK);
	else
	if(recruit)
	{
	    AnimationRemoveObject(tile);
	    tile.SetCountMonster(0);
	    tile.RemoveObjectSprite();
	    tile.SetObject(MP2::OBJ_ZERO);
	}
    }

    if(H2Config::Debug()) Error::Verbose("ActionToTreasureChest: " + hero.GetName() + " pickup chest");
}

void ActionToTeleports(Heroes &hero, const u16 index_from)
{
    const u16 index_to = world.NextTeleport(index_from);
    hero.ApplyPenaltyMovement();

    if(index_from == index_to)
    {
	AGG::PlaySound(M82::RSBRYFZL);
	Error::Warning("ActionToTeleports: action unsuccessfully...");
	return;
    }

    AGG::PlaySound(M82::KILLFADE);
    hero.GetPath().Hide();
    hero.FadeOut();

    Maps::Tiles & tiles_from = world.GetTiles(index_from);
    Maps::Tiles & tiles_to = world.GetTiles(index_to);

    if(MP2::OBJ_HEROES != hero.GetUnderObject()) tiles_from.SetObject(MP2::OBJ_STONELIGHTS);

    hero.SaveUnderObject(MP2::OBJ_STONELIGHTS);
    hero.SetCenter(index_to);
    hero.Scoute();

    tiles_to.SetObject(MP2::OBJ_HEROES);

    Game::Focus::Get().Redraw();
    Display::Get().Flip();

    AGG::PlaySound(M82::KILLFADE);
    hero.GetPath().Hide();
    hero.FadeIn();

    // check monster
    u16 dst_index2 = MAXU16;
    if(Maps::TileUnderProtection(index_to, dst_index2)) hero.Action(dst_index2);
}

void ActionToWhirlpools(Heroes &hero, const u16 index_from)
{
    const u16 index_to = world.NextWhirlpool(index_from);
    hero.ApplyPenaltyMovement();

    if(index_from == index_to)
    {
	AGG::PlaySound(M82::RSBRYFZL);
	Error::Warning("ActionToWhirlpools: action unsuccessfully...");
	return;
    }

    AGG::PlaySound(M82::KILLFADE);
    hero.GetPath().Hide();
    hero.FadeOut();

    Maps::Tiles & tiles_from = world.GetTiles(index_from);
    Maps::Tiles & tiles_to = world.GetTiles(index_to);

    if(MP2::OBJ_HEROES != hero.GetUnderObject()) tiles_from.SetObject(MP2::OBJ_WHIRLPOOL);

    hero.SaveUnderObject(MP2::OBJ_WHIRLPOOL);
    hero.SetCenter(index_to);
    hero.Scoute();

    tiles_to.SetObject(MP2::OBJ_HEROES);

    Game::Focus::Get().Redraw();
    Display::Get().Flip();

    AGG::PlaySound(M82::KILLFADE);
    hero.GetPath().Hide();
    hero.FadeIn();

    if(Rand::Get(1))
    {
	PlaySoundWarning;
	Dialog::Message("A whirlpool engulfs your ship.", "Some of your army has fallen overboard.", Font::BIG, Dialog::OK);
	Army::Troop & troops = hero.GetArmy().GetWeakestTroop();
	const u16 c = troops.Count() / 2;
	troops.SetCount(c ? c : 1);
    }
}

void ActionToAbandoneMine(Heroes &hero, const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);
    const MP2::object_t obj = tile.GetObject();

    PlaySoundWarning;
    if(Dialog::YES == Dialog::Message("You come upon an abandoned gold mine.", "The mine appears to be haunted. Do you wish to enter?", Font::BIG, Dialog::YES | Dialog::NO))
    {
	Army::army_t army;
	ArrangeTroopsForBattle(army, Monster::GHOST, tile.GetQuantity1() * ARMYMAXTROOPS);

	const u32 exp = army.CalculateExperience();
	const Army::battle_t b = Army::Battle(hero, army, tile);

	switch(b)
	{
	    case Army::WIN:
		hero.IncreaseExperience(exp);
		PlaySoundSuccess;
		DialogWithGold(MP2::StringObject(obj), "You beat the Ghosts and are able to restore the mine to production.", 1000);
		tile.SetQuantity1(0);
		tile.UpdateAbandoneMineSprite();
		world.CaptureObject(dst_index, hero.GetColor());
		world.GetTiles(dst_index).CaptureFlags32(MP2::OBJ_MINES, hero.GetColor());
		hero.ActionAfterBattle();
		break;

	    case Army::RETREAT:
	    case Army::SURRENDER:
	    case Army::LOSE:
		BattleLose(hero, b);
		break;
        
    	    default: break;
	}
    }

    if(H2Config::Debug()) Error::Verbose("ActionToAbandoneMine: " + hero.GetName() + " captured: " + std::string(MP2::StringObject(obj)));
}

//

void ActionToDwellingJoinMonster(Heroes &hero, const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);
    const MP2::object_t obj = tile.GetObject();

    switch(obj)
    {
        case MP2::OBJ_WATCHTOWER:
        case MP2::OBJ_EXCAVATION:
        case MP2::OBJ_CAVE:
        case MP2::OBJ_TREEHOUSE:
	case MP2::OBJ_ARCHERHOUSE:
        case MP2::OBJ_GOBLINHUT:
        case MP2::OBJ_DWARFCOTT:
        case MP2::OBJ_HALFLINGHOLE:
	case MP2::OBJ_PEASANTHUT:
        case MP2::OBJ_THATCHEDHUT: break;
	default: return;
    }

    const u32 count = tile.GetCountMonster();

    if(count)
    {
	const Monster::monster_t monster = Monster::Monster(obj);
        const std::string & message = "A group of " + Monster::String(monster) + " with a desire for greater glory wish to join you.";

	PlaySoundSuccess;
	if(Dialog::YES == Dialog::Message(message, "Do you accept?", Font::BIG, Dialog::YES|Dialog::NO))
	{
	    if(!hero.GetArmy().JoinTroop(monster, count))
		Dialog::Message(Monster::String(monster), "You are unable to recruit at this time, your ranks are full.", Font::BIG, Dialog::OK);
	    else
		tile.SetCountMonster(0);
	}
    }
    else
    {
	PlaySoundVisited;
	Dialog::Message("", "As you approach the dwelling, you notice that there is no one here.", Font::BIG, Dialog::OK);
    }

    if(H2Config::Debug()) Error::Verbose("ActionToDwellingJoinMonster: " + hero.GetName() + ", object: " + std::string(MP2::StringObject(obj)));
}

void ActionToDwellingRecruitMonster(Heroes &hero, const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);
    const MP2::object_t obj = tile.GetObject();
    
    std::string msg_full1, msg_full2, msg_void1, msg_void2;

    switch(obj)
    {
        case MP2::OBJ_RUINS:
    	    msg_void1 = "You search the ruins, but the Medusas that used to live here are gone.";
    	    msg_void2 = "Perhaps there will be more next week.";
    	    msg_full1 = "You've found some Medusas living in the ruins. They are willing to join your army for a price.";
    	    msg_full2 = "Do you want to recruit Medusas?";
    	    break;

        case MP2::OBJ_TREECITY:
    	    msg_void1 = "You've found a Sprite Tree City.";
    	    msg_void2 = "Unfortunately, none of the Sprites living there wish to join an army. Maybe next week.";
    	    msg_full1 = "Some of the Sprites living in the tree city are willing to join your army for a price.";
    	    msg_full2 = "Do you want to recruit Sprites?";
    	    break;
  
        case MP2::OBJ_WAGONCAMP:
    	    msg_void1 = "A colorful Rogues' wagon stands empty here.";
    	    msg_void2 = "Perhaps more Rogues will be here later.";
    	    msg_full1 = "Distant sounds of music and laughter draw you to a colorful wagon housing Rogues.";
    	    msg_full2 = "Do you wish to have any Rogues join your army?";
    	    break;

	case MP2::OBJ_DESERTTENT:
    	    msg_void1 = "A group of tattered tents, billowing in the sandy wind, beckons you.";
    	    msg_void2 = "The tents are unoccupied. Perhaps more Nomads will be here later.";
    	    msg_full1 = "A group of tattered tents, billowing in the sandy wind, beckons you.";
    	    msg_full2 = "Do you wish to have any Nomads join you during your travels?";
    	    break;

	default: return;
    }

    const u32 count = tile.GetCountMonster();

    if(count)
    {
	const Monster::monster_t monster = Monster::Monster(obj);
	PlaySoundSuccess;
	if(Dialog::YES == Dialog::Message(msg_full1, msg_full2, Font::BIG, Dialog::YES | Dialog::NO))
	{
	    const u16 recruit = Dialog::RecruitMonster(monster, count);
	    if(recruit)
	    {
		if(!hero.GetArmy().JoinTroop(monster, recruit))
		    Dialog::Message(Monster::String(monster), "You are unable to recruit at this time, your ranks are full.", Font::BIG, Dialog::OK);
		else
		    tile.SetCountMonster(count - recruit);
	    }
	}
    }
    else
    {
	PlaySoundVisited;
	Dialog::Message(msg_void1, msg_void2, Font::BIG, Dialog::OK);
    }

    if(H2Config::Debug()) Error::Verbose("ActionToDwellingRecruitMonster: " + hero.GetName() + ", object: " + std::string(MP2::StringObject(obj)));
}

void ActionToDwellingBattleMonster(Heroes &hero, const u16 dst_index)
{
    Maps::Tiles & tile = world.GetTiles(dst_index);
    const MP2::object_t obj = tile.GetObject();

    // yet no one captured.
    const bool battle = (Color::GRAY == world.ColorCapturedObject(dst_index));
    const u32 count = tile.GetCountMonster();
    bool complete = false;

    switch(obj)
    {
        case MP2::OBJ_CITYDEAD:
	    if(!battle)
	    {
	    	if(0 == count)
	    	{
		    PlaySoundVisited;
	    	    Dialog::Message("The City of the Dead is empty of life, and empty of unlife as well.", "Perhaps some undead will move in next week.", Font::BIG, Dialog::OK);
		    break;
		}
		else
	    	{
		    PlaySoundSuccess;
		    complete = (Dialog::YES == Dialog::Message("Some Liches living here are willing to join your army for a price.", "Do you want to recruit Liches?", Font::BIG, Dialog::YES | Dialog::NO));
		}
	    }
	    else
	    {
		Army::army_t army;
		army.At(0).Set(Monster::ZOMBIE, 20);
		army.At(1).Set(Monster::LORD_VAMPIRE, 5);
		army.At(2).Set(Monster::POWER_LICH, 5);
		army.At(3).Set(Monster::LORD_VAMPIRE, 5);
		army.At(4).Set(Monster::ZOMBIE, 20);

		PlaySoundWarning;
		if(Dialog::YES == Dialog::Message("You've found the ruins of an ancient city, now inhabited solely by the undead.", "Will you search?", Font::BIG, Dialog::YES | Dialog::NO))
		{
		    // battle
		    const u32 exp = army.CalculateExperience();
		    const Army::battle_t b = Army::Battle(hero, army, tile);
		    switch(b)
		    {
			case Army::WIN:
			    hero.IncreaseExperience(exp);
			    world.CaptureObject(dst_index, hero.GetColor());
			    PlaySoundSuccess;
			    complete = (Dialog::YES == Dialog::Message("Some of the surviving Liches are impressed by your victory over their fellows, and offer to join you for a price.", "Do you want to recruit Liches?", Font::BIG, Dialog::YES | Dialog::NO));
			    hero.ActionAfterBattle();
			break;

			case Army::RETREAT:
			case Army::SURRENDER:
			case Army::LOSE:
			    BattleLose(hero, b);
			    break;
        
    			default: break;
		    }
		}
	    }
	    break;

        case MP2::OBJ_TROLLBRIDGE:
	    if(!battle)
	    {
	    	if(0 == count)
	    	{
		    PlaySoundVisited;
	    	    Dialog::Message("You've found one of those bridges that Trolls are so fond of living under, but there are none here.", "Perhaps there will be some next week.", Font::BIG, Dialog::OK);
		    break;
		}
		else
		{
		    PlaySoundSuccess;
	    	    complete = (Dialog::YES == Dialog::Message("Some Trolls living under a bridge are willing to join your army, but for a price.", "Do you want to recruit Trolls?", Font::BIG, Dialog::YES | Dialog::NO));
		}
	    }
	    else
	    {
		Army::army_t army;
		army.At(0).Set(Monster::WAR_TROLL, 4);
		army.At(1).Set(Monster::TROLL, 4);
		army.At(2).Set(Monster::TROLL, 4);
		army.At(3).Set(Monster::TROLL, 4);
		army.At(4).Set(Monster::WAR_TROLL, 4);

		PlaySoundWarning;
		if(Dialog::YES == Dialog::Message("Trolls living under the bridge challenge you.", "Will you fight them?", Font::BIG, Dialog::YES | Dialog::NO))
		{
		    // battle
		    const u32 exp = army.CalculateExperience();
		    const Army::battle_t b = Army::Battle(hero, army, tile);
		    switch(b)
		    {
			case Army::WIN:
			    hero.IncreaseExperience(exp);
			    world.CaptureObject(dst_index, hero.GetColor());
			    PlaySoundSuccess;
			    complete = (Dialog::YES == Dialog::Message("A few Trolls remain, cowering under the bridge. They approach you and offer to join your forces as mercenaries.", "Do you want to buy any Trolls?", Font::BIG, Dialog::YES | Dialog::NO));
			    hero.ActionAfterBattle();
			break;

			case Army::RETREAT:
			case Army::SURRENDER:
			case Army::LOSE:
			    BattleLose(hero, b);
			    break;
        
    			default: break;
		    }
		}
	    }
    	    break;

	case MP2::OBJ_DRAGONCITY:
	    if(!battle)
	    {
	    	if(0 == count)
	    	{
		    PlaySoundVisited;
	    	    Dialog::Message("The Dragon city has no Dragons willing to join you this week.", "Perhaps a Dragon will become available next week.", Font::BIG, Dialog::OK);
		    break;
		}
		else
		{
		    PlaySoundSuccess;
	    	    complete = (Dialog::YES == Dialog::Message("The Dragon city is willing to offer some Dragons for your army for a price.", "Do you wish to recruit Dragons?", Font::BIG, Dialog::YES | Dialog::NO));
		}
	    }
	    else
	    {
		Army::army_t army;
		army.At(0).Set(Monster::GREEN_DRAGON, 3);
		army.At(1).Set(Monster::RED_DRAGON, 2);
		army.At(2).Set(Monster::BLACK_DRAGON, 1);

		PlaySoundWarning;
		if(Dialog::YES == Dialog::Message("You stand before the Dragon City, a place off-limits to mere humans.", "Do you wish to violate this rule and challenge the Dragons to a fight?", Font::BIG, Dialog::YES | Dialog::NO))
		{
		    // battle
		    const u32 exp = army.CalculateExperience();
		    const Army::battle_t b = Army::Battle(hero, army, tile);
		    switch(b)
		    {
			case Army::WIN:
			    hero.IncreaseExperience(exp);
			    world.CaptureObject(dst_index, hero.GetColor());
			    PlaySoundSuccess;
			    complete = (Dialog::YES == Dialog::Message("Having defeated the Dragon champions, the city's leaders agree to supply some Dragons to your army for a price.", "Do you wish to recruit Dragons?", Font::BIG, Dialog::YES | Dialog::NO));
			    hero.ActionAfterBattle();
			break;

			case Army::RETREAT:
			case Army::SURRENDER:
			case Army::LOSE:
			    BattleLose(hero, b);
			    break;
        
    			default: break;
		    }
		}
	    }
	    break;

	default: return;
    }

    // recruit monster
    if(count && complete)
    {
	const Monster::monster_t monster = Monster::Monster(obj);
        const u16 recruit = Dialog::RecruitMonster(monster, count);
        if(recruit)
        {
    	    if(!hero.GetArmy().JoinTroop(monster, recruit))
		Dialog::Message(Monster::String(monster), "You are unable to recruit at this time, your ranks are full.", Font::BIG, Dialog::OK);
    	    else
    		tile.SetCountMonster(count - recruit);
    	}
    }

    if(H2Config::Debug()) Error::Verbose("ActionToDwellingBattleMonster: " + hero.GetName() + ", object: " + std::string(MP2::StringObject(obj)));
}

void ActionToObservationTower(Heroes &hero, const u16 dst_index)
{
    const Maps::Tiles & tile = world.GetTiles(dst_index);

    PlaySoundSuccess;
    Dialog::Message(MP2::StringObject(tile.GetObject()), "From the observation tower, you are able to see distant lands.", Font::BIG, Dialog::OK);
    Maps::ClearFog(Point(dst_index % world.w(), dst_index / world.h()), OBSERVATIONTOWERSCOUTE, hero.GetColor());
}

void ActionToArtesianSpring(Heroes &hero, const u16 dst_index)
{
    const u16 max = hero.GetMaxSpellPoints();
    const std::string & name = MP2::StringObject(MP2::OBJ_ARTESIANSPRING);

    if(hero.isVisited(MP2::OBJ_ARTESIANSPRING))
    {
	PlaySoundVisited;
	Dialog::Message(name, "The spring only refills once a week, and someone's already been here this week.", Font::BIG, Dialog::OK);
    }
    else
    if(hero.GetSpellPoints() == max * 2)
    {	
	PlaySoundFailure;
	Dialog::Message(name, "A drink at the spring is supposed to give you twice your normal spell points, but you are already at that level.", Font::BIG, Dialog::OK);
    }
    else
    {
	PlaySoundSuccess;
	hero.SetVisited(dst_index);
	hero.SetSpellPoints(max * 2);
	Dialog::Message(name, "A drink from the spring fills your blood with magic!  You have twice your normal spell points in reserve.", Font::BIG, Dialog::OK);
    }

    if(H2Config::Debug()) Error::Verbose("ActionToArtesianSpring: " + hero.GetName());
}

void ActionToXanadu(Heroes &hero, const u16 dst_index)
{
    const Maps::Tiles & tile = world.GetTiles(dst_index);

    if(hero.isVisited(tile))
    {
	PlaySoundVisited;
	Dialog::Message("Recognizing you, the butler refuses to admit you.", "\"The master,\" he says, \"will not see the same student twice.\"", Font::BIG, Dialog::OK);
    }
    else
    {
	bool access = false;
	switch(hero.GetLevelSkill(Skill::Secondary::DIPLOMACY))
	{
	    case Skill::Level::BASIC:
		if(7 < hero.GetLevel()) access = true; break;
	    case Skill::Level::ADVANCED:
		if(5 < hero.GetLevel()) access = true; break;
	    case Skill::Level::EXPERT:
		if(3 < hero.GetLevel()) access = true; break;
	    default:
		if(9 < hero.GetLevel()) access = true; break;
	}

	if(access)
	{
	    PlaySoundSuccess;
	    Dialog::Message("The butler admits you to see the master of the house.", "He trains you in the four skills a hero should know.", Font::BIG, Dialog::OK);
	    hero.IncreasePrimarySkill(Skill::Primary::ATTACK);
	    hero.IncreasePrimarySkill(Skill::Primary::DEFENCE);
	    hero.IncreasePrimarySkill(Skill::Primary::KNOWLEDGE);
	    hero.IncreasePrimarySkill(Skill::Primary::POWER);
	    hero.SetVisited(dst_index);
	}
	else
	{
	    PlaySoundFailure;
	    Dialog::Message("The butler opens the door and looks you up and down.", "\"You are neither famous nor diplomatic enough to be admitted to see my master,\" he sniffs. \"Come back when you think yourself worthy.\"", Font::BIG, Dialog::OK);
	}
    }

    if(H2Config::Debug()) Error::Verbose("ActionToXanadu: " + hero.GetName());
}

void ActionToUpgradeArmyObject(Heroes &hero, const u16 dst_index)
{
    const Maps::Tiles & tile = world.GetTiles(dst_index);
    const MP2::object_t & obj = tile.GetObject();

    std::string msg1;
    std::string msg2;
    std::string msg3;
    
    std::vector<Monster::monster_t> mons;
    mons.reserve(3);

    switch(obj)
    {
	case MP2::OBJ_HILLFORT:
	    if(hero.GetArmy().HasMonster(Monster::DWARF))
	    {
		hero.GetArmy().UpgradeMonsters(Monster::DWARF);
		mons.push_back(Monster::DWARF);
		msg1 = Monster::MultipleNames(Monster::DWARF);
	    }
	    if(hero.GetArmy().HasMonster(Monster::ORC))
	    {
		hero.GetArmy().UpgradeMonsters(Monster::ORC);
		mons.push_back(Monster::ORC);
		if(msg1.size()) msg1 += ", ";
		msg1 += Monster::MultipleNames(Monster::ORC);
	    }
	    if(hero.GetArmy().HasMonster(Monster::OGRE))
	    {
		hero.GetArmy().UpgradeMonsters(Monster::OGRE);
		mons.push_back(Monster::OGRE);
		if(msg1.size()) msg1 += ", ";
		msg1 += Monster::MultipleNames(Monster::OGRE);
	    }
	    msg1 = "All of the " + msg1 + " you have in your army have been trained by the battle masters of the fort.  Your army now contains " + msg1 + ".";
	    msg2 = "An unusual alliance of Orcs, Ogres, and Dwarves offer to train (upgrade) any such troops brought to them.  Unfortunately, you have none with you.";
	    break;

	case MP2::OBJ_FREEMANFOUNDRY:
	    if(hero.GetArmy().HasMonster(Monster::PIKEMAN))
	    {
		hero.GetArmy().UpgradeMonsters(Monster::PIKEMAN);
		mons.push_back(Monster::PIKEMAN);
		msg1 = Monster::MultipleNames(Monster::PIKEMAN);
	    }
	    if(hero.GetArmy().HasMonster(Monster::SWORDSMAN))
	    {
		hero.GetArmy().UpgradeMonsters(Monster::SWORDSMAN);
		mons.push_back(Monster::SWORDSMAN);
		if(msg1.size()) msg1 += ", ";
		msg1 += Monster::MultipleNames(Monster::SWORDSMAN);
	    }
	    if(hero.GetArmy().HasMonster(Monster::IRON_GOLEM))
	    {
		hero.GetArmy().UpgradeMonsters(Monster::IRON_GOLEM);
		mons.push_back(Monster::IRON_GOLEM);
		if(msg1.size()) msg1 += ", ";
		msg1 += Monster::MultipleNames(Monster::IRON_GOLEM);
	    }
	    msg1 = "All of your " + msg1 + " have been upgraded into " + msg1 + ".";
	    msg2 = "A blacksmith working at the foundry offers to convert all Pikemen and Swordsmen's weapons brought to him from iron to steel. He also says that he knows a process that will convert Iron Golems into Steel Golems.  Unfortunately, you have none of these troops in your army, so he can't help you.";
	    break;

	default: break;
    }

    if(mons.size())
    {
	// composite sprite
	u8 ox = 0;
	const Sprite & br = AGG::GetICN(ICN::STRIP, 12);
	Surface sf(br.w() * mons.size() + (mons.size() - 1) * 4, br.h());
	sf.SetColorKey();
	std::vector<Monster::monster_t>::const_iterator it1 = mons.begin();
	std::vector<Monster::monster_t>::const_iterator it2 = mons.end();
	for(; it1 != it2; ++it1)
	{
	    sf.Blit(br, ox, 0);
	    switch(Monster::GetRace(*it1))
	    {
		case Race::KNGT:	sf.Blit(AGG::GetICN(ICN::STRIP, 4), ox + 6, 6); break;
		case Race::BARB:	sf.Blit(AGG::GetICN(ICN::STRIP, 5), ox + 6, 6); break;
		case Race::SORC:	sf.Blit(AGG::GetICN(ICN::STRIP, 6), ox + 6, 6); break;
		case Race::WRLK:	sf.Blit(AGG::GetICN(ICN::STRIP, 7), ox + 6, 6); break;
		case Race::WZRD:	sf.Blit(AGG::GetICN(ICN::STRIP, 8), ox + 6, 6); break;
		case Race::NECR:	sf.Blit(AGG::GetICN(ICN::STRIP, 9), ox + 6, 6); break;
		default:		sf.Blit(AGG::GetICN(ICN::STRIP, 10), ox + 6, 6); break;
	    }
	    const Sprite & mon = AGG::GetICN(Monster::GetStats(Monster::Upgrade(*it1)).monh_icn, 0);
	    sf.Blit(mon, ox + 6 + mon.x(), 6 + mon.y());
	    ox += br.w() + 4;
	}
	Dialog::SpriteInfo(MP2::StringObject(obj), msg1, sf);
    }
    else
    {
	PlaySoundFailure;
	Dialog::Message(MP2::StringObject(obj), msg2, Font::BIG, Dialog::OK);
    }

    if(H2Config::Debug()) Error::Verbose("ActionToUpgradeArmyObject: " + hero.GetName());
}

void ActionToMagellanMaps(Heroes &hero, const u16 dst_index)
{
    if(1000 > world.GetKingdom(hero.GetColor()).GetFundsGold())
    {
	PlaySoundFailure;
	Dialog::Message("The captain sighs.", "\"You don't have enough money, eh?  You can't expect me to give my maps away for free!\"", Font::BIG, Dialog::OK);
    }
    else
    {
	PlaySoundWarning;
	if(Dialog::YES == Dialog::Message("A retired captain living on this refurbished fishing platform offers to sell you maps of the sea he made in his younger days for 1,000 gold.", "Do you wish to buy the maps?", Font::BIG, Dialog::YES | Dialog::NO))
	    world.ActionForMagellanMaps(hero.GetColor());

	Game::Focus::Get().Redraw();
    }

    if(H2Config::Debug()) Error::Verbose("ActionToMagellanMaps: " + hero.GetName());
}

void ActionToEvent(Heroes &hero, const u16 dst_index)
{
    // check event maps
    const GameEvent::Coord* event_maps = world.GetEventMaps(hero.GetColor(), dst_index);
    if(event_maps)
    {
        if(event_maps->GetResource().GetValidItems())
        {
    	    world.GetKingdom(hero.GetColor()).AddFundsResource(event_maps->GetResource());
	    PlaySoundSuccess;
    	    Dialog::ResourceInfo(event_maps->GetMessage(), "", event_maps->GetResource());
	}
	if(Artifact::UNKNOWN != event_maps->GetArtifact())
	{
	    if(hero.PickupArtifact(event_maps->GetArtifact()))
	    {
		PlayPickupSound();
		DialogWithArtifact("You find " + Artifact::String(event_maps->GetArtifact()), "", event_maps->GetArtifact());
	    }
	}
    }

    world.GetTiles(dst_index).SetObject(MP2::OBJ_ZERO);

    if(H2Config::Debug()) Error::Verbose("ActionToEvent: " + hero.GetName());
}
*/