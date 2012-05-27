/*  Project Starshatter 4.5
	Destroyer Studios LLC
	Copyright � 1997-2004. All Rights Reserved.

	SUBSYSTEM:    Stars.exe
	FILE:         NetAuth.cpp
	AUTHOR:       John DiCamillo


	OVERVIEW
	========
	This class represents a user connecting to the multiplayer lobby
*/


#include "MemDebug.h"
#include "NetAuth.h"
#include "NetLobby.h"
#include "NetUser.h"
#include "ModConfig.h"
#include "ModInfo.h"
#include "Random.h"
#include "sha1.h"

static int auth_level = NetAuth::NET_AUTH_MINIMAL;

// +-------------------------------------------------------------------+

int
NetAuth::AuthLevel()
{
	return auth_level;
}


void
NetAuth::SetAuthLevel(int n)
{
	if (n >= NET_AUTH_MINIMAL && n <= NET_AUTH_SECURE)
	auth_level = n;
}

// +-------------------------------------------------------------------+

Text
NetAuth::CreateAuthRequest(NetUser* u)
{
	Text request;

	if (u) {
		u->SetAuthLevel(auth_level);

		if (auth_level == NET_AUTH_MINIMAL) {
			u->SetAuthState(NET_AUTH_OK);
			u->SetSalt("Very Low Sodium");
		}

		else if (auth_level == NET_AUTH_STANDARD) {
			u->SetAuthState(NET_AUTH_INITIAL);
			u->SetSalt("Very Low Sodium");

			request = "level 1";
		}

		else {
			char salt[33];

			for (int i = 0; i < 32; i++)
			salt[i] = (char) ('0' + (int) Random(0, 9.4));

			salt[32] = 0;
			u->SetSalt(salt);
			u->SetAuthState(NET_AUTH_INITIAL);

			request = "level 2 salt ";
			request += salt;
		}
	}

	return request;
}

// +-------------------------------------------------------------------+

static Text Digest(const char* salt, const char* file)
{
	int  length = 0;
	int  offset = 0;
	char block[4096];
	char digest[64];

	ZeroMemory(digest, sizeof(digest));

	if (file) {
		FILE* f;
		fopen_s(&f, file, "rb");

		if (f) {
			SHA1 sha1;

			if (salt) {
				sha1.Input(salt, strlen(salt));
			}

			fseek(f, 0, SEEK_END);
			length = ftell(f);
			fseek(f, 0, SEEK_SET);

			while (offset < length) {
				int n = fread(block, sizeof(char), 4096, f);
				sha1.Input(block, n);
				offset += n;
			}

			fclose(f);

			unsigned result[5];
			if (sha1.Result(result)) {
				sprintf_s(digest, "SHA1_%08X_%08X_%08X_%08X_%08X",
				result[0], result[1], result[2], result[3], result[4]);
			}
		}
	}

	return digest;
}

// +-------------------------------------------------------------------+

Text
NetAuth::CreateAuthResponse(int level, const char* salt)
{
	Text        response;
	ModConfig*  config = ModConfig::GetInstance();

	if (level == NET_AUTH_SECURE) {

#ifdef STARSHATTER_DEMO_RELEASE

		response += "exe ";
		response += Digest(salt, "StarDemo.exe"); // XXX should look up name of this exe
		response += " ";

#else

		response += "exe ";
		response += Digest(salt, "stars.exe");    // XXX should look up name of this exe
		response += " ";

#endif

		response += "dat ";
		response += Digest(salt, "shatter.dat");
		response += " ";

		response += "etc ";
		response += Digest(salt, "start.dat");
		response += " ";
	}

	if (level >= NET_AUTH_STANDARD) {
		List<ModInfo>&    mods     = config->GetModInfoList();
		ListIter<ModInfo> mod_iter = mods;

		char buffer[32];
		sprintf_s(buffer, "num %d ", mods.size());
		response += buffer;

		while (++mod_iter) {
			ModInfo* info = mod_iter.value();

			response += "mod \"";
			response += info->Name();
			response += "\" ver \"";
			response += info->Version();
			response += "\" ";

			if (level == NET_AUTH_SECURE) {
				response += "sha ";
				response += Digest(salt, info->Filename());
				response += " ";
			}
		}
	}

	return response;
}

// +-------------------------------------------------------------------+

bool
NetAuth::AuthUser(NetUser* u, Text response)
{
	bool authentic = false;

	if (auth_level == NET_AUTH_MINIMAL) {  // (this case should not occur)
		if (u) {
			u->SetAuthLevel(auth_level);
			u->SetAuthState(NET_AUTH_OK);
		}

		authentic = (u != 0);
	}

	else if (u) {
		Text expected_response = CreateAuthResponse(auth_level, u->Salt());
		if (expected_response == response)
		authentic = true; 

		u->SetAuthState(authentic ? NET_AUTH_OK : NET_AUTH_FAILED);
	}

	return authentic;
}

// +-------------------------------------------------------------------+

