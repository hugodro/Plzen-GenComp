#ifndef _CLASSFILEOBJECT_H_
#define _CLASSFILEOBJECT_H_
/**************************************************
* File: classFileObject.h.
* Desc: Basic object holder for class file instantiation in memory.
* Module: AkraLog : JavaKit.
* Rev: 3 mai 1996 : REV 0.1 : Hugo DesRosiers : Independance clean-up.
**************************************************/

#include <iostream>

#include <akra/AkObject.h>
#include <akra/portableDefs.h>		// For the 'bool' type.

/* Access flags for various class file components. */
#define ACC_PUBLIC				0x0001
#define ACC_PRIVATE				0x0002
#define ACC_PROTECTED			0x0004
#define ACC_STATIC				0x0008
#define ACC_FINAL				0x0010
#define ACC_SYNCHRONIZED		0x0020
#define ACC_VOLATILE			0x0040
#define ACC_TRANSIENT			0x0080
#define ACC_NATIVE				0x0100
#define ACC_INTERFACE			0x0200
#define ACC_ABSTRACT			0x0400

#define INTERN_ACC_CONSTR		0x0800

#define MOD_IMPLICIT			0x0001
#define MOD_BLOCKING			0x0002
#define MOD_IMMEDIATE			0x0004
#define MOD_PARALLEL			0x0008
#define MOD_PUSH				0x0010
#define MOD_POP				0x0020
#define MOD_ROLLBACK			0x0040
#define MOD_PASSBY				0x0080
#define MOD_CONSUME			0x0100
#define MOD_DEFAULT			0x0200
#define MOD_LOCALVAR			0x0000	// NB: (0 << 9) == 0.
#define MOD_FIELD				0x0400
#define MOD_EXIT				0x0800


// Types used to express data size in objects.
typedef unsigned char u1;
typedef unsigned short u2;
typedef unsigned int u4;


class ClassFileObject : public AkObject {
  public:
    enum PrimitiveTypes {
	ptBoolean= 0, ptChar, ptByte, ptShort, ptInteger, ptFloat, ptLong, ptDouble
	, ptVoid
	, numOfPrimitiveTypes
    };

  protected:
    bool isResolved;

  public:
    ClassFileObject();
    virtual bool read(std::istream *aStream);
    virtual bool write(std::ostream *aStream);
 };


#endif	/* _CLASSFILEOBJECT_H_ */
