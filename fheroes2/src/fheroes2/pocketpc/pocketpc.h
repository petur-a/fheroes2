/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov                               *
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

#ifndef H2POCKETPC_H
#define H2POCKETPC_H

#include "game.h"
#include "dialog.h"
#include "maps_fileinfo.h"
#include "gamedefs.h"

namespace PocketPC
{
    Game::menu_t	MainMenu(void);
    Game::menu_t	SelectScenario(void);
    Game::menu_t	ScenarioInfo(void);
    Game::menu_t	LoadGame(void);
    bool		DialogSelectMapsFileList(MapsFileInfoList &, std::string &);
    Dialog::answer_t	HeroesOpenDialog(Heroes &, bool);
    Dialog::answer_t	CastleOpenDialog(Castle &);
    Dialog::answer_t	DialogArmyInfo(const Army::BattleTroop &, u16);
};

#endif