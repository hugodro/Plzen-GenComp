/**************************************************
* File: threadManager.cc.
* Desc: Implementation of the JVThreadManager class
* Module: AkraLog : JavaKit.
* Rev: 16 novembre 1996 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <vm/jvThread.h>
#include <vm/fsmThread.h>
#include <vm/jvInstance.h>
#include <vm/classFile.h>
#include <vm/jvcup.h>
#include <vm/valueTypes.h>
#include <vm/stacks.h>

#include "rawThreads.h"
#include "synchro.h"
#include "threadManager.h"


// ATTENTION: Work in progress.


/**************************************************
* Implementation: JVThreadManager.
**************************************************/

JVThreadManager::JVThreadManager(JVCup *aCup)
{
    kernel= aCup;
    threadGroups= NULL;
    systemThreads= NULL;
    javaThreads= NULL;
    fsmThreads= NULL;
}


JVThreadManager::~JVThreadManager(void)
{

}


void JVThreadManager::lowLevelInit(void)
{
    centralCond= new JVCondition();
    centralLock= new JVMutex();
}


int JVThreadManager::initJavaSupport(void)
{
    JVMachine *defaultMachine;
    MethodInfo *initInfo= NULL;
    JVMethod *initMethod;
    JVInstance *arguments[2];
    JVRunValue *tmpValue;

    defaultMachine= kernel->getDefaultMachine();
    mainThread= createNewThreadFrom(NULL);
    defaultMachine->setWorkContext(mainThread);

    jlThread= kernel->loadClass("java/lang/Thread");
    jlThreadGroup= kernel->loadClass("java/lang/ThreadGroup");
    if ((jlThread == NULL) || (jlThreadGroup == NULL)) {
	return -1;
    }

    jvSystemGroup= new JVInstance(jlThreadGroup);
    jvMainGroup= new JVInstance(jlThreadGroup);

    initInfo= jlThreadGroup->getMethodNamed("<init>", "()V");
    if (initInfo != NULL) {
	initMethod= initInfo->resolve(kernel->getClassManager());
	if (initMethod != NULL) {
	    defaultMachine->runMethod(initMethod, jvSystemGroup);
	}
	else return -2;
    }
    else return -3;

    initInfo= jlThreadGroup->getMethodNamed("<init>", "(Ljava/lang/ThreadGroup;Ljava/lang/String;)V");
    if (initInfo != NULL) {
	initMethod= initInfo->resolve(kernel->getClassManager());
	if (initMethod != NULL) {
	    arguments[0]= jvSystemGroup;
	    arguments[1]= defaultMachine->createStringFromNarrowArray("Main");
	    defaultMachine->runMethodWithArguments(jvMainGroup, initMethod, 2, (void **)arguments);
	}
	else return -4;
    }
    else return -5;

    jvMainThread= new JVInstance(jlThread);
    mainThread->setJavaDual(jvMainThread);

    /*********
    *    Set the PrivateInfo value to point to ourself; it will be used by the initializer as the
    *   parent thread.
    *    ATTN-971006: Check that there is no problem with having a circularity
    *	in the parent pointer.
    *********/
    tmpValue= new JVRunValue(TYPE_BASIC, (void *)mainThread->getID());
    jvMainThread->putAtIndex(*tmpValue, 3);
	// Set priority.
    tmpValue->setValue((void *)5);
    jvMainThread->putAtIndex(*tmpValue, 1);
	// Set group.
    tmpValue->setType(TYPE_INSTANCE);
    tmpValue->setValue(jvMainGroup);
    jvMainThread->putAtIndex(*tmpValue, 10);
    delete tmpValue;

    initInfo= jlThread->getMethodNamed("<init>", "(Ljava/lang/ThreadGroup;Ljava/lang/String;)V");
    if (initInfo != NULL) {
	initMethod= initInfo->resolve(kernel->getClassManager());
	if (initMethod != NULL) {
	    arguments[0]= jvMainGroup;
	    defaultMachine->runMethodWithArguments(jvMainThread, initMethod, 2, (void **)arguments);
	}
	else {
	    return -6;
	}
    }
    else {
	return -7;
    }

    return 0;
}


void JVThreadManager::doSharing(void)
{

}


unsigned int JVThreadManager::getUniqueID(void)
{
    static unsigned int uniqueID= 1;

    return uniqueID++;
}


JVThread *JVThreadManager::currentThread(void)
{
    return javaThreads;		// ATTENTION: Must re-implement for correct OS.
}


