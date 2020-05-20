/**************************************************
* File: nsThread.cc.
* Desc: Implementation of the NsJVThread class.
* Module: AkraLog : Plzen.
* Rev: 22 sept. 1997 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <iostream.h>

#include "threadManager.h"
#include "nsThread.h"


/**************************************************
* Implementation: NsJVThread.
**************************************************/

any_t NsJVThread::nsMachineLauncher(any_t anObject)
{

    ((NsJVThread *)anObject)->status= (((NsJVThread *)anObject)->status & ~statusMask) | armed;
    cthread_priority(cthread_self(), JavaToNSPriority(((NsJVThread *)anObject)->priority), FALSE);

    if (((NsJVThread *)anObject)->machine != NULL) {
	((NsJVThread *)anObject)->runMethod("run", "()V");
    }
    threadManager->handleDeath(((JVThread *)anObject));
    return 0;
}


unsigned int NsJVThread::JavaToNSPriority(unsigned int aPriority)
{
    return aPriority;
}


NsJVThread::NsJVThread(unsigned int anID, JVThread *parent)
    : JVThread(anID, parent)
{
    nsThread= NULL;
    init();
}


NsJVThread::NsJVThread(unsigned int anID, JVThreadSet *aSet)
    : JVThread(anID, aSet)
{
    nsThread= NULL;
    init();
}


NsJVThread::~NsJVThread(void)
{
    if (nsThread != NULL) {
	if (isAlive()) {
	    cthread_exit(0);
	}
    }
}


void NsJVThread::init(void)
{
    struct thread_sched_info  info;
    kern_return_t error;
    unsigned int info_count= THREAD_SCHED_INFO_COUNT;
    
    error=thread_info(thread_self(), THREAD_SCHED_INFO, (thread_info_t)&info, &info_count);
    if (error == KERN_SUCCESS) {
	JVAbstractThread::setPriority(info.cur_priority);
    }

    status= (status & ~statusMask) | ready;
}


JVAbstractThread::SubType NsJVThread::getSubClass(void)
{
    return (JVAbstractThread::SubType) ((unsigned int)java | 1);
}


unsigned int NsJVThread::setPriority(unsigned int aPriority)
{
    kern_return_t result= FALSE;


    if (nsThread != NULL) {
	result= cthread_priority(nsThread, aPriority, FALSE);
    }
    else {
	result= KERN_SUCCESS;
    }
    if (result != KERN_SUCCESS) {
	return priority;
    }
    else {
	return JVAbstractThread::setPriority(aPriority);
    }
}


void NsJVThread::start(void)
{
    if (nsThread == NULL) {
	nsThread= cthread_fork(nsMachineLauncher, this);
//	cthread_detach(nsThread);	// ATTN: What to do, detach or keep 'joinable' ?
    }
}


void NsJVThread::sleep(unsigned long someTime)
{
    if ((nsThread != NULL) && ((status & statusMask) == running)) {
	    // ATTN: Should be able to indicate that we are going to sleep...
	thread_switch(THREAD_NULL, SWITCH_OPTION_WAIT, someTime);
    }
}


void NsJVThread::yield(void)
{
    if ((nsThread != NULL) && ((status & statusMask) == running)) cthread_yield();
}


void NsJVThread::resume(void)
{
    if ((nsThread != NULL) && ((status & statusMask) == interrupted)) {
	status= (status & ~statusMask) | running;
	thread_resume(cthread_thread(nsThread));
    }
}


void NsJVThread::suspend(void)
{
    if ((nsThread != NULL) && ((status & statusMask) == running)) {
	thread_suspend(cthread_thread(nsThread));
	status= (status & ~statusMask) | interrupted;
    }
}


void NsJVThread::exit(void)
{
    if ((nsThread != NULL) && ((status & statusMask) == running)) {
// TODO.
	status= (status & ~statusMask) | dead;
    }
}


void NsJVThread::join(JVAbstractThread *aThread)
{
// ATTN: We can only wait on threads that are NsJVThread...  How can this be asserted ?
    if (aThread != NULL) {
	if ((aThread->getSubClass() & 0x0FFFF) == 1) {
	    if (((aThread->status & statusMask) == running) || ((aThread->status & statusMask) == interrupted)) {
		cthread_join(((NsJVThread *)aThread)->nsThread);
	    }
	}
    }
}


