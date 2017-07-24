/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   funcBase/FuncDataBase.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include <iterator>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "support.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "XMLwriteVisitor.h"
#include "XMLload.h"
#include "XMLattribute.h"
#include "XMLobject.h"
#include "XMLgroup.h"
#include "XMLcollect.h"
#include "Code.h"
#include "FItem.h"
#include "funcList.h"
#include "varList.h"
#include "MD5hash.h"
#include "FuncDataBase.h"


FuncDataBase::FuncDataBase()
  /*!
    Standard Constructor
  */
{}

FuncDataBase::FuncDataBase(const FuncDataBase& A) :
  VList(A.VList),Build(A.Build)
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
      VList=A.VList;
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
   return VList.findVar(Key);
}

int
FuncDataBase::hasVariable(const std::string& Key) const
  /*!
    Determine if we have a variable
    \param Key :: variable name to test
    \return true if variable exists
  */
{
  const FItem* FI=findItem(Key);
  return (FI) ? 1 : 0;
}

template<typename T>
T
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
    throw ColErr::InContainerError<std::string>
      (Key,"FuncDataBase::EvalVar variable not found");

  T Out;
  FI->getValue(Out);
  return Out;
}

template<typename T>
T
FuncDataBase::EvalDefVar(const std::string& Key,const T& def) const
  /*!
    Finds the value of a variable item 
    \param Key :: string to search
    \param def :: default value
    \return Value of variable / def value
  */
{
  const FItem* FI=findItem(Key);
  if (!FI)
    return def;
  T Out;
  FI->getValue(Out);
  return Out;
}

template<typename T>
T
FuncDataBase::EvalTriple(const std::string& KeyA,
			 const std::string& KeyB,
			 const std::string& KeyC) const
  /*!
    Finds the value of a variable item 
    \param KeyA :: First string to search    
    \param KeyB :: Second string to search 
    \param KeyC :: Third string to search 

    \return Value of variable 
    \throw InContainterError if no variables exists
  */
{
  const FItem* FI=findItem(KeyA);
  if (!FI)
    FI=findItem(KeyB);
  if (!FI)
    FI=findItem(KeyC);
  if (!FI)
    throw ColErr::InContainerError<std::string>
      (KeyC+":"+KeyB+":"+KeyA,"FuncDataBase::EvalTriple no variables found");
  
  T Out;
  FI->getValue(Out);
  return Out;
}

template<typename T>
T
FuncDataBase::EvalTriple(const std::string& KeyA,
			 const std::string& KeyB,
			 const std::string& KeyC,
			 const std::string& Tail) const
  /*!
    Finds the value of a variable item 
    \param KeyA :: First string to search    
    \param KeyB :: Second string to search 
    \param KeyC :: Third string to search 
    \param Tail :: Tail string for keys
    \return Value of variable 
    \throw InContainterError if no variables exists
  */
{
  return EvalTriple<T>(KeyA+Tail,KeyB+Tail,KeyC+Tail);
}

template<typename T>
T
FuncDataBase::EvalPair(const std::string& KeyA,
		       const std::string& KeyB) const
  /*!
    Finds the value of a variable item 
    \param KeyA :: First string to search    
    \param KeyB :: Second string to search 
    \return Value of variable 
    \throw InContainterError if no variables exists
  */
{
  const FItem* FI=findItem(KeyA);
  if (!FI)
    FI=findItem(KeyB);
  if (!FI)
    throw ColErr::InContainerError<std::string>
      (KeyB+":"+KeyA,"FuncDataBase::EvalPair no variables found");
  
  T Out;
  FI->getValue(Out);
  return Out;
}

template<typename T>
T
FuncDataBase::EvalPair(const std::string& KeyA,
		       const std::string& KeyB,
		       const std::string& Tail) const
  /*!
    Finds the value of a variable item 
    \param KeyA :: First string to search    
    \param KeyB :: Second string to search 
    \param Tail :: Addition to both string [common]
    \return Value of variable 
    \throw InContainterError if no variables exists
  */
{
  return EvalPair<T>(KeyA+Tail,KeyB+Tail);
}

