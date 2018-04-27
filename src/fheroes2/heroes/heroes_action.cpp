/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov <fheroes2@gmail.com>          *
 *                                                                         *
 *   Part of the Free Heroes2 Engine:                                      *
 *   http://sourceforge.net/projects/fheroes2                              *
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
#include "mp2.h"
#include "world.h"
#include "settings.h"
#include "castle.h"
#include "monster.h"
#include "heroes.h"
#include "race.h"
#include "battle2.h"
#include "game_focus.h"
#include "game_interface.h"
#include "kingdom.h"
#include "morale.h"
#include "cursor.h"
#include "algorithm.h"
#include "payment.h"
#include "profit.h"
#include "gameevent.h"
#include "ai.h"

#define PlayMusicReplacement(m82) if(MUS::FromMapObject((MP2::object_t)obj) == MUS::UNKNOWN) \
                                                                             AGG::PlaySound(m82)
#define PlaySoundWarning    PlayMusicReplacement(M82::EXPERNCE)
#define PlaySoundSuccess    PlayMusicReplacement(M82::TREASURE)
#define PlaySoundFailure    PlayMusicReplacement(M82::H2MINE)
#define PlaySoundVisited    PlayMusicReplacement(M82::RSBRYFZL)

void ActionToCastle(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToHeroes(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToMonster(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToBoat(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToCoast(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToWagon(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToSkeleton(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToResource(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToPickupResource(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToFlotSam(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToArtifact(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToShipwreckSurvivor(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToShrine(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToWitchsHut(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToGoodLuckObject(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToPoorLuckObject(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToSign(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToMagicWell(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToTradingPost(Heroes &hero, const u8 obj);
void ActionToPrimarySkillObject(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToPoorMoraleObject(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToGoodMoraleObject(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToExperienceObject(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToTreasureChest(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToAncientLamp(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToTeleports(Heroes & hero, const s32 & dst_index);
void ActionToWhirlpools(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToObservationTower(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToCaptureObject(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToDwellingJoinMonster(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToDwellingRecruitMonster(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToDwellingBattleMonster(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToArtesianSpring(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToAbandoneMine(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToXanadu(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToUpgradeArmyObject(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToMagellanMaps(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToEvent(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToObelisk(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToTreeKnowledge(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToOracle(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToDaemonCave(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToAlchemistsTower(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToStables(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToArena(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToSirens(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToJail(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToHutMagi(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToEyeMagi(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToSphinx(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToBarrier(Heroes & hero, const u8 & obj, const s32 & dst_index);
void ActionToTravellersTent(Heroes & hero, const u8 & obj, const s32 & dst_index);

u16 DialogCaptureResourceObject(const std::string & hdr, const std::string & str, const u8 res, const u16 buttons = Dialog::OK)
{
  const Sprite & sprite = AGG::GetICN(ICN::RESOURCE, Resource::GetIndexSprite2(res));
  std::string msg = str;

  // sprite resource with x / day test
  Surface sf(sprite.w() + 30, sprite.h() + 14);
  sprite.Blit(15, 0, sf);

  std::string perday = _("%{count} / day");
  payment_t info = ProfitConditions::FromMine(res);
  s32* current = NULL;

  switch(res)
  {
    case Resource::MERCURY: current = &info.mercury;  break;
    case Resource::WOOD:    current = &info.wood; break;
    case Resource::ORE: current = &info.ore; break;
    case Resource::SULFUR:  current = &info.sulfur; break;
    case Resource::CRYSTAL: current = &info.crystal; break;
    case Resource::GEMS:    current = &info.gems; break;
    case Resource::GOLD:    current = &info.gold; break;
    default: break;
  }

  if(current)
  {
    String::Replace(perday, "%{count}", *current);

    switch(*current)
    {
      case 1:  String::Replace(msg, "%{count}", _("one")); break;
      case 2:  String::Replace(msg, "%{count}", _("two")); break;
      default: String::Replace(msg, "%{count}", *current); break;
    }
  }

  Text text(perday, Font::SMALL);
  text.Blit((sf.w() - text.w()) / 2, sf.h() - 12, sf);

  return Dialog::SpriteInfo(hdr, msg, sf, buttons);
}

u16 DialogGoldWithExp(const std::string & hdr, const std::string & msg, const u16 count, const u16 exp, const u16 buttons = Dialog::OK)
{
  const Sprite & gold = AGG::GetICN(ICN::RESOURCE, 6);
  const Sprite & sprite = AGG::GetICN(ICN::EXPMRL, 4);
  Surface image(sprite.w() + gold.w() + 50, sprite.h() + 12);
  gold.Blit(0, image.h() - gold.h() - 12, image);
  sprite.Blit(gold.w() + 50, 0, image);
  Text text(GetString(count), Font::SMALL);
  text.Blit((gold.w() - text.w()) / 2, image.h() - 12, image);
  text.Set(GetString(exp));
  text.Blit(gold.w() + 50 + (sprite.w() - text.w()) / 2, image.h() - 12, image);

  return Dialog::SpriteInfo(hdr, msg, image, buttons);
}

u16 DialogArtifactWithExp(const std::string & hdr, const std::string & msg, const Artifact & art, const u16 exp, const u16 buttons = Dialog::OK)
{
  const Sprite & sprite = AGG::GetICN(ICN::EXPMRL, 4);
  const Sprite & border = AGG::GetICN(ICN::RESOURCE, 7);
  const Sprite & artifact = AGG::GetICN(ICN::ARTIFACT, art.IndexSprite64());
  Surface image(sprite.w() + border.w() + 50, border.h());
  border.Blit(image);
  artifact.Blit(5, 5, image);
  sprite.Blit(border.w() + 50, (border.h() - sprite.h()) / 2, image);
  Text text(GetString(exp), Font::SMALL);
  text.Blit(border.w() + 50 + (sprite.w() - text.w()) / 2, image.h() - 12, image);

  return Dialog::SpriteInfo(hdr, msg, image, buttons);
}

u16 DialogWithExp(const std::string & hdr, const std::string & msg, const u16 exp, const u16 buttons = Dialog::OK)
{
  const Sprite & sprite = AGG::GetICN(ICN::EXPMRL, 4);
  Surface image(sprite.w(), sprite.h() + 12);
  sprite.Blit(image);
  Text text(GetString(exp), Font::SMALL);
  text.Blit((sprite.w() - text.w()) / 2, sprite.h(), image);

  return Dialog::SpriteInfo(hdr, msg, image, buttons);
}

u16 DialogWithArtifactAndGold(const std::string & hdr, const std::string & msg, const Artifact & art, const u16 count, const u16 buttons = Dialog::OK)
{
  const Sprite & gold = AGG::GetICN(ICN::RESOURCE, 6);
  const Sprite & border = AGG::GetICN(ICN::RESOURCE, 7);
  const Sprite & artifact = AGG::GetICN(ICN::ARTIFACT, art.IndexSprite64());
  Surface image(gold.w() + border.w() + 50, border.h());
  border.Blit(image);
  artifact.Blit(5, 5, image);
  gold.Blit(border.w() + 50, (border.h() - gold.h()) / 2, image);
  Text text(GetString(count), Font::SMALL);
  text.Blit(border.w() + 50 + (gold.w() - text.w()) / 2, border.h() - 25, image);

  return Dialog::SpriteInfo(hdr, msg, image, buttons);
}

u16 DialogWithGold(const std::string & hdr, const std::string & msg, const u16 count, const u16 buttons = Dialog::OK)
{
  const Sprite & gold = AGG::GetICN(ICN::RESOURCE, 6);
  Surface image(gold.w(), gold.h() + 12);
  gold.Blit(image);
  Text text(GetString(count), Font::SMALL);
  text.Blit((gold.w() - text.w()) / 2, gold.h(), image);

  return Dialog::SpriteInfo(hdr, msg, image, buttons);
}

u16 DialogMorale(const std::string & hdr, const std::string & msg, const bool good, u8 count)
{
  if(1 > count) count = 1;
  if(3 < count) count = 3;
  const Sprite & sprite = AGG::GetICN(ICN::EXPMRL, (good ? 2 : 3));
  u8 offset = sprite.w() * 4 / 3;
  Surface image(sprite.w() + offset * (count - 1), sprite.h());
  for(u8 ii = 0; ii < count; ++ii) sprite.Blit(offset * ii, 0, image);

  return Dialog::SpriteInfo(hdr, msg, image);
}

u16 DialogLuck(const std::string & hdr, const std::string & msg, const bool good, u8 count)
{
  if(1 > count) count = 1;
  if(3 < count) count = 3;
  const Sprite & sprite = AGG::GetICN(ICN::EXPMRL, (good ? 0 : 1));
  u8 offset = sprite.w() * 4 / 3;
  Surface image(sprite.w() + offset * (count - 1), sprite.h());
  for(u8 ii = 0; ii < count; ++ii) sprite.Blit(offset * ii, 0, image);

  return Dialog::SpriteInfo(hdr, msg, image);
}

void BattleLose(Heroes &hero, const Battle2::Result & res, bool attacker, Color::color_t color = Color::NONE)
{
  u8 reason = attacker ? res.AttackerResult() : res.DefenderResult();

  if(Settings::Get().ExtHeroSurrenderingGiveExp() &&
      Battle2::RESULT_SURRENDER == reason)
  {
    const u32 & exp = attacker ? res.GetExperienceAttacker() : res.GetExperienceDefender();

    if(CONTROL_HUMAN == hero.GetControl())
    {
      std::string msg = _("Hero %{name} also got a %{count} experience.");
      String::Replace(msg, "%{name}", hero.GetName());
      String::Replace(msg, "%{count}", exp);
      Dialog::Message("", msg, Font::BIG, Dialog::OK);
    }
    hero.IncreaseExperience(exp);
  }

  AGG::PlaySound(M82::KILLFADE);
  hero.FadeOut();
  hero.SetKillerColor(color);
  hero.SetFreeman(reason);
  GameFocus::Reset(GameFocus::HEROES);
  GameFocus::SetRedraw();
}

void PlayPickupSound(void)
{
  M82::m82_t wav = M82::UNKNOWN;

  switch(Rand::Get(1, 7))
  {
    case 1: wav = M82::PICKUP01; break;
    case 2: wav = M82::PICKUP02; break;
    case 3: wav = M82::PICKUP03; break;
    case 4: wav = M82::PICKUP04; break;
    case 5: wav = M82::PICKUP05; break;
    case 6: wav = M82::PICKUP06; break;
    case 7: wav = M82::PICKUP07; break;

    default: return;
  }

  AGG::PlaySound(wav);
}

void AnimationRemoveObject(Maps::Tiles & tile)
{
  Maps::TilesAddon *addon = MP2::isRemoveObject(tile.GetObject()) ? tile.FindObject(tile.GetObject()) : NULL;

  if(NULL == addon) return;

  const Interface::GameArea & gamearea = Interface::GameArea::Get();
  const Point & area = gamearea.GetMapsPos();
  const Rect & rect = gamearea.GetRectMaps();
  const Point pos(tile.GetIndex() % world.w() - rect.x, tile.GetIndex() / world.w() - rect.y);

  const s16 dstx = area.x + TILEWIDTH * pos.x;
  const s16 dsty = area.y + TILEWIDTH * pos.y;

  Cursor & cursor = Cursor::Get();
  Display & display = Display::Get();

  const MapsIndexes & heroes = Maps::ScanAroundObject(tile.GetIndex(), MP2::OBJ_HEROES);
  const Surface & stile = tile.GetTileSurface();
  Surface sobj(stile.w(), stile.h());

  const Sprite & sprite = AGG::GetICN(MP2::GetICNObject(addon->object), addon->index);
  sprite.Blit(sprite.x(), sprite.y(), sobj);

  // if animation sprite
  u16 index;
  if(0 != (index = ICN::AnimationFrame(MP2::GetICNObject(addon->object), addon->index, 0)))
  {
    const Sprite & sprite = AGG::GetICN(MP2::GetICNObject(addon->object), index);
    sprite.Blit(sprite.x(), sprite.y(), sobj);
  }

  LocalEvent & le = LocalEvent::Get();
  u8 alpha = 250;

  while(le.HandleEvents() && alpha > 10)
  {
    if(Game::AnimateInfrequent(Game::HEROES_PICKUP_DELAY))
    {
      cursor.Hide();
      stile.Blit(dstx, dsty, display);
      sobj.Blit(alpha, dstx, dsty, display);

      if(heroes.size())
      {
        for(MapsIndexes::const_iterator
            it = heroes.begin(); it != heroes.end(); ++it)
        {
          Heroes* hero = world.GetTiles(*it).GetHeroes();
          if(hero) hero->Redraw(display, false);
        }
      }
      else
        tile.RedrawTop(display);
      cursor.Show();
      display.Flip();
      alpha -= 20;
    }
  }
}

void RecruitMonsterFromTile(Heroes & hero, Maps::Tiles & tile, const std::string & msg, const Army::Troop & troop, bool remove)
{
  if(! hero.GetArmy().CanJoinTroop(troop))
    Dialog::Message(msg, _("You are unable to recruit at this time, your ranks are full."), Font::BIG, Dialog::OK);
  else
  {
    const u16 recruit = Dialog::RecruitMonster(troop(), troop.GetCount());

    if(recruit)
    {
      if(remove && recruit == troop.GetCount())
      {
        PlayPickupSound();
        AnimationRemoveObject(tile);
        tile.MonsterSetCount(0);
        tile.RemoveObjectSprite();
        tile.SetObject(MP2::OBJ_ZERO);
      }
      else
        tile.MonsterSetCount(troop.GetCount() - recruit);

      const payment_t paymentCosts = troop().GetCost() * recruit;
      world.GetKingdom(hero.GetColor()).OddFundsResource(paymentCosts);

      hero.GetArmy().JoinTroop(troop(), recruit);
      hero.MovePointsScaleFixed();

      if(Settings::Get().ExtHeroRecalculateMovement())
        hero.RecalculateMovePoints();

      Interface::Basic::Get().SetRedraw(REDRAW_STATUS);
    }
  }
}

// action to next cell
void Heroes::Action(const s32 dst_index)
{
  if(CONTROL_AI == world.GetKingdom(GetColor()).GetControl())
    return AI::HeroesAction(*this, dst_index);

  const Maps::Tiles & tile = world.GetTiles(dst_index);
  const MP2::object_t object = (dst_index == GetIndex() ?
      tile.GetObject(false) : tile.GetObject());

  if(MUS::FromMapObject(object) != MUS::UNKNOWN)
    AGG::PlayMusic(MUS::FromMapObject(object), false);

  if(MP2::isActionObject(object, isShipMaster()))
  {
    Interface::Basic::Get().statusWindow.ResetTimer();
    SetModes(ACTION);
  }

  switch(object)
  {
    case MP2::OBJ_MONSTER:  ActionToMonster(*this, object, dst_index); break;

    case MP2::OBJ_CASTLE:   ActionToCastle(*this, object, dst_index); break;
    case MP2::OBJ_HEROES:   ActionToHeroes(*this, object, dst_index); break;

    case MP2::OBJ_BOAT: ActionToBoat(*this, object, dst_index); break;
    case MP2::OBJ_COAST:    ActionToCoast(*this, object, dst_index); break;

                            // resource object
    case MP2::OBJ_WINDMILL:
    case MP2::OBJ_WATERWHEEL:
    case MP2::OBJ_MAGICGARDEN:
    case MP2::OBJ_LEANTO:   ActionToResource(*this, object, dst_index); break;

    case MP2::OBJ_WAGON:        ActionToWagon(*this, object, dst_index); break;
    case MP2::OBJ_SKELETON:     ActionToSkeleton(*this, object, dst_index); break;

                                // pickup object
    case MP2::OBJ_RESOURCE:
    case MP2::OBJ_BOTTLE:
    case MP2::OBJ_CAMPFIRE:     ActionToPickupResource(*this, object, dst_index); break;

    case MP2::OBJ_WATERCHEST:
    case MP2::OBJ_TREASURECHEST:    ActionToTreasureChest(*this, object, dst_index); break;
    case MP2::OBJ_ANCIENTLAMP:  ActionToAncientLamp(*this, object, dst_index); break;
    case MP2::OBJ_FLOTSAM:      ActionToFlotSam(*this, object, dst_index); break;

    case MP2::OBJ_SHIPWRECKSURVIROR:ActionToShipwreckSurvivor(*this, object, dst_index); break;
    case MP2::OBJ_ARTIFACT:     ActionToArtifact(*this, object, dst_index); break;

                                // shrine circle
    case MP2::OBJ_SHRINE1:
    case MP2::OBJ_SHRINE2:
    case MP2::OBJ_SHRINE3:      ActionToShrine(*this, object, dst_index); break;

                                // witchs hut
    case MP2::OBJ_WITCHSHUT:    ActionToWitchsHut(*this, object, dst_index); break;

                                // info message
    case MP2::OBJ_SIGN:     ActionToSign(*this, object, dst_index); break;

                            // luck modification
    case MP2::OBJ_FOUNTAIN:
    case MP2::OBJ_FAERIERING:
    case MP2::OBJ_IDOL:     ActionToGoodLuckObject(*this, object, dst_index); break;

    case MP2::OBJ_PYRAMID:      ActionToPoorLuckObject(*this, object, dst_index); break;
    case MP2::OBJ_MAGICWELL:    ActionToMagicWell(*this, object, dst_index); break;
    case MP2::OBJ_TRADINGPOST:  ActionToTradingPost(*this, object); break;

                                // primary skill modification
    case MP2::OBJ_FORT:
    case MP2::OBJ_MERCENARYCAMP:
    case MP2::OBJ_DOCTORHUT:
    case MP2::OBJ_STANDINGSTONES:   ActionToPrimarySkillObject(*this, object, dst_index); break;

                                    // morale modification
    case MP2::OBJ_OASIS:
    case MP2::OBJ_TEMPLE:
    case MP2::OBJ_WATERINGHOLE:
    case MP2::OBJ_BUOY:     ActionToGoodMoraleObject(*this, object, dst_index); break;

    case MP2::OBJ_SHIPWRECK:
    case MP2::OBJ_GRAVEYARD:
    case MP2::OBJ_DERELICTSHIP: ActionToPoorMoraleObject(*this, object, dst_index); break;

                                // experience modification
    case MP2::OBJ_GAZEBO:       ActionToExperienceObject(*this, object, dst_index); break;
    case MP2::OBJ_DAEMONCAVE:   ActionToDaemonCave(*this, object, dst_index); break;

                                // teleports
    case MP2::OBJ_STONELIGHTS:  ActionToTeleports(*this, dst_index); break;
    case MP2::OBJ_WHIRLPOOL:    ActionToWhirlpools(*this, object, dst_index); break;

                                // obsv tower
    case MP2::OBJ_OBSERVATIONTOWER: ActionToObservationTower(*this, object, dst_index); break;
    case MP2::OBJ_MAGELLANMAPS: ActionToMagellanMaps(*this, object, dst_index); break;

                                // capture color object
    case MP2::OBJ_ALCHEMYLAB:
    case MP2::OBJ_MINES:
    case MP2::OBJ_SAWMILL:
    case MP2::OBJ_LIGHTHOUSE:   ActionToCaptureObject(*this, object, dst_index); break;

    case MP2::OBJ_ABANDONEDMINE:    ActionToAbandoneMine(*this, object, dst_index); break;

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
    case MP2::OBJ_THATCHEDHUT:  ActionToDwellingJoinMonster(*this, object, dst_index); break;

                                // recruit army
    case MP2::OBJ_RUINS:
    case MP2::OBJ_TREECITY:
    case MP2::OBJ_WAGONCAMP:
    case MP2::OBJ_DESERTTENT:   ActionToDwellingRecruitMonster(*this, object, dst_index); break;

                                // battle and recruit army
    case MP2::OBJ_DRAGONCITY:
    case MP2::OBJ_CITYDEAD:
    case MP2::OBJ_TROLLBRIDGE:  ActionToDwellingBattleMonster(*this, object, dst_index); break;

    case MP2::OBJ_ARTESIANSPRING:   ActionToArtesianSpring(*this, object, dst_index); break;

    case MP2::OBJ_XANADU:       ActionToXanadu(*this, object, dst_index); break;

    case MP2::OBJ_HILLFORT:
    case MP2::OBJ_FREEMANFOUNDRY:   ActionToUpgradeArmyObject(*this, object, dst_index); break;

    case MP2::OBJ_EVENT:        ActionToEvent(*this, object, dst_index); break;

    case MP2::OBJ_OBELISK:          ActionToObelisk(*this, object, dst_index); break;

    case MP2::OBJ_TREEKNOWLEDGE:    ActionToTreeKnowledge(*this, object, dst_index); break;

    case MP2::OBJ_ORACLE:       ActionToOracle(*this, object, dst_index); break;
    case MP2::OBJ_SPHINX:       ActionToSphinx(*this, object, dst_index); break;

                                // loyalty version
    case MP2::OBJ_WATERALTAR:
    case MP2::OBJ_AIRALTAR:
    case MP2::OBJ_FIREALTAR:
    case MP2::OBJ_EARTHALTAR:
    case MP2::OBJ_BARROWMOUNDS: ActionToDwellingRecruitMonster(*this, object, dst_index); break;
    case MP2::OBJ_ALCHEMYTOWER: ActionToAlchemistsTower(*this, object, dst_index); break;
    case MP2::OBJ_STABLES:      ActionToStables(*this, object, dst_index); break;
    case MP2::OBJ_ARENA:        ActionToArena(*this, object, dst_index); break;
    case MP2::OBJ_MERMAID:      ActionToGoodLuckObject(*this, object, dst_index); break;
    case MP2::OBJ_SIRENS:       ActionToSirens(*this, object, dst_index); break;
    case MP2::OBJ_JAIL:     ActionToJail(*this, object, dst_index); break;
    case MP2::OBJ_HUTMAGI:      ActionToHutMagi(*this, object, dst_index); break;
    case MP2::OBJ_EYEMAGI:      ActionToEyeMagi(*this, object, dst_index); break;

    case MP2::OBJ_BARRIER:      ActionToBarrier(*this, object, dst_index); break;
    case MP2::OBJ_TRAVELLERTENT:    ActionToTravellersTent(*this, object, dst_index); break;

                                    // object
    default: break;
  }
}

void ActionToMonster(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  bool destroy = false;
  Maps::Tiles & tile = world.GetTiles(dst_index);
  const Army::Troop & troop = tile.QuantityTroop();
  //const Settings & conf = Settings::Get();


  u32 join = 0;
  Funds cost;

  u8 reason = Army::GetJoinSolution(hero, tile, join, cost.gold);

  // free join
  if(1 == reason)
  {
    DEBUG(DBG_GAME, DBG_INFO, hero.GetName() << " join monster " << troop.GetName());

    std::string message = _("A group of %{monster} with a desire for greater glory wish to join you.\nDo you accept?");
    String::Replace(message, "%{monster}", String::Lower(troop.GetMultiName()));

    if(Dialog::Message("Followers", message, Font::BIG, Dialog::YES | Dialog::NO) == Dialog::YES)
    {
      hero.GetArmy().JoinTroop(troop);
      Interface::Basic::Get().SetRedraw(REDRAW_STATUS);
    }
    else
    {
      Dialog::Message("", _("Insulted by your refusal of their offer, the monsters attack!"), Font::BIG, Dialog::OK);
      reason = 0;
    }
  }
  else
    // join with cost
    if(2 == reason)
    {
      std::string message;
      if(troop.GetCount() == 1)
        message = _("The creature is swayed by your diplomatic tongue, and offers to join your army for the sum of %{gold} gold.\nDo you accept?");
      else
      {
        message = _("The creatures are swayed by your diplomatic\ntongue, and make you an offer:\n \n");
        if(join != troop.GetCount())
          message += _("%{offer} of the %{total} %{monster} will join your army, and the rest will leave you alone, for the sum of %{gold} gold.\nDo you accept?");
        else
          message += _("All %{offer} of the %{monster} will join your army for the sum of %{gold} gold.\nDo you accept?");
      }

      String::Replace(message, "%{offer}", join);
      String::Replace(message, "%{total}", troop.GetCount());
      String::Replace(message, "%{monster}", String::Lower(troop.GetPluralName(join)));
      String::Replace(message, "%{gold}", cost.gold);

      if(Dialog::YES == Dialog::ResourceInfo("", message, cost, Dialog::YES | Dialog::NO))
      {
        DEBUG(DBG_GAME, DBG_INFO, hero.GetName() << " join monster " << troop.GetName() <<
            ", count: " << join << ", cost: " << cost.gold);
        hero.GetArmy().JoinTroop(troop(), join);
        world.GetKingdom(hero.GetColor()).OddFundsResource(cost);
        Interface::Basic::Get().SetRedraw(REDRAW_STATUS);
      }
      else
      {
        Dialog::Message("", _("Insulted by your refusal of their offer, the monsters attack!"), Font::BIG, Dialog::OK);
        reason = 0;
      }
    }
    else
      // flee
      if(3 == reason)
      {
        std::string message = _("The %{monster}, awed by the power of your forces, begin to scatter.\nDo you wish to pursue and engage them?");
        String::Replace(message, "%{monster}", String::Lower(troop.GetMultiName()));

        if(Dialog::Message("", message, Font::BIG, Dialog::YES | Dialog::NO) == Dialog::NO)
          destroy = true;
        else
          reason = 0;
      }

  //bool allow_move = false;

  // fight
  if(0 == reason)
  {
    DEBUG(DBG_GAME, DBG_INFO, hero.GetName() << " attack monster " << troop.GetName());

    Army::army_t army;
    army.JoinTroop(troop);
    army.ArrangeForBattle();

    Battle2::Result res = Battle2::Loader(hero.GetArmy(), army, dst_index);

    if(res.AttackerWins())
    {
      hero.IncreaseExperience(res.GetExperienceAttacker());
      destroy = true;
      //allow_move = true;
    }
    else
    {
      BattleLose(hero, res, true);
      if(Settings::Get().ExtWorldSaveMonsterBattle())
      {
        tile.MonsterSetCount(army.MonsterCounts(troop()));
        // reset "can join"
        if(tile.MonsterJoinConditionFree()) tile.MonsterSetJoinCondition(Monster::JOIN_CONDITION_MONEY);
      }
    }
  }
  // unknown
  else
    destroy = true;

  if(destroy)
  {
    Maps::TilesAddon *addon = tile.FindObject(MP2::OBJ_MONSTER);
    if(addon)
    {
      AGG::PlaySound(M82::KILLFADE);
      const u32 uniq = addon->uniq;
      AnimationRemoveObject(tile);
      tile.Remove(uniq);
      tile.MonsterSetCount(0);
      tile.SetObject(MP2::OBJ_ZERO);

      // remove shadow from left cell
      if(Maps::isValidDirection(dst_index, Direction::LEFT))
        world.GetTiles(Maps::GetDirectionIndex(dst_index, Direction::LEFT)).Remove(uniq);
    }

    // auto move hero
    // disable: https://sourceforge.net/tracker/index.php?func=detail&aid=3155230&group_id=96859&atid=616180
    /*
       if(conf.ExtHeroAutoMove2BattleTarget() && allow_move)
       {
       hero.Move2Dest(dst_index);
       }
       */
  }
}

void ActionToHeroes(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  Heroes *other_hero = world.GetTiles(dst_index).GetHeroes();
  const Settings & conf = Settings::Get();

  if(! other_hero) return;

  if(hero.GetColor() == other_hero->GetColor() ||
      (conf.ExtUnionsAllowHeroesMeetings() && Players::isFriends(hero.GetColor(), other_hero->GetColor())))
  {
    DEBUG(DBG_GAME, DBG_INFO, hero.GetName() << " meeting " << other_hero->GetName());
    hero.MeetingDialog(*other_hero);
  }
  else
    if(Players::isFriends(hero.GetColor(), other_hero->GetColor()))
    {
      DEBUG(DBG_GAME, DBG_INFO, hero.GetName() << " disable meeting");
    }
    else
      if(! hero.AllowBattle(true))
      {
        DEBUG(DBG_GAME, DBG_INFO, hero.GetName() << " currently can not allow battle");
      }
      else
        if(! other_hero->AllowBattle(false))
        {
          DEBUG(DBG_GAME, DBG_INFO, other_hero->GetName() << " currently can not allow battle");
        }
        else
        {
          Castle *other_hero_castle = other_hero->inCastle();
          if(other_hero_castle && other_hero == other_hero_castle->GetHeroes().GuardFirst())
          {
            ActionToCastle(hero, MP2::OBJ_CASTLE, dst_index);
            return;
          }

          bool disable_auto_move = hero.isShipMaster() || other_hero->isShipMaster() ||
            other_hero_castle || world.GetTiles(hero.GetIndex()).GetObject(false) == MP2::OBJ_STONELIGHTS;
          DEBUG(DBG_GAME, DBG_INFO, hero.GetName() << " attack enemy hero " << other_hero->GetName());

          // new battle2
          Battle2::Result res = Battle2::Loader(hero.GetArmy(), other_hero->GetArmy(), dst_index);

          // loss defender
          if(!res.DefenderWins())
            BattleLose(*other_hero, res, false, hero.GetColor());

          // loss attacker
          if(!res.AttackerWins())
            BattleLose(hero, res, true, other_hero->GetColor());

          // wins attacker
          if(res.AttackerWins())
          {
            hero.IncreaseExperience(res.GetExperienceAttacker());

            // auto move hero
            if(conf.ExtHeroAutoMove2BattleTarget() && !disable_auto_move)
            {
              hero.Move2Dest(dst_index);
            }
          }
          else
            // wins defender
            if(res.DefenderWins())
            {
              other_hero->IncreaseExperience(res.GetExperienceDefender());
            }
        }
}

void ActionToCastle(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  Castle *castle = world.GetCastle(dst_index);
  const Settings & conf = Settings::Get();

  if(! castle) return;

  if(hero.GetColor() == castle->GetColor() ||
      (conf.ExtUnionsAllowCastleVisiting() && Players::isFriends(hero.GetColor(), castle->GetColor())))
  {
    DEBUG(DBG_GAME, DBG_INFO, hero.GetName() << " goto castle " << castle->GetName());
    Mixer::Reduce();
    if(!conf.ExtHeroLearnSpellsWithDay()) castle->MageGuildEducateHero(hero);
    Game::OpenCastleDialog(castle);
    Mixer::Enhance();
  }
  else
    if(Players::isFriends(hero.GetColor(), castle->GetColor()))
    {
      DEBUG(DBG_GAME, DBG_INFO, hero.GetName() << " disable visiting");
    }
    else
    {
      CastleHeroes heroes = castle->GetHeroes();

      // first attack to guest hero
      if(heroes.FullHouse())
      {
        ActionToHeroes(hero, MP2::OBJ_HEROES, dst_index);
        return;
      }

      Army::army_t & army = castle->GetActualArmy();
      bool allow_enter = false;

      if(army.isValid())
      {
        DEBUG(DBG_GAME, DBG_INFO, hero.GetName() << " attack enemy castle " << castle->GetName());

        Heroes* defender = heroes.GuardFirst();
        castle->ActionPreBattle();

        // new battle2
        Battle2::Result res = Battle2::Loader(hero.GetArmy(), army, dst_index);

        castle->ActionAfterBattle(res.AttackerWins());

        // loss defender
        if(!res.DefenderWins() && defender)
          BattleLose(*defender, res, false, hero.GetColor());

        // loss attacker
        if(!res.AttackerWins())
          BattleLose(hero, res, true, castle->GetColor());

        // wins attacker
        if(res.AttackerWins())
        {
          world.GetKingdom(castle->GetColor()).RemoveCastle(castle);
          world.GetKingdom(hero.GetColor()).AddCastle(castle);
          world.CaptureObject(dst_index, hero.GetColor());
          castle->Scoute();
          Interface::Basic::Get().SetRedraw(REDRAW_CASTLES);

          hero.IncreaseExperience(res.GetExperienceAttacker());
          allow_enter = true;
        }
        else
          // wins defender
          if(res.DefenderWins() && defender)
          {
            defender->IncreaseExperience(res.GetExperienceDefender());
          }
      }
      else
      {
        DEBUG(DBG_GAME, DBG_INFO, hero.GetName() << " capture enemy castle " << castle->GetName());

        world.GetKingdom(castle->GetColor()).RemoveCastle(castle);
        world.GetKingdom(hero.GetColor()).AddCastle(castle);
        world.CaptureObject(dst_index, hero.GetColor());
        castle->Scoute();
        Interface::Basic::Get().SetRedraw(REDRAW_CASTLES);
        allow_enter = true;
      }

      // auto move hero to castle
      if(conf.ExtHeroAutoMove2BattleTarget() && allow_enter)
      {
        hero.Move2Dest(dst_index);
        ActionToCastle(hero, MP2::OBJ_CASTLE, dst_index);
      }
    }
}

void ActionToBoat(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  if(hero.isShipMaster()) return;

  AGG::PlaySound(M82::KILLFADE);
  hero.GetPath().Hide();
  hero.FadeOut();
  hero.ResetMovePoints();
  hero.Move2Dest(dst_index);
  hero.SetMapsObject(MP2::OBJ_ZERO);
  hero.SetShipMaster(true);
  hero.GetPath().Reset();

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}

void ActionToCoast(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  if(! hero.isShipMaster()) return;

  Maps::Tiles & from = world.GetTiles(hero.GetIndex());

  hero.ResetMovePoints();
  hero.Move2Dest(dst_index);
  from.SetObject(MP2::OBJ_BOAT);
  hero.SetShipMaster(false);
  AGG::PlaySound(M82::KILLFADE);
  hero.GetPath().Hide();
  hero.FadeIn();
  hero.GetPath().Reset();

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}

void ActionToPickupResource(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  Maps::Tiles & tile = world.GetTiles(dst_index);

  PlayPickupSound();
  AnimationRemoveObject(tile);
  tile.RemoveObjectSprite();

  if(obj == MP2::OBJ_BOTTLE)
    Dialog::Message(MP2::StringObject(obj), world.MessageSign(dst_index), Font::BIG, Dialog::OK);
  else
  {
    const Funds & funds = tile.QuantityFunds();

    if(obj == MP2::OBJ_CAMPFIRE)
    {
      // force reset sound
      tile.SetObject(MP2::OBJ_ZERO);
      Game::EnvironmentSoundMixer();

      Dialog::ResourceInfo(MP2::StringObject(obj), _("Ransacking an enemy camp, you discover a hidden cache of treasures."), funds);
    }
    else
    {
      const ResourceCount & rc = tile.QuantityResourceCount();
      Interface::Basic & I = Interface::Basic::Get();
      I.statusWindow.SetResource(rc.first, rc.second);
      I.SetRedraw(REDRAW_STATUS);
    }

    world.GetKingdom(hero.GetColor()).AddFundsResource(funds);
  }

  tile.QuantityReset();

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}

void ActionToResource(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  Maps::Tiles & tile = world.GetTiles(dst_index);
  const ResourceCount & rc = tile.QuantityResourceCount();
  bool cancapture = Settings::Get().ExtWorldExtObjectsCaptured();
  bool showinvalid = cancapture && hero.GetColor() == tile.QuantityColor() ? false : true;

  std::string msg;

  // dialog
  switch(obj)
  {
    case MP2::OBJ_WINDMILL:
      msg = rc.isValid() ?
        _("The keeper of the mill announces:\n\"Milord, I have been working very hard to provide you with these resources, come back next week for more.\"") :
        _("The keeper of the mill announces:\n\"Milord, I am sorry, there are no resources currently available. Please try again next week.\"");
      break;

    case MP2::OBJ_WATERWHEEL:
      msg = rc.isValid() ?
        _("The keeper of the mill announces:\n\"Milord, I have been working very hard to provide you with this gold, come back next week for more.\"") :
        _("The keeper of the mill announces:\n\"Milord, I am sorry, there is no gold currently available. Please try again next week.\"");
      break;

    case MP2::OBJ_LEANTO:
      cancapture = false;
      msg = rc.isValid() ?
        _("You've found an abandoned lean-to.\nPoking about, you discover some resources hidden nearby.") :
        _("The lean-to is long abandoned. There is nothing of value here.");
      break;

    case MP2::OBJ_MAGICGARDEN:
      msg = rc.isValid() ?
        _("You catch a leprechaun foolishly sleeping amidst a cluster of magic mushrooms.\nIn exchange for his freedom, he guides you to a small pot filled with precious things.") :
        _("You've found a magic garden, the kind of place that leprechauns and faeries like to cavort in, but there is no one here today.\nPerhaps you should try again next week.");
      break;

    default:
      cancapture = false;
      break;
  }

  if(rc.isValid())
  {
    const Funds funds(rc);
    PlaySoundSuccess;
    Dialog::ResourceInfo("", msg, funds);
    world.GetKingdom(hero.GetColor()).AddFundsResource(funds);

    if(cancapture)
      ActionToCaptureObject(hero, obj, dst_index);
  }
  else
  {
    if(cancapture)
      ActionToCaptureObject(hero, obj, dst_index);

    if(showinvalid)
      Dialog::Message("", msg, Font::BIG, Dialog::OK);
  }

  tile.QuantityReset();

  hero.SetVisited(dst_index, Visit::GLOBAL);

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}

void ActionToSkeleton(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  Maps::Tiles & tile = world.GetTiles(dst_index);
  std::string message(_("You come upon the remains of an unfortunate adventurer."));

  // artifact
  if(tile.QuantityIsValid())
  {
    PlayPickupSound();

    if(hero.IsFullBagArtifacts())
    {
      u16 gold = GoldInsteadArtifact(obj);
      Funds funds(Resource::GOLD, gold);
      Dialog::ResourceInfo("", _("Treasure"), funds, Dialog::OK);
      world.GetKingdom(hero.GetColor()).AddFundsResource(funds);
    }
    else
    {
      const Artifact & art = tile.QuantityArtifact();
      message.append("\n");
      message.append(_("Searching through the tattered clothing, you find %{artifact}."));
      String::Replace(message, "%{artifact}", art.GetName());
      Dialog::ArtifactInfo("", message, art);
      hero.PickupArtifact(art);
    }

    tile.QuantityReset();
  }
  else
  {
    message.append("\n");
    message.append(_("Searching through the tattered clothing, you find nothing."));
    Dialog::Message("", message, Font::BIG, Dialog::OK);
  }

  hero.SetVisitedWideTile(dst_index, obj, Visit::GLOBAL);

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}

void ActionToWagon(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  Maps::Tiles & tile = world.GetTiles(dst_index);
  std::string message(_("You come across an old wagon left by a trader who didn't quite make it to safe terrain."));

  if(tile.QuantityIsValid())
  {
    const Artifact & art = tile.QuantityArtifact();

    if(art.isValid())
    {
      if(hero.IsFullBagArtifacts())
      {
        message.append("\n");
        message.append(_("Unfortunately, others have found it first, and the wagon is empty."));
        Dialog::Message("", message, Font::BIG, Dialog::OK);
      }
      else
      {
        PlayPickupSound();
        message.append("\n");
        message.append(_("Searching inside, you find the %{artifact}."));
        String::Replace(message, "%{artifact}", art.GetName());
        Dialog::ArtifactInfo("", message, art);
        hero.PickupArtifact(art);
      }
    }
    else
    {
      const Funds & funds = tile.QuantityFunds();
      PlayPickupSound();
      message.append("\n");
      message.append(_("Inside, you find some of the wagon's cargo still intact."));
      Dialog::ResourceInfo("", message, funds);
      world.GetKingdom(hero.GetColor()).AddFundsResource(funds);
    }

    tile.QuantityReset();
  }
  else
  {
    message.append("\n");
    message.append(_("Unfortunately, others have found it first, and the wagon is empty."));
    Dialog::Message("", message, Font::BIG, Dialog::OK);
  }

  hero.SetVisited(dst_index, Visit::GLOBAL);

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}

void ActionToFlotSam(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  Maps::Tiles & tile = world.GetTiles(dst_index);
  std::string msg;
  const Funds & funds = tile.QuantityFunds();

  PlayPickupSound();
  AnimationRemoveObject(tile);

  if(0 < funds.GetValidItems())
  {
    msg = funds.wood && funds.gold ? _("You search through the flotsam, and find some wood and some gold.") :
      _("You search through the flotsam, and find some wood.");
    Dialog::ResourceInfo(MP2::StringObject(obj), msg, funds);
    world.GetKingdom(hero.GetColor()).AddFundsResource(funds);
  }
  else
  {
    msg = _("You search through the flotsam, but find nothing.");
    Dialog::Message(MP2::StringObject(obj), msg, Font::BIG, Dialog::OK);
  }

  tile.RemoveObjectSprite();
  tile.QuantityReset();

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}

void ActionToShrine(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  const Spell & spell = world.GetTiles(dst_index).QuantitySpell();
  const u8 spell_level = spell.Level();

  std::string head;
  std::string body;

  switch(spell_level)
  {
    case 1:
      head = _("Shrine of the 1st Circle");
      body = _("You come across a small shrine attended by a group of novice acolytes.\nIn exchange for your protection, they agree to teach you a simple spell - '%{spell}'.");
      break;
    case 2:
      head = _("Shrine of the 2nd Circle");
      body = _("You come across an ornate shrine attended by a group of rotund friars.\nIn exchange for your protection, they agree to teach you a spell - '%{spell}'.");
      break;
    case 3:
      head = _("Shrine of the 3rd Circle");
      body = _("You come across a lavish shrine attended by a group of high priests.\nIn exchange for your protection, they agree to teach you a sophisticated spell - '%{spell}'.");
      break;
    default: return;
  }

  String::Replace(body, "%{spell}", spell.GetName());

  // check spell book
  if(!hero.HaveSpellBook())
  {
    if(!Settings::Get().ExtHeroBuySpellBookFromShrine() || !hero.BuySpellBook(NULL, spell_level))
    {
      PlaySoundFailure;
      body += _("\nUnfortunately, you have no Magic Book to record the spell with.");
      Dialog::Message(head, body, Font::BIG, Dialog::OK);
    }
  }

  if(hero.HaveSpellBook())
  {
    // check valid level spell and wisdom skill
    if(3 == spell_level && Skill::Level::NONE == hero.GetLevelSkill(Skill::Secondary::WISDOM))
    {
      PlaySoundFailure;
      body += _("\nUnfortunately, you do not have the wisdom to understand the spell, and you are unable to learn it.");
      Dialog::Message(head, body, Font::BIG, Dialog::OK);
    }
    else
      // already know (skip bag artifacts)
      if(hero.HaveSpell(spell(), true))
      {
        PlaySoundFailure;
        body += _("\nUnfortunately, you already have knowledge of this spell, so there is nothing more for them to teach you.");
        Dialog::Message(head, body, Font::BIG, Dialog::OK);
      }
      else
      {
        PlaySoundSuccess;
        hero.AppendSpellToBook(spell());
        Dialog::SpellInfo(head, body, spell());
      }
  }

  hero.SetVisited(dst_index, Visit::GLOBAL);
  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}

void ActionToWitchsHut(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  const Skill::Secondary & skill = world.GetTiles(dst_index).QuantitySkill();

  AGG::PlayMusic(MUS::SKILL, false);

  if(skill.isValid())
  {
    std::string msg = _("You approach the hut and observe a witch inside studying an ancient tome on %{skill}.\n \n");
    const std::string & skill_name = Skill::Secondary::String(skill.Skill());
    String::Replace(msg, "%{skill}", skill_name);

    // check full
    if(hero.HasMaxSecondarySkill())
    {
      msg.append(_("As you approach, she turns and focuses her one glass eye on you.\n\"You already know everything you deserve to learn!\" the witch screeches. \"NOW GET OUT OF MY HOUSE!\""));
      Dialog::Message(MP2::StringObject(obj), msg, Font::BIG, Dialog::OK);
    }
    else
      // check present skill
      if(hero.HasSecondarySkill(skill.Skill()))
      {
        msg.append(_("As you approach, she turns and speaks.\n\"You already know that which I would teach you. I can help you no further.\""));
        Dialog::Message(MP2::StringObject(obj), msg, Font::BIG, Dialog::OK);
      }
      else
      {
        hero.LearnSkill(skill);

        msg.append(_("An ancient and immortal witch living in a hut with bird's legs for stilts teaches you %{skill} for her own inscrutable purposes."));
        String::Replace(msg, "%{skill}", skill_name);
        Dialog::SecondarySkillInfo(MP2::StringObject(obj), msg, skill);
      }

  }

  hero.SetVisited(dst_index, Visit::GLOBAL);
  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}

void ActionToGoodLuckObject(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  bool visited = hero.isVisited(obj);
  std::string msg;

  switch(obj)
  {
    case MP2::OBJ_FOUNTAIN:
      msg = visited ? _("You drink from the enchanted fountain, but nothing happens.") :
        _("As you drink the sweet water, you gain luck for your next battle.");
      break;

    case MP2::OBJ_FAERIERING:
      msg = visited ? _("You enter the faerie ring, but nothing happens.") :
        _("Upon entering the mystical faerie ring, your army gains luck for its next battle.");
      break;

    case MP2::OBJ_IDOL:
      msg = visited ? _("You've found an ancient and weathered stone idol.\nIt is supposed to grant luck to visitors, but since the stars are already smiling upon you, it does nothing.") :
        _("You've found an ancient and weathered stone idol.\nKissing it is supposed to be lucky, so you do. The stone is very cold to the touch.");
      break;

    case MP2::OBJ_MERMAID:
      msg = visited ? _("The mermaids silently entice you to return later and be blessed again.") :
        _("The magical, soothing beauty of the Mermaids reaches you and your crew.\nJust for a moment, you forget your worries and bask in the beauty of the moment.\nThe mermaids charms bless you with increased luck for your next combat.");
      break;

    default: break;
  }

  // check already visited
  if(visited)
  {
    PlaySoundVisited;
    Dialog::Message(MP2::StringObject(obj), msg, Font::BIG, Dialog::OK);
  }
  else
  {
    // modify luck
    hero.SetVisited(dst_index);
    AGG::PlaySound(M82::GOODLUCK);
    DialogLuck(MP2::StringObject(obj), msg, true, 1);
  }

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}

void ActionToPoorLuckObject(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  Maps::Tiles & tile = world.GetTiles(dst_index);
  const Spell & spell = tile.QuantitySpell();
  std::string ask, msg;

  switch(obj)
  {
    case MP2::OBJ_PYRAMID:
      ask = _("You come upon the pyramid of a great and ancient king.\nYou are tempted to search it for treasure, but all the old stories warn of fearful curses and undead guardians.\nWill you search?");
      msg = _("You come upon the pyramid of a great and ancient king.\nRoutine exploration reveals that the pyramid is completely empty.");
      break;

    default: break;
  }

  if(Dialog::YES == Dialog::Message("", ask, Font::BIG, Dialog::YES|Dialog::NO))
  {
    if(spell.isValid())
    {
      PlaySoundWarning;

      // battle
      Army::army_t army(tile);

      Battle2::Result res = Battle2::Loader(hero.GetArmy(), army, dst_index);
      if(res.AttackerWins())
      {
        PlaySoundSuccess;

        hero.IncreaseExperience(res.GetExperienceAttacker());
        bool valid = false;

        // check magick book
        if(!hero.HaveSpellBook())
          msg = _("Unfortunately, you have no Magic Book to record the spell with.");
        else
          // check skill level for wisdom
          if(Skill::Level::EXPERT > hero.GetLevelSkill(Skill::Secondary::WISDOM))
            msg = _("Unfortunately, you do not have the wisdom to understand the spell, and you are unable to learn it.");
          else
          {
            valid = true;
            msg = _("Upon defeating the monsters, you decipher an ancient glyph on the wall, telling the secret of the spell.");
          }

        if(valid)
        {
          Dialog::SpellInfo(spell.GetName(), msg, spell, true);
          hero.AppendSpellToBook(spell);
        }
        else
          Dialog::Message(MP2::StringObject(obj), msg, Font::BIG, Dialog::OK);

        tile.QuantityReset();
        hero.SetVisited(dst_index, Visit::GLOBAL);
      }
      else
      {
        BattleLose(hero, res, true);
      }
    }
    else
    {
      // modify luck
      AGG::PlaySound(M82::BADLUCK);
      DialogLuck(MP2::StringObject(obj), msg, false, 2);

      hero.SetVisited(dst_index, Visit::LOCAL);
      hero.SetVisited(dst_index, Visit::GLOBAL);
    }
  }

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}

void ActionToSign(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  PlaySoundWarning;
  Dialog::Message("Sign", world.MessageSign(dst_index), Font::BIG, Dialog::OK);
  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}

void ActionToMagicWell(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  const u16 max = hero.GetMaxSpellPoints();

  if(hero.GetSpellPoints() == max)
  {
    PlaySoundFailure;
    Dialog::Message(MP2::StringObject(MP2::OBJ_MAGICWELL), _("A drink at the well is supposed to restore your spell points, but you are already at maximum."), Font::BIG, Dialog::OK);
  }
  else
    // check already visited
    if(hero.isVisited(MP2::OBJ_MAGICWELL))
    {
      PlaySoundVisited;
      Dialog::Message(MP2::StringObject(MP2::OBJ_MAGICWELL), _("A second drink at the well in one day will not help you."), Font::BIG, Dialog::OK);
    }
    else
    {
      PlaySoundSuccess;
      hero.SetVisited(dst_index);
      hero.SetSpellPoints(max);
      Dialog::Message(MP2::StringObject(MP2::OBJ_MAGICWELL), _("A drink from the well has restored your spell points to maximum."), Font::BIG, Dialog::OK);
    }

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}

void ActionToTradingPost(Heroes &hero, const u8 obj)
{
  PlaySoundSuccess;
  Dialog::Marketplace(true);
  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}

void ActionToPrimarySkillObject(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  const Maps::Tiles & tile = world.GetTiles(dst_index);

  std::string msg;
  Skill::Primary::skill_t skill = Skill::Primary::ATTACK;
  bool visited = hero.isVisited(tile);

  switch(obj)
  {
    case MP2::OBJ_FORT:
      skill = Skill::Primary::DEFENSE;
      msg = visited ? _("\"I'm sorry sir,\" The leader of the soldiers says, \"but you already know everything we have to teach.\"") :
        _("The soldiers living in the fort teach you a few new defensive tricks.");
      break;

    case MP2::OBJ_MERCENARYCAMP:
      skill = Skill::Primary::ATTACK;
      msg = visited ? _("You've come upon a mercenary camp practicing their tactics. \"You're too advanced for us,\" the mercenary captain says. \"We can teach nothing more.\"") :
        _("You've come upon a mercenary camp practicing their tactics. The mercenaries welcome you and your troops and invite you to train with them.");
      break;

    case MP2::OBJ_DOCTORHUT:
      skill = Skill::Primary::KNOWLEDGE;
      msg = visited ? _("\"Go 'way!\", the witch doctor barks, \"you know all I know.\"") :
        _("An Orcish witch doctor living in the hut deepens your knowledge of magic by showing you how to cast stones, read portents, and decipher the intricacies of chicken entrails.");
      break;

    case MP2::OBJ_STANDINGSTONES:
      skill = Skill::Primary::POWER;
      msg = visited ? _("You've found a group of Druids worshipping at one of their strange stone edifices. Silently, the Druids turn you away, indicating they have nothing new to teach you.") :
        _("You've found a group of Druids worshipping at one of their strange stone edifices. Silently, they teach you new ways to cast spells.");
      break;

    default: return;
  }

  // check already visited
  if(visited)
  {
    PlaySoundVisited;
    Dialog::Message(MP2::StringObject(obj), msg, Font::BIG, Dialog::OK);
  }
  else
  {
    PlaySoundSuccess;
    // increase skill
    hero.IncreasePrimarySkill(skill);
    hero.SetVisited(dst_index);
    Dialog::PrimarySkillInfo(MP2::StringObject(obj), msg, skill);

    // fix double action tile
    hero.SetVisitedWideTile(dst_index, obj);
  }

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}

void ActionToPoorMoraleObject(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  Maps::Tiles & tile = world.GetTiles(dst_index);
  u16  gold = tile.QuantityGold();
  bool complete = false;
  std::string ask, msg, win;

  switch(obj)
  {
    case MP2::OBJ_GRAVEYARD:
      ask = _("You tentatively approach the burial ground of ancient warriors. Do you want to search the graves?");
      msg = _("Upon defeating the Zombies you spend several hours searching the graves and find nothing. Such a despicable act reduces your army's morale.");
      win = _("Upon defeating the zomies you search the graves and find something!");
      break;
    case MP2::OBJ_SHIPWRECK:
      ask = _("The rotting hulk of a great pirate ship creaks eerily as it is pushed against the rocks. Do you wish to search the shipwreck?");
      msg = _("Upon defeating the Ghosts you spend several hours sifting through the debris and find nothing. Such a despicable act reduces your army's morale.");
      win = _("Upon defeating the Ghosts you sift through the debris and find something!");
      break;
    case MP2::OBJ_DERELICTSHIP:
      ask = _("The rotting hulk of a great pirate ship creaks eerily as it is pushed against the rocks. Do you wish to search the ship?");
      msg = _("Upon defeating the Skeletons you spend several hours sifting through the debris and find nothing. Such a despicable act reduces your army's morale.");
      win = _("Upon defeating the Skeletons you sift through the debris and find something!");
      break;
    default: break;
  }

  if(Dialog::YES == Dialog::Message("", ask, Font::BIG, Dialog::YES | Dialog::NO))
  {
    if(gold)
    {
      PlaySoundWarning;

      Army::army_t army(tile);

      Battle2::Result res = Battle2::Loader(hero.GetArmy(), army, dst_index);
      if(res.AttackerWins())
      {
        hero.IncreaseExperience(res.GetExperienceAttacker());
        complete = true;
        const Artifact & art = tile.QuantityArtifact();

        PlaySoundSuccess;

        if(art.isValid())
        {
          if(hero.IsFullBagArtifacts())
          {
            gold = GoldInsteadArtifact(obj);
            DialogWithGold(MP2::StringObject(obj), win, gold);
          }
          else
          {
            DialogWithArtifactAndGold(MP2::StringObject(obj), win, art, gold);
            hero.PickupArtifact(art);
          }
        }
        else
          DialogWithGold(MP2::StringObject(obj), win, gold);

        world.GetKingdom(hero.GetColor()).AddFundsResource(Funds(Resource::GOLD, gold));
      }
      else
      {
        BattleLose(hero, res, true);
      }
    }

    if(complete)
      tile.QuantityReset();
    else
      if(0 == gold && !hero.isVisited(obj))
      {
        // modify morale
        hero.SetVisited(dst_index);
        hero.SetVisited(dst_index, Visit::GLOBAL);
        AGG::PlaySound(M82::BADMRLE);
        DialogMorale(MP2::StringObject(obj), msg, false, 1);
      }
  }

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}

void ActionToGoodMoraleObject(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  std::string msg;
  u16 move = 0;
  bool visited = hero.isVisited(obj);

  switch(obj)
  {
    case MP2::OBJ_BUOY:
      msg = visited ? _("Your men spot a navigational buoy, confirming that you are on course.") :
        _("Your men spot a navigational buoy, confirming that you are on course and increasing their morale.");
      break;

    case MP2::OBJ_OASIS:
      msg = visited ? _("The drink at the oasis is refreshing, but offers no further benefit. The oasis might help again if you fought a battle first.") :
        _("A drink at the oasis fills your troops with strength and lifts their spirits.  You can travel a bit further today.");
      move = 800; // + 8TP, from FAQ
      break;

    case MP2::OBJ_WATERINGHOLE:
      msg = visited ? _("The drink at the watering hole is refreshing, but offers no further benefit. The watering hole might help again if you fought a battle first.") :
        _("A drink at the watering hole fills your troops with strength and lifts their spirits. You can travel a bit further today.");
      move = 400; // + 4TP, from FAQ
      break;

    case MP2::OBJ_TEMPLE:
      msg = visited ? _("It doesn't help to pray twice before a battle. Come back after you've fought.") :
        _("A visit and a prayer at the temple raises the morale of your troops.");
      break;

    default: return;
  }

  // check already visited
  if(visited)
  {
    PlaySoundVisited;
    Dialog::Message(MP2::StringObject(obj), msg, Font::BIG, Dialog::OK);
  }
  else
  {
    // modify morale
    hero.SetVisited(dst_index);
    AGG::PlaySound(M82::GOODMRLE);
    DialogMorale(MP2::StringObject(obj), msg, true, (obj == MP2::OBJ_TEMPLE ? 2 : 1));
    hero.IncreaseMovePoints(move);

    // fix double action tile
    hero.SetVisitedWideTile(dst_index, obj);
  }

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}


void ActionToExperienceObject(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  const Maps::Tiles & tile = world.GetTiles(dst_index);

  bool visited = hero.isVisited(tile);
  std::string msg;

  u32 exp = 0;

  switch(obj)
  {
    case MP2::OBJ_GAZEBO:
      msg = visited ? _("An old Knight appears on the steps of the gazebo. \"I am sorry, my liege, I have taught you all I can.\"") :
        _("An old Knight appears on the steps of the gazebo. \"My liege, I will teach you all that I know to aid you in your travels.\"");
      exp = 1000;
      break;

    default: return;
  }

  if(! Settings::Get().MusicMIDI())
    AGG::PlayMusic(MUS::EXPERIENCE, false);

  // check already visited
  if(visited)
  {
    if(Settings::Get().MusicMIDI()){ PlaySoundVisited; }
    Dialog::Message(MP2::StringObject(obj), msg, Font::BIG, Dialog::OK);
  }
  else
  {
    if(Settings::Get().MusicMIDI()){ PlaySoundWarning; }
    DialogWithExp(MP2::StringObject(obj), msg, exp);

    // visit
    hero.SetVisited(dst_index);
    hero.IncreaseExperience(exp);
  }

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}

void ActionToShipwreckSurvivor(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  Maps::Tiles & tile = world.GetTiles(dst_index);

  PlaySoundSuccess;

  if(hero.IsFullBagArtifacts())
  {
    const u16 gold = GoldInsteadArtifact(obj);
    DialogWithGold(MP2::StringObject(obj),
        _("You've pulled a shipwreck survivor from certain death in an unforgiving ocean. Grateful, he says, \"I would give you an artifact as a reward, but you're all full.\""),
        gold, Dialog::OK);
    world.GetKingdom(hero.GetColor()).AddFundsResource(Funds(Resource::GOLD, gold));
  }
  else
  {
    const Artifact & art = tile.QuantityArtifact();
    std::string str = _("You've pulled a shipwreck survivor from certain death in an unforgiving ocean. Grateful, he rewards you for your act of kindness by giving you the %{art}.");
    String::Replace(str, "%{art}", art.GetName());
    Dialog::ArtifactInfo("", str, art);
    hero.PickupArtifact(art);
  }

  AnimationRemoveObject(tile);

  tile.RemoveObjectSprite();
  tile.QuantityReset();

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}

void ActionToArtifact(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  Maps::Tiles & tile = world.GetTiles(dst_index);

  if(hero.IsFullBagArtifacts())
    Dialog::Message("", _("You have no room to carry another artifact!"), Font::BIG, Dialog::OK);
  else
  {
    const u8 cond = tile.QuantityVariant();
    const Artifact & art = tile.QuantityArtifact();

    bool result = false;
    std::string msg;

    // 1,2,3 - gold, gold + res
    if(0 < cond && cond < 4)
    {
      const Funds & payment = tile.QuantityFunds();

      if(1 == cond)
      {
        msg = _("A leprechaun offers you the %{art} for the small price of %{gold} Gold.");
        String::Replace(msg, "%{gold}", payment.Get(Resource::GOLD));
      }
      else
      {
        msg = _("A leprechaun offers you the %{art} for the small price of %{gold} Gold and %{count} %{res}.");

        String::Replace(msg, "%{gold}", payment.Get(Resource::GOLD));
        const ResourceCount & rc = tile.QuantityResourceCount();
        String::Replace(msg, "%{count}", rc.second);
        String::Replace(msg, "%{res}", Resource::String(rc.first));
      }
      String::Replace(msg, "%{art}", art.GetName());
      msg.append("\n");
      msg.append(_("Do you wish to buy this artifact?"));

      PlaySoundWarning;
      if(Dialog::YES == Dialog::ArtifactInfo("", msg, art, Dialog::YES | Dialog::NO))
      {
        if(world.GetKingdom(hero.GetColor()).AllowPayment(payment))
        {
          result = true;
          world.GetKingdom(hero.GetColor()).OddFundsResource(payment);
        }
        else
        {
          PlaySoundFailure;
          Dialog::Message("", _("You try to pay the leprechaun, but realize that you can't afford it. The leprechaun stamps his foot and ignores you."), Font::BIG, Dialog::OK);
        }
      }
      else
        Dialog::Message("", _("Insulted by your refusal of his generous offer, the leprechaun stamps his foot and ignores you."), Font::BIG, Dialog::OK);
    }
    else
      // 4,5 - need have skill wisard or leadership,
      if(3 < cond && cond < 6)
      {
        const Skill::Secondary & skill = tile.QuantitySkill();

        if(hero.HasSecondarySkill(skill.Skill()))
        {
          PlaySoundSuccess;
          msg = _("You've found the artifact: ");
          msg.append(art.GetName());
          Dialog::ArtifactInfo("", msg, art, Dialog::OK);
          result = true;
        }
        else
        {
          PlaySoundFailure;

          if(skill.Skill() == Skill::Secondary::WISDOM)
            msg = _("You've found the humble dwelling of a withered hermit. The hermit tells you that he is willing to give the %{art} to the first wise person he meets.");
          else
            if(skill.Skill() == Skill::Secondary::LEADERSHIP)
              msg = _("You've come across the spartan quarters of a retired soldier. The soldier tells you that he is willing to pass on the %{art} to the first true leader he meets.");
            else
              msg = "FIXME: (unknown condition): %{art}";

          String::Replace(msg, "%{art}", art.GetName());
          Dialog::Message("", msg, Font::BIG, Dialog::OK);
        }
      }
      else
        // 6 - 50 rogues, 7 - 1 gin, 8,9,10,11,12,13 - 1 monster level4
        if(5 < cond && cond < 14)
        {
          bool battle = true;
          Army::army_t army(tile);
          Army::Troop & troop = army.FirstValid();

          PlaySoundWarning;

          if(Monster::ROGUE  == troop().GetID())
            Dialog::Message("", _("You come upon an ancient artifact. As you reach for it, a pack of Rogues leap out of the brush to guard their stolen loot."), Font::BIG, Dialog::OK);
          else
          {
            msg = _("Through a clearing you observe an ancient artifact. Unfortunately, it's guarded by a nearby %{monster}. Do you want to fight the %{monster} for the artifact?");
            String::Replace(msg, "%{monster}", troop.GetName());
            battle = (Dialog::YES == Dialog::Message("", msg, Font::BIG, Dialog::YES | Dialog::NO));
          }

          if(battle)
          {
            // new battle2
            Battle2::Result res = Battle2::Loader(hero.GetArmy(), army, dst_index);
            if(res.AttackerWins())
            {
              hero.IncreaseExperience(res.GetExperienceAttacker());
              result = true;
              PlaySoundSuccess;
              msg = _("Victorious, you take your prize, the %{art}.");
              String::Replace(msg, "%{art}", art.GetName());
              Dialog::ArtifactInfo("", msg, art());
            }
            else
            {
              BattleLose(hero, res, true);
            }
          }
          else
          {
            PlaySoundFailure;
            Dialog::Message("", _("Discretion is the better part of valor, and you decide to avoid this fight for today."), Font::BIG, Dialog::OK);
          }
        }
        else
        {
          PlaySoundSuccess;

          if(Artifact::GetScenario(art))
            msg = Artifact::GetScenario(art);
          else
          {
            msg = _("You've found the artifact: ");
            msg.append("\n");
            msg.append(art.GetName());
          }

          Dialog::ArtifactInfo("", msg, art);
          result = true;
        }

    if(result && hero.PickupArtifact(art))
    {
      PlayPickupSound();
      AnimationRemoveObject(tile);

      tile.RemoveObjectSprite();
      tile.QuantityReset();
    }
  }

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}

void ActionToTreasureChest(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  Maps::Tiles & tile = world.GetTiles(dst_index);
  const std::string & hdr = MP2::StringObject(obj);

  std::string msg;
  u16 gold = tile.QuantityGold();

  PlayPickupSound();
  AnimationRemoveObject(tile);

  // dialog
  if(tile.isWater())
  {
    if(gold)
    {
      const Artifact & art = tile.QuantityArtifact();

      if(art.isValid())
      {
        if(hero.IsFullBagArtifacts())
        {
          gold = GoldInsteadArtifact(obj);
          msg = _("After spending hours trying to fish the chest out of the sea, you open it and find %{gold} gold pieces.");
          String::Replace(msg, "%{gold}", gold);
          DialogWithGold(hdr, msg, gold);
        }
        else
        {
          msg = _("After spending hours trying to fish the chest out of the sea, you open it and find %{gold} gold and the %{art}.");
          String::Replace(msg, "%{gold}", gold);
          String::Replace(msg, "%{art}", art.GetName());
          DialogWithArtifactAndGold(hdr, msg, art, gold);
          hero.PickupArtifact(art);
        }
      }
      else
      {
        msg = _("After spending hours trying to fish the chest out of the sea, you open it and find %{gold} gold pieces.");
        String::Replace(msg, "%{gold}", gold);
        DialogWithGold(hdr, msg, gold);
      }
    }
    else
    {
      Dialog::Message(hdr, _("After spending hours trying to fish the chest out of the sea, you open it, only to find it empty."), Font::BIG, Dialog::OK);
    }
  }
  else
  {
    const Artifact & art = tile.QuantityArtifact();

    if(gold)
    {
      const u16 expr = gold > 500 ? gold - 500 : 500;
      msg = _("After scouring the area, you fall upon a hidden treasure cache. You may take the gold or distribute the gold to the peasants for experience. Do you wish to keep the gold?");

      if(! Dialog::SelectGoldOrExp(hdr, msg, gold, expr))
      {
        gold = 0;
        hero.IncreaseExperience(expr);
      }
    }
    else
      if(art.isValid())
      {
        if(hero.IsFullBagArtifacts())
        {
          gold = GoldInsteadArtifact(obj);
          msg = _("After scouring the area, you fall upon a hidden chest, containing the %{gold} gold pieces.");
          String::Replace(msg, "%{gold}", gold);
          DialogWithGold(hdr, msg, gold);
        }
        else
        {
          msg = _("After scouring the area, you fall upon a hidden chest, containing the ancient artifact %{art}.");
          String::Replace(msg, "%{art}", art.GetName());
          Dialog::ArtifactInfo("", msg, art);
          hero.PickupArtifact(art);
        }
      }
  }

  if(gold)
    world.GetKingdom(hero.GetColor()).AddFundsResource(Funds(Resource::GOLD, gold));

  tile.RemoveObjectSprite();
  tile.QuantityReset();

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}

void ActionToAncientLamp(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  Maps::Tiles & tile = world.GetTiles(dst_index);
  const Army::Troop & troop = tile.QuantityTroop();

  PlaySoundSuccess;
  if(troop.isValid() &&
      Dialog::YES == Dialog::Message(MP2::StringObject(obj), _("You stumble upon a dented and tarnished lamp lodged deep in the earth. Do you wish to rub the lamp?"), Font::BIG, Dialog::YES|Dialog::NO))
    RecruitMonsterFromTile(hero, tile, MP2::StringObject(obj), troop, true);

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}

void ActionToTeleports(Heroes & hero, const s32 & index_from)
{
  s32 index_to = world.NextTeleport(index_from, hero.isShipMaster());
  hero.ApplyPenaltyMovement();

  if(index_from == index_to)
  {
    AGG::PlaySound(M82::RSBRYFZL);
    DEBUG(DBG_GAME, DBG_WARN, "action unsuccessfully...");
    return;
  }

  const Heroes* other_hero = world.GetTiles(index_to).GetHeroes();
  if(other_hero)
  {
    ActionToHeroes(hero, MP2::OBJ_STONELIGHTS, index_to);

    // lose battle
    if(hero.isFreeman())
      return;
    else
      if(! other_hero->isFreeman())
        DEBUG(DBG_GAME, DBG_WARN, "is busy...");
  }

  AGG::PlaySound(M82::KILLFADE);
  hero.GetPath().Hide();
  hero.FadeOut();

  Cursor::Get().Hide();

  hero.Move2Dest(index_to, true);

  Interface::Basic & I = Interface::Basic::Get();
  I.gameArea.SetCenter(hero.GetCenter());
  GameFocus::SetRedraw();
  I.Redraw();

  AGG::PlaySound(M82::KILLFADE);
  hero.GetPath().Hide();
  hero.FadeIn();

  hero.GetPath().Reset();
  hero.ActionNewPosition();

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}

void ActionToWhirlpools(Heroes & hero, const u8 & obj, const s32 & index_from)
{
  const s32 index_to = world.NextWhirlpool(index_from);
  hero.ApplyPenaltyMovement();

  if(index_from == index_to)
  {
    AGG::PlaySound(M82::RSBRYFZL);
    DEBUG(DBG_GAME, DBG_WARN, "action unsuccessfully...");
    return;
  }

  AGG::PlaySound(M82::KILLFADE);
  hero.GetPath().Hide();
  hero.FadeOut();

  Cursor::Get().Hide();

  hero.Move2Dest(index_to, true);

  Interface::Basic & I = Interface::Basic::Get();
  I.gameArea.SetCenter(hero.GetCenter());
  GameFocus::SetRedraw();
  I.Redraw();

  AGG::PlaySound(M82::KILLFADE);
  hero.GetPath().Hide();
  hero.FadeIn();

  Army::Troop & troops = hero.GetArmy().GetWeakestTroop();

  if(Rand::Get(1) && 1 < troops.GetCount())
  {
    PlaySoundWarning;
    Dialog::Message(_("A whirlpool engulfs your ship."), _("Some of your army has fallen overboard."), Font::BIG, Dialog::OK);
    troops.SetCount(Monster::GetCountFromHitPoints(troops(), troops.GetHitPoints() - troops.GetHitPoints() * Game::GetWhirlpoolPercent() / 100));
  }

  hero.GetPath().Reset();
  hero.ActionNewPosition();

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}

void ActionToAbandoneMine(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  PlaySoundWarning;

  if(Dialog::YES == Dialog::Message("",
        _("You come upon an abandoned gold mine. The mine appears to be haunted. Do you wish to enter?"), Font::BIG, Dialog::YES | Dialog::NO))
  {
    ActionToCaptureObject(hero, obj, dst_index);
  }
}

/* capture color object */
void ActionToCaptureObject(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  Maps::Tiles & tile = world.GetTiles(dst_index);
  std::string header;
  std::string body;
  u8 resource = Resource::UNKNOWN;

  switch(obj)
  {
    case MP2::OBJ_ALCHEMYLAB:
      resource = Resource::MERCURY;
      header = MP2::StringObject(obj);
      body = _("You have taken control of the local Alchemist shop. It will provide you with %{count} unit of Mercury per day.");
      break;

    case MP2::OBJ_SAWMILL:
      resource = Resource::WOOD;
      header = MP2::StringObject(obj);
      body = _("You gain control of a sawmill. It will provide you with %{count} units of wood per day.");
      break;

    case MP2::OBJ_MINES:
      {
        resource = tile.QuantityResourceCount().first;
        header = Maps::GetMinesName(resource);

        switch(resource)
        {
          case Resource::ORE:
            body = _("You gain control of an ore mine. It will provide you with %{count} units of ore per day."); break;
          case Resource::SULFUR:
            body = _("You gain control of a sulfur mine. It will provide you with %{count} unit of sulfur per day."); break;
          case Resource::CRYSTAL:
            body = _("You gain control of a crystal mine. It will provide you with %{count} unit of crystal per day."); break;
          case Resource::GEMS:
            body = _("You gain control of a gem mine. It will provide you with %{count} unit of gems per day."); break;
          case Resource::GOLD:
            body = _("You gain control of a gold mine. It will provide you with %{count} gold per day."); break;
          default: break;
        }
      }
      break;

    case MP2::OBJ_ABANDONEDMINE:
      body = _("You beat the Ghosts and are able to restore the mine to production.");
      break;

    case MP2::OBJ_LIGHTHOUSE:
      header = MP2::StringObject(obj);
      body = _("The lighthouse is now under your control, and all of your ships will now move further each turn.");
      break;

    default:
      body = _("You gain control of a %{name}.");
      String::Replace(body, "%{name}", MP2::StringObject(obj));
      break;
  }

  // capture object
  if(! Players::isFriends(hero.GetColor(), tile.QuantityColor()))
  {
    bool capture = true;

    // check guardians
    if(tile.CaptureObjectIsProtection())
    {
      Army::army_t army(tile);
      const Monster & mons =  tile.QuantityMonster();

      Battle2::Result result = Battle2::Loader(hero.GetArmy(), army, dst_index);

      if(result.AttackerWins())
      {
        hero.IncreaseExperience(result.GetExperienceAttacker());
      }
      else
      {
        capture = false;
        BattleLose(hero, result, true);
        if(Settings::Get().ExtWorldSaveMonsterBattle())
          tile.MonsterSetCount(army.MonsterCounts(mons));
      }
    }

    if(capture)
    {
      if(resource == Resource::UNKNOWN)
        Dialog::Message(header, body, Font::BIG, Dialog::OK);
      else
        DialogCaptureResourceObject(header, body, resource);

      // update abandone mine
      if(obj == MP2::OBJ_ABANDONEDMINE)
      {
        tile.UpdateAbandoneMineSprite(tile);
        hero.SetMapsObject(MP2::OBJ_MINES);
      }

      // reset spell info
      Maps::TilesAddon* addon = tile.FindObject(MP2::OBJ_MINES);
      if(addon) addon->tmp  = 0;

      tile.QuantitySetColor(hero.GetColor());

      if(MP2::OBJ_LIGHTHOUSE == obj)
        Maps::ClearFog(dst_index, Game::GetViewDistance(Game::VIEW_LIGHT_HOUSE), hero.GetColor());
    }
  }
  else
    // set guardians
    if(Settings::Get().ExtWorldAllowSetGuardian())
    {
      Army::Troop & troop1 = world.GetCapturedObject(dst_index).GetTroop();
      Army::Troop   troop2 = troop1;

      // check set with spell ?
      Maps::TilesAddon* addon = tile.FindObject(MP2::OBJ_MINES);
      bool readonly = addon ? addon->tmp : false;

      if(Dialog::SetGuardian(hero, troop2, readonly))
        troop1.Set(troop2(), troop2.GetCount());
    }

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName() << " captured: " << MP2::StringObject(obj));
}

void ActionToDwellingJoinMonster(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  Maps::Tiles & tile = world.GetTiles(dst_index);
  const Army::Troop & troop = tile.QuantityTroop();

  if(troop.isValid())
  {
    hero.MovePointsScaleFixed();

    std::string message = _("A group of %{monster} with a desire for greater glory wish to join you. Do you accept?");
    String::Replace(message, "%{monster}", troop.GetMultiName());

    if(! Settings::Get().MusicMIDI() && obj == MP2::OBJ_WATCHTOWER)
      AGG::PlayMusic(MUS::WATCHTOWER, false);
    else
      AGG::PlaySound(M82::EXPERNCE);

    if(Dialog::YES == Dialog::Message(MP2::StringObject(obj), message, Font::BIG, Dialog::YES|Dialog::NO))
    {
      if(! hero.GetArmy().CanJoinTroop(troop))
        Dialog::Message(troop.GetName(), _("You are unable to recruit at this time, your ranks are full."), Font::BIG, Dialog::OK);
      else
      {
        tile.MonsterSetCount(0);
        hero.GetArmy().JoinTroop(troop);
        hero.MovePointsScaleFixed();

        if(Settings::Get().ExtHeroRecalculateMovement())
          hero.RecalculateMovePoints();

        Interface::Basic::Get().SetRedraw(REDRAW_STATUS);
      }
    }
  }
  else
  {
    Dialog::Message("", _("As you approach the dwelling, you notice that there is no one here."), Font::BIG, Dialog::OK);
  }

  hero.SetVisited(dst_index, Visit::GLOBAL);

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName() << ", object: " << MP2::StringObject(obj));
}

void ActionToDwellingRecruitMonster(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  Maps::Tiles & tile = world.GetTiles(dst_index);

  std::string msg_full, msg_void;

  switch(obj)
  {
    case MP2::OBJ_RUINS:
      msg_void = _("You search the ruins, but the Medusas that used to live here are gone. Perhaps there will be more next week.");
      msg_full = _("You've found some Medusas living in the ruins. They are willing to join your army for a price. Do you want to recruit Medusas?");
      AGG::PlayMusic(MUS::DEATH, false);
      break;

    case MP2::OBJ_TREECITY:
      msg_void = _("You've found a Sprite Tree City. Unfortunately, none of the Sprites living there wish to join an army. Maybe next week.");
      msg_full = _("Some of the Sprites living in the tree city are willing to join your army for a price. Do you want to recruit Sprites?");
      AGG::PlayMusic(MUS::TREEHOUSE, false);
      break;

    case MP2::OBJ_WAGONCAMP:
      msg_void = _("A colorful Rogues' wagon stands empty here. Perhaps more Rogues will be here later.");
      msg_full = _("Distant sounds of music and laughter draw you to a colorful wagon housing Rogues. Do you wish to have any Rogues join your army?");
      AGG::PlayMusic(MUS::ARABIAN, false);
      break;

    case MP2::OBJ_DESERTTENT:
      msg_void = _("A group of tattered tents, billowing in the sandy wind, beckons you. The tents are unoccupied. Perhaps more Nomads will be here later.");
      msg_full = _("A group of tattered tents, billowing in the sandy wind, beckons you. Do you wish to have any Nomads join you during your travels?");
      AGG::PlayMusic(MUS::NOMADTENTS, false);
      break;

    case MP2::OBJ_EARTHALTAR:
      msg_void = _("The pit of mud bubbles for a minute and then lies still.");
      msg_full = _("As you approach the bubbling pit of mud, creatures begin to climb out and position themselves around it. In unison they say: \"Mother Earth would like to offer you a few of her troops. Do you want to recruit Earth Elementals?\"");
      break;

    case MP2::OBJ_AIRALTAR:
      msg_void = _("You enter the structure of white stone pillars, and find nothing.");
      msg_full = _("White stone pillars support a roof that rises up to the sky. As you enter the structure, the dead air of the outside gives way to a whirling gust that almost pushes you back out. The air current materializes into a barely visible form. The creature asks, in what can only be described as a loud whisper: \"Why have you come? Are you here to call upon the forces of the air?\"");
      break;

    case MP2::OBJ_FIREALTAR:
      msg_void = _("No Fire Elementals approach you from the lava pool.");
      msg_full = _("Beneath a structure that serves to hold in heat, Fire Elementals move about in a fiery pool of molten lava. A group of them approach you and offer their services. Would you like to recruit Fire Elementals?");
      break;

    case MP2::OBJ_WATERALTAR:
      msg_void = _("A face forms in the water for a moment, and then is gone.");
      msg_full = _("Crystalline structures cast shadows over a small reflective pool of water. You peer into the pool, and a face that is not your own peers back. It asks: \"Would you like to call upon the powers of water?\"");
      break;

    case MP2::OBJ_BARROWMOUNDS:
      msg_void = _("This burial site is deathly still.");
      msg_full = _("Restless spirits of long dead warriors seeking their final resting place offer to join you in hopes of finding peace. Do you wish to recruit ghosts?");
      break;

    default: return;
  }

  const Army::Troop & troop = tile.QuantityTroop();

  if(! troop.isValid())
    Dialog::Message(MP2::StringObject(obj), msg_void, Font::BIG, Dialog::OK);
  else
    if(Dialog::YES == Dialog::Message(MP2::StringObject(obj), msg_full, Font::BIG, Dialog::YES | Dialog::NO))
      RecruitMonsterFromTile(hero, tile, MP2::StringObject(obj), troop, false);

  hero.SetVisited(dst_index, Visit::GLOBAL);

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName() << ", object: " << MP2::StringObject(obj));
}

void ActionToDwellingBattleMonster(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  Maps::Tiles & tile = world.GetTiles(dst_index);

  // yet no one captured.
  const bool & battle = Color::NONE == tile.QuantityColor();
  const Army::Troop & troop = tile.QuantityTroop();

  const char* str_empty = NULL;
  const char* str_recr  = NULL;
  const char* str_warn  = NULL;
  const char* str_wins  = NULL;
  const char* str_scss  = NULL;

  switch(obj)
  {
    case MP2::OBJ_CITYDEAD:
      str_empty = _("The City of the Dead is empty of life, and empty of unlife as well. Perhaps some undead will move in next week.");
      str_recr  = _("Some Liches living here are willing to join your army for a price. Do you want to recruit Liches?");
      str_warn  = _("You've found the ruins of an ancient city, now inhabited solely by the undead. Will you search?");
      str_wins  = _("Some of the surviving Liches are impressed by your victory over their fellows, and offer to join you for a price. Do you want to recruit Liches?");
      break;

    case MP2::OBJ_TROLLBRIDGE:
      str_empty = _("You've found one of those bridges that Trolls are so fond of living under, but there are none here. Perhaps there will be some next week.");
      str_recr  = _("Some Trolls living under a bridge are willing to join your army, but for a price. Do you want to recruit Trolls?");
      str_warn  = _("Trolls living under the bridge challenge you. Will you fight them?");
      str_wins  = _("A few Trolls remain, cowering under the bridge. They approach you and offer to join your forces as mercenaries. Do you want to buy any Trolls?");
      break;

    case MP2::OBJ_DRAGONCITY:
      str_empty = _("The Dragon city has no Dragons willing to join you this week. Perhaps a Dragon will become available next week.");
      str_recr  = _("The Dragon city is willing to offer some Dragons for your army for a price. Do you wish to recruit Dragons?");
      str_warn  = _("You stand before the Dragon City, a place off-limits to mere humans. Do you wish to violate this rule and challenge the Dragons to a fight?");
      str_wins  = _("Having defeated the Dragon champions, the city's leaders agree to supply some Dragons to your army for a price. Do you wish to recruit Dragons?");
      break;

    default: return;
  }

  if(!battle)
  {
    if(troop.isValid())
    {
      PlaySoundSuccess;
      str_scss = str_recr;
    }
    else
    {
      PlaySoundVisited;
      Dialog::Message(MP2::StringObject(obj), str_empty, Font::BIG, Dialog::OK);
    }
  }
  else
  {
    Army::army_t army(tile);

    PlaySoundWarning;
    if(Dialog::YES == Dialog::Message(MP2::StringObject(obj), str_warn, Font::BIG, Dialog::YES | Dialog::NO))
    {
      // new battle2
      Battle2::Result res = Battle2::Loader(hero.GetArmy(), army, dst_index);
      if(res.AttackerWins())
      {
        hero.IncreaseExperience(res.GetExperienceAttacker());
        tile.QuantitySetColor(hero.GetColor());
        tile.SetObjectPassable(true);
        PlaySoundSuccess;
        str_scss = str_wins;
      }
      else
      {
        BattleLose(hero, res, true);
      }
    }
  }

  // recruit monster
  if(str_scss)
  {
    if(troop.isValid() &&
        Dialog::YES == Dialog::Message(MP2::StringObject(obj), str_scss, Font::BIG, Dialog::YES | Dialog::NO))
      RecruitMonsterFromTile(hero, tile, MP2::StringObject(obj), troop, false);

    hero.SetVisited(dst_index, Visit::GLOBAL);
  }

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName() << ", object: " << MP2::StringObject(obj));
}

void ActionToObservationTower(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  PlaySoundWarning;
  Dialog::Message(MP2::StringObject(obj), _("From the observation tower, you are able to see distant lands."), Font::BIG, Dialog::OK);
  Maps::ClearFog(dst_index, Game::GetViewDistance(Game::VIEW_OBSERVATION_TOWER), hero.GetColor());
}

void ActionToArtesianSpring(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  const u16 max = hero.GetMaxSpellPoints();
  const std::string & name = MP2::StringObject(MP2::OBJ_ARTESIANSPRING);

  if(hero.isVisited(MP2::OBJ_ARTESIANSPRING))
  {
    PlaySoundVisited;
    Dialog::Message(name, _("The spring only refills once a week, and someone's already been here this week."), Font::BIG, Dialog::OK);
  }
  else
    if(hero.GetSpellPoints() == max * 2)
    {
      PlaySoundFailure;
      Dialog::Message(name, _("A drink at the spring is supposed to give you twice your normal spell points, but you are already at that level."), Font::BIG, Dialog::OK);
    }
    else
    {
      if(Settings::Get().MusicMIDI())
      {
        PlaySoundSuccess;
      }
      else
        AGG::PlayMusic(MUS::WATERSPRING, false);
      hero.SetSpellPoints(max * 2);
      Dialog::Message(name, _("A drink from the spring fills your blood with magic! You have twice your normal spell points in reserve."), Font::BIG, Dialog::OK);

      if(Settings::Get().ExtWorldArtesianSpringSeparatelyVisit())
        hero.SetVisited(dst_index, Visit::LOCAL);
      else
        // fix double action tile
        hero.SetVisitedWideTile(dst_index, obj, Visit::LOCAL);
    }

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}

void ActionToXanadu(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  const Maps::Tiles & tile = world.GetTiles(dst_index);

  if(hero.isVisited(tile))
  {
    PlaySoundVisited;
    Dialog::Message(MP2::StringObject(obj), _("Recognizing you, the butler refuses to admit you. \"The master,\" he says, \"will not see the same student twice.\""), Font::BIG, Dialog::OK);
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
      Dialog::Message(MP2::StringObject(obj), _("The butler admits you to see the master of the house. He trains you in the four skills a hero should know."), Font::BIG, Dialog::OK);
      hero.IncreasePrimarySkill(Skill::Primary::ATTACK);
      hero.IncreasePrimarySkill(Skill::Primary::DEFENSE);
      hero.IncreasePrimarySkill(Skill::Primary::KNOWLEDGE);
      hero.IncreasePrimarySkill(Skill::Primary::POWER);
      hero.SetVisited(dst_index);
    }
    else
    {
      PlaySoundFailure;
      Dialog::Message(MP2::StringObject(obj), _("The butler opens the door and looks you up and down. \"You are neither famous nor diplomatic enough to be admitted to see my master,\" he sniffs. \"Come back when you think yourself worthy.\""), Font::BIG, Dialog::OK);
    }
  }

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}

bool ActionToUpgradeArmy(Army::army_t & army, const Monster & mons, std::string & str1, std::string & str2)
{
  if(army.HasMonster(mons))
  {
    army.UpgradeMonsters(mons);
    if(str1.size()) str1 += ", ";
    str1 += mons.GetMultiName();
    if(str2.size()) str2 += ", ";
    str2 += mons.GetUpgrade().GetMultiName();
    return true;
  }
  return false;
}

void ActionToUpgradeArmyObject(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  std::string monsters;
  std::string monsters_upgrade;
  std::string msg1;
  std::string msg2;

  std::vector<Monster> mons;
  mons.reserve(3);

  hero.MovePointsScaleFixed();

  switch(obj)
  {
    case MP2::OBJ_HILLFORT:
      if(ActionToUpgradeArmy(hero.GetArmy(), Monster(Monster::DWARF), monsters, monsters_upgrade))
        mons.push_back(Monster(Monster::DWARF));
      if(ActionToUpgradeArmy(hero.GetArmy(), Monster(Monster::ORC), monsters, monsters_upgrade))
        mons.push_back(Monster(Monster::ORC));
      if(ActionToUpgradeArmy(hero.GetArmy(), Monster(Monster::OGRE), monsters, monsters_upgrade))
        mons.push_back(Monster(Monster::OGRE));

      msg1 = _("All of the %{monsters} you have in your army have been trained by the battle masters of the fort. Your army now contains %{monsters2}.");
      String::Replace(msg1, "%{monsters}", monsters);
      String::Replace(msg1, "%{monsters2}", monsters_upgrade);
      msg2 = _("An unusual alliance of Orcs, Ogres, and Dwarves offer to train (upgrade) any such troops brought to them. Unfortunately, you have none with you.");
      break;

    case MP2::OBJ_FREEMANFOUNDRY:
      if(ActionToUpgradeArmy(hero.GetArmy(), Monster(Monster::PIKEMAN), monsters, monsters_upgrade))
        mons.push_back(Monster(Monster::PIKEMAN));
      if(ActionToUpgradeArmy(hero.GetArmy(), Monster(Monster::SWORDSMAN), monsters, monsters_upgrade))
        mons.push_back(Monster(Monster::SWORDSMAN));
      if(ActionToUpgradeArmy(hero.GetArmy(), Monster(Monster::IRON_GOLEM), monsters, monsters_upgrade))
        mons.push_back(Monster(Monster::IRON_GOLEM));

      msg1 = _("All of your %{monsters} have been upgraded into %{monsters2}.");
      String::Replace(msg1, "%{monsters}", monsters);
      String::Replace(msg1, "%{monsters2}", monsters_upgrade);
      msg2 = _("A blacksmith working at the foundry offers to convert all Pikemen and Swordsmen's weapons brought to him from iron to steel. He also says that he knows a process that will convert Iron Golems into Steel Golems.  Unfortunately, you have none of these troops in your army, so he can't help you.");
      break;

    default: break;
  }

  if(mons.size())
  {
    // composite sprite
    u8 ox = 0;
    const Sprite & br = AGG::GetICN(ICN::STRIP, 12);
    Surface sf(br.w() * mons.size() + (mons.size() - 1) * 4, br.h());

    for(std::vector<Monster>::const_iterator
        it = mons.begin(); it != mons.end(); ++it)
    {
      br.Blit(ox, 0, sf);
      switch(Monster(*it).GetRace())
      {
        case Race::KNGT:    AGG::GetICN(ICN::STRIP, 4).Blit(ox + 6, 6, sf); break;
        case Race::BARB:    AGG::GetICN(ICN::STRIP, 5).Blit(ox + 6, 6, sf); break;
        case Race::SORC:    AGG::GetICN(ICN::STRIP, 6).Blit(ox + 6, 6, sf); break;
        case Race::WRLK:    AGG::GetICN(ICN::STRIP, 7).Blit(ox + 6, 6, sf); break;
        case Race::WZRD:    AGG::GetICN(ICN::STRIP, 8).Blit(ox + 6, 6, sf); break;
        case Race::NECR:    AGG::GetICN(ICN::STRIP, 9).Blit(ox + 6, 6, sf); break;
        default:        AGG::GetICN(ICN::STRIP, 10).Blit(ox + 6, 6, sf); break;
      }
      const Sprite & mon = AGG::GetICN((*it).GetUpgrade().ICNMonh(), 0);
      mon.Blit(ox + 6 + mon.x(), 6 + mon.y(), sf);
      ox += br.w() + 4;
    }
    Dialog::SpriteInfo(MP2::StringObject(obj), msg1, sf);

    if(Settings::Get().ExtHeroRecalculateMovement())
      hero.RecalculateMovePoints();
  }
  else
  {
    PlaySoundFailure;
    Dialog::Message(MP2::StringObject(obj), msg2, Font::BIG, Dialog::OK);
  }

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}

void ActionToMagellanMaps(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  if(1000 > world.GetKingdom(hero.GetColor()).GetFundsGold())
  {
    PlaySoundFailure;
    Dialog::Message(MP2::StringObject(obj), _("The captain sighs. \"You don't have enough money, eh?  You can't expect me to give my maps away for free!\""), Font::BIG, Dialog::OK);
  }
  else
  {
    PlaySoundWarning;
    if(Dialog::YES == Dialog::Message(MP2::StringObject(obj), _("A retired captain living on this refurbished fishing platform offers to sell you maps of the sea he made in his younger days for 1,000 gold. Do you wish to buy the maps?"), Font::BIG, Dialog::YES | Dialog::NO))
      world.ActionForMagellanMaps(hero.GetColor());

    GameFocus::SetRedraw();
  }

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}

void ActionToEvent(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  // check event maps
  EventMaps* event_ptr = world.GetEventMaps(hero.GetColor(), dst_index);

  if(event_ptr)
  {
    EventMaps & event_maps = *event_ptr;

    hero.SetMove(false);

    if(event_maps.resource.GetValidItems())
    {
      world.GetKingdom(hero.GetColor()).AddFundsResource(event_maps.resource);
      PlaySoundSuccess;
      Dialog::ResourceInfo("", event_maps.message, event_maps.resource);
    }
    else
      if(event_maps.message.size())
        Dialog::Message("", event_maps.message, Font::BIG, Dialog::OK);

    const Artifact & art = event_maps.artifact;
    if(art.isValid())
    {
      if(hero.PickupArtifact(art))
      {
        PlayPickupSound();
        std::string message(_("You find %{artifact}."));
        String::Replace(message, "%{artifact}", art.GetName());
        Dialog::ArtifactInfo("", message, art);
      }
    }

    event_maps.SetVisited(hero.GetColor());

    if(event_maps.cancel)
      hero.SetMapsObject(MP2::OBJ_ZERO);
  }

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}

void ActionToObelisk(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  Kingdom & kingdom = world.GetKingdom(hero.GetColor());
  if(!hero.isVisited(world.GetTiles(dst_index), Visit::GLOBAL))
  {
    hero.SetVisited(dst_index, Visit::GLOBAL);
    kingdom.PuzzleMaps().Update(kingdom.CountVisitedObjects(MP2::OBJ_OBELISK), world.CountObeliskOnMaps());
    AGG::PlaySound(M82::EXPERNCE);
    Dialog::Message(MP2::StringObject(obj), _("You come upon an obelisk made from a type of stone you have never seen before. Staring at it intensely, the smooth surface suddenly changes to an inscription. The inscription is a piece of a lost ancient map. Quickly you copy down the piece and the inscription vanishes as abruptly as it appeared."), Font::BIG, Dialog::OK);
    kingdom.PuzzleMaps().ShowMapsDialog();
  }
  else
    Dialog::Message(MP2::StringObject(obj), _("You have already been to this obelisk."), Font::BIG, Dialog::OK);

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}

void ActionToTreeKnowledge(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  const Maps::Tiles & tile = world.GetTiles(dst_index);

  if(hero.isVisited(tile))
  {
    PlaySoundVisited;
    Dialog::Message(MP2::StringObject(obj), _("Upon your approach, the tree opens its eyes in delight. \"It is good to see you, my student. I hope my teachings have helped you.\""), Font::BIG, Dialog::OK);
  }
  else
  {
    const Funds & funds = tile.QuantityFunds();
    bool conditions = 0 == funds.GetValidItems();
    std::string msg;

    // free
    if(conditions)
    {
      const Sprite & sprite = AGG::GetICN(ICN::EXPMRL, 4);
      msg = _("Upon your approach, the tree opens its eyes in delight. \"Ahh, an adventurer! Allow me to teach you a little of what I have learned over the ages.\"");
      Dialog::SpriteInfo(MP2::StringObject(obj), msg, sprite);
    }
    else
    {
      const ResourceCount & rc = tile.QuantityResourceCount();

      if(world.GetKingdom(hero.GetColor()).AllowPayment(funds))
      {
        msg = _("Upon your approach, the tree opens its eyes in delight.");
        msg.append("\n");
        msg.append(_("\"Ahh, an adventurer! I will be happy to teach you a little of what I have learned over the ages for a mere %{count} %{res}.\""));
        msg.append("\n");
        msg.append(_("(Just bury it around my roots.)"));
        String::Replace(msg, "%{res}", Resource::String(rc.first));
        String::Replace(msg, "%{count}", rc.second);
        conditions = Dialog::YES == Dialog::SpriteInfo(MP2::StringObject(obj), msg, AGG::GetICN(ICN::EXPMRL, 4), Dialog::YES|Dialog::NO);
      }
      else
      {
        msg = _("Tears brim in the eyes of the tree.");
        msg.append("\n");
        msg.append(_("\"I need %{count} %{res}.\""));
        msg.append("\n");
        msg.append(_("it whispers. (sniff) \"Well, come back when you can pay me.\""));
        String::Replace(msg, "%{res}", Resource::String(rc.first));
        String::Replace(msg, "%{count}", rc.second);
        Dialog::Message(MP2::StringObject(obj), msg, Font::BIG, Dialog::OK);
      }
    }

    if(conditions)
    {
      world.GetKingdom(hero.GetColor()).OddFundsResource(funds);
      hero.SetVisited(dst_index);
      hero.IncreaseExperience(hero.GetExperienceFromLevel(hero.GetLevel()) - hero.GetExperience());
    }
  }

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}

void ActionToOracle(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  Dialog::ThievesGuild(true);
  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}

void ActionToDaemonCave(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  Maps::Tiles & tile = world.GetTiles(dst_index);

  AGG::PlayMusic(MUS::DEMONCAVE, false);

  if(Dialog::YES == Dialog::Message(MP2::StringObject(obj), _("The entrance to the cave is dark, and a foul, sulfurous smell issues from the cave mouth. Will you enter?"), Font::BIG, Dialog::YES|Dialog::NO))
  {
    u8 variant = tile.QuantityVariant();

    if(variant)
    {
      const u16 gold = tile.QuantityGold();
      std::string msg;

      if(variant == 2 && hero.IsFullBagArtifacts())
        variant = 3;

      if(Dialog::YES == Dialog::Message("", _("You find a powerful and grotesque Demon in the cave. \"Today,\" it rasps, \"you will fight and surely die. But I will give you a choice of deaths. You may fight me, or you may fight my servants. Do you prefer to fight my servants?\""), Font::BIG, Dialog::YES|Dialog::NO))
      {
        // battle with earth elements
        Army::army_t army(tile);

        Battle2::Result res = Battle2::Loader(hero.GetArmy(), army, dst_index);
        if(res.AttackerWins())
        {
          hero.IncreaseExperience(res.GetExperienceAttacker());
          msg = _("Upon defeating the daemon's servants, you find a hidden cache with %{count} gold.");
          String::Replace(msg, "%{count}", gold);
          DialogWithGold("", msg, gold);
          world.GetKingdom(hero.GetColor()).AddFundsResource(Funds(Resource::GOLD, gold));
        }
        else
        {
          BattleLose(hero, res, true);
        }
      }
      // check variants
      else
        if(1 == variant)
        {
          const u16 exp = 1000;
          msg = _("The Demon screams its challenge and attacks! After a short, desperate battle, you slay the monster and receive %{exp} experience points.");
          String::Replace(msg, "%{exp}", exp);
          DialogWithExp("", msg, exp);
          hero.IncreaseExperience(exp);
        }
        else
          if(2 == variant)
          {
            const u16 exp = 1000;
            const Artifact & art = tile.QuantityArtifact();
            msg = _("The Demon screams its challenge and attacks! After a short, desperate battle, you slay the monster and find the %{art} in the back of the cave.");
            String::Replace(msg, "%{art}", art.GetName());
            if(art.isValid()) DialogArtifactWithExp("", msg, art, exp);
            hero.PickupArtifact(art);
            hero.IncreaseExperience(exp);
          }
          else
            if(3 == variant)
            {
              const u16 exp = 1000;
              msg = _("The Demon screams its challenge and attacks! After a short, desperate battle, you slay the monster and receive %{exp) experience points and %{count} gold.");
              String::Replace(msg, "%{exp}", exp);
              String::Replace(msg, "%{count}", gold);
              DialogGoldWithExp("", msg, gold, exp);
              hero.IncreaseExperience(exp);
              world.GetKingdom(hero.GetColor()).AddFundsResource(Funds(Resource::GOLD, gold));
            }
            else
            {
              bool remove = true;

              msg = gold <= world.GetKingdom(hero.GetColor()).GetFundsGold() ?
                _("The Demon leaps upon you and has its claws at your throat before you can even draw your sword. \"Your life is mine,\" it says. \"I will sell it back to you for %{count} gold.\"") :
                _("Seeing that you do not have %{count} gold, the demon slashes you with its claws, and the last thing you see is a red haze.");
              String::Replace(msg, "%{count}", gold);

              if(gold <= world.GetKingdom(hero.GetColor()).GetFundsGold())
              {
                if(Dialog::YES == Dialog::Message("", msg, Font::BIG, Dialog::YES|Dialog::NO))
                {
                  remove = false;
                  world.GetKingdom(hero.GetColor()).OddFundsResource(Funds(Resource::GOLD, gold));
                }
              }
              else
                Dialog::Message("", msg, Font::BIG, Dialog::OK);

              if(remove)
              {
                Battle2::Result res;
                res.army1 = Battle2::RESULT_LOSS;
                BattleLose(hero, res, true);
              }
            }

      tile.QuantityReset();
    }
    else
      Dialog::Message("", _("Except for evidence of a terrible battle, the cave is empty."), Font::BIG, Dialog::OK);

    hero.SetVisited(dst_index, Visit::GLOBAL);
  }

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}

void ActionToAlchemistsTower(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  /*
     As you enter the Alchemist's Tower, a hobbled, graying man in a brown cloak makes his way towards you. e checks your pack, and sees
     that you have 1 cursed item. For 750 gold, the alchemist will remove it for you. Do you pay?
     As you enter the Alchemist's Tower, a hobbled, graying man in a brown cloak makes his way towards you. He checks your pack, and sees
     that you have %d cursed items. For 750 gold, the alchemist will remove them for you. Do you pay?
     You hear a voice from behind the locked door, "You don't have enough gold to pay for my services."
     You hear a voice from high above in the tower, "Go away! I can't help you!"
     */
  Dialog::Message("", _("You hear a voice from high above in the tower, \"Go away! I can't help you!\""), Font::BIG, Dialog::OK);

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}

void ActionToStables(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  const bool cavalry = hero.GetArmy().HasMonster(Monster::CAVALRY);
  const bool visited = hero.isVisited(obj);
  std::string body;

  if(!cavalry && visited)
    body = _("The head groom approaches you and speaks, \"You already have a fine horse, and have no inexperienced cavalry which might make use of our trained war horses.\"");
  else
    if(!cavalry && !visited)
      body = _("As you approach the stables, the head groom appears, leading a fine looking war horse. \"This steed will help speed you in your travels. Alas, his endurance will wane with a lot of heavy riding, and you must return for a fresh mount in a week. We also have many fine war horses which could benefit mounted soldiers, but you have none we can help.\"");
    else
      if(cavalry && visited)
        body = _("The head groom speaks to you, \"That is a fine looking horse you have. I am afraid we can give you no better, but the horses your cavalry are riding look to be of poor breeding stock. We have many trained war horses which would aid your riders greatly. I insist you take them.\"");
      else
        if(cavalry && !visited)
          body = _("As you approach the stables, the head groom appears, leading a fine looking war horse. \"This steed will help speed you in your travels. Alas, he will grow tired in a week. You must also let me give better horses to your mounted soldiers, their horses look shoddy and weak.\"");


  // check already visited
  if(visited)
  {
    PlaySoundVisited;
  }
  else
  {
    hero.SetVisited(dst_index);
    PlaySoundSuccess;
    hero.IncreaseMovePoints(400);
  }

  if(cavalry) hero.GetArmy().UpgradeMonsters(Monster::CAVALRY);

  Dialog::Message("", body, Font::BIG, Dialog::OK);

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}

void ActionToArena(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  if(hero.isVisited(obj))
  {
    PlaySoundVisited;
    Dialog::Message(MP2::StringObject(obj), _("The Arena guards turn you away."), Font::BIG, Dialog::OK);
  }
  else
  {
    hero.SetVisited(dst_index);
    PlaySoundSuccess;
    hero.IncreasePrimarySkill(Dialog::SelectSkillFromArena());
  }

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}

void ActionToSirens(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  if(hero.isVisited(obj))
  {
    PlaySoundVisited;
    Dialog::Message(MP2::StringObject(obj), _("As the sirens sing their eerie song, your small, determined army manages to overcome the urge to dive headlong into the sea."), Font::BIG, Dialog::OK);
  }
  else
  {
    u32 exp = hero.GetArmy().ActionToSirens();
    std::string str = _("You have your crew stop up their ears with wax before the sirens' eerie song has any chance of luring them to a watery grave. An eerie wailing song emanates from the sirens perched upon the rocks. Many of your crew fall under its spell, and dive into the water where they drown. You are now wiser for the visit, and gain %{exp} experience.");
    String::Replace(str, "%{exp}", exp);

    hero.SetVisited(dst_index);
    PlaySoundSuccess;
    Dialog::Message(MP2::StringObject(obj), str, Font::BIG, Dialog::OK);
    hero.IncreaseExperience(exp);
  }

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}

void ActionToJail(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  Kingdom & kingdom = world.GetKingdom(hero.GetColor());

  if(kingdom.AllowRecruitHero(false, 0))
  {
    Maps::Tiles & tile = world.GetTiles(dst_index);
    PlaySoundSuccess;
    Dialog::Message(MP2::StringObject(obj), _("In a dazzling display of daring, you break into the local jail and free the hero imprisoned there, who, in return, pledges loyalty to your cause."), Font::BIG, Dialog::OK);

    AnimationRemoveObject(tile);
    tile.RemoveObjectSprite();
    tile.SetObject(MP2::OBJ_ZERO);

    Heroes *prisoner = world.FromJail(dst_index);

    if(prisoner)
    {
      const Point center(dst_index % world.w(), dst_index / world.w());
      if(prisoner->Recruit(hero.GetColor(), center))
        prisoner->ResetModes(Heroes::JAIL);
    }
  }
  else
  {
    std::string str = _("You already have %{count} heroes, and regretfully must leave the prisoner in this jail to languish in agony for untold days.");
    String::Replace(str, "%{count}", Kingdom::GetMaxHeroes());
    PlaySoundFailure;
    Dialog::Message(MP2::StringObject(obj), str, Font::BIG, Dialog::OK);
  }

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}

void ActionToHutMagi(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  Dialog::Message(MP2::StringObject(obj), _("You enter a rickety hut and talk to the magician who lives there. He tells you of places near and far which may aid you in your journeys."), Font::BIG, Dialog::OK);

  if(!hero.isVisited(obj, Visit::GLOBAL))
  {
    hero.SetVisited(dst_index, Visit::GLOBAL);
    world.ActionToEyeMagi(hero.GetColor());
  }

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}

void ActionToEyeMagi(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  Dialog::Message(MP2::StringObject(obj), _("This eye seems to be intently studying its surroundings."), Font::BIG, Dialog::OK);

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}

void ActionToSphinx(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  Riddle* riddle = world.GetSphinxRiddle(dst_index);

  if(riddle && riddle->valid)
  {
    if(Dialog::YES == Dialog::Message("", _("\"I have a riddle for you,\" the Sphinx says. \"Answer correctly, and you shall be rewarded. Answer incorrectly, and you shall be eaten. Do you accept the challenge?\""), Font::BIG, Dialog::YES|Dialog::NO))
    {
      std::string header(_("The Sphinx asks you the following riddle: %{riddle}. Your answer?"));
      String::Replace(header, "%{riddle}", riddle->message);
      std::string answer;
      Dialog::InputString(header, answer);
      if(riddle->AnswerCorrect(answer))
      {
        const Funds & res = riddle->resource;
        const Artifact art = riddle->artifact;
        const std::string say = _("Looking somewhat disappointed, the Sphinx sighs. You've answered my riddle so here's your reward. Now begone.");
        const u8 count = res.GetValidItems();

        if(count)
        {
          if(1 == count && res.gold && art.isValid())
            DialogWithArtifactAndGold("", say, art, res.gold);
          else
          {
            Dialog::ResourceInfo("", say, res);
            if(art.isValid()) Dialog::ArtifactInfo("", say, art);
          }
        }
        else
          if(art.isValid()) Dialog::ArtifactInfo("", say, art);

        riddle->SetQuiet();
        hero.SetVisited(dst_index, Visit::GLOBAL);

        if(art.isValid())
          hero.PickupArtifact(art);

        if(count)
          world.GetKingdom(hero.GetColor()).AddFundsResource(res);
      }
      else
      {
        Dialog::Message("", _("\"You guessed incorrectly,\" the Sphinx says, smiling. The Sphinx swipes at you with a paw, knocking you to the ground. Another blow makes the world go black, and you know no more."), Font::BIG, Dialog::OK);
        Battle2::Result res;
        res.army1 = Battle2::RESULT_LOSS;
        BattleLose(hero, res, true);
      }
    }
  }
  else
    Dialog::Message(MP2::StringObject(obj), _("You come across a giant Sphinx. The Sphinx remains strangely quiet."), Font::BIG, Dialog::OK);

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}

void ActionToBarrier(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  Maps::Tiles & tile = world.GetTiles(dst_index);
  Kingdom & kingdom = world.GetKingdom(hero.GetColor());

  if(kingdom.IsVisitTravelersTent(tile.QuantityColor()))
  {
    Dialog::Message(MP2::StringObject(obj),
        _("A magical barrier stands tall before you, blocking your way. Runes on the arch read,\n\"Speak the key and you may pass.\"\nAs you speak the magic word, the glowing barrier dissolves into nothingness."),
        Font::BIG, Dialog::OK);

    AnimationRemoveObject(tile);
    tile.RemoveObjectSprite();
    tile.SetObject(MP2::OBJ_ZERO);
  }
  else
  {
    Dialog::Message(MP2::StringObject(obj),
        _("A magical barrier stands tall before you, blocking your way. Runes on the arch read,\n\"Speak the key and you may pass.\"\nYou speak, and nothing happens."),
        Font::BIG, Dialog::OK);
  }

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}

void ActionToTravellersTent(Heroes & hero, const u8 & obj, const s32 & dst_index)
{
  Dialog::Message(MP2::StringObject(obj),
      _("You enter the tent and see an old woman gazing into a magic gem. She looks up and says,\n\"In my travels, I have learned much in the way of arcane magic. A great oracle taught me his skill. I have the answer you seek.\""),
      Font::BIG, Dialog::OK);

  const Maps::Tiles & tile = world.GetTiles(dst_index);
  Kingdom & kingdom = world.GetKingdom(hero.GetColor());

  kingdom.SetVisitTravelersTent(tile.QuantityColor());

  DEBUG(DBG_GAME, DBG_INFO, hero.GetName());
}