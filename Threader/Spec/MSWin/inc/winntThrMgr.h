#ifndef _WINNTTHRMGR_H_
#define _WINNTTHRMGR_H_
/**************************************************
* File: winntThrMgr.h.
* Desc: Definition of the WntThreadManager class
* Module: AkraLog : JavaKit.
* Rev: 23 septembre 1997 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include "threadManager.h"

class WntThreadManager : public JVThreadManager {
  public:
    WntThreadManager(JVCup *aCup);

    virtual void lowLevelInit(void);
    virtual JVThread *createThread(unsigned int anID, JVThreadSet *aSet);
    virtual JVThread *createThread(unsigned int anID, JVThread *aParent);
    virtual JVSynchronizer * createSynchronizer(void);
};


#endif		/* _WINNTTHRMGR_H_ */