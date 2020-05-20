/**************************************************
* File: rawThreads.cc.
* Desc: Implementation of the JVThread and JVThreadSet classes.
* Module: AkraLog : JavaKit.
* Rev: 16 novembre 1996 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <stdio.h>
#include <string.h>
#include "threadManager.h"
#include "rawThreads.h"


/**************************************************
* Implementation: JVAbstractThread.
**************************************************/

JVThreadManager *JVAbstractThread::threadManager= NULL;

void JVAbstractThread::setThreadManager(JVThreadManager *aManager)
{
    threadManager= aManager;
}


JVAbstractThread::JVAbstractThread(unsigned int anID)
{
    priority= 0;
    priorityOffset= 0;
    status= useless;
    privateID= anID;
    quantumTime= 0L;
    runTime= 0L;
}



JVAbstractThread::~JVAbstractThread(void)
{
// TODO.
}


JVAbstractThread::SubType JVAbstractThread::getSubClass(void)
{
    return lowLevel;		// Means it is an almost-abstract type.
}


unsigned int JVAbstractThread::setPriority(unsigned int aPriority)
{
    unsigned int oldPriority= priority;

    priority= aPriority;
    return oldPriority;
}


bool JVAbstractThread::shakePriority(int aDelta)
{
    priorityOffset+= aDelta;	// Must test of offset validity.
    return true;			// Must return if the aDelta was valid.
}


unsigned int JVAbstractThread::getPriority(void)
{
    return (priority + priorityOffset);
}


unsigned int JVAbstractThread::getStatus(void)
{
    return (status & statusMask);
}


unsigned int JVAbstractThread::getID(void)
{
    return privateID;
}


bool JVAbstractThread::isAlive(void)
{
    return ((status & statusMask) == running);
}


void JVAbstractThread::start(void)
{
// TODO.
}


void JVAbstractThread::sleep(unsigned long someTime)
{
// TODO.
}


void JVAbstractThread::yield(void)
{
// TODO.
}


void JVAbstractThread::resume(void)
{
// TODO.
}


void JVAbstractThread::suspend(void)
{
// TODO.
}


void JVAbstractThread::exit(void)
{
// TODO.
}


void JVAbstractThread::join(JVAbstractThread *aThread)
{
    if (aThread != NULL) {
// TODO.
    }
}


void JVAbstractThread::spawn(void (*aFunction)(void *), void *theReceiver)
{
// TODO.
}


JVCondition *JVAbstractThread::getACondition(void)
{
    return NULL;		// The default is to return nothing.
}


JVMutex *JVAbstractThread::getAMutex()
{
    return NULL;		// The default is to return nothing.
}


/**************************************************
* Implementation: JVMutex.
**************************************************/

JVMutex::JVMutex(void)
{
// TODO.
}


JVMutex::JVMutex(char *aName)
{
    if (aName != NULL) {
	// TODO.
    }
}


JVMutex::~JVMutex(void)
{
// TODO.
}


void JVMutex::lock(void)
{
// TODO.
}


bool JVMutex::lockNonInfinite(unsigned int waitTime)
{
    if (waitTime > 0) {
	// TODO.
    }
    return false;
}


void JVMutex::unlock(void)
{
// TODO.
}


void JVMutex::releaseWhile(JVCondition *aCondition)
{
    if (aCondition != NULL) {
	// TODO.
    }
}


bool JVMutex::releaseWhileNonInfinite(JVCondition *aCondition, unsigned int waitTime)
{
    if ((aCondition != NULL) && (waitTime > 0)) {
	// TODO.
    }
    return false;
}




/**************************************************
* Implementation: JVCondition.
**************************************************/

JVCondition::JVCondition(void)
{
// TODO.
}


JVCondition::JVCondition(char *aName)
{
    if (aName != NULL) {
	// TODO.
    }
}


JVCondition::~JVCondition(void)
{
// TODO.
}


void JVCondition::declareSatisfied(void)
{
// TODO.
}


void JVCondition::declareSatisfiedToAll(void)
{
// TODO.
}


void JVCondition::wait(void)
{
// TODO.
}


bool JVCondition::waitWithTimeout(unsigned long someTime)
{
    return false;		// Indicate a timeout.
}

