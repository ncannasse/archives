/* **************************************************************************** */
/*																				*/
/*   M3D Events Handling														*/
/*	 Nicolas Cannasse															*/
/*	 (c)2002 Motion-Twin														*/
/*																				*/
/* **************************************************************************** */
#include <windows.h>
#include <wlist.h>
#include <m3d.h>

namespace M3D {

typedef struct event {
	Events::EVENT e;
	bool sent;
	void *obj;
} event;

typedef struct ewait {
	Events::EVENT e;
	FLOAT time;
	void *obj;
	~ewait() {
		delete obj;
	}
} ewait;

#ifdef USE_R_THREAD
static LOCK lock,lock_events,lock_urgent;
#endif

static WList<event*> events;
static WList<ewait*> delayed;
static bool init = false;
static int ecount_urgent = 0;

void Events::Init() {
	Close();
	init = true;	
	ecount_urgent = 0;
	INITLOCK(lock);
	INITLOCK(lock_events);
	INITLOCK(lock_urgent);
}

void Events::Close() {
	if( init ) {
		BEGINLOCK(lock);
		events.Delete();
		delayed.Delete();
		ENDLOCK(lock);
		DELETELOCK(lock);
		DELETELOCK(lock_events);
		DELETELOCK(lock_urgent);
		init = false;
	}
}

void AddEvent( Events::EVENT e, void *obj, bool sent ) {
	event *evt = new event;
	evt->e = e;
	evt->obj = obj;
	evt->sent = sent;
	BEGINLOCK(lock);
	events.Add(evt);
#ifdef USE_R_THREAD	
	if( events.Length() == 1 )
		BEGINLOCK(lock_events);
	if( sent ) {
		ecount_urgent++;
		if( ecount_urgent == 1 )
			BEGINLOCK(lock_urgent);
	}
	ENDLOCK(lock);
#endif
}

void Events::Delayed( FLOAT time, Events::EVENT e, void *obj ) {
	ewait *evt = new ewait;
	evt->e = e;
	evt->obj = obj;
	evt->time = curtime+SecTo3DTime(time);
	BEGINLOCK(lock);
	delayed.Add(evt);
	ENDLOCK(lock);
}

void Events::Flush() {
	BEGINLOCK(lock_events);
	ENDLOCK(lock_events);
}

void Events::FlushUrgent() {
	BEGINLOCK(lock_urgent);
	ENDLOCK(lock_urgent);
}

void Events::Post( EVENT e, void *obj ) {
	AddEvent(e,obj,false);
}

void Events::Send( EVENT e, void *obj ) {
	AddEvent(e,obj,true);
}

bool Events::Get( EVENT *e, void **obj ) {
	event *evt = events.First();
	if( evt == NULL )
		return false;
	BEGINLOCK(lock);
	*e = evt->e;
	*obj = evt->obj;
	ENDLOCK(lock);
	return true;
}

bool ProcessDelayed() {
	BEGINLOCK(lock);
	FOREACH(ewait*,delayed);
		if( curtime >= item->time ) {
			delayed.Remove(item);
			ENDLOCK(lock);
			Events::Send(item->e,item->obj);
			item->obj = NULL;
			delete item;
			return true;
		}
	ENDFOR;
	ENDLOCK(lock);
	return false;
}

void Events::Process() {
	while( ProcessDelayed() )
		;
}

void Events::Next() {
	event *evt = events.First();	
	if( evt == NULL )
		return;

	BEGINLOCK(lock);
	events.Remove(evt);
#ifdef USE_R_THREAD
	if( events.Length() == 0 )
		ENDLOCK(lock_events);
	if( evt->sent ) {
		ecount_urgent--;
		if( ecount_urgent == 0 )
			ENDLOCK(lock_urgent);
	}
#endif
	delete evt;
	ENDLOCK(lock);
}

};// namespace
/* **************************************************************************** */