template<typename T>
T
FuncDataBase::EvalDefPair(const std::string& KeyA,
			  const std::string& KeyB,
			  const T& defVal) const
  /*!
    Finds the value of a variable item 
    \param KeyA :: First string to search    
    \param KeyB :: Second string to search 
    \param defVal :: Default value
    \return Value of variable 
  */
{
  const FItem* FI=findItem(KeyA);
  if (!FI)
    FI=findItem(KeyB);
  if (!FI)
    return defVal;
  
  T Out;
  FI->getValue(Out);
  return Out;
}

template<typename T>
T
FuncDataBase::EvalDefPair(const std::string& KeyA,
		       const std::string& KeyB,
		       const std::string& Tail,
		       const T& defVal) const
  /*!
    Finds the value of a variable item 
    \param KeyA :: First string to search    
    \param KeyB :: Second string to search 
    \param Tail :: Addition to both string [common]
    \param defVal :: default value
    \return Value of variable 
  */
{
  return EvalDefPair<T>(KeyA+Tail,KeyB+Tail,defVal);
}

template<typename T>
T
FuncDataBase::Eval()
  /*!
    Evaluate the function (after pasrsing)
    \return F(var1,var2...)
  */
{
  return Build.Eval<T>(&VList);
}

std::string
FuncDataBase::subProcVar(std::string& Var) const
  /*!
    Process a single variable
    \param Var :: String to determine if a variable exists
    \return String of variables
   */
{
  std::string Out(
    (!Var.empty() && hasVariable(Var)) ? 
       EvalVar<std::string>(Var) : Var);
  Var="";
  return Out;
}

int 
FuncDataBase::Parse(const std::string& Function)
  /*!
    Given a function and some variables do the parse
    \param Function :: the string to parse 
    \retval -1 : failed on syntax
    \retval -2 : Faild on compile
    \retval 0 on success
  */
    
{
  ELog::RegMethod RegA("FuncDataBase","Parse");
  // Remove Spaces
  std::string FunStrip=Function;
  FunStrip.erase(remove_if(FunStrip.begin(),FunStrip.end(),isspace),
		 FunStrip.end());
  
  // Check 
  const std::pair<std::string,size_t> Result = CheckSyntax(FunStrip);
  if(Result.second) 
    {
      ELog::EM<<"FuncDataBase::Parse Error ("<<Result.second<<") :"
	      <<FunStrip.substr(0,Result.second-1)<<ELog::endCrit;
      ELog::EM<<"           "
	      <<FunStrip.substr(Result.second-1)<<ELog::endCrit;
      ELog::EM<<"    Error Type: "<<Result.first<<ELog::endErr;
      return -1;
    }
  // First make Code invalid
  // Process
  if(!Compile(FunStrip)) 
    return -2;
    
  return 0;
}

int
FuncDataBase::stripEqual(std::string& ComLine)
  /*!
    Make x=y into a function 
    also adds a variable x if required.
    ComLine needs to have been string stripped
    \param ComLine :: Line to process
    \return 0 
    \todo to be completed
   */
{
  std::string::size_type pos=ComLine.find("=");
  while(pos!=std::string::npos)
    {
      pos=ComLine.find("=",pos);
    }
  return 0;
}

std::string
FuncDataBase::getItem(const std::string& Astr) const
  /*!
    Get working object. The object is to split 
    into valid component:
    \param Astr :: String to cut
    \return Sliced off string
  */   
{
  std::string::size_type pos;
  // All components split on 
  pos=Astr.find_first_of("+-*/^(),=");
  if (pos==std::string::npos)
    return Astr;
  // Split on any symbolic component avoiding 3.4e+4 etc
  if (pos!=0 && (Astr[pos]!='+' && Astr[pos]!='-') &&
      tolower(Astr[pos-1])!='e')
    return Astr.substr(0,pos);

  std::string::size_type px;
  for(px=0;px!=pos-1 && 
	(isdigit(Astr[px]) || Astr[px]=='.');px++) ;
  if (px!=pos-1)
    return Astr.substr(0,pos);
  
  for(px=pos+1;px<Astr.length() && isdigit(Astr[px]);px++) ;
  return Astr.substr(0,px-1);
}


//-----------------------------
// Check function string syntax
// ----------------------------

