#ifndef _THREADMANAGER_H_
#define _THREADMANAGER_H_
/**************************************************
* File: threadManager.h.
* Desc: Definition of the JVThreadManager class
* Module: AkraLog : JavaKit.
* Rev: 30 octobre 1996 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <akra/AkObject.h>
#include <akra/portableDefs.h>


class JVCup;
class JVThread;
class JVSynchro;
class JVThreadSet;
class PVFsmThread;
class JVCondition;
class JVMutex;
class JVAbstractInstance;
class JVSynchronizer;
class ClassFile;
class JVAbstractThread;
class JVInstance;


class JVThreadManager : public AkObject {
  protected:
    JVCup *kernel;
    ClassFile *jlThread, *jlThreadGroup;
    JVThreadSet *threadGroups;		// Groups of threads.  Currently not managed.
    JVAbstractThread *systemThreads;		// System thread list.
    JVThread *javaThreads;				// Java thread list.
    PVFsmThread *fsmThreads;			// Finite automata thread list.
    JVCondition *centralCond;		// Kernel-like synchronization.
    JVMutex *centralLock;			// Kernel-like resources lock.
    JVThread *mainThread;			// Initial JVThread, created to support all general operations.
    JVInstance *jvMainThread;		// Main Java Thread, used as default parent.
    JVInstance *jvSystemGroup;		// System group, as required by j.l.ThreadGroup.
    JVInstance *jvMainGroup;		// Main Java ThreadGroup, for all normal threads.

  public:
    JVThreadManager(JVCup *aCup);
    virtual ~JVThreadManager(void);

    virtual void lowLevelInit(void);
    virtual int initJavaSupport(void);
    virtual unsigned int getUniqueID(void);		// Provides a unique thread id.
    virtual JVThread *currentThread(void);		// Returns the current running thread.
    virtual JVThread *getThreadForID(unsigned int anID);    // Looks for a thread with a given ID.
    virtual JVCup *getKernel(void);

    virtual void doSharing(void);		// Does some cpu sharing between the awaiting threads.

    virtual JVThread *spawnThreadFrom(JVThread *parent);    // Get a free Java thread (using the right subclass).
    virtual JVThread *createNewThreadFrom(JVThread *parent);    // Create a new Java thread (using the right subclass).

    virtual PVFsmThread *createNewFsmThread(void);    // TODO.
    virtual JVThread *createThread(unsigned int anID, JVThreadSet *aSet);    // Create a real thread (usually subclassed).
    virtual JVThread *createThread(unsigned int anID, JVThread *aParent);    // Create a real thread (usually subclassed).
    virtual JVThreadSet *createNewSet(char *groupName, JVThreadSet *parent);
    virtual JVThreadSet *createThreadSet(char *groupName, JVThreadSet *parent);
    virtual void waitJavaThreads(void);
    virtual void handleDeath(JVThread *aThread);
    virtual bool addSynchronizerFor(JVAbstractInstance *anInstance);
    virtual bool addSynchronizerFor(ClassFile *aClass);
    virtual JVSynchronizer *createSynchronizer(void);
};


#endif		/* _THREADMANAGER_H_ */
