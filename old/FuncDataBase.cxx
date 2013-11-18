/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/FuncDataBase.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <functional>
#include "support.h"
#include "Exception.h"
#include "Code.h"
#include "FItem.h"
#include "funcList.h"
#include "varList.h"
#include "FuncDataBase.h"

FuncDataBase::FuncDataBase()
  /*!
    Standard Constructor
  */
{}

FuncDataBase::FuncDataBase(const FuncDataBase& A) :
  Build(A.Build)
  /*!
    Standard Copy Constructor
    \param A :: FuncDataBase to copy
  */
{}

FuncDataBase&
FuncDataBase::operator=(const FuncDataBase& A)
  /*!
    Standard copy constructor
    \param A :: FuncDataBase Object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Build=A.Build;
    }
  return *this;
}

FuncDataBase::~FuncDataBase()
  /*!
    Standard destructor
  */
{}

const FItem*
FuncDataBase::findItem(const std::string& Key) const
  /*!
    Finds a variable item
    \param Key :: string to search
    \return FItem pointer (or 0 on failure to find)
  */
{
  const varList* VList=varList::Instance();  
  return VList->findVar(Key);
}

double 
FuncDataBase::EvalVar(const std::string& Key) const
  /*!
    Finds the value of a variable item 
    \param Key :: string to search
    \return Value of variable 
    \throw InContainterError if no variable exists
  */
{
  const FItem* FI=findItem(Key);
  if (!FI)
    throw ColErr::InContainerError<std::string>(
      Key,"FuncDataBase::EvalVar no variable found");
  return FI->getValue();
}

double
FuncDataBase::Eval()
  /*!
    Evaluate the function (after pasrsing)
    \return F(var1,var2...)
  */
{
  return Build.Eval(varList::Instance());
}

int 
FuncDataBase::Parse(const std::string& Function)
  /*!
    Given a function and some variables do the parse
    \param Function is the string to parse 
    \retval -ve if error
    \retval 0 on success
  */
    
{
  // Remove Spaces
  std::string FunStrip=Function;
  FunStrip.erase(remove_if(FunStrip.begin(),FunStrip.end(),isspace),
		 FunStrip.end());

  
  // Check 
  std::pair<int,int> Result = CheckSyntax(FunStrip);
  if(Result.first) 
    {
      std::cerr<<"Error in ("<<Result.second<<") :"
	       <<FunStrip.substr(0,Result.second-1)<<std::endl;
      std::cerr<<"           ";
      std::cerr<<FunStrip.substr(Result.second-1)<<std::endl;
      
      return Result.first;
    }
  // First make Code invalid
  // Process
  if(!Compile(FunStrip)) 
    return -1;
    
  return 0;
}


std::string
FuncDataBase::getItem(const std::string& Astr) const
  /*!
    Get working object
    \param Astr :: String to cut
    \return Sliced off string
  */   
{
  std::string::size_type pos;
  pos=Astr.find_first_of("+-*/^(),");
  if (pos==std::string::npos)
    return Astr;
  if (pos!=0 || (Astr[pos]!='+' && Astr[pos]!='-') ||
      tolower(Astr[pos-1])!='e')
    return Astr.substr(0,pos);

  std::string::size_type px;
  for(px=0;px!=pos-1 && 
	(isdigit(Astr[px]) || Astr[px]=='.');px++);
  if (px!=pos-1)
    return Astr.substr(0,pos);
  
  for(px=pos+1;px<Astr.length() && isdigit(Astr[px]);px++);
  return Astr.substr(0,px-1);
}


//-----------------------------
// Check function string syntax
// ----------------------------