std::pair<std::string,size_t>
FuncDataBase::CheckSyntax(const std::string& FString) const
  /*! 
    Function checks the syntax of the string and carries 
    out initial parsing.

    \param FString :: Full function to test. 
    \retval 0,index :: success
    \retval -ve ErrorNumber ,index
  */ 
{
  typedef std::pair<std::string,size_t> retTYPE; 

  const funcList& FList(*funcList::Instance());

  std::vector<int> funcPDepth; 
  std::vector<int> cmdPDepth; 
  size_t Ind(0);  // Current index
  int parenthCnt(0);    // Parenthesis

  // Check that the last ) exists  
  const char cc=FString[FString.length()-1];
  if (!isalnum(cc) && cc!=')' && cc!=']')
    return retTYPE("Symbol ends expression:",Ind+1);

  int foundFunc(0),foundVar(0);
  int foundOp(0);

  while(Ind<FString.length()) 
    {
      char cIndex=FString[Ind];
      // Check for leading -
      if(cIndex=='-') 
        {
	  Ind++;
	  cIndex=FString[Ind]; 
	}
      std::string Fpart=getItem(FString.substr(Ind));
      // REMOVE equal (=)
      std::string Comp[2];
      const std::string::size_type pos=Fpart.find("=");
      if (!Fpart.empty())
        {
	  Comp[0]=Fpart.substr(0,pos);
	  Comp[1]=(pos!=std::string::npos) ? 
	    Fpart.substr(pos+1,std::string::npos) : "";
	  
	  for(int i=0;i<2 && !Comp[i].empty();i++)
	    {
	      Fpart=Comp[i];
	      foundOp=0;  // reset operator
	      
	      // Will this work for part 2:
	      if (i) Ind=pos+1;
	      Ind+=Fpart.length();
	      cIndex=FString[Ind];

	      // Check for math function:: we at start of math-func
	      const FuncDefinition& BComp=FList.findFunction(Fpart);
	      foundFunc=BComp.opCode;
	      // If NOT a function try a variable
	      const FItem* Var=(!foundFunc) ? 
		findItem(Fpart) : 0;

	      foundVar= (Var) ? 1 : 0;
	      if (foundFunc)
	        {
		  if (cIndex!='(')
		    return retTYPE("Bracket does not follow function",Ind+1);
		  parenthCnt++;
		  funcPDepth.push_back(parenthCnt);
		  Ind++;
		  if (FString[Ind]==')' && BComp.Np!=0)
		    return retTYPE("No terms in a () function",Ind);
		}
	      else if (foundVar)
	        {
		  if (cIndex=='(')
		    return retTYPE("Variable used as a function",Ind+1);
		}
	    }
	}
      else 
        {
	  if (cIndex=='(')
	    {
	      foundOp=0;
	      parenthCnt++;
	    }
	  
	  while(cIndex==')')
	    { 
	      if(!funcPDepth.empty() &&
		 funcPDepth.back() == parenthCnt)
		funcPDepth.pop_back();
	      
	      if((--parenthCnt)<0 || foundOp)  
		return retTYPE("Too many close brackets",Ind);
	      Ind++;
	      cIndex=FString[Ind];
	    }
	  Ind++;
	}
    }
  // TEST for funct Depths:
  return retTYPE("",0);
}

long int
FuncDataBase::Compile(const std::string& FString)
  /*!
    Compile function string to bytecode.
    \param FString :: previously checked function string
    \returns 1 on success (0 on failure)
  */
{
  ELog::RegMethod RegA("FuncDataBase","Compile");

  Build.clear();
  try
    {
      compileExpression(FString, 0);
    }
  catch (const ColErr::ExBase& Error)
    {
      ELog::EM<<"Compile error:"<<Error.what()<<ELog::endErr;
      return 0;
    }

  return 1;
}

size_t
FuncDataBase::compileExpression(const std::string& FString,
				const size_t Ind)
  /*!
    Process expression components : everything,
    needs to be in-reverse order of precedence.
    \param FString :: Full Expression string 
    \param Ind :: number of first part of string
    \returns error number
  */
{
  size_t Ind2 = compileAdd(FString,Ind);
  while(FString[Ind2]==',')
    Ind2 = compileAdd(FString, Ind2+1);
  return Ind2;
}

