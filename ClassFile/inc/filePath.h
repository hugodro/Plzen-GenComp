#ifndef _FILEPATH_H_
#define _FILEPATH_H_
/**************************************************
* File: filePath.h.
* Desc: Definition of a file path (includes URL).
* Module: AkraLog : JavaKit.
* Rev: 3 mai 1996 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <akra/AkObject.h>
#include <akra/portableDefs.h>



#define NBR_URL_TYPES		5

#ifndef S_ISDIR
#define S_ISDIR(mode)   (((mode) & (_S_IFMT)) == (_S_IFDIR))
#endif


class ZipArchive;


static unsigned int nbrUrlTypes= NBR_URL_TYPES;
static char *urlTypes[NBR_URL_TYPES] = { "file", "ftp", "gopher", "http", "wais"};


class Path : public AkObject {
  private:
    Path *next;

  public:
    enum Flags {
	exists= 1,
	archive= 2, directory= 4
    };

  protected:
    unsigned int type;	// Type.
    char *fullPath;		// Type + host + filePath + fileName + extension.
    char *host;			// Host name in internet.
    char *localPath;		// file path, once on  host.
    char *fileName; 		// file name (without extension).
    char *extension;		// extension of the file.
    unsigned short hostNameLength;		// These are 0 if component is not available.
    unsigned short localPathLength;
    unsigned short fileNameLength;
    unsigned short extensionLength;
    unsigned int flags;
    ZipArchive *zipArchive;		// If the path is the lead to an archive.

  public:
    Path();
    Path(char *aURLPath);
    Path(char *type, char *host, char *path, char *fileName, char *extension);
    virtual ~Path();
    virtual bool setURLPath(char *aURLPath);
    virtual bool setMultiComponentPath(char *type, char *host, char *path, char *fileName, char *extension);
    virtual void getType(char *holder);
    virtual unsigned int getTypeIndex(void);
    virtual void getFullPath(char *holder);
    virtual void getHostName(char *holder);
    virtual void getFilePath(char *holder);
    virtual void getFileName(char *holder);
    virtual void getExtension(char *holder);

    virtual void linkTo(Path *nextPath);
    virtual Path *giveNext(void);
    virtual void test(void);
    virtual bool isArchive(void);
    virtual ZipArchive *getArchive(void);

private:
    int findIndexForType(char *aType);
 };


class PathList : public AkObject {
  protected:
    unsigned int nbrElements;
    Path *paths;		// Head of list.
    Path *lastPath;		// Tail of list.
    Path *cursor;		// List browser.

  public:
    PathList();
    virtual ~PathList();
    virtual bool addPath(char *aURLPath);
    virtual Path *getFirst(void);
    virtual Path *getNext(void);
    virtual Path *getIth(unsigned int i);
 };


#endif	/* _FILEPATH_H_ */