std::pair<int,int>
FuncDataBase::CheckSyntax(const std::string& FString) const
  /*!
    Function checks the syntax of the string and carries 
    out initial parsing.
    \param FString :: Full function to test. 
    \retval 0,index :: success
    \retval -ve ErrorNumber ,index
  */ 
{
  const funcList& FList(*funcList::Instance());

  std::vector<int> funcPDepth; 
  int Ind(0);  // Current index
  int parenthCnt(0);    // Parenthesis

  char cc=FString[FString.length()-1];
  if (!isalnum(cc) && cc!=')')
    return std::pair<int,int>(-8,Ind);

  int foundFunc(0),foundVar(0);
  int foundOp(0);
  double num;
  while(Ind<static_cast<int>(FString.length())) 
    {
      char c=FString[Ind];
      // Check for leading -
      if(c=='-') 
        {
	  Ind++;
	  if(Ind==static_cast<int>(FString.length())) 
	    return std::pair<int,int>(-2,Ind);
	  c=FString[Ind]; 
	}
      std::string Fpart=getItem(FString.substr(Ind));
      if (!Fpart.empty())
        {
	  foundOp=0;  // reset operator

	  Ind+=Fpart.length();
	  c=FString[Ind];

	  // Check for math function:: we at start of math-func
 	  const FuncDefinition& BComp=FList.findFunction(Fpart);
	  foundFunc=BComp.opCode;
	  const FItem* Var=(!foundFunc) ? findItem(Fpart) : 0;
	  foundVar= (Var) ? 1 : 0;
	  
	  if (foundFunc)
	    {
	      if (c!='(')
		return std::pair<int,int>(-3,Ind);
	      parenthCnt++;
	      funcPDepth.push_back(parenthCnt);
	      Ind++;
	      if (FString[Ind]==')')
		return std::pair<int,int>(-4,Ind);
	    }
	  else if (foundVar)
	    {
	      if (c=='(')
		return std::pair<int,int>(-5,Ind);
	    }
	  else if (!StrFunc::convert(Fpart,num))          
	    {
	      return std::pair<int,int>(-6,Ind);
	    }
	}
      else 
        {
	  if (c=='(')
	    {
	      foundOp=0;
	      parenthCnt++;
	    }
	  
	  while(c==')')
             {
	       if(!funcPDepth.empty() &&
		  funcPDepth.back() == parenthCnt)
		 funcPDepth.pop_back();

	       if((--parenthCnt)<0 || foundOp)  
		 return std::pair<int,int>(-7,Ind);
	       Ind++;
	       c=FString[Ind];
	     }
	  Ind++;
	}
    }
  return std::pair<int,int>(0,0);
}


int
FuncDataBase::Compile(const std::string& FString)
  /*!
    Compile function string to bytecode.
    \param FString :: previously checked function string
    \returns 1 on success (0 on failure)
  */
{
  Build.clear();
  try
    {
      compileExpression(FString, 0);
    }
  catch (const ColErr::ExBase& Error)
    {
      std::cerr<<Error.getErrorStr()<<std::endl;
      return 0;
    }
  // Do the work here (a lot!)
  return 1;
}

int
FuncDataBase::compileExpression(const std::string& FString,
				  const int Ind)
  /*!
    Process expression components
    \param FString :: Full Expression string 
    \param Ind :: number of first part of string
    \returns error number
  */
{
  int Ind2 = compileAdd(FString,Ind);
  while(FString[Ind2]==',')
    Ind2 = compileAdd(FString, Ind2+1);
  return Ind2;
}

int 
FuncDataBase::compileAdd(const std::string& FString,const int Ind)
  /*!
    Compiles both + and - 
    Down cast to mult first
    \param FString :: Full string
    \param Ind :: index of string
    \returns  new index point
  */
{
  int Ind2 = compileMult(FString, Ind);
  
  char op;
  while(FString[Ind2] == '+' || FString[Ind2]=='-')
    {
      op=FString[Ind2];
      Ind2 = compileMult(FString, Ind2+1);
      Build.addByte(op=='+' ? Opcodes::cAdd : Opcodes::cSub );
    }

  return Ind2;
}

int 
FuncDataBase::compileMult(const std::string& FString,const int Ind)
  /*!
    Compiles '*', '/' and '%'    
    Down calls to unary minus (for -Value etc.)
    \param FString :: Full string
    \param Ind :: index of string
    \returns  new index point
  */
{
  int Ind2 = compileUnaryMinus(FString, Ind);   /// Should this be here ????
  char op=FString[Ind2];

  while(op == '*' || op == '/' || op == '%')
    {
      Ind2 = compileUnaryMinus(FString, Ind2+1);
      switch(op)
        {
	case '*': 
	  Build.addByte(Opcodes::cMul);
	  break;
	case '/': 
	  Build.addByte(Opcodes::cDiv);
	  break;
	case '%': 
	  Build.addByte(Opcodes::cMod);
	  break;
        }
      op=FString[Ind2];
    }
  return Ind2;
}

