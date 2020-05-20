/**************************************************
* File: winntThrMgr.cc.
* Desc: Implementation of the WntThreadManager class
* Module: AkraLog : JavaKit.
* Rev: 23 septembre 1997 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include "winntThread.h"
#include "winntThrMgr.h"

WntThreadManager::WntThreadManager(JVCup *aCup)
    : JVThreadManager(aCup)
{

}


void WntThreadManager::lowLevelInit(void)
{
    centralCond= new WntJVCondition();
    centralLock= new WntJVMutex();
}


JVThread *WntThreadManager::createThread(unsigned int anID, JVThreadSet *aSet)
{
    return (JVThread *)new WntJVThread(anID, aSet);
}


JVThread *WntThreadManager::createThread(unsigned int anID, JVThread *aParent)
{
    return (JVThread *)new WntJVThread(anID, aParent);
}


JVSynchronizer *WntThreadManager::createSynchronizer(void)
{
    return new WntJVSynchro();
}

