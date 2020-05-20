/**************************************************
* File: posixThread.cc.
* Desc: Implementation of the PosixJVThread class.
* Module: AkraLog : Plzen.
* Rev: 2 Dec. 1999 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#if defined(__linux__)
#include <string.h>
#endif

#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <iostream>

#include <threader/threadManager.h>
#include "posixThread.h"

#if !defined(ETIMEDOUT)
#define ETIMEDOUT    110
#endif


/**************************************************
* Implementation: PosixJVThread.
**************************************************/

void *PosixJVThread::posixMachineLauncher(void *anObject)
{

    ((PosixJVThread *)anObject)->status= (((PosixJVThread *)anObject)->status & ~statusMask) | armed;

    if (((PosixJVThread *)anObject)->machine != NULL) {
	((PosixJVThread *)anObject)->runMethod("run", "()V");
    }
    threadManager->handleDeath(((JVThread *)anObject));

    return 0;
}


unsigned int PosixJVThread::JavaToPosixPriority(unsigned int aPriority)
{
    return aPriority;
}


PosixJVThread::PosixJVThread(unsigned int anID, JVThread *parent)
    : JVThread(anID, parent)
{
    init();
}


PosixJVThread::PosixJVThread(unsigned int anID, JVThreadSet *aSet)
    : JVThread(anID, aSet)
{
    init();
}


PosixJVThread::~PosixJVThread(void)
{
    if (uID != 0) {
	if (isAlive()) {
	    pthread_exit(0);
	}
    }
}


void PosixJVThread::init(void)
{
    uID= 0;
    JVAbstractThread::setPriority(0);
     status= (status & ~statusMask) | ready;
}


JVAbstractThread::SubType PosixJVThread::getSubClass(void)
{
    return (JVAbstractThread::SubType) ((unsigned int)java | 4);
}


unsigned int PosixJVThread::setPriority(unsigned int aPriority)
{
     if (uID != 0) {
// TODO: Implement priority updates.
    }

     return JVAbstractThread::setPriority(aPriority);
}


void PosixJVThread::start(void)
{
    if (uID == 0) {
	int result;

	pthread_attr_t attribs;
	pthread_attr_init(&attribs);
	pthread_attr_setdetachstate(&attribs, PTHREAD_CREATE_JOINABLE);
	pthread_attr_setinheritsched(&attribs, PTHREAD_INHERIT_SCHED);
	result= pthread_create(&uID, &attribs, posixMachineLauncher, this);
	if (result != 0) {
// TODO: Handle thread creation error.
	}
	pthread_attr_destroy(&attribs);
    }
}


void PosixJVThread::sleep(unsigned long someTime)
{
    if ((uID != 0) && ((status & statusMask) == running)) {
	usleep(someTime);
    }
}


void PosixJVThread::yield(void)
{
    if ((uID != 0) && ((status & statusMask) == running)) sched_yield();
}


void PosixJVThread::resume(void)
{
    if ((uID != 0) && ((status & statusMask) == interrupted)) {
	status= (status & ~statusMask) | running;
	kill(uID, SIGCONT);
    }
}


void PosixJVThread::suspend(void)
{
    if ((uID != 0) && ((status & statusMask) == running)) {
	status= (status & ~statusMask) | interrupted;
	pause();
    }
}


void PosixJVThread::exit(void)
{
    if ((uID != 0) && ((status & statusMask) == running)) {
	status= (status & ~statusMask) | dead;
	pthread_exit(0);
    }
}


void PosixJVThread::join(JVAbstractThread *aThread)
{
// ATTN: We can only wait on threads that are PosixJVThread...  How can this be asserted ?
    if (aThread != NULL) {
	void *fakeResult;
	if ((aThread->getSubClass() & 0x0FFFF) == 4) {
	    if (((aThread->getStatus() & statusMask) == running) || ((aThread->getStatus() & statusMask) == interrupted)) {
		pthread_join(((PosixJVThread *)aThread)->uID, &fakeResult);
	    }
	}
    }
}


void PosixJVThread::spawn(void (*aFunction)(void *), void *theReceiver)
{
    if ((status & statusMask) == ready) {
	int result;

	if (uID != 0) {
// TODO: destroy the thread initially created for normal use.
	}
	pthread_attr_t attribs;
	pthread_attr_init(&attribs);
	pthread_attr_setdetachstate(&attribs, PTHREAD_CREATE_JOINABLE);
	pthread_attr_setinheritsched(&attribs, PTHREAD_INHERIT_SCHED);
	result= pthread_create(&uID, &attribs, (void *(*)(void *))aFunction, theReceiver);
	if (result != 0) {
// TODO: Handle thread creation error.
	}
	pthread_attr_destroy(&attribs);
    }
}


JVSynchronizer *PosixJVThread::createSynchronizer(void)
{
    return new PosixJVSynchro();
}


/**************************************************
* Implementation: PosixJVMutex.
**************************************************/

PosixJVMutex::PosixJVMutex(void)
{
    pthread_mutex_init(&mutex, NULL);
    name= NULL;
}


PosixJVMutex::PosixJVMutex(char *aName)
{
    pthread_mutex_init(&mutex, NULL);
    if (aName != NULL) {
	int nameLength= strlen(aName)+1;
	name= new char[nameLength];
	memcpy(name, aName, nameLength * sizeof(char));
    }
    else
	name= NULL;
}


PosixJVMutex::~PosixJVMutex(void)
{
    if (name != NULL) delete[] name;
    pthread_mutex_destroy(&mutex);
}


void PosixJVMutex::lock(void)
{
    pthread_mutex_lock(&mutex);
}


