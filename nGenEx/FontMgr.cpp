/*  Project nGenEx
	Destroyer Studios LLC
	Copyright © 1997-2004. All Rights Reserved.

	SUBSYSTEM:    nGenEx.lib
	FILE:         FontMgr.cpp
	AUTHOR:       John DiCamillo


	OVERVIEW
	========
	Font Resource Manager class implementation
*/

#include "MemDebug.h"
#include "FontMgr.h"

// +--------------------------------------------------------------------+

std::vector<FontItem> FontMgr::fonts;

// +--------------------------------------------------------------------+

void
FontMgr::Close()
{
	fonts.clear();
}

// +--------------------------------------------------------------------+

void
FontMgr::Register(const char* name, Font* font)
{
	FontItem* item = new(__FILE__,__LINE__) FontItem;

	if (item) {
		item->name = name;
		item->size = 0;
		item->font = font;

		fonts.push_back(*item);
	}
}

// +--------------------------------------------------------------------+

Font*
FontMgr::Find(const char* name)
{
	for (auto fiter = fonts.begin(); fiter != fonts.end(); ++fiter) {
		if (fiter->name == name)
			return fiter->font;
	}

	return 0;
}
