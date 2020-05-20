/**************************************************
* File: zipStream.h.
* Desc: Defines the ZipArchive classes.
* Module: AkraLog : JavaKit.
* Rev: 24 juin 1997 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <fstream>

#include <akra/AkObject.h>
#include <akra/portableDefs.h>



#define PkZipMakeUShort(x)		\
		((unsigned short)((unsigned short)((x)[0]) + (((unsigned short)((x)[1])) << 8)))

#define PkZipMakeUInteger(x)	\
		((unsigned int)	 \
			((unsigned int)((x)[0])) + (((unsigned int)((x)[1])) << 8) + \
			(((unsigned int)((x)[2])) << 16) + (((unsigned int)((x)[3])) << 24))



#define UNZIP_VERSION     	20   // PKZip version 2.0 compatible.
#define VMS_UNZIP_VERSION	42   //  Understand VMS enzipping.

#define ZIPSUFFIX	"zip"
#define CENTRAL_HDR_SIG	"\113\001\002"
#define LOCAL_HDR_SIG		"\113\003\004"
#define END_CENTRAL_SIG	"\113\005\006"
#define EXTD_LOCAL_SIG		"\113\007\010"


class LocalFileEntry : public AkObject {
    enum LocalLayout {
	VERSION_NEEDED_TO_EXTRACT_0= 0,
	VERSION_NEEDED_TO_EXTRACT_1= 1,
	GENERAPURPOSE_BIT_FLAG= 2,
	COMPRESSION_METHOD= 4,
	LAST_MOD_FILE_TIME= 6,
	LAST_MOD_FILE_DATE= 8,
	CRC32= 10,
	COMPRESSED_SIZE= 14,
	UNCOMPRESSED_SIZE= 18,
	FILENAME_LENGTH= 22,
	EXTRA_FIELD_LENGTH= 24
    };

  public:
    enum Constants {
	Size= 26
    };

  protected:
    unsigned char version_needed_to_extract[2];
    unsigned short general_purpose_bit_flag;
    unsigned short compression_method;
    unsigned short last_mod_file_time;
    unsigned short last_mod_file_date;
    unsigned int crc32;
    unsigned int csize;
    unsigned int ucsize;
    unsigned short filename_length;
    unsigned short extra_field_length;

  public:
    virtual bool read(std::istream *aStream);
};


class DirectoryEntry : public AkObject {
    enum DirectoryLayout {
	VERSION_MADE_BY_0= 0,
	VERSION_MADE_BY_1= 1,
	VERSION_NEEDED_TO_EXTRACT_0= 2,
	VERSION_NEEDED_TO_EXTRACT_1= 3,
	GENERAL_PURPOSE_BIT_FLAG= 4,
	COMPRESSION_METHOD= 6,
	LAST_MOD_FILE_TIME= 8,
	LAST_MOD_FILE_DATE= 10,
	CRC32= 12,
	COMPRESSED_SIZE= 16,
	UNCOMPRESSED_SIZE= 20,
	FILENAME_LENGTH= 24,
	EXTRA_FIELD_LENGTH= 26,
	FILE_COMMENT_LENGTH= 28,
	DISK_NUMBER_START= 30,
	INTERNAL_FILE_ATTRIBUTES= 32,
	EXTERNAL_FILE_ATTRIBUTES= 34,
	RELATIVE_OFFSET_LOCAL_HEADER= 38
    };

  public:
    enum Constants {
	Size= 42
    };

  protected:
       unsigned char version_made_by[2];
       unsigned char version_needed_to_extract[2];
       unsigned short general_purpose_bit_flag;
       unsigned short compression_method;
       unsigned short last_mod_file_time;
       unsigned short last_mod_file_date;
       unsigned int crc32;
       unsigned int csize;
       unsigned int ucsize;
       unsigned short filename_length;
       unsigned short extra_field_length;
       unsigned short file_comment_length;
       unsigned short disk_number_start;
       unsigned short internal_file_attributes;
       unsigned int external_file_attributes;
       unsigned int relative_offset_local_header;
       char *fileName;

  public:
    DirectoryEntry(unsigned char *aBuffer);
    virtual bool readFileName(std::istream *aStream);
    virtual bool isNamed(char *aName);
    virtual unsigned int getDataStart(void);
    virtual unsigned int getRelOffset(void);
};


class CentralDirectoryTable : public AkObject {
    unsigned int nbrEntries;
    DirectoryEntry **entries;

  public:
    CentralDirectoryTable(unsigned int aCount);
    bool read(std::istream *aStream, unsigned int dirSize);
    DirectoryEntry *getEntryCalled(char *aName);
};


class EndCentralHeader : public AkObject {
  protected:
    enum EndCentralDirLayout {
	SIGNATURE= 0,
	NUMBER_THIS_DISK= 4,
	NUM_DISK_WITH_START_CENTRAL_DIR= 6,
	NUM_ENTRIES_CENTRL_DIR_THS_DISK= 8,
	TOTAL_ENTRIES_CENTRAL_DIR= 10,
	SIZE_CENTRAL_DIRECTORY= 12,
	OFFSET_START_CENTRAL_DIRECTORY= 16,
	ZIPFILE_COMMENT_LENGTH= 20
    };

  public:
    enum Constants {
	Size= 18
    };

  protected:
    unsigned short number_this_disk;
    unsigned short num_disk_with_start_central_dir;
    unsigned short num_entries_centrl_dir_ths_disk;
    unsigned short total_entries_central_dir;
    unsigned int size_central_directory;
    unsigned int offset_start_central_directory;
    unsigned short zipfile_comment_length;

  public:
    EndCentralHeader(void);
    virtual bool read(std::istream *aStream);
    virtual unsigned int getDirCount(void);
    virtual unsigned int getDirSize(void);
};


class ZipStream : public std::ifstream {
  public:
    ZipStream(char *aName);
};


class ZipArchive : public AkObject {
  public:
    enum CompMethods {
	STORED= 0,
	SHRUNK= 1,
	REDUCED1= 2,
	REDUCED2= 3,
	REDUCED3= 4,
	REDUCED4= 5,
	IMPLODED= 6,
	TOKENIZED= 7,
	DEFLATED= 8
      , NUM_METHODS= 9
    };

  protected:
    char *archiveFilePath;
    EndCentralHeader *endCentralDir;
    CentralDirectoryTable *centralDir;

  public:
    ZipArchive(void);
    virtual bool loadArchiveDirectory(char *aName);
    virtual ZipStream *getFile(char *aName);
    virtual DirectoryEntry *statFile(char *aName);
};


