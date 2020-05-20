#ifndef _WINNTTHREAD_H_
#define _WINNTTHREAD_H_
/**************************************************
* File: winntThread.cc.
* Desc: Definition of the wntJvThread class.
* Module: AkraLog : Plzen.
* Rev: 22 sept. 1997 : REV 0 : Hugo DesRosiers.
**************************************************/

#include <windows.h>
#include "synchro.h"
#include "jvThread.h"


class WntJVMutex : public JVMutex {
  protected:
    HANDLE wntMutex;

  public:
    WntJVMutex(void);
    WntJVMutex(char *aName);
    virtual ~WntJVMutex(void);
    virtual void lock(void);
    virtual bool lockNonInfinite(unsigned int waitTime);
    virtual void unlock(void);
    virtual void releaseWhile(JVCondition *aCondition);
    virtual bool releaseWhileNonInfinite(JVCondition *aCondition, unsigned int waitTime);
};


class WntJVCondition : public JVCondition {
    friend class WntJVMutex;
  protected:
    HANDLE wntCondition;

  public:
    WntJVCondition(void);
    WntJVCondition(char *aName);
    virtual ~WntJVCondition(void);
    virtual void declareSatisfied(void);
    virtual void declareSatisfiedToAll(void);
    virtual void wait(void);
    virtual bool waitWithTimeout(unsigned long someTime);
};


class WntJVSynchro : public JVSynchronizer {
  protected:
    WntJVMutex mutex;
    WntJVCondition condition;

  public:
    WntJVSynchro(void);
    virtual ~WntJVSynchro(void);

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


class WntJVThread : public JVThread {
  private:
    static int wntMachineLauncher(void *anObject);

  protected:
    HANDLE winThread;		// WinNT thread structure.
    DWORD winID;		// Thread ID given by WinNT.

  public:
    WntJVThread(unsigned int anID, JVThread *parent);
    WntJVThread(unsigned int anID, JVThreadSet *aSet);
    virtual ~WntJVThread(void);

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


#endif		/* _WINNTTHREAD_H_ */