int 
FuncDataBase::compileUnaryMinus(const std::string& FString,const int Ind)
  /*!
    Function to compile Uniary Minus :: 
    This defers to power
    \param FString :: Full string
    \param Ind :: index of string
    \returns  new index point
  */
{
  if(FString[Ind] == '-')
    {
      int Ind2 = Ind+1;
      Ind2 = compilePow(FString, Ind2);
      
      std::vector<int>& BCref=Build.getBC();
      // if we are negating a constant, negate the constant itself:
      if(BCref.back() == Opcodes::cImmed)
        {
	  Build.minusImmed();
	}
      // if we are negating a negation, we can remove both:
      else if(BCref.back() == Opcodes::cNeg)
        {
	  BCref.pop_back();
	}
      else
        {
	  Build.addByte(Opcodes::cNeg);
	}
      
      return Ind2;
    }
  
  return compilePow(FString, Ind);
}


int 
FuncDataBase::compilePow(const std::string& FString,const int Ind)
  /*!
    Function to compile '^' as power function
    This function calls the final Element compile
    which deals with var/func.
    \param FString :: Full string
    \param Ind :: index of string
    \returns  new index point
  */
{
  int Ind2 = compileElement(FString, Ind);  

  while(FString[Ind2] == '^')
    {
      Ind2 = compileUnaryMinus(FString, Ind2+1);
      Build.addByte(Opcodes::cPow);
    }
  return Ind2;
}

int
FuncDataBase::compileFunctionParams(
  const std::string& FString,const int Ind,
  const int reqParam)
  /*!
    Compile a function name and paramters
    \param FString :: string to process
    \param Ind :: Point int string to start
    \param reqParam :: number of parameter for the function
    \return Place in the string that has been processed
   */
{
  const int curStackPtr = Build.getStackPtr();
  int Ind2 = compileExpression(FString, Ind); 

  if(Build.getStackPtr() != curStackPtr+reqParam)
    throw ColErr::ExBase(Ind,"Error with N_Params");
  
  Build.addStackPtr( -(reqParam-1) );
  return Ind2+1; // F[ind2] is ')'
}

int 
FuncDataBase::compileElement(const std::string& FString,const int Ind)
  /*!
     Near Top level function :: compiles everything 
     except brackets ( ).
     ie Functions, variables and simple numbers etc. 

     The template form of this function is taken from 
     CheckSyntax.
     \param FString :: string to process
     \param Ind :: index point
     \returns Index point (place that processing finished)
  */
{
  char c = FString[Ind];
  if(c == '(')
    {
      // process Bracket bit
      const int Ind2 = compileExpression(FString, Ind+1); 
      return Ind2+1;                         // F[ind] is ')'
    }

  const funcList& FList(*funcList::Instance());
  double num;
  int numflag=StrFunc::convPartNum(FString.substr(Ind),num);
  if(numflag)
    {
      Build.addImmediate(num);
      Build.addByte(Opcodes::cImmed);
      Build.addStackPtr(2);
      return Ind+numflag;
    }

  std::string FPart=getItem(FString.substr(Ind));
  if(!FPart.empty())     // Function, variable or constant
    {
      const FuncDefinition& BComp=FList.findFunction(FPart);
      const FItem* Var=(BComp.opCode) ? 0 : findItem(FPart);
      if(BComp.opCode)     // is function
         {
	   int Ind2 = Ind + BComp.nameLength;
	   Ind2=compileFunctionParams(FString,Ind2+1,BComp.Np);
	   Build.addFunctionOpcode(BComp.opCode);
	   return Ind2;     // F[ind2-1] is ')'
	 }
      if (Var)
        {
	  Build.addByte(Opcodes::varBegin+Var->getIndex()); 
	  //	  Build.incStackPtr(2);
	  Build.addStackPtr(2);
	  return Ind + FPart.length();
        }
      
      /// NO USER FUNCTIONS::
    }
  
  throw ColErr::ExBase(Ind,"Compile UNEXPECTED_ERROR");
  return -Ind;
}

void
FuncDataBase::addVariable(const std::string& Name,const double V)  const
  /*!
    Accessor function to add a variable to the list
    \param Name :: Name of the variable
    \param V :: current value
  */
{
  varList* VX=varList::Instance();
  VX->addVar(Name,V);
  return;
}

void
FuncDataBase::addVariable(const std::string& Name) const
  /*!
    Adds this function if the Code system has been 
    executed
    \param Name :: Name of the variable
  */
{
  varList* VX=varList::Instance();
  VX->addVar(Name,Build);
  return;
}
