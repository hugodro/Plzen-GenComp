/**************************************************
* File: filePath.cc.
* Desc: Implementation of Path and PathList classes.
* Module: AkraLog : JavaKit.
* Rev: 3 mai 1996 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

extern "C" {
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

// MOD-970520 [HD]: Pour NT.
#if !defined(_WIN32) && !defined(MSDOS)
// MOD-970415 [JST]:
#include <strings.h>
#endif
}


#if defined(_WIN32)
#define index(a, b)		strchr((a),(b))
#define rindex(a,b)		strrchr((a), (b))
#endif

#include <vm/runtimeDefs.h>

#include "zipStream.h"
#include "filePath.h"


/**************************************************
* Implementation: Path.
**************************************************/
Path::Path()
{
    type= 0;
    localPathLength= hostNameLength= fileNameLength= extensionLength= 0;
    fullPath= NULL;
    next= NULL;
    flags= 0;
    zipArchive= NULL;
}


Path::Path(char *aURLPath)
{
    type= 0;
    localPathLength= hostNameLength= fileNameLength= extensionLength= 0;
    next= NULL;
    zipArchive= NULL;
    flags= 0;
    setURLPath(aURLPath);
}


Path::Path(char *type, char *host, char *path, char *fileName, char *extension)
{
    type= 0;
    localPathLength= hostNameLength= fileNameLength= extensionLength= 0;
    next= NULL;
    zipArchive= NULL;
    flags= 0;
    setMultiComponentPath(type, host, path, fileName, extension);
}


Path::~Path()
{
    delete[] fullPath;
}


bool Path::setURLPath(char *aURLPath)
{
    unsigned int typeLength, i;
    char *tmpMemberPtr, *sndMemberPtr;
    bool result= true;			// Suppose success, and prove error, for now.

    fullPath= new char[strlen(aURLPath)+1];
    strcpy(fullPath, aURLPath);

    // Find URL type.
    sndMemberPtr= fullPath;
    tmpMemberPtr= index(fullPath, ':');
    if (tmpMemberPtr != NULL) {
	sndMemberPtr= tmpMemberPtr+1;
	typeLength= tmpMemberPtr - fullPath;
	for (i= 0; i < nbrUrlTypes; i++) {
	    if (strncmp(fullPath, urlTypes[i], typeLength) == 0) {
		break;
	     }
	 }
	type= i;		// If (i == nbrUrlTypes) --> unknown type.
     }
    else type= 0;	// Suppose file.

    // Find host.
    tmpMemberPtr= index(sndMemberPtr, '/');
    if (tmpMemberPtr != NULL) {
	if (*(tmpMemberPtr+1) == '/') {	// Host definition.
	    sndMemberPtr= index(tmpMemberPtr+2, '/');
	    host= tmpMemberPtr+2;
	    if (sndMemberPtr == NULL) {
		hostNameLength= strlen(host);
	     }
	    else {
		hostNameLength= sndMemberPtr - host;
		sndMemberPtr++;
	     }
	 }
	else {		// No host definition.
	    host= NULL;
	    hostNameLength= 0;
	 }
     }

    // Find file path.  Base the 'localPath' on the last fileSep found.  The 'test' method will correct
    // the problem that the last path component was not followed by a trailing fileSep.
    if (sndMemberPtr != NULL) {
	tmpMemberPtr= rindex(sndMemberPtr, FILE_SEPARATOR);
	if (tmpMemberPtr != NULL) {
	    localPath= sndMemberPtr;
	    localPathLength= tmpMemberPtr - sndMemberPtr;
	    sndMemberPtr= tmpMemberPtr + 1;
	}
	else if (sndMemberPtr[0]= '.') {
	    if (sndMemberPtr[1] == '\0') {
		localPath= sndMemberPtr;
		localPathLength= 1;
		sndMemberPtr= NULL;
	    }
	    else if ((sndMemberPtr[1]= '.' ) && (sndMemberPtr[2]= '\0')) {
		localPath= sndMemberPtr;
		localPathLength= 2;
		sndMemberPtr= NULL;
	    }
	}
	else localPath= NULL;
     }

    // Find file name.
    if (sndMemberPtr != NULL) {
	fileName= sndMemberPtr;
	tmpMemberPtr= rindex(sndMemberPtr, '.');
	if (tmpMemberPtr != NULL) {
	    if (tmpMemberPtr == sndMemberPtr) {	// Take care of the '.' as current path or firt char in file name.
		fileNameLength= strlen(fileName);
		sndMemberPtr= NULL;		// No extension left to cover.
	     }
	    else {
		fileNameLength= tmpMemberPtr - sndMemberPtr;
		sndMemberPtr= tmpMemberPtr + 1;
	     }
	 }
	else {
	    fileNameLength= strlen(fileName);
	    sndMemberPtr= NULL;
	}
     }

    // Find extension.
    if (sndMemberPtr != NULL) {
	extension= sndMemberPtr;
	extensionLength= strlen(extension);
     }

    return result;
}


