/*  Project Starshatter 4.5
	Destroyer Studios LLC
	Copyright � 1997-2004. All Rights Reserved.

	SUBSYSTEM:    Stars.exe
	FILE:         CombatEvent.cpp
	AUTHOR:       John DiCamillo


	OVERVIEW
	========
	A significant (newsworthy) event in the dynamic campaign.
*/

#include "MemDebug.h"
#include "CombatEvent.h"
#include "CombatGroup.h"
#include "Campaign.h"
#include "Player.h"
#include "ShipDesign.h"
#include "Ship.h"

#include "Term.h"
#include "ParseUtil.h"
#include "FormatUtil.h"
#include "DataLoader.h"

// +----------------------------------------------------------------------+

CombatEvent::CombatEvent(Campaign* c, int typ, int tim, int tem, 
int src, const char* rgn)
: campaign(c), type(typ), time(tim), team(tem), source(src),
region(rgn), points(0), visited(false)
{ }

// +----------------------------------------------------------------------+

const char*
CombatEvent::SourceName() const
{
	return SourceName(source);
}

// +----------------------------------------------------------------------+

const char*
CombatEvent::TypeName() const
{
	return TypeName(type);
}

// +----------------------------------------------------------------------+

const char*
CombatEvent::SourceName(int n)
{
	switch (n) {
	case FORCOM:      return "FORCOM";
	case TACNET:      return "TACNET";
	case INTEL:       return "SECURE";
	case MAIL:        return "Mail";
	case NEWS:        return "News";
	}

	return "Unknown";
}

int
CombatEvent::SourceFromName(const char* n)
{
	for (int i = FORCOM; i <= NEWS; i++)
	if (!_stricmp(n, SourceName(i)))
	return i;

	return -1;
}

// +----------------------------------------------------------------------+

const char*
CombatEvent::TypeName(int n)
{
	switch (n) {
	case ATTACK:            return "ATTACK";
	case DEFEND:            return "DEFEND";
	case MOVE_TO:           return "MOVE_TO";
	case CAPTURE:           return "CAPTURE";
	case STRATEGY:          return "STRATEGY";
	case STORY:             return "STORY";
	case CAMPAIGN_START:    return "CAMPAIGN_START";
	case CAMPAIGN_END:      return "CAMPAIGN_END";
	case CAMPAIGN_FAIL:     return "CAMPAIGN_FAIL";
	}

	return "Unknown";
}

int
CombatEvent::TypeFromName(const char* n)
{
	for (int i = ATTACK; i <= CAMPAIGN_FAIL; i++)
	if (!_stricmp(n, TypeName(i)))
	return i;

	return -1;
}

// +----------------------------------------------------------------------+

void
CombatEvent::Load()
{
	DataLoader* loader   = DataLoader::GetLoader();

	if (!campaign || !loader)
	return;

	loader->SetDataPath(campaign->Path());

	if (file.length() > 0) {
		const char* filename = file.data();
		BYTE*       block    = 0;

		loader->LoadBuffer(filename, block, true);
		info = (const char*) block;
		loader->ReleaseBuffer(block);

		if (info.contains('$')) {
			Player*        player = Player::GetCurrentPlayer();
			CombatGroup*   group  = campaign->GetPlayerGroup();

			if (player) {
				info = FormatTextReplace(info, "$NAME",  player->Name().data());
				info = FormatTextReplace(info, "$RANK",  Player::RankName(player->Rank()));
			}

			if (group) {
				info = FormatTextReplace(info, "$GROUP", group->GetDescription());
			}

			char timestr[32];
			FormatDayTime(timestr, campaign->GetTime(), true);
			info = FormatTextReplace(info, "$TIME", timestr);
		}
	}

	if (type < CAMPAIGN_END && image_file.length() > 0) {
		loader->LoadBitmap(image_file, image);
	}

	loader->SetDataPath(0);
}

