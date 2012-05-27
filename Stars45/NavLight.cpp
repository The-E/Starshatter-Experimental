/*  Project Starshatter 5.0
	Destroyer Studios LLC
	Copyright � 1997-2007. All Rights Reserved.

	SUBSYSTEM:    Stars.exe
	FILE:         NavLight.cpp
	AUTHOR:       John DiCamillo


	OVERVIEW
	========
	Navigation Light System class
*/

#include "MemDebug.h"
#include "NavLight.h"

#include "Game.h"
#include "Bitmap.h"
#include "DataLoader.h"

// +----------------------------------------------------------------------+

static Bitmap* images[4];

// +----------------------------------------------------------------------+

NavLight::NavLight(double p, double s)
: System(COMPUTER, 32, "Navigation Lights", 1, 0),
period(p), nlights(0), scale(s), enable(true)
{
	name = Game::GetText("sys.nav-light");
	abrv = Game::GetText("sys.nav-light.abrv");

	ZeroMemory(beacon,      sizeof(beacon));
	ZeroMemory(beacon_type, sizeof(beacon_type));
	ZeroMemory(pattern,     sizeof(pattern));
}

// +----------------------------------------------------------------------+

NavLight::NavLight(const NavLight& c)
: System(c), period(c.period), scale(c.scale),
nlights(0), enable(true)
{
	Mount(c);
	SetAbbreviation(c.Abbreviation());
	ZeroMemory(beacon,      sizeof(beacon));
	ZeroMemory(beacon_type, sizeof(beacon_type));

	nlights = c.nlights;

	for (int i = 0; i < nlights; i++) {
		loc[i]         = c.loc[i];
		pattern[i]     = c.pattern[i];
		beacon_type[i] = c.beacon_type[i];
		
		DriveSprite* rep = new(__FILE__,__LINE__) DriveSprite(images[beacon_type[i]]);
		rep->Scale(c.scale);

		beacon[i] = rep;
	}

	offset = rand();
}

// +--------------------------------------------------------------------+

NavLight::~NavLight()
{
	for (int i = 0; i < nlights; i++)
	GRAPHIC_DESTROY(beacon[i]);
}

// +--------------------------------------------------------------------+

void
NavLight::Initialize()
{
	static int initialized = 0;
	if (initialized) return;

	DataLoader* loader = DataLoader::GetLoader();
	loader->LoadTexture("beacon1.pcx", images[0], Bitmap::BMP_TRANSLUCENT);
	loader->LoadTexture("beacon2.pcx", images[1], Bitmap::BMP_TRANSLUCENT);
	loader->LoadTexture("beacon3.pcx", images[2], Bitmap::BMP_TRANSLUCENT);
	loader->LoadTexture("beacon4.pcx", images[3], Bitmap::BMP_TRANSLUCENT);

	initialized = 1;
}

void
NavLight::Close()
{
}

// +--------------------------------------------------------------------+

void
NavLight::ExecFrame(double seconds)
{
	if (enable && power_on) {
		double t = (Game::GameTime()+offset) / 1000.0;
		DWORD  n = (int) (fmod(t, period) * 32 / period);
		DWORD  code = 1 << n;

		for (int i = 0; i < nlights; i++) {
			if (beacon[i]) {
				if (pattern[i] & code)
				beacon[i]->SetShade(1);
				else
				beacon[i]->SetShade(0);
			}
		}
	}
	else {
		for (int i = 0; i < nlights; i++) {
			if (beacon[i]) {
				beacon[i]->SetShade(0);
			}
		}
	}
}

void
NavLight::Enable()
{
	enable = true;
}

void
NavLight::Disable()
{
	enable = false;
}

void
NavLight::AddBeacon(Point l, DWORD ptn, int t)
{
	if (nlights < MAX_LIGHTS) {
		loc[nlights]         = l;
		pattern[nlights]     = ptn;
		beacon_type[nlights] = t;

		DriveSprite* rep = new(__FILE__,__LINE__) DriveSprite(images[t]);
		rep->Scale(scale);

		beacon[nlights] = rep;
		nlights++;
	}
}

void
NavLight::SetPeriod(double p)
{
	period = p;
}

void
NavLight::SetPattern(int index, DWORD ptn)
{
	if (index >= 0 && index < nlights)
	pattern[index] = ptn;
}

void
NavLight::SetOffset(DWORD o)
{
	offset = o;
}

// +--------------------------------------------------------------------+

void
NavLight::Orient(const Physical* rep)
{
	System::Orient(rep);

	const Matrix& orientation = rep->Cam().Orientation();
	Point         ship_loc    = rep->Location();

	for (int i = 0; i < nlights; i++) {
		Point projector = (loc[i] * orientation) + ship_loc;
		if (beacon[i]) beacon[i]->MoveTo(projector);
	}
}