bool Path::setMultiComponentPath(char *aType, char *aHost, char *aPath, char *aFileName, char *anExtension)
{
    unsigned int fullPathLen= 0;
    bool result= true;

    if (aType != NULL) fullPathLen+= strlen(aType) + 1;    // Add ':' length.
    if (aHost != NULL) fullPathLen+= (hostNameLength= strlen(aHost)) + 3;      // Add '//' and '/' length.
    if (aPath != NULL) fullPathLen+= (localPathLength= strlen(aPath)) + 1;    // Add the '/' length.
    if (aFileName != NULL) fullPathLen+= (fileNameLength= strlen(aFileName));
    if (anExtension != NULL) fullPathLen+= (extensionLength= strlen(anExtension)) + 1;    // Add the '.' length.

    if (fullPathLen > 0) {
	fullPath= new char[fullPathLen+1];
	fullPath[0]= '\0';
	if (aType != NULL) {
	    strcpy(fullPath, aType);
	    strcat(fullPath, ":");
	    type= findIndexForType(aType);
	 }
	if (aHost != NULL) {
	    strcat(fullPath, "//");
	    host= fullPath + strlen(fullPath);
	    strcat(fullPath, aHost);
	    if ((aPath != NULL) || (aFileName != NULL) || (anExtension != NULL)) strcat(fullPath, "/");
	 }
	if (aPath != NULL) {
	    char fileSeparator[2]= { FILE_SEPARATOR, '\0' };
	    localPath= fullPath + strlen(fullPath);
	    strcat(fullPath, aPath);
	    if ((aFileName != NULL) || (anExtension != NULL)) strcat(fullPath, fileSeparator);
	 }
	if (aFileName != NULL) {
	    fileName= fullPath + strlen(fullPath);
	    strcat(fullPath, aFileName);
	 }
	if (anExtension != NULL) {
	    strcat(fullPath, ".");
	    extension= fullPath + strlen(fullPath);
	    strcat(fullPath, anExtension);
	 }

     }
    else result= false;

    return result;
}


// Not responsable to free allocated memory when unknown type in URL !
void Path::getType(char *holder)
{
    char *tmpTypeEnd;
    
    if (type < nbrUrlTypes) {
	strcpy(holder, urlTypes[type]);
     }
    else {
	tmpTypeEnd= index(fullPath, ':');
	if (tmpTypeEnd) {
	    memcpy(holder, fullPath, (tmpTypeEnd - fullPath));
	    holder[(tmpTypeEnd - fullPath)]= '\0';
	 }
	else strcpy(holder, urlTypes[type]);		// Bail out with a 'file' type.
     }
}


unsigned int Path::getTypeIndex(void)
{
    return type;
}


void Path::getFullPath(char *holder)
{
    if (holder != NULL) strcpy(holder, fullPath);
}


void Path::getHostName(char *holder)
{
    if (hostNameLength > 0) {
	if (holder != NULL) {
	    memcpy(holder, host, hostNameLength);
	    holder[hostNameLength]= '\0';
	 }
     }
}


