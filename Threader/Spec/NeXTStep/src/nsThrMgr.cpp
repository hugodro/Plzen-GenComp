/**************************************************
* File: nsThrMgr.cc.
* Desc: Implementation of the NsThreadManager class
* Module: AkraLog : JavaKit.
* Rev: 23 septembre 1997 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include "nsThread.h"
#include "nsThrMgr.h"

NsThreadManager::NsThreadManager(JVCup *aCup)
    : JVThreadManager(aCup)
{

}


void NsThreadManager::lowLevelInit(void)
{
    centralCond= new NsJVCondition();
    centralLock= new NsJVMutex();
}


JVThread *NsThreadManager::createThread(unsigned int anID, JVThreadSet *aSet)
{
    return (JVThread *)new NsJVThread(anID, aSet);
}


JVThread *NsThreadManager::createThread(unsigned int anID, JVThread *aParent)
{
    return (JVThread *)new NsJVThread(anID, aParent);
}


JVSynchronizer *NsThreadManager::createSynchronizer(void)
{
    return new NsJVSynchro();
}

