#ifndef _NEXTSTEPTHREAD_H_
#define _NEXTSTEPTHREAD_H_
/**************************************************
* File: nsThread.cc.
* Desc: Definition of the NsJVThread class.
* Module: AkraLog : Plzen.
* Rev: 22 sept. 1997 : REV 0 : Hugo DesRosiers.
**************************************************/

#include <mach/cthreads.h>
#include "synchro.h"
#include "jvThread.h"


class NsJVMutex : public JVMutex {
  protected:
    struct mutex nsMutex;

  public:
    NsJVMutex(void);
    NsJVMutex(char *aName);
    virtual ~NsJVMutex(void);
    virtual void lock(void);
    virtual bool lockNonInfinite(unsigned int waitTime);
    virtual void unlock(void);
    virtual void releaseWhile(JVCondition *aCondition);
    virtual bool releaseWhileNonInfinite(JVCondition *aCondition, unsigned int waitTime);
};


class NsJVCondition : public JVCondition {
    friend class NsJVMutex;
  protected:
    struct condition nsCondition;

  public:
    NsJVCondition(void);
    NsJVCondition(char *aName);
    virtual ~NsJVCondition(void);
    virtual void declareSatisfied(void);
    virtual void declareSatisfiedToAll(void);
    virtual void wait(void);
    virtual bool waitWithTimeout(unsigned long someTime);
};


class NsJVSynchro : public JVSynchronizer {
  protected:
    NsJVMutex mutex;
    NsJVCondition condition;
    JVThread *lockOwner;	// Use to manage multi-relocking.
    unsigned int lockCount;	// How many time a some object has locked the synchro.

  public:
    NsJVSynchro(void);
    virtual ~NsJVSynchro(void);

	// Mutex part.
    virtual void lock(JVThread *aThread);
    virtual boolean lockNonInfinite(JVThread *aThread, unsigned int waitTime);
    virtual void unlock(JVThread *aThread);
    virtual void releaseWhile(JVCondition *aCondition);
    virtual bool releaseWhileNonInfinite(JVCondition *aCondition, unsigned int waitTime);

	// Condition part.
    virtual void declareSatisfied(void);
    virtual void declareSatisfiedToAll(void);
    virtual void wait(void);
    virtual bool waitWithTimeout(unsigned long someTime);
};


class NsJVThread : public JVThread {
  private:
    static any_t nsMachineLauncher(any_t anObject);
    static unsigned int JavaToNSPriority(unsigned int aPriority);

  protected:
    cthread_t nsThread;
    NsJVMutex mainMutex;
    NsJVCondition mainCondition;

  public:
    NsJVThread(unsigned int anID, JVThread *parent);
    NsJVThread(unsigned int anID, JVThreadSet *aSet);
    virtual ~NsJVThread(void);

    virtual void init(void);
    virtual SubType getSubClass(void);
    virtual unsigned int setPriority(unsigned int aPriority);

    virtual void start(void);
    virtual void sleep(unsigned long someTime);
    virtual void yield(void);			// Offer cpu time to others.
    virtual void resume(void);
    virtual void suspend(void);
    virtual void exit(void);

    virtual void join(JVAbstractThread *aThread);
    virtual void spawn(void (*aFunction)(void *), void *theReceiver);

    virtual JVSynchronizer *createSynchronizer(void);
};


#endif		/* _NEXTSTEPTHREAD_H_ */
