#ifndef _POSIXTHRMGR_H_
#define _POSIXTHRMGR_H_
/**************************************************
* File: posixhrMgr.h.
* Desc: Definition of the PosixThreadManager class
* Module: AkraLog : JavaKit.
* Rev: 2 decembre 1999 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <threader/threadManager.h>

class PosixThreadManager : public JVThreadManager {
  public:
    PosixThreadManager(JVCup *aCup);

    virtual void lowLevelInit(void);
    virtual JVThread *createThread(unsigned int anID, JVThreadSet *aSet);
    virtual JVThread *createThread(unsigned int anID, JVThread *aParent);
    virtual JVSynchronizer * createSynchronizer(void);
};


#endif		/* _POSIXTHRMGR_H_ */