size_t
FuncDataBase::compileEqual(const std::string& FString,const size_t Ind)
  /*!
    Compiles = 
    Down cast to mult first
    \param FString :: Full string
    \param Ind :: index of string
    \returns  new index point
  */
{
  size_t Ind2 = compileAdd(FString, Ind);
  while(FString[Ind2] == '=')
    {
      
//      std::cout<<"----------- "<<std::endl;
//      Build.printByteCode(std::cout);
//      std::cout<<"----------- "<<std::endl;
      const int varNum=Build.popByte();
      Ind2 = compileMult(FString, Ind2+1);
      Build.addByte(Opcodes::cEqual);
      Build.addByte(varNum);
    }

  return Ind2;
}

size_t
FuncDataBase::compileAdd(const std::string& FString,const size_t Ind)
  /*!
    Compiles both + and - 
    Down cast to mult first
    \param FString :: Full string
    \param Ind :: index of string
    \returns  new index point
  */
{
  size_t Ind2 = compileMult(FString, Ind);
  
  char op;
  while(FString[Ind2] == '+' || FString[Ind2]=='-')
    {
      op=FString[Ind2];
      Ind2 = compileMult(FString, Ind2+1);
      Build.addByte(op=='+' ? Opcodes::cAdd : Opcodes::cSub );
    }

  return Ind2;
}

size_t
FuncDataBase::compileMult(const std::string& FString,const size_t Ind)
  /*!
    Compiles '*', '/' and '%'    
    Down calls to unary minus (for -Value etc.)
    \param FString :: Full string
    \param Ind :: index of string
    \returns  new index point
  */
{
  size_t Ind2 = compileUnaryMinus(FString, Ind);   /// Should this be here ????
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

size_t
FuncDataBase::compileUnaryMinus(const std::string& FString,
				const size_t Ind)
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
      size_t Ind2 = Ind+1;
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


size_t
FuncDataBase::compilePow(const std::string& FString,const size_t Ind)
  /*!
    Function to compile '^' as power function
    This function calls the final Element compile
    which deals with var/func.
    \param FString :: Full string
    \param Ind :: index of string
    \returns  new index point
  */
{
  size_t Ind2 = compileElement(FString, Ind);  

  while(FString[Ind2] == '^')
    {
      Ind2 = compileUnaryMinus(FString, Ind2+1);
      Build.addByte(Opcodes::cPow);
    }
  return Ind2;
}

size_t
FuncDataBase::compileFunctionParams(
  const std::string& FString,const size_t Ind,
  const size_t reqParam)
  /*!
    Compile a function name and paramters
    \param FString :: string to process
    \param Ind :: Point int string to start
    \param reqParam :: number of parameter for the function
    \return Place in the string that has been processed
   */
{
  const size_t curStackPtr = Build.getStackPtr();
  const size_t Ind2 = compileExpression(FString, Ind); 
  
  if(Build.getStackPtr() != curStackPtr+reqParam)
    throw ColErr::IndexError<size_t>(Build.getStackPtr(),reqParam,
				  "FuncDataBase::compileFunctionParams "
				  "Error with N_Params");
  
  if (reqParam)
    Build.subStackPtr(reqParam-1);

  return Ind2+1; // F[ind2] is ')'
}

size_t
FuncDataBase::compileElement(const std::string& FString,const size_t Ind)
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
  ELog::RegMethod RegA("FuncDataBase","compileElement");
  const char c = FString[Ind];
  if(c == '(')
    {
      // process Bracket bit
      const size_t Ind2=compileExpression(FString, Ind+1); 
      return Ind2+1;                         // F[ind] is ')'
    }

  const funcList& FList(*funcList::Instance());
  double num;
  Geometry::Vec3D numV;

  const size_t numflag=StrFunc::convPartNum(FString.substr(Ind),num);
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
	   size_t Ind2 = Ind + BComp.nameLength;
	   Ind2=compileFunctionParams(FString,Ind2+1,BComp.Np);
	   Build.addFunctionOpcode(BComp.opCode);
	   return Ind2;     // F[ind2-1] is ')'
	 }
      else if (Var)
        {
	  Build.addByte(Opcodes::varBegin+Var->getIndex()); 
	  //	  Build.incStackPtr(2);
	  Build.addStackPtr(2);
	  return Ind + FPart.length();
        }
      
      /// NO USER FUNCTIONS:: 
    }  
  throw ColErr::InvalidLine("FuncDataBase::compileElement: UNEXPECTED_ERROR:",
			    FString,Ind);
}

