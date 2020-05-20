/**************************************************
* File: synchro.cc.
* Desc: Implementation of the synchronization between multiple threads.
* Module: AkraLog : JavaKit.
* Rev: 9 mai 1996 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include "synchro.h"


JVSynchronizer::JVSynchronizer(void)
{
    
}


JVSynchronizer::~JVSynchronizer(void)
{

}


void JVSynchronizer::lock(JVThread *aThread)
{
    // Do nothing.
}


bool JVSynchronizer::lockNonInfinite(JVThread *aThread, unsigned int waitTime)
{
    // Do nothing.
    return true;		// Always succeed.
}


void JVSynchronizer::unlock(JVThread *aThread)
{
    // Do nothing.
}


void JVSynchronizer::releaseWhile(JVCondition *aCondition)
{
    // Do nothing.
}


bool JVSynchronizer::releaseWhileNonInfinite(JVCondition *aCondition, unsigned int waitTime)
{
    return true;		// Always succeed.
}


void JVSynchronizer::declareSatisfied(void)
{
    // Do nothing.
}


void JVSynchronizer::declareSatisfiedToAll(void)
{
    // Do nothing.
}


void JVSynchronizer::wait(void)
{
    // Do nothing.
}


bool JVSynchronizer::waitWithTimeout(unsigned long someTime)
{
    return true;		// Always succeed.
}