bool PosixJVMutex::lockNonInfinite(unsigned int waitTime)
{
// TMPTMP: No timeout.
    struct timeval start, now;
    long long uStart, uNow;

    gettimeofday(&start, NULL);
    uStart= start.tv_sec * 1000000 + start.tv_usec;
    while (pthread_mutex_trylock(&mutex) != 0) {
	gettimeofday(&now, NULL);
	uNow= now.tv_sec * 1000000 + now.tv_usec;
	if (uNow > uStart + waitTime) return false;
    }

    return true;
}


void PosixJVMutex::unlock(void)
{
    pthread_mutex_unlock(&mutex);
}


void PosixJVMutex::releaseWhile(JVCondition *aCondition)
{
    pthread_cond_wait(&((PosixJVCondition *)aCondition)->condition, &mutex);
}


bool PosixJVMutex::releaseWhileNonInfinite(JVCondition *aCondition, unsigned int waitTime)
{
    struct timespec maxWait;

    maxWait.tv_sec= waitTime / 1000000;
    maxWait.tv_nsec= (waitTime % 1000000) * 1000;
    return (pthread_cond_timedwait(&((PosixJVCondition *)aCondition)->condition, &mutex, &maxWait) != ETIMEDOUT);
}


/**************************************************
* Implementation: PosixJVCondition.
**************************************************/

PosixJVCondition::PosixJVCondition(void)
{
    pthread_cond_init(&condition, NULL);
}


PosixJVCondition::PosixJVCondition(char *aName)
{
    pthread_cond_init(&condition, NULL);
    if (aName != NULL) {
	int nameLength= strlen(aName)+1;
	name= new char[nameLength];
	memcpy(name, aName, nameLength * sizeof(char));
    }
    else
	name= NULL;
}


PosixJVCondition::~PosixJVCondition(void)
{
    if (name != NULL) delete[] name;
    pthread_cond_destroy(&condition);
}


void PosixJVCondition::declareSatisfied(void)
{
    pthread_cond_signal(&condition);
}


void PosixJVCondition::declareSatisfiedToAll(void)
{
    pthread_cond_broadcast(&condition);
}


void PosixJVCondition::wait(void)
{
    pthread_mutex_t fakeMutex;

    pthread_mutex_init(&fakeMutex, NULL);
    pthread_mutex_lock(&fakeMutex);
    pthread_cond_wait(&condition, &fakeMutex);
    pthread_mutex_unlock(&fakeMutex);
    pthread_mutex_destroy(&fakeMutex);
}


bool PosixJVCondition::waitWithTimeout(unsigned long someTime)
{
    pthread_mutex_t fakeMutex;
    struct timespec maxWait;
    bool result;

    pthread_mutex_init(&fakeMutex, NULL);
    pthread_mutex_lock(&fakeMutex);
    if (someTime > 0) {
	maxWait.tv_sec= someTime / 1000000;
	maxWait.tv_nsec= (someTime % 1000000) * 1000;
	result= (pthread_cond_timedwait(&condition, &fakeMutex, &maxWait) != ETIMEDOUT);
    }
    else {
	pthread_cond_wait(&condition, &fakeMutex);
    }
    pthread_mutex_unlock(&fakeMutex);
    pthread_mutex_destroy(&fakeMutex);
    return result;
}


/**************************************************
* Implementation: PosixJVSynchro.
**************************************************/

PosixJVSynchro::PosixJVSynchro(void)
{
    lockOwner= NULL;
    lockCount= 0;
}


PosixJVSynchro::~PosixJVSynchro(void)
{
    // Nothing to do, mutex and condition are automatically deleted.
}


void PosixJVSynchro::lock(JVThread *aThread)
{
    if (lockOwner != aThread) {
	mutex.lock();
	lockOwner= aThread;
	lockCount= 1;
    }
    else {
	lockCount++;
    }
}


bool PosixJVSynchro::lockNonInfinite(JVThread *aThread, unsigned int waitTime)
{
    if (lockOwner != aThread) {
	if (mutex.lockNonInfinite(waitTime)) {
	    lockOwner= aThread;
	    lockCount= 1;
	    return true;		// Warning: quick exit.
	}
	else {
	    return false;		// Warning: quick exit.
	}
    }
    else {
	lockCount++;
	return true;		// Warning: quick exit.
    }
}


void PosixJVSynchro::unlock(JVThread *aThread)
{
    if (lockOwner == aThread) {
	lockCount--;
	if (lockCount == 0) {
	    lockOwner= NULL;
	    mutex.unlock();
	}
    }
    else {
	// ATTN: This should not happen.  How could we unlock a lock we don't have ??
std::cout << "ERRMSG: Tried to unlock a lock that is not owned by us.\n";
    }
}


void PosixJVSynchro::releaseWhile(JVCondition *aCondition)
{
    JVThread *tmpOwner= lockOwner;	// We save this since another lock might overwrite the info.

    mutex.releaseWhile(aCondition);
    lockOwner= tmpOwner;			// At this point, we have regained the lock.
}


bool PosixJVSynchro::releaseWhileNonInfinite(JVCondition *aCondition, unsigned int waitTime)
{
    JVThread *tmpOwner= lockOwner;	// We save this since another lock might overwrite the info.

    if (mutex.releaseWhileNonInfinite(aCondition, waitTime)) {
	lockOwner= tmpOwner;			// At this point, we have regained the lock.
	return true;
    }
    else {
	return false;		// We couldn't regain the lock within time.
    }
}


void PosixJVSynchro::declareSatisfied(void)
{
    condition.declareSatisfied();
}


void PosixJVSynchro::declareSatisfiedToAll(void)
{
    condition.declareSatisfiedToAll();
}


void PosixJVSynchro::wait(void)
{
    condition.wait();
}


bool PosixJVSynchro::waitWithTimeout(unsigned long someTime)
{
    return condition.waitWithTimeout(someTime);
}

