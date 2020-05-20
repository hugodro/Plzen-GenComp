/**************************************************
* File: posixThrMgr.cc.
* Desc: Implementation of the PosixThreadManager class
* Module: AkraLog : JavaKit.
* Rev: 23 septembre 1997 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include "posixThread.h"
#include "posixThrMgr.h"

PosixThreadManager::PosixThreadManager(JVCup *aCup)
    : JVThreadManager(aCup)
{

}


void PosixThreadManager::lowLevelInit(void)
{
    centralCond= new PosixJVCondition();
    centralLock= new PosixJVMutex();
}


JVThread *PosixThreadManager::createThread(unsigned int anID, JVThreadSet *aSet)
{
    return (JVThread *)new PosixJVThread(anID, aSet);
}


JVThread *PosixThreadManager::createThread(unsigned int anID, JVThread *aParent)
{
    return (JVThread *)new PosixJVThread(anID, aParent);
}


JVSynchronizer *PosixThreadManager::createSynchronizer(void)
{
    return new PosixJVSynchro();
}

