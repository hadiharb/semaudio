/***************************************************************************
                          bcport.h  -  description
                             -------------------
    begin                : Wed Sep 28 2005
    copyright            : (C) 2005 by 
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

#ifndef BCPORT_H
#define BCPORT_H

#define _FILE_OFFSET_BITS 64

#include "string.h"
#include <stdint.h>
#include <stdlib.h>

#define fmOpenRead 0
#define fmOpenWrite 1
#define fmOpenReadWrite 2

class AnsiString {
    friend AnsiString operator +(const char*, const AnsiString& rhs);
	public:
    // constructors
		AnsiString();
		AnsiString(const char* s);
		AnsiString(const char* s, int l);
		AnsiString(float v);
		AnsiString(int v);
		AnsiString(unsigned long v);
		AnsiString(unsigned int v);
		AnsiString(const AnsiString& s);

		// destructors
		~AnsiString ();

    // Assignments
    AnsiString& operator =(const AnsiString& rhs);
    AnsiString& operator +=(const AnsiString& rhs);

    // Comparisons
    bool operator ==(const AnsiString& rhs) const;
    bool operator !=(const AnsiString& rhs) const;
    bool operator <(const AnsiString& rhs) const;
    bool operator >(const AnsiString& rhs) const;
    bool operator <=(const AnsiString& rhs) const;
    bool operator >=(const AnsiString& rhs) const;

    char operator [](const int idx) const
    {
      if (idx>(int)strlen(Data)) return 0; else
        return Data[idx-1];
    }

    // Concatenation
    AnsiString operator +(const AnsiString& rhs) const;

    // C string operator
    char* c_str() const        { return Data;}

    // Read access to raw Data ptr.  Will be NULL for an empty string.
    const void* data() const   { return Data; }

    // Query attributes of string
    int  Length() const				{ return strlen(Data); }
    
    bool IsEmpty() const { return Data == NULL; }

    AnsiString&  Insert(const AnsiString& str, int index);
    AnsiString&  Delete(int index, int count);

    int Pos(const AnsiString& subStr) const;
    AnsiString   LowerCase() const;
    AnsiString   UpperCase() const;
    AnsiString   Trim() const;
    AnsiString   TrimLeft() const;
    AnsiString   TrimRight() const;
    AnsiString   SubString(int index, int count) const;
    bool IsDelimiter(const char* list, int index);
    

    int          ToInt() const;
    int          ToIntDef(int defaultValue) const;
    double       ToDouble() const;

  private:
		char* Data;
		int mem;
};

class TStringList {
	public:
		//constructors
		TStringList();

		//destructors
		~TStringList();

		//methods
  	int Add(const AnsiString S);
    void Clear();
    AnsiString Strings(int index) { if ((index>=0)&&(index<n)) return *(strs[index]); else return AnsiString(); }
		
		int Count;
	private:
		int n;
    int mem;
		AnsiString** strs;
};

long FileOpen(const AnsiString FileName, unsigned Mode);
long FileCreate(const AnsiString FileName, bool lck = false);
long FileRead(long Handle, void *Buffer, unsigned long Count);
long FileWrite(long Handle, const void *Buffer, unsigned long Count);
long long FileSeek(long Handle, long long Offset, int Origin);
//__int64 FileSeek(int Handle, const __int64 Offset, int Origin);
void FileClose(long Handle);
long FileDelete(AnsiString FileName);

char* strlwr(char* s);
char UpCase(char c);

AnsiString HEX(unsigned char c);
bool FileExists(AnsiString &fn);
bool isRegularFile(AnsiString &strFilename);


#endif //BCPORT_H