void NsJVThread::spawn(void (*aFunction)(void *), void *theReceiver)
{
    if ((status & statusMask) == ready) {
	if (nsThread != NULL) {
// TODO: destroy the thread initially created for normal use.
	}
	nsThread= cthread_fork((void * (*)(void *))aFunction, (any_t)theReceiver);
    }
}


JVSynchronizer *NsJVThread::createSynchronizer(void)
{
    return new NsJVSynchro();
}


/**************************************************
* Implementation: NsJVMutex.
**************************************************/

NsJVMutex::NsJVMutex(void)
{
    mutex_init(&nsMutex);
}


NsJVMutex::NsJVMutex(char *aName)
{
    mutex_init(&nsMutex);
    mutex_set_name(&nsMutex, aName);
}


NsJVMutex::~NsJVMutex(void)
{
    mutex_clear(&nsMutex);
}


void NsJVMutex::lock(void)
{
    mutex_lock(&nsMutex);
}


bool NsJVMutex::lockNonInfinite(unsigned int waitTime)
{
// TMPTMP: No timeout.
    mutex_lock(&nsMutex);
    return true;
}


void NsJVMutex::unlock(void)
{
    mutex_unlock(&nsMutex);
}


void NsJVMutex::releaseWhile(JVCondition *aCondition)
{
    condition_wait(&((NsJVCondition *)aCondition)->nsCondition, &nsMutex);
}


bool NsJVMutex::releaseWhileNonInfinite(JVCondition *aCondition, unsigned int waitTime)
{
// TMPTMP: No timeout.
    condition_wait(&((NsJVCondition *)aCondition)->nsCondition, &nsMutex);
    return true;
}


/**************************************************
* Implementation: NsJVCondition.
**************************************************/

NsJVCondition::NsJVCondition(void)
{
    condition_init(&nsCondition);
}


NsJVCondition::NsJVCondition(char *aName)
{
    condition_init(&nsCondition);
    condition_set_name(&nsCondition, aName);
}


NsJVCondition::~NsJVCondition(void)
{
    condition_clear(&nsCondition);
}


void NsJVCondition::declareSatisfied(void)
{
    condition_signal(&nsCondition);
}


void NsJVCondition::declareSatisfiedToAll(void)
{
    condition_broadcast(&nsCondition);
}


void NsJVCondition::wait(void)
{
    struct mutex fakeMutex;

    mutex_init(&fakeMutex);
    mutex_lock(&fakeMutex);
    condition_wait(&nsCondition, &fakeMutex);
    mutex_unlock(&fakeMutex);
    mutex_clear(&fakeMutex);
}


bool NsJVCondition::waitWithTimeout(unsigned long someTime)
{
    struct mutex fakeMutex;

// TMPTMP: No timeout.
    mutex_init(&fakeMutex);
    mutex_lock(&fakeMutex);
    condition_wait(&nsCondition, &fakeMutex);
    mutex_unlock(&fakeMutex);
    mutex_clear(&fakeMutex);
    return true;
}


/**************************************************
* Implementation: NsJVSynchro.
**************************************************/

NsJVSynchro::NsJVSynchro(void)
{
    lockOwner= NULL;
    lockCount= 0;
}


NsJVSynchro::~NsJVSynchro(void)
{
    // Nothing to do, mutex and condition are automatically deleted.
}


void NsJVSynchro::lock(JVThread *aThread)
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


bool NsJVSynchro::lockNonInfinite(JVThread *aThread, unsigned int waitTime)
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


void NsJVSynchro::unlock(JVThread *aThread)
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
	cout << "ERRMSG: Tried to unlock a lock that is not owned by us.\n";
    }
}


void NsJVSynchro::releaseWhile(JVCondition *aCondition)
{
    JVThread *tmpOwner= lockOwner;	// We save this since another lock might overwrite the info.

    mutex.releaseWhile(aCondition);
    lockOwner= tmpOwner;			// At this point, we have regained the lock.
}


bool NsJVSynchro::releaseWhileNonInfinite(JVCondition *aCondition, unsigned int waitTime)
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


void NsJVSynchro::declareSatisfied(void)
{
    condition.declareSatisfied();
}


void NsJVSynchro::declareSatisfiedToAll(void)
{
    condition.declareSatisfiedToAll();
}


void NsJVSynchro::wait(void)
{
    condition.wait();
}


bool NsJVSynchro::waitWithTimeout(unsigned long someTime)
{
    return condition.waitWithTimeout(someTime);
}