JVThread *JVThreadManager::getThreadForID(unsigned int anID)
{
    JVThread *tmpThread= NULL;

    if (javaThreads != NULL) {
	tmpThread= javaThreads->findID(anID);
    }

// ATTN: Must scan in other queues for the thread, in case it is not a JVThread.

    return tmpThread;
}


JVCup *JVThreadManager::getKernel(void)
{
    return kernel;
}


JVThread *JVThreadManager::spawnThreadFrom(JVThread *parent)
{
    JVThread *tmpThread;

    tmpThread= javaThreads->getFirstDead();

    if (tmpThread == NULL) {
	tmpThread= createNewThreadFrom(parent);
    }
    else {
	parent->getThreadSet()->addThread(tmpThread);
    }

    return tmpThread;
}


JVThread *JVThreadManager::createNewThreadFrom(JVThread *parent)
{
    JVThread *newThread= NULL;
    JVThreadSet *threadSet;

    if (parent == NULL) {		// Should only occur when the first thread of them all is launched.
	threadSet= createNewSet("Main", NULL);
	newThread= createThread(getUniqueID(), threadSet);
    }
    else {
	newThread= createThread(getUniqueID(), parent);
    }

    if (newThread) {
	newThread->setNextGlobal(javaThreads);
	javaThreads= newThread;
    }

    return newThread;
}


PVFsmThread *JVThreadManager::createNewFsmThread(void)
{
    PVFsmThread *result= NULL;

    // TMP-970526 [HD]: Just create an abstract thread for now.
    result= new PVFsmThread(getUniqueID());
    return result;
}


JVThread *JVThreadManager::createThread(unsigned int anID, JVThreadSet *aSet)
{
    return new JVThread(anID, aSet);
}


JVThread *JVThreadManager::createThread(unsigned int anID, JVThread *aParent)
{
    return new JVThread(anID, aParent);
}


JVThreadSet *JVThreadManager::createNewSet(char *groupName, JVThreadSet *parent)
{
    JVThreadSet *threadSet;

    if ((threadSet= createThreadSet(groupName, parent)) != NULL) {
	if (threadGroups != NULL) {
	    threadGroups->setNextInList(threadSet);
	}
	threadGroups= threadSet;
    }
    return threadSet;
}


JVThreadSet *JVThreadManager::createThreadSet(char *groupName, JVThreadSet *parent)
{
    return new JVThreadSet(groupName, parent);
}


void JVThreadManager::waitJavaThreads(void)
{
    JVThreadSet *tmpThread;

    centralLock->lock();	
#if !defined(MSDOS)
    while (javaThreads->globalNext != NULL) {
	centralLock->releaseWhile(centralCond);
    }
#endif
    centralLock->unlock();
}


void JVThreadManager::handleDeath(JVThread *aThread)
{
    centralLock->lock();
    if (javaThreads == aThread) {
	javaThreads= aThread->globalNext;
    }
    else {
	// Find the thread, get rid of it.
	JVThread *lastThread, *tmpThread;

	lastThread= javaThreads;
	if (javaThreads != NULL) {
	    tmpThread= javaThreads->globalNext;
	}
	else {
	    tmpThread= NULL;
	}
	while (tmpThread != NULL) {
	    if (aThread == tmpThread) {
		lastThread->globalNext= tmpThread->globalNext;
// ATTN: Must put the thread in a 'dead' queue.
//		delete aThread;
		break;
	    }
	    lastThread= tmpThread;
	    tmpThread= tmpThread->globalNext;
	}
    }
    centralLock->unlock();
    centralCond->declareSatisfied();
}


bool JVThreadManager::addSynchronizerFor(JVAbstractInstance *anInstance)
{
    bool result= true;

    centralLock->lock();		// ATTN: Could be another lock (synchro queue lock).
    if (anInstance->getSynchronizer() == NULL) {
	JVSynchronizer *aSynchro;
	
	if ((aSynchro= createSynchronizer()) != NULL) {
	    anInstance->setSynchronizer(aSynchro);
	}
	else result= false;
    }
    centralLock->unlock();
    return result;
}


bool JVThreadManager::addSynchronizerFor(ClassFile *aClass)
{
    bool result= true;

    centralLock->lock();		// ATTN: Could be another lock (synchro queue lock).
    if (aClass->getSynchronizer() == NULL) {
	JVSynchronizer *aSynchro;
	
	if ((aSynchro= createSynchronizer()) != NULL) {
	    aClass->setSynchronizer(aSynchro);
	}
	else result= false;
    }
    centralLock->unlock();
    return result;
}


JVSynchronizer *JVThreadManager::createSynchronizer(void)
{
    return new JVSynchronizer();
}

