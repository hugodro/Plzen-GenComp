#ifndef _NEXTSTEPTHRMGR_H_
#define _NEXTSTEPTHRMGR_H_
/**************************************************
* File: nsThrMgr.h.
* Desc: Definition of the NsThreadManager class
* Module: AkraLog : JavaKit.
* Rev: 23 septembre 1997 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include "threadManager.h"

class NsThreadManager : public JVThreadManager {
  public:
    NsThreadManager(JVCup *aCup);

    virtual void lowLevelInit(void);
    virtual JVThread *createThread(unsigned int anID, JVThreadSet *aSet);
    virtual JVThread *createThread(unsigned int anID, JVThread *aParent);
    virtual JVSynchronizer * createSynchronizer(void);
};


#endif		/* _NEXTSTEPTHRMGR_H_ */