size_t
FuncDataBase::extractComponents(const std::string& Line,
				std::vector<std::string>& Comp) const
  /*!
    This function takes a line after a cmd work and process
    the command components out.
    \param Line :: Process string (xx,yy,zz)
    \param Comp :: vector string
    \return Index in line that ends the function 
  */
{
  ELog::RegMethod RegA("FuncDataBase","extractComponents");

  const size_t maxLine=Line.length();
  size_t secStart(0);
  size_t i(0);       // index
  int quote(0);   // in quote
  int escape(0);
  int bracket(0);
  while(i<maxLine)
    {
      // Process in a quote [highest precidence]:
      
      if (!escape && (Line[i]=='\'' || Line[i]=='\"'))
	quote=1-quote;
      // If in bracket ignore all others
      if (!quote)
	{
	  if (bracket)
	    {
	      if (!escape)
		{
		  if (Line[i]=='(')
		    bracket++;
		  else if (Line[i]==')')
		    bracket--;
		}
	    } 
	  else if (!escape && Line[i]==',')      // separate
	    {
	      Comp.push_back(Line.substr(secStart,i-secStart));
	      secStart=i+1;
	    }
	  else if (!escape && Line[i]=='(')      // start of new bracket
	    bracket++;
	  else if (!escape && Line[i]==')')       // last bracket
	    {
	      Comp.push_back(Line.substr(secStart,i-secStart));
	      return i;
	    }
	}
      if (Line[i]=='\\')
	escape=1-escape;
      i++;
    }
  throw ColErr::InvalidLine(Line,"failure at point",i);
}

template<typename T>
void
FuncDataBase::addVariable(const std::string& Name,const T& V)
  /*!
    Adds this function if the Code system has been 
    executed
    \param Name :: Name of the variable
    \param V :: Variable to add
  */
{
  VList.addVar<T>(Name,V);
  return;
}

void
FuncDataBase::addVariable(const std::string& Name,const char* V)
  /*!
    Adds this function if the Code system has been 
    executed
    \param Name :: Name of the variable
    \param V :: Variable to add
  */
{
  VList.addVar<std::string>(Name,std::string(V));
  return;
}

void
FuncDataBase::removeVariable(const std::string& Name)
  /*!
    Remove this function 
     - can cause problems re-evaluating other variables if dependeny
    \param Name :: Name of the variable
  */
{
  VList.removeVar(Name);
  return;
}

template<typename T>
void
FuncDataBase::addParse(const std::string& Name,const std::string& VParse)
  /*!
    Adds this function if the Code system has been 
    executed
    \param Name :: Name of the variable
    \param VParse :: expression to parse
  */
{
  Parse(VParse);
  VList.addVar<T>(Name,Eval<T>());
  return;
}


void
FuncDataBase::copyVar(const std::string& Name,const std::string& otherVar)
  /*!
    Adds this function if the Code system has been 
    executed
    \param Name :: Name of the variable
    \param otherVar :: Other variable
  */
{
  VList.copyVar(Name,otherVar);
  return;
}

void
FuncDataBase::copyVarSet(const std::string& oldHead,
                         const std::string& newHead)
/*!
    Adds this function if the Code system has been 
    executed
    \param oldHead :: Head name for variables to find
    \param newHead :: new head for variables
  */
{
  
  VList.copyVarSet(oldHead,newHead);
  return;
}

template<typename T>
void
FuncDataBase::setVariable(const std::string& Name,const T& V)
  /*!
    Set the varable to the value given
    \param Name :: Name of the variable
    \param V :: Variable to add
  */
{
  VList.setVar(Name,V);
  return;
}

