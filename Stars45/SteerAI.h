/*  Project Starshatter 4.5
	Destroyer Studios LLC
	Copyright � 1997-2004. All Rights Reserved.

	SUBSYSTEM:    Stars.exe
	FILE:         SteerAI.h
	AUTHOR:       John DiCamillo


	OVERVIEW
	========
	Steering (low-level) Artifical Intelligence class
*/

#ifndef SteerAI_h
#define SteerAI_h

#include "Types.h"
#include "SimObject.h"
#include "Director.h"
#include "Geometry.h"

// +--------------------------------------------------------------------+

class System;

// +--------------------------------------------------------------------+

struct Steer
{
	Steer() : yaw(0), pitch(0), roll(0), brake(0), stop(0) { }
	Steer(double y, double p, double r, double b=0) : yaw(y), pitch(p), roll(r), brake(b), stop(0) { }
	Steer(const Steer& s) : yaw(s.yaw), pitch(s.pitch), roll(s.roll), brake(s.brake), stop(s.stop) { }

	Steer& operator = (const Steer& s) { yaw=s.yaw; pitch=s.pitch; roll=s.roll; brake = s.brake; stop = s.stop; return *this; }

	Steer  operator+(const Steer& s) const;
	Steer  operator-(const Steer& s) const;
	Steer  operator*(double f)       const;
	Steer  operator/(double f)       const;

	Steer& operator+=(const Steer& s);
	Steer& operator-=(const Steer& s);

	double Magnitude() const;

	void Clear() { yaw=0; pitch=0; roll=0; brake=0; stop=0; }

	double yaw, pitch, roll;
	double brake;
	int    stop;
};

// +--------------------------------------------------------------------+

class SteerAI : public Director, public SimObserver
{
public:
	enum Type { SEEKER = 1000, FIGHTER, STARSHIP, GROUND };

	SteerAI(SimObject* self);
	virtual ~SteerAI();

	static Director*  Create(SimObject*, int type);

	virtual void      SetTarget(SimObject* targ, System* sub=0);
	virtual SimObject* GetTarget() const { return target; }
	virtual System*   GetSubTarget() const { return subtarget; }
	virtual void      DropTarget(double drop_time=1.5);
	virtual int       Type()      const { return ai_type; }

	virtual bool         Update(SimObject* obj);
	virtual const char*  GetObserverName() const;

	// debug:
	virtual Point        GetObjective() const { return obj_w; }
	virtual SimObject*   GetOther()     const { return other; }

protected:

	// accumulate behaviors:
	virtual void      Navigator();
	virtual int       Accumulate(const Steer& steer);

	// steering functions:
	virtual Steer     Seek(const Point& point);
	virtual Steer     Flee(const Point& point);
	virtual Steer     Avoid(const Point& point, float radius);
	virtual Steer     Evade(const Point& point, const Point& vel);

	// compute the goal point based on target stats:
	virtual void      FindObjective();
	virtual Point     ClosingVelocity();

	virtual Point     Transform(const Point& pt);
	virtual Point     AimTransform(const Point& pt);

	int               seeking;

	SimObject*        self;
	SimObject*        target;
	System*           subtarget;
	SimObject*        other;

	Point             obj_w;
	Point             objective;

	double            distance;
	double            az[3], el[3];

	Steer             accumulator;
	double            magnitude;
	DWORD             evade_time;

	double            seek_gain;
	double            seek_damp;

	int               ai_type;
};


// +--------------------------------------------------------------------+

#endif SteerAI_h

