/*  Project STARS
    John DiCamillo Software Consulting
    Copyright � 1997-2000. All Rights Reserved.

    SUBSYSTEM:    Stars
    FILE:         term.h
    AUTHOR:       John DiCamillo


    OVERVIEW
    ========
    Declaration of the Abstract Syntax Tree classes
*/


#ifndef TERM_H
#define TERM_H

#include "Text.h"
#include "List.h"

// +-------------------------------------------------------------------+

class Term;
class TermBool;
class TermNumber;
class TermText;
class TermArray;
class TermDef;
class TermStruct;

// +-------------------------------------------------------------------+

class Term
{
public:
   static const char* TYPENAME() { return "Term"; }

	Term()            { }
   virtual ~Term()   { }

   virtual int operator==(const Term& rhs) const { return 0; }

	virtual void print(int level=10) { }

	// conversion tests
	virtual Term*              touch()     { return this; }
	virtual TermBool*          isBool()    { return 0; }
	virtual TermNumber*        isNumber()  { return 0; }
	virtual TermText*          isText()    { return 0; }
	virtual TermArray*         isArray()   { return 0; }
	virtual TermDef*           isDef()     { return 0; }
	virtual TermStruct*        isStruct()  { return 0; }
};

Term* error(char*, char* = 0);

// +-------------------------------------------------------------------+

typedef List<Term>      TermList;
typedef ListIter<Term>  TermListIter;

// +-------------------------------------------------------------------+

class TermBool : public Term
{
public:
   static const char* TYPENAME() { return "TermBool"; }

   TermBool(bool v) : val(v) { }
   
   virtual void      print(int level=10);
	virtual TermBool* isBool()      { return this; }
           bool      value() const { return val;  }

private:
   bool val;
};

// +-------------------------------------------------------------------+

class TermNumber : public Term
{
public:
   static const char* TYPENAME() { return "TermNumber"; }

   TermNumber(double v) : val(v) { }
   
   virtual void         print(int level=10);
	virtual TermNumber*  isNumber()     { return this; }
           double       value() const  { return val;  }

private:
   double val;
};

// +-------------------------------------------------------------------+

class TermText : public Term
{
public:
   static const char* TYPENAME() { return "TermText"; }

   TermText(const Text& v) : val(v)   { }
   
   virtual void      print(int level=10);
	virtual TermText* isText()       { return this; }
           Text      value() const  { return val;  }

private:
   Text val;
};

// +-------------------------------------------------------------------+

class TermArray : public Term
{
public:
   static const char* TYPENAME() { return "TermArray"; }

   TermArray(TermList* elist);
   virtual ~TermArray();
   
   virtual void         print(int level=10);
   virtual TermArray*   isArray()   { return this;  }
           TermList*    elements()  { return elems; }

private:
   TermList*   elems;
};

// +-------------------------------------------------------------------+

class TermStruct : public Term
{
public:
   static const char* TYPENAME() { return "TermStruct"; }

   TermStruct(TermList* elist);
   virtual ~TermStruct();

   virtual void         print(int level=10);

   virtual TermStruct*  isStruct()  { return this; }
           TermList*    elements()  { return elems; }

private:
   TermList*   elems;
};

// +-------------------------------------------------------------------+

class TermDef : public Term
{
public:
   static const char* TYPENAME() { return "TermDef"; }

   TermDef(TermText* n, Term* v) : mname(n), mval(v) { }
   virtual ~TermDef();

   virtual void         print(int level=10);
   virtual TermDef*     isDef()     { return this; }
   
   virtual TermText*    name()      { return mname; }
   virtual Term*        term()      { return mval;  }

private:
   TermText*   mname;
   Term*       mval;
};

#endif
