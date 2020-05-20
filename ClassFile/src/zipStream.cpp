/**************************************************
* File: zipStream.cc.
* Desc: Implements the ZipArchive classes.
* Module: AkraLog : JavaKit.
* Rev: 24 juin 1997 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

extern "C" {
#include <string.h>
}

#include <fstream>
#include <istream>

#include <akra/convertMacro.h>
#include "zipStream.h"


/**************************************************
* Implementation: LocalFileEntry.
**************************************************/

bool LocalFileEntry::read(std::istream *aStream)
{
     char FullBuffer[Size+4], *buffer;

    aStream->read(FullBuffer, Size+4);
    buffer= &FullBuffer[4];
    if (!aStream->fail()) {
	version_needed_to_extract[0]= buffer[VERSION_NEEDED_TO_EXTRACT_0];
	version_needed_to_extract[1]= buffer[VERSION_NEEDED_TO_EXTRACT_1];
	general_purpose_bit_flag= PkZipMakeUShort(buffer +GENERAPURPOSE_BIT_FLAG);
	compression_method= PkZipMakeUShort(buffer +COMPRESSION_METHOD);
	last_mod_file_time= PkZipMakeUShort(buffer +LAST_MOD_FILE_TIME);
	last_mod_file_date= PkZipMakeUShort(buffer +LAST_MOD_FILE_DATE);
	crc32= PkZipMakeUInteger(buffer +CRC32);
	csize= PkZipMakeUInteger(buffer +COMPRESSED_SIZE);
	ucsize= PkZipMakeUInteger(buffer +UNCOMPRESSED_SIZE);
	filename_length= PkZipMakeUShort(buffer +FILENAME_LENGTH);
	extra_field_length= PkZipMakeUShort(buffer +EXTRA_FIELD_LENGTH);

	char *tmpName= new char[filename_length];

	aStream->read(tmpName, filename_length);
	aStream->seekg((std::istream::off_type)extra_field_length, std::ios_base::cur);

	int tmpPos= aStream->tellg();

	delete[] tmpName;
	return true;
    }

    return false;
}


/**************************************************
* Implementation: DirectoryEntry.
**************************************************/

DirectoryEntry::DirectoryEntry(unsigned char *aBuffer)
{
    aBuffer+= 4;
    version_made_by[0]= aBuffer[VERSION_MADE_BY_0];
    version_made_by[1]= aBuffer[VERSION_MADE_BY_1];
    version_needed_to_extract[0]= aBuffer[VERSION_NEEDED_TO_EXTRACT_0];
    version_needed_to_extract[1]= aBuffer[VERSION_NEEDED_TO_EXTRACT_1];
    general_purpose_bit_flag= PkZipMakeUShort(aBuffer + GENERAL_PURPOSE_BIT_FLAG);
    compression_method= PkZipMakeUShort(aBuffer + COMPRESSION_METHOD);
    last_mod_file_time= PkZipMakeUShort(aBuffer + LAST_MOD_FILE_TIME);
    last_mod_file_date= PkZipMakeUShort(aBuffer + LAST_MOD_FILE_DATE);
    crc32= PkZipMakeUInteger(aBuffer + CRC32);
    csize= PkZipMakeUInteger(aBuffer + COMPRESSED_SIZE);
    ucsize= PkZipMakeUInteger(aBuffer + UNCOMPRESSED_SIZE);
    filename_length= PkZipMakeUShort(aBuffer + FILENAME_LENGTH);
    extra_field_length= PkZipMakeUShort(aBuffer + EXTRA_FIELD_LENGTH);
    file_comment_length= PkZipMakeUShort(aBuffer + FILE_COMMENT_LENGTH);
    disk_number_start= PkZipMakeUShort(aBuffer + DISK_NUMBER_START);
    internal_file_attributes= PkZipMakeUShort(aBuffer + INTERNAL_FILE_ATTRIBUTES);
    external_file_attributes= PkZipMakeUInteger(aBuffer + EXTERNAL_FILE_ATTRIBUTES);
    relative_offset_local_header= PkZipMakeUInteger(aBuffer + RELATIVE_OFFSET_LOCAL_HEADER);
}


bool DirectoryEntry::readFileName(std::istream *aStream)
{
    fileName= new char[filename_length+1];
    aStream->read(fileName, filename_length);
    fileName[filename_length]= '\0';
    return (!aStream->fail());
}


bool DirectoryEntry::isNamed(char *aName)
{
    return (strcmp(aName, fileName) == 0);
}


unsigned int DirectoryEntry::getDataStart(void)
{
    return relative_offset_local_header + LocalFileEntry::Size + 4 + filename_length;
}


unsigned int DirectoryEntry::getRelOffset(void)
{
    return relative_offset_local_header;
}


/**************************************************
* Implementation: CentralDirectoryTable.
**************************************************/

CentralDirectoryTable::CentralDirectoryTable(unsigned int aCount)
{
    nbrEntries= aCount;
    entries= new DirectoryEntry*[nbrEntries];
}