void  Path::getFilePath(char *holder)
{    
    if (localPathLength > 0) {
	if (holder != NULL) {
	    memcpy(holder, localPath, localPathLength);
	    holder[localPathLength]= '\0';
	 }
     }
    else {
	if (holder != NULL) holder[0]= '\0';
    }
}


void Path::getFileName(char *holder)
{
    if (fileNameLength > 0) {
	if (holder != NULL) {
	    memcpy(holder, fileName, fileNameLength);
	    holder[fileNameLength]= '\0';
	 }
     }
}


void Path::getExtension(char *holder)
{    
    if (extensionLength > 0) {
	if (holder != NULL) {
	    memcpy(holder, extension, extensionLength);
	    holder[extensionLength]= '\0';
	 }
     }
}


void Path::linkTo(Path *nextPath)
{
    next= nextPath;
}


int Path::findIndexForType(char *aType)
{
    unsigned int typeLength, i;

    if (aType != NULL) {
	typeLength= strlen(aType);
	for (i= 0; i < nbrUrlTypes; i++) {
	    if (strncmp(aType, urlTypes[i], typeLength) == 0) {
		break;
	     }
	 }
     }
    else i= 0;	// Suppose file.

    return i;
}


Path * Path::giveNext(void)
{
    return next;
}


void Path::test(void)
{
    struct stat tmpStatInfo;
    bool result= false;

    if (extensionLength == 3) {		// Test for archive.
#if defined(MSDOS)
	if (stricmp(extension, ZIPSUFFIX) == 0) {
#else
	if (strcmp(extension, ZIPSUFFIX) == 0) {
#endif
	    if (stat(localPath, &tmpStatInfo) == 0) {
		zipArchive= new ZipArchive();
		if (!zipArchive->loadArchiveDirectory(localPath)) {
		    // ATTN: Must give an error msg.
		}
		flags= exists | archive;
	    }
	}
    }
    if ((flags & exists) == 0) {
	if (stat(localPath, &tmpStatInfo) == 0) {
	    flags= exists;
	    if (S_ISDIR(tmpStatInfo.st_mode)) {
		if (fileNameLength != 0) localPathLength+= fileNameLength + 1;
		if (extensionLength != 0) localPathLength+= extensionLength + 1;
		fileName= extension= NULL;
		flags|= directory;
	    }
	}
    }
}


bool Path::isArchive(void)
{
    return ((flags & archive) != 0);
}


ZipArchive *Path::getArchive(void)
{
    return zipArchive;
}


/**************************************************
* Implementation: PathList.
**************************************************/

PathList::PathList()
{
    nbrElements= 0;
    paths= lastPath= cursor= 0;
}


PathList::~PathList()
{
    while (paths != NULL) {
	cursor= paths;
	paths= paths->giveNext();
	delete cursor;
     }
}


bool PathList::addPath(char *aURLPath)
{
    Path *newPath;
    bool result= true;

    newPath= new Path;
    if (newPath->setURLPath(aURLPath)) {
	nbrElements++;
	if (lastPath != NULL) lastPath->linkTo(newPath);
	else {
	    paths= newPath;
	 }
	lastPath= newPath;
     }
    else {
	delete newPath;
	result= false;
     }

    return result;
}


Path * PathList::getFirst(void)
{
    if (paths != NULL) {
/* MOD-970611 [HD]: The cursor is looking at the current element.	cursor= paths->giveNext();
*/
	cursor= paths;
     }
    return paths;
}


Path * PathList::getNext(void)
{
/* MOD-970611 [HD]: The result is always current cursor.
    Path *result= cursor;
*/
    Path *result;

    if (cursor != NULL) {
	cursor= cursor->giveNext();
/* MOD-970611 [HD]: The result is always current cursor.
	if (cursor != NULL) result= cursor;
*/
     }

    result= cursor;
    return result;
}


Path * PathList::getIth(unsigned int i)
{
    Path *result;	

    result= paths;
    while (result != NULL) {
	if (i == 0) break;
	i--;
	result= result->giveNext();
     }

    if (result != NULL) cursor= result->giveNext();
	return result;
}