void
FuncDataBase::addVariable(const std::string& Name) 
  /*!
    Adds this function if the Code system has been 
    executed
    \param Name :: Name of the variable
  */
{
  VList.addVar(Name,Build);
  return;
}

void
FuncDataBase::setVariable(const std::string& Name)
  /*!
    Sets this function if the Code system has been 
    executed
    \param Name :: Name of the variable
  */
{

  VList.setVar(Name,Build);
  return;
}

void
FuncDataBase::writeAll(const std::string& Fname) const
  /*!
    Write all the variables to a file
    \param Fname :: Filename 
  */
{
  std::ofstream OX(Fname.c_str());
  VList.writeAll(OX);
  OX.close();
  return;
}

std::string
FuncDataBase::variableHash() const
  /*!
    Calculates the hash value for the variables
    \return Hash string
  */
{
  std::ostringstream cx;
  VList.writeActive(cx);
  MD5hash sum;
  return sum.processMessage(cx.str());
}

void
FuncDataBase::processXML(const std::string& FName) 
  /*!
    Process an XML file to set/add variables
    \param FName :: filename 
  */
{
  ELog::RegMethod RegA("FuncDataBase","processXML");
  XML::XMLcollect CO;
  if (FName.empty() || CO.loadXML(FName))
    throw ColErr::FileError(0,FName,"XMLcollect file");
  // Parse for variables:
  XML::XMLobject* AR=CO.findObj("variable");
  XML::XMLgroup* AG;
  double V;
  Geometry::Vec3D VUnit;
  std::string VStr;
  int VInt;
  while(AR)
    {
      const std::string Name=AR->getItem<std::string>("name");
      const std::string Type=AR->getDefItem<std::string>("type","double");

      if (!hasVariable(Name))
	ELog::EM<<"Re-Adding variable "<<Name<<ELog::endWarn;
      
      // Only vector type 
      if (Type=="function")
	{
	  if ( (AG=dynamic_cast<XML::XMLgroup*>(AR)) )
	    VStr=AG->getItem<std::string>("value");
	  else
	    VStr=AR->getItem<std::string>();
	  if (Parse(VStr))
	    ELog::EM<<"Failed to parse  == "<<VStr<<ELog::endErr;
	  else
	    addVariable(Name);
	}
      else if (Type=="Geometry::Vec3D") 
	{
	  if ( (AG=dynamic_cast<XML::XMLgroup*>(AR)) )
	    VUnit=AG->getItem<Geometry::Vec3D>("value");
	  else
	    VUnit=AR->getItem<Geometry::Vec3D>();
	  addVariable(Name,VUnit);
	}
      else if (Type=="std::string") 
	{
	  if ( (AG=dynamic_cast<XML::XMLgroup*>(AR)) )
	    VStr=AG->getItem<std::string>("value");
	  else
	    VStr=AR->getItem<std::string>();
	  addVariable(Name,VStr);
	}
      else if (Type=="int") 
	{
	  if ( (AG=dynamic_cast<XML::XMLgroup*>(AR)) )
	    VInt=AG->getItem<int>("value");
	  else
	    VInt=AR->getItem<int>();
	  addVariable(Name,VInt);
	}
      else
        {
	  if ( (AG=dynamic_cast<XML::XMLgroup*>(AR)) )
	    V=AG->getItem<double>("value");
	  else
	    V=AR->getItem<double>();
	  addVariable(Name,V);
	}
      CO.deleteObj(AR);      
      AR=CO.findObj("variable");
    }
  return;
}

void
FuncDataBase::writeXML(const std::string& FName) const
  /*!
    Write out the variables 
    \param FName :: filename
  */
{
  ELog::RegMethod RegA("FuncDataBase","writeXML");

  XML::XMLwriteVisitor XVisit;
  if (FName.empty()) return;

  std::ofstream WFile(FName.c_str());

  XVisit.Accept(*this);
  XVisit.writeStream(WFile);

  WFile.close();

  return;
}

void
FuncDataBase::resetActive()
  /*!
    Reset the active flag on all variables
   */
{
  ELog::RegMethod RegA("FuncDataBase","resetActive");
  VList.resetActive();
  
  return;
}


/// \cond TEMPLATE