bool CentralDirectoryTable::read(std::istream *aStream, unsigned int dirSize)
{
     char buffer[DirectoryEntry::Size+4];

    for (unsigned int i= 0; i < nbrEntries; i++) {
	aStream->read(buffer, DirectoryEntry::Size+4);
	if (!aStream->fail()) {
	    entries[i]= new DirectoryEntry((unsigned char *)buffer);
	    if (!entries[i]->readFileName(aStream)) return false;
	}
	else {
	    return false;		// Warning: QUICK exit.
	}
    }
    return true;
}


DirectoryEntry *CentralDirectoryTable::getEntryCalled(char *aName)
{
    for (unsigned int i= 0; i< nbrEntries; i++) {
	if (entries[i]->isNamed(aName)) return entries[i];
    }

    return NULL;
}


/**************************************************
* Implementation: EndCentralHeader.
**************************************************/

EndCentralHeader::EndCentralHeader(void)
{
    number_this_disk= 0;
    num_disk_with_start_central_dir= 0;
    num_entries_centrl_dir_ths_disk= 0;
    total_entries_central_dir= 0;
    size_central_directory= 0;
    offset_start_central_directory= 0;
    zipfile_comment_length= 0;
}


bool EndCentralHeader::read(std::istream *aStream)
{
     char buffer[Size+4];

    aStream->read(buffer, Size+4);
    if (!aStream->fail()) {
	number_this_disk= PkZipMakeUShort(buffer+NUMBER_THIS_DISK);
	num_disk_with_start_central_dir= PkZipMakeUShort(buffer + NUM_DISK_WITH_START_CENTRAL_DIR);
	num_entries_centrl_dir_ths_disk= PkZipMakeUShort(buffer + NUM_ENTRIES_CENTRL_DIR_THS_DISK);
	total_entries_central_dir= PkZipMakeUShort(buffer + TOTAL_ENTRIES_CENTRAL_DIR);
	size_central_directory= PkZipMakeUInteger(buffer + SIZE_CENTRAL_DIRECTORY);
	offset_start_central_directory= PkZipMakeUInteger(buffer + OFFSET_START_CENTRAL_DIRECTORY);
	zipfile_comment_length= PkZipMakeUShort(buffer + ZIPFILE_COMMENT_LENGTH);
	return true;
    }

    return false;
}


unsigned int EndCentralHeader::getDirCount(void)
{
    return total_entries_central_dir;
}


unsigned int EndCentralHeader::getDirSize(void)
{
    return size_central_directory;
}


/**************************************************
* Implementation: ZipStream.
**************************************************/
ZipStream::ZipStream(char *aName)
#if defined(_WIN32)  || defined(MSDOS)
    : std::ifstream(aName, ios_base::in | ios_base::binary)
#else
    : std::ifstream(aName, ios_base::in)
#endif
{

}


/**************************************************
* Implementation: ZipArchive.
**************************************************/

ZipArchive::ZipArchive(void)
{
    archiveFilePath= NULL;
    endCentralDir= NULL;
    centralDir= NULL;
}


bool ZipArchive::loadArchiveDirectory(char *aName)
{
    std::ifstream *archiveFile;
    unsigned int size;
    bool result= false;

    archiveFilePath= aName;
#if defined(_WIN32) || defined(MSDOS)
    archiveFile= new std::ifstream(archiveFilePath, std::ios_base::in | std::ios_base::binary);
#else
    archiveFile= new std::ifstream(archiveFilePath, std::ios_base::in);
#endif

    if (archiveFile->good()) {
	archiveFile->seekg((std::istream::off_type)0, std::ios_base::end);
	size= archiveFile->tellg();
	if (size >= (EndCentralHeader::Size + 4)) {
	  int tmpPos;
	  archiveFile->seekg((std::istream::off_type)(-(EndCentralHeader::Size + 4)), std::ios_base::cur);
	  tmpPos= archiveFile->tellg();
	    if (tmpPos == (int)(size - (EndCentralHeader::Size+4))) {
		endCentralDir= new EndCentralHeader();
		if (endCentralDir->read(archiveFile)) {
		    centralDir= new CentralDirectoryTable(endCentralDir->getDirCount());
		    archiveFile->seekg((std::istream::off_type)(-(endCentralDir->getDirSize()+(EndCentralHeader::Size + 4))), std::ios_base::cur);
		    int tmpPos= archiveFile->tellg();
		    if (centralDir->read(archiveFile, endCentralDir->getDirSize())) {
			result= true;
		    }
		}
	    }
	}
    }

    delete archiveFile;

    return result;
}



ZipStream *ZipArchive::getFile(char *aName)
{
    DirectoryEntry *dirEntry;
    ZipStream *result;

     if (centralDir != NULL) {
	if ((dirEntry= centralDir->getEntryCalled(aName)) != NULL) {
	    result= new ZipStream(archiveFilePath);
	    result->seekg((std::istream::off_type)dirEntry->getDataStart(), std::ios_base::beg);

/* Test:
	    unsigned int theoPos= dirEntry->getDataStart();
	    result->seekg(dirEntry->getRelOffset(), ios_base::beg);
	    LocalFileEntry *tmpEntry= new LocalFileEntry();
	    tmpEntry->read(result);
*/
	}
    }
    return result;
}


DirectoryEntry *ZipArchive::statFile(char *aName)
{
    if (centralDir != NULL) {
	return centralDir->getEntryCalled(aName);
    }
    return NULL;
}

