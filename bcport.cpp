/***************************************************************************
                          bcport.cpp  -  description
                             -------------------
    begin                : Wed Sep 28 2005
    copyright            : (C) 2005 by Alex Paradinets
    email                : 
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "bcport.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

AnsiString::AnsiString()
{
	  Data = new char[256];
	  Data[0] = 0;
	  mem = 256;
}

AnsiString::AnsiString(const char* s)
{
		int l = strlen(s);
		mem = l+64;
		Data = new char[mem];
		strcpy(Data,s);
}

AnsiString::AnsiString(const char* s, int l)
{
		mem = l+64;
		Data = new char[mem];
		strncpy(Data,s,l);
		Data[l]=0;
}

AnsiString::AnsiString(int v)
{
	  Data = new char[64];
	  mem = 64;
	  sprintf(Data,"%i",v);
}

AnsiString::AnsiString(float v)
{
	  Data = new char[64];
	  mem = 64;
	  sprintf(Data,"%f",v);
}

AnsiString::AnsiString(unsigned long v)
{
	  Data = new char[64];
	  mem = 64;
	  sprintf(Data,"%u",v);
}

AnsiString::AnsiString(unsigned int v)
{
	  Data = new char[64];
	  mem = 64;
	  sprintf(Data,"%u",v);
}

AnsiString::AnsiString(const AnsiString& s)
{
		int l = s.Length();
		mem = l+64;
		Data = new char[mem];
		strcpy(Data, s.c_str());
}

AnsiString::~AnsiString()
{
   if ((mem>0)&&(Data)) delete Data;
}

AnsiString& AnsiString::operator =(const AnsiString& rhs)
{
   if ((mem>0)&&(Data)) delete Data;

   int l = rhs.Length();
   mem = l+64;
   Data = new char[mem];
   strcpy(Data,rhs.c_str());
   
   return (*this);
}

AnsiString& AnsiString::operator +=(const AnsiString& rhs)
{
   int l = rhs.Length();
   int ml = strlen(Data);
   
   if (l+ml+1 >= mem)
   {
      mem=(l+ml+64);
      Data = (char*)realloc(Data,mem);
   }
    
   strcat(Data,rhs.c_str());

   return (*this);
}

    // Comparisons
bool AnsiString::operator ==(const AnsiString& rhs) const
{
		return (strcmp(Data,rhs.c_str())==0)?true:false;
}

bool AnsiString::operator !=(const AnsiString& rhs) const
{
		return (strcmp(Data,rhs.c_str())!=0)?true:false;
}

bool AnsiString::operator <(const AnsiString& rhs) const
{
		return (strcmp(Data,rhs.c_str())<0)?true:false;
}

bool AnsiString::operator >(const AnsiString& rhs) const
{
		return (strcmp(Data,rhs.c_str())>0)?true:false;
}

bool AnsiString::operator <=(const AnsiString& rhs) const
{
		return (strcmp(Data,rhs.c_str())<=0)?true:false;
}

bool AnsiString::operator >=(const AnsiString& rhs) const
{
		return (strcmp(Data,rhs.c_str())>=0)?true:false;
}

AnsiString AnsiString::operator +(const AnsiString& rhs) const
{
   AnsiString n(Data);

   n+=rhs;

   return n;
}

AnsiString operator +(const char* s, const AnsiString& rhs)
{
   AnsiString n(s);

   n+=rhs;

   return n;
}

AnsiString&  AnsiString::Insert(const AnsiString& str, int index)
{
   index--;
   
   if (index<(int)strlen(Data)) {

     int l = str.Length();
     int ml = strlen(Data);

     if (l+ml+1 >= mem)
     {
        mem=(l+ml+64);
        Data = (char*)realloc(Data,mem);
     }

     char* D2 = str.c_str();
     int j = l;

     for (int i=ml-1; i>=index; i--)
     {
        Data[i+l]=Data[i];
        Data[i]=D2[j--];
     }
     Data[ml+l]=0;
   }
		 
   return (*this);
}

AnsiString&  AnsiString::Delete(int index, int count)
{
    index--;
    int l = strlen(Data);
    
    if (index<l)
    {
       if (index+count>l) count=l-index;
    }

    for (int i=index+count; i<=l; i++)
    {
       Data[i-count]=Data[i];
    }

    return (*this);
}

int AnsiString::Pos(const AnsiString& subStr) const
{
	  char* c = strstr(Data,subStr.c_str());
	  if (c)
			return (long)c-(long)Data+1; else
			return 0;
}

AnsiString   AnsiString::LowerCase() const
{
   AnsiString n(Data);

   char* c = n.c_str();

   int l = n.Length();
   
   for (int i=0; i<l; i++)
   {
    if ( ((*c) >= 'A')&&((*c) <= 'Z')) (*c)+=32;
    c++;
   }

   return n;

}

AnsiString   AnsiString::UpperCase() const
{
   AnsiString n(Data);

   char* c = n.c_str();

   int l = n.Length();

   for (int i=0; i<l; i++)
   {
    if ( ((*c) >= 'a')&&((*c) <= 'z')) (*c)-=32;
    c++;
   }

   return n;
}

AnsiString   AnsiString::SubString(int index, int count) const
{
    index--;
    int l = strlen(Data);

    if (index<l)
    {
       if (index+count>l) count=l-index;
    }

    char a = Data[index+count];
    Data[index+count]=0;

    AnsiString n(Data+index);

    Data[index+count]=a;
    
    return n;
}

bool AnsiString::IsDelimiter(const char* list, int index)
{
    index--;
    int l = strlen(Data);

    if (index>=l)
      return false;

    char a = Data[index];

    for (int i=0; i<strlen(list); i++)
      if (a==list[i]) return true;

    return false;
}


int AnsiString::ToInt() const
{
   int r;

   sscanf(Data,"%i",&r);

   return r;
}

int AnsiString::ToIntDef(int defaultValue) const
{
   int r;

   int l = strlen(Data);

   if (l==0) return defaultValue;
   char* ptr;
   r = strtol(Data,&ptr,10);
   if (ptr==Data) r = defaultValue;

   return r;
}

double AnsiString::ToDouble() const
{
   float r;

   sscanf(Data,"%f",&r);

   return r;
}

// ======================

TStringList::TStringList()
{
   mem = 64;
   strs = new AnsiString*[mem];
   Count = n = 0;
}

TStringList::~TStringList()
{
   Clear();

   delete [] strs;
}

int TStringList::Add(const AnsiString S)
{
   if (n>=mem)
   {
      mem+=128;
      strs = (AnsiString**) realloc(strs, mem*sizeof(AnsiString*));
   }
   strs[n++]=new AnsiString(S);

   Count=n;
   return n-1;
}

void TStringList::Clear()
{
   for (int i=0; i<n; i++)
      delete strs[i];

   Count = n = 0;
}

long FileOpen(const AnsiString FileName, unsigned Mode)
{
   char* md = "rb";
   if ((Mode & 3) == 1) md="wb";
   if ((Mode & 3) == 2) md="rb+";

   FILE* f = fopen(FileName.c_str(),md);
   if (Mode==0x10 && f)
   {
       struct flock lock;
       lock.l_type   = F_RDLCK;
       lock.l_whence = SEEK_SET;
       lock.l_start  = 0;
       lock.l_len    = 0;
       fcntl(fileno(f), F_SETLKW, &lock);
   } else 
       if ((Mode==0x11 || Mode==0x12) && f)
       {
           struct flock lock;
           lock.l_type   = F_WRLCK;
           lock.l_whence = SEEK_SET;
           lock.l_start  = 0;
           lock.l_len    = 0;
           fcntl(fileno(f), F_SETLKW, &lock);
       }

   if (!f) return -1;
   return (long)f;
  
}

long FileCreate(const AnsiString FileName, bool lck)
{
   FILE* f = fopen(FileName.c_str(),"wb+");

   if (!f) return -1;
   if (lck)
   {
       struct flock lock;
       lock.l_type   = F_WRLCK;
       lock.l_whence = SEEK_SET;
       lock.l_start  = 0;
       lock.l_len    = 0;
       fcntl(fileno(f), F_SETLKW, &lock);
   }
   return (long)f;
}

long FileRead(long Handle, void *Buffer, unsigned long Count)
{
   return fread(Buffer,1,Count,(FILE*)Handle);
}
long FileWrite(long Handle, const void *Buffer, unsigned long Count)
{
   return fwrite(Buffer,1,Count,(FILE*)Handle);
}

long long FileSeek(long Handle, long long Offset, int Origin)
{
   fseeko((FILE*)Handle, Offset, Origin);
	 return ftello((FILE*)Handle);
}

void FileClose(long Handle)
{
   fclose((FILE*)Handle);
}

long FileDelete(AnsiString FileName)
{
   return remove(FileName.c_str());
}

char* strlwr(char* s)
{
   for (int i=0; i<(int)strlen(s); i++)
   {
    if ( (s[i] >= 'A')&&(s[i] <= 'Z')) s[i]+=32;
   }

   return s;
}

char UpCase(char c)
{
    if ( (c >= 'a')&&(c <= 'z')) c-=32;

    return c;
}

AnsiString HEX(unsigned char c)
{
   char r[3];
   char* h="0123456789ABCDEF";
   r[0]=h[c >> 4];
   r[1]=h[c & 15];
   r[2]=0;
   return AnsiString(r);
}

bool FileExists(AnsiString &strFilename) 
{
  struct stat stFileInfo;
  bool blnReturn;
  int intStat;

  intStat = stat(strFilename.c_str(),&stFileInfo);
  if(intStat == 0) {
    blnReturn = true;
  } else {
    blnReturn = false;
  }
  
  return(blnReturn);
}

bool isRegularFile(AnsiString &strFilename) 
{
  struct stat stFileInfo;
  bool blnReturn = false;
  int intStat;

  intStat = stat(strFilename.c_str(),&stFileInfo);
  if (intStat == 0) {
    if (S_ISREG(stFileInfo.st_mode)) blnReturn = true;
  }
  return(blnReturn);
}

