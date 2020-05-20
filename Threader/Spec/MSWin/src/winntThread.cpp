/**************************************************
* File: winntThread.cc.
* Desc: Implementation of the WntJVThread class.
* Module: AkraLog : Plzen.
* Rev: 22 sept. 1997 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <windowsx.h>
#include <process.h>	    // For _beginthreadex.
#include "threadManager.h"
#include "winntThread.h"

/*
_CRTIMP unsigned long __cdecl _beginthreadex(void *, unsigned,
	unsigned (__stdcall *) (void *), void *, unsigned, unsigned *);
*/

int WntJVThread::wntMachineLauncher(void *anObject)
{
    int returnCode= 0; 

    ((WntJVThread *)anObject)->status= (((WntJVThread *)anObject)->status & ~statusMask) | armed;
    if (((WntJVThread *)anObject)->machine != NULL) {
	returnCode= (int)((WntJVThread *)anObject)->runMethod("run", "()V");
    }

    threadManager->handleDeath(((JVThread *)anObject));
    return returnCode;
}


WntJVThread::WntJVThread(unsigned int anID, JVThread *parent)
    : JVThread(anID, parent)
{
    winThread= NULL;
    winID= 0;
    init();
}


WntJVThread::WntJVThread(unsigned int anID, JVThreadSet *aSet)
    : JVThread(anID, aSet)
{
    winThread= NULL;
    winID= 0;
    init();
}


WntJVThread::~WntJVThread(void)
{
    if (winThread != NULL) {
	if (isAlive()) {
	    TerminateThread(winThread, 0x0FFFFFFFF);
	}
	CloseHandle(winThread);
    }
}


void WntJVThread::init(void)
{
    status= (status & ~statusMask) | ready;
}


JVAbstractThread::SubType WntJVThread::getSubClass(void)
{
    return (JVAbstractThread::SubType) ((unsigned int)java | 2);
}



unsigned int WntJVThread::setPriority(unsigned int aPriority)
{
    BOOL result= FALSE;

    if (winThread != NULL) {
	if (aPriority < priority) {
	    if (aPriority < (priority-1)) {
		result= SetThreadPriority(winThread, THREAD_PRIORITY_LOWEST);
	    }
	    else {
		result= SetThreadPriority(winThread, THREAD_PRIORITY_BELOW_NORMAL);
	    }
	}
	else if (aPriority > priority) {
	    if (aPriority > (priority+1)) {
		result= SetThreadPriority(winThread, THREAD_PRIORITY_HIGHEST);
	    }
	    else {
		result= SetThreadPriority(winThread, THREAD_PRIORITY_ABOVE_NORMAL);
	    }
	}
    }
    else {
	return JVAbstractThread::setPriority(aPriority);    // Warning: quick exit.
    }
    if (result) {
	return JVAbstractThread::setPriority(aPriority);
    }
    else {
	return priority;
    }
}


void WntJVThread::start(void)
{
    if (winThread == NULL) {
	winThread= (HANDLE)_beginthreadex(NULL, 0, (unsigned int (__stdcall *)(void *))wntMachineLauncher, this, CREATE_SUSPENDED, (unsigned int *)&winID);
	SetThreadPriority(winThread, THREAD_PRIORITY_NORMAL);
	ResumeThread(winThread);
    }
    else {
	ResumeThread(winThread);
    }
}


void WntJVThread::sleep(unsigned long someTime)
{
    if ((winThread != NULL) && ((status & statusMask) == running)) {
	Sleep(someTime);
    }
}


void WntJVThread::yield(void)
{
    if ((winThread != NULL) && ((status & statusMask) == running)) {
	Sleep(0);
    }
}


void WntJVThread::resume(void)
{
    if ((winThread != NULL) && ((status & statusMask) == interrupted)) {
	status= (status & ~statusMask) | running;
	ResumeThread(winThread);
    }
}


void WntJVThread::suspend(void)
{
    if ((winThread != NULL) && ((status & statusMask) == running)) {
	SuspendThread(winThread);
	status= (status & ~statusMask) | interrupted;
    }
}


void WntJVThread::exit(void)
{
    if ((winThread != NULL) && ((status & statusMask) == running)) {
	TerminateThread(winThread, 0);
	status= (status & ~statusMask) | dead;
    }
}


void WntJVThread::join(JVAbstractThread *aThread)
{
    if (aThread != NULL) {
	if ((aThread->getSubClass() & 0x0FFFF) == 2) {
	    if (((aThread->status & statusMask) == running) || ((aThread->status & statusMask) == interrupted)) {
		WaitForSingleObject(aThread, 0x0FFFFFFFF);
	    }
	}
    }
}


void WntJVThread::spawn(void (*aFunction)(void *), void *theReceiver)
{
    if ((status & statusMask) == useless) {
	if (winThread != NULL) CloseHandle(winThread);
	    winThread= (HANDLE)_beginthreadex(NULL, 0, (unsigned int (__stdcall *)(void *))aFunction, theReceiver, 0, (unsigned int *)&winID);
    }
}


