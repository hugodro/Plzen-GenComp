#ifndef _THREADSUPPORT_H_
#define _THREADSUPPORT_H_
/**************************************************
* File: rawThreads.h.
* Desc: Definition of the JVAbstractThread, JVMutex and JVCondition.
* Module: AkraLog : JavaKit.
* Rev: 30 octobre 1996 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <akra/AkObject.h>
#include <akra/portableDefs.h>

class JVThreadManager;
class JVCondition;
class JVMutex;


/********************************************************
* Class: JVAbstractThread.
* Desc: Basic content and functionality of a thread.
* Class variables:
* Instance variables:
* Super-Class: AkObject.
* Module: JavaKit : RunTime.
********************************************************/

class JVAbstractThread : public AkObject {
  public:
    enum Status {		// Uses the first 8 bits.
	useless= 1,		// Not usable, must be first initialized.
	armed= 2,		// Low-level support is there, but not yet running a machine.
	ready= 3,			// Initialized with data (but not low-level support).
	interrupted= 4,	// Was running something, got interrupted.
	running= 5,		// Doing its main job.
	sleeping= 6,		// Waiting for a given moment.
	dying= 7,			// Main job finished, but low-level thread still active (doing cleanup...).
	dead= 8,			// The low-level thread has terminated.
	statusMask= (0x000ff)
    };

    enum SubType {		// A way to identify subtypes of threads.
	lowLevel= 0x01000000,		// A thread running some low-level code.
	java= 0x02000000,		// A thread running a JVMachine.
	fsm= 0x04000000			// A Thread running a PVFsmMachine.
    };

    typedef void (*LauncherType)(void *);


  protected:
    static JVThreadManager *threadManager;	// This JVThread class ThreadManager.

  protected:
    unsigned int priority;				// Priority for the thread.
    int priorityOffset;					// Inc/Dec priority from quantum to quantum.
#if defined(_WIN32)
  public:
    unsigned int status;				// Run status, thread category, sharing mode.
  protected:
#else
    unsigned int status;				// Run status, thread category, sharing mode.
#endif
    unsigned int privateID;			// System internal id.
    unsigned long quantumTime;		// Time ran in current quantum.
    unsigned long runTime;			// Total run time for thread.

  public:
    static void setThreadManager(JVThreadManager *aManager);

  public:
    JVAbstractThread(unsigned int anID);
    virtual ~JVAbstractThread(void);

    virtual SubType getSubClass(void);	// Return the category (sub-class) of the thread.
    virtual unsigned int setPriority(unsigned int aPriority);
    virtual bool shakePriority(int aDelta);

    virtual unsigned int getPriority(void);
    virtual unsigned int getStatus(void);
    virtual unsigned int getID(void);

    virtual bool isAlive(void);		// Indicates if the thread is between a start and an exit.
    virtual void start(void);		// Make thread all ready to execute code (must 'resume' it to really get activity).
    virtual void sleep(unsigned long someTime);	// Put in wait mode for a while.
    virtual void yield(void);			// Offer cpu time to others.
    virtual void resume(void);			// Resume execution of code.
    virtual void suspend(void);		// Stop execution of code.
    virtual void exit(void);				// End execution of code.  Closes a 'start' on a thread.
    virtual void join(JVAbstractThread *aThread);	// Wait for the other thread to finish before unblocking.
    virtual void spawn(void (*aFunction)(void *), void *theReceiver);  // Start the function in its own system thread.

	// Providing subclassed items.
    virtual JVCondition *getACondition(void);
    virtual JVMutex *getAMutex();

};


class JVMutex : public AkObject {
  protected:

  public:
    JVMutex(void);
    JVMutex(char *aName);
    virtual ~JVMutex(void);
    virtual void lock(void);
    virtual bool lockNonInfinite(unsigned int waitTime);
    virtual void unlock(void);
    virtual void releaseWhile(JVCondition *aCondition);
    virtual bool releaseWhileNonInfinite(JVCondition *aCondition, unsigned int waitTime);
};


class JVCondition : public AkObject {
  protected:
  public:
    JVCondition(void);
    JVCondition(char *aName);
    virtual ~JVCondition(void);
    virtual void declareSatisfied(void);
    virtual void declareSatisfiedToAll(void);
    virtual void wait(void);
    virtual bool waitWithTimeout(unsigned long someTime);
};


#endif		/* _THREADSUPPORT_H_  */