template void FuncDataBase::addVariable(const std::string&,const Geometry::Vec3D&);
template void FuncDataBase::addVariable(const std::string&,const double&);
template void FuncDataBase::addVariable(const std::string&,const std::string&);
template void FuncDataBase::addVariable(const std::string&,const Code&);
template void FuncDataBase::addVariable(const std::string&,const int&);
template void FuncDataBase::addVariable(const std::string&,const long int&);
template void FuncDataBase::addVariable(const std::string&,const size_t&);


template void FuncDataBase::setVariable(const std::string&,
					const Geometry::Vec3D&);
template void FuncDataBase::setVariable(const std::string&,const double&);
template void FuncDataBase::setVariable(const std::string&,const std::string&);
template void FuncDataBase::setVariable(const std::string&,const Code&);
template void FuncDataBase::setVariable(const std::string&,const int&);
template void FuncDataBase::setVariable(const std::string&,const long int&);
template void FuncDataBase::setVariable(const std::string&,const size_t&);

template double FuncDataBase::Eval();
template Geometry::Vec3D FuncDataBase::Eval();

template double FuncDataBase::EvalVar(const std::string&) const;
template long int FuncDataBase::EvalVar(const std::string&) const;
template Geometry::Vec3D FuncDataBase::EvalVar(const std::string&) const;
template int FuncDataBase::EvalVar(const std::string&) const;
template size_t FuncDataBase::EvalVar(const std::string&) const;
template std::string FuncDataBase::EvalVar(const std::string&) const;

template double FuncDataBase::EvalDefVar(const std::string&,
					 const double&) const;
template int FuncDataBase::EvalDefVar(const std::string&,const int&) const;
template long int
FuncDataBase::EvalDefVar(const std::string&,const long int&) const;

template size_t 
FuncDataBase::EvalDefVar(const std::string&,const size_t&) const;
template Geometry::Vec3D 
FuncDataBase::EvalDefVar(const std::string&,const Geometry::Vec3D&) const;
template std::string
FuncDataBase::EvalDefVar(const std::string&,const std::string&) const;

template double FuncDataBase::EvalPair(const std::string&,
				       const std::string&) const;
template int FuncDataBase::EvalPair(const std::string&,
				    const std::string&) const;
template size_t FuncDataBase::EvalPair(const std::string&,
				       const std::string&) const;
template std::string FuncDataBase::EvalPair(const std::string&,
					    const std::string&) const;
template Geometry::Vec3D FuncDataBase::EvalPair(const std::string&,
						const std::string&) const;


template double FuncDataBase::EvalPair(const std::string&,
				       const std::string&,
				       const std::string&) const;
template int FuncDataBase::EvalPair(const std::string&,
				    const std::string&,
				    const std::string&) const;
template size_t FuncDataBase::EvalPair(const std::string&,
				       const std::string&,
				       const std::string&) const;
template std::string FuncDataBase::EvalPair(const std::string&,
					    const std::string&,
					    const std::string&) const;
template Geometry::Vec3D FuncDataBase::EvalPair(const std::string&,
						const std::string&,
						const std::string&) const;




// EVALDEFPAIR


template double FuncDataBase::EvalDefPair
(const std::string&,const std::string&,const double&) const;
template int FuncDataBase::EvalDefPair
(const std::string&,const std::string&,const int&) const;

template double FuncDataBase::EvalDefPair
(const std::string&,const std::string&,
 const std::string&, const double&) const;

template int FuncDataBase::EvalDefPair
(const std::string&,const std::string&,
 const std::string&, const int&) const;

template size_t FuncDataBase::EvalDefPair
(const std::string&,const std::string&,
 const std::string&, const size_t&) const;

template Geometry::Vec3D FuncDataBase::EvalDefPair
(const std::string&,const std::string&,
 const std::string&, const Geometry::Vec3D&) const;


// EVALTRIPLE

template int FuncDataBase::EvalTriple
(const std::string&,const std::string&,
 const std::string&) const;

template int FuncDataBase::EvalTriple
(const std::string&,const std::string&,
 const std::string&,const std::string&) const;

// PARSE
template
void FuncDataBase::addParse<double>(const std::string&,const std::string&);


/// \endcond TEMPLATE
 

