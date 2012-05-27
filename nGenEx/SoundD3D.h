/*  Project nGenEx
	Destroyer Studios LLC
	Copyright � 1997-2004. All Rights Reserved.

	SUBSYSTEM:    nGenEx.lib
	FILE:         SoundD3D.h
	AUTHOR:       John DiCamillo


	OVERVIEW
	========
	DirectSound3D Audio Output and Buffer classes
*/

#ifndef SoundD3D_h
#define SoundD3D_h

//#define DIRECT_SOUND_3D
#include "SoundCard.h"
#include "Sound.h"
#include "Camera.h"
#include "ThreadSync.h"
#include <stdio.h>
#include <dsound.h>
#include <vorbis/vorbisfile.h>

// +--------------------------------------------------------------------+

class SoundD3D;
class SoundCardD3D;

// +--------------------------------------------------------------------+
// Sound Implementation for DirectSound and DirectSound3D

class SoundD3D : public Sound
{
public:
	static const char* TYPENAME() { return "SoundD3D"; }

	SoundD3D(LPDIRECTSOUND card, DWORD flags, LPWAVEFORMATEX format);
	SoundD3D(LPDIRECTSOUND card, DWORD flags, LPWAVEFORMATEX format, DWORD len, LPBYTE data);
	virtual ~SoundD3D();

	virtual void      Update();

	virtual HRESULT   StreamFile(const char* name, DWORD offset);
	virtual HRESULT   Load(DWORD bytes, BYTE* data);
	virtual HRESULT   Play();
	virtual HRESULT   Rewind();
	virtual HRESULT   Pause();
	virtual HRESULT   Stop();

	virtual Sound*    Duplicate();

	// (only for streamed sounds)
	virtual double    GetTotalTime()     const   { return total_time; }
	virtual double    GetTimeRemaining() const;
	virtual double    GetTimeElapsed()   const;

	// (only used for localized sounds)
	virtual void      SetVolume(long v);
	virtual long      GetPan() const;
	virtual void      SetPan(long p);
	virtual void      SetLocation(const Vec3& l);
	virtual void      SetVelocity(const Vec3& v);

	virtual float     GetMinDistance() const;
	virtual void      SetMinDistance(float f);
	virtual float     GetMaxDistance() const;
	virtual void      SetMaxDistance(float f);


protected:
	void      Localize();
	HRESULT   AllocateBuffer(DWORD bytes);
	HRESULT   StreamOggFile();

	void      StreamBlock();
	void      StreamOggBlock();
	void      RewindStream();
	void      RewindOggStream();

	LPDIRECTSOUND              soundcard;
	WAVEFORMATEX               wfex;
	DSBUFFERDESC               dsbd;
	LPDIRECTSOUNDBUFFER        buffer;

	DWORD                      data_len;
	LPBYTE                     data;

#ifdef DIRECT_SOUND_3D
	LPDIRECTSOUND3DBUFFER      sound3d;
#endif

	float                      min_dist;
	float                      max_dist;

	// STREAMED SOUND SUPPORT:
	FILE*             stream;
	DWORD             stream_left;
	double            total_time;
	DWORD             min_safety;
	DWORD             read_size;
	BYTE*             transfer;
	DWORD             w, r;
	DWORD             stream_offset;
	bool              eos_written;
	BYTE              eos_latch;
	bool              moved;

	ThreadSync        sync;
	OggVorbis_File*   ov_file;
};

// +--------------------------------------------------------------------+
// Sound Card Implementation for DS and DS3D

class SoundCardD3D : public SoundCard
{
	friend class SoundD3D;

public:
	static const char* TYPENAME() { return "SoundCardD3D"; }

	SoundCardD3D(HWND hwnd);
	virtual ~SoundCardD3D();

	// Format of the sound card's primary buffer:
	virtual bool   GetFormat(LPWAVEFORMATEX format);
	virtual bool   SetFormat(LPWAVEFORMATEX format);
	virtual bool   SetFormat(int bits, int channels, int hertz);

	virtual void   ShowFormat();

	// Get a blank, writable sound buffer:
	virtual Sound* CreateSound(DWORD flags, LPWAVEFORMATEX format);

	// Create a sound resource:
	virtual Sound* CreateSound(DWORD flags, LPWAVEFORMATEX format, DWORD len, LPBYTE data);

	virtual void   SetListener(const Camera& cam, const Vec3& vel);
	virtual bool   Pause();
	virtual bool   Resume();
	virtual bool   StopSoundEffects();

protected:
	LPDIRECTSOUND              soundcard;
	LPDIRECTSOUNDBUFFER        primary;

#ifdef DIRECT_SOUND_3D
	LPDIRECTSOUND3DLISTENER    listener;
#else
	Camera                     listener;
	Vec3                       velocity;
#endif

	WAVEFORMATEX               wfex;
	DSBUFFERDESC               dsbd;
};

#endif SoundD3D_h

