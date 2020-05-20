#ifndef _SYNCHRO_H_
#define _SYNCHRO_H_
/**************************************************
* File: synchro.h.
* Desc: Definition of the synchronization mechanism.
* Module: AkraLog : JavaKit.
* Rev: 9 mai 1996 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <akra/AkObject.h>
#include <akra/portableDefs.h>


class JVMutex;
class JVCondition;
class JVThread;


class JVSynchronizer : public AkObject {
  protected:
// ATTN: Should we make up the Synchro with the sub-items, or with whatever
//	is most convenient ?
//    JVMutex *mutex;
//    JVCondition *condition;

  public:
    JVSynchronizer(void);
    virtual ~JVSynchronizer(void);

	// Mutex part.
    virtual void lock(JVThread *aThread);
    virtual bool lockNonInfinite(JVThread *aThread, unsigned int waitTime);
    virtual void unlock(JVThread *aThread);
    virtual void releaseWhile(JVCondition *aCondition);
    virtual bool releaseWhileNonInfinite(JVCondition *aCondition, unsigned int waitTime);

	// Condition part.
    virtual void declareSatisfied(void);
    virtual void declareSatisfiedToAll(void);
    virtual void wait(void);
    virtual bool waitWithTimeout(unsigned long someTime);
};

#endif	/* _SYNCHRO_H_ */
