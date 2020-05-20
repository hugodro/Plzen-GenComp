/**************************************************
* File: classFileObject.cc.
* Desc: Implementation of the basic object holder for class file instantiation in memory.
* Module: AkraLog : JavaKit.
* Rev: 3 mai 1996 : REV 0.1 : Hugo DesRosiers : Independance clean-up.
**************************************************/

#include "classFileObject.h"

/**************************************************
* Implementation: ClassFileObject.
**************************************************/

ClassFileObject::ClassFileObject()
{
    isResolved= false;
}

bool ClassFileObject::read(std::istream *aStream)
{
    return true;
}

bool ClassFileObject::write(std::ostream *aStream)
{
    return true;
}