JVSynchronizer *WntJVThread::createSynchronizer(void)
{
    return new WntJVSynchro();
}


/**************************************************
* Implementation: WntJVMutex.
**************************************************/

WntJVMutex::WntJVMutex(void)
{
    wntMutex= CreateMutex(NULL, FALSE, NULL);

/* TODO: Catch impossibility of creation.
    if (GetLastError() == ERROR_ALREADY_EXISTS) {

    }
*/
}


WntJVMutex::WntJVMutex(char *aName)
{
    wntMutex= CreateMutex(NULL, FALSE, aName);
/* TODO: Catch impossibility of creation.
    if (GetLastError() == ERROR_ALREADY_EXISTS) {

    }
*/
}


WntJVMutex::~WntJVMutex(void)
{
    CloseHandle(wntMutex);
}


void WntJVMutex::lock(void)
{
    int result;

    result= WaitForSingleObject(wntMutex, INFINITE);

/* TODO: Catch Error.
    if (result != WAIT_OBJECT_0) {
    }
*/
}


bool WntJVMutex::lockNonInfinite(unsigned int waitTime)
{
    int result;

    result= WaitForSingleObject(wntMutex, waitTime);

    if ((result == WAIT_OBJECT_0) || (result == WAIT_ABANDONED)) {
	return true;
    }
    else {
	return false;
    }

}


void WntJVMutex::unlock(void)
{
    ReleaseMutex(wntMutex);
}


void WntJVMutex::releaseWhile(JVCondition *aCondition)
{
    int result;

    ReleaseMutex(wntMutex);
	// Wait for condition to be signaled.
    aCondition->wait();
    result= WaitForSingleObject(wntMutex, INFINITE);
}


bool WntJVMutex::releaseWhileNonInfinite(JVCondition *aCondition, unsigned int waitTime)
{
    int result;

    ReleaseMutex(wntMutex);
	// Wait for condition to be signaled, up to waitTime.
	// If no timeout, try to reacquire mutex.
    if (aCondition->waitWithTimeout(waitTime)) {
	result= WaitForSingleObject(wntMutex, INFINITE);
        return true;
    }
    else return false;
}


/**************************************************
* Implementation: WntJVCondition.
**************************************************/

WntJVCondition::WntJVCondition(void)
{
    wntCondition= CreateEvent(NULL, FALSE, FALSE, NULL);
}


WntJVCondition::WntJVCondition(char *aName)
{
    wntCondition= CreateEvent(NULL, FALSE, FALSE, aName);
}


WntJVCondition::~WntJVCondition(void)
{
    CloseHandle(wntCondition);
}


void WntJVCondition::declareSatisfied(void)
{
    SetEvent(wntCondition);
}


void WntJVCondition::declareSatisfiedToAll(void)
{
// ATTN: Is this good ?
    SetEvent(wntCondition);
}


void WntJVCondition::wait(void)
{
    int result;

    result= WaitForSingleObject(wntCondition, INFINITE);

/* TODO: Catch Error.
    if (result != WAIT_OBJECT_0) {
    }
*/
}


bool WntJVCondition::waitWithTimeout(unsigned long someTime)
{
    int result;

    result= WaitForSingleObject(wntCondition, INFINITE);
    if ((result == WAIT_OBJECT_0) || (result == WAIT_ABANDONED)) {
	return true;
    }
    else {
	return false;
    }

}


/**************************************************
* Implementation: WntJVSynchro.
**************************************************/

WntJVSynchro::WntJVSynchro(void)
{
}


WntJVSynchro::~WntJVSynchro(void)
{
    // Nothing to do, mutex and condition are automatically deleted.
}


void WntJVSynchro::lock(JVThread *aThread)
{
    mutex.lock();
}


bool WntJVSynchro::lockNonInfinite(JVThread *aThread, unsigned int waitTime)
{
    return mutex.lockNonInfinite(waitTime);
}


void WntJVSynchro::unlock(JVThread *aThread)
{
    mutex.unlock();
}


void WntJVSynchro::releaseWhile(JVCondition *aCondition)
{
    mutex.releaseWhile(aCondition);
}


bool WntJVSynchro::releaseWhileNonInfinite(JVCondition *aCondition, unsigned int waitTime)
{
    return mutex.releaseWhileNonInfinite(aCondition, waitTime);
}


void WntJVSynchro::declareSatisfied(void)
{
    condition.declareSatisfied();
}


void WntJVSynchro::declareSatisfiedToAll(void)
{
    condition.declareSatisfiedToAll();
}


void WntJVSynchro::wait(void)
{
    condition.wait();
}


bool WntJVSynchro::waitWithTimeout(unsigned long someTime)
{
    return condition.waitWithTimeout(someTime);
}

