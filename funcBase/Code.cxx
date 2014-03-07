/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   funcBase/Code.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <string>
#include <vector>
#include <map>
#include <iterator>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Code.h"
#include "funcList.h"
#include "varList.h"


Code::Code() :
  valid(0),StackPtr(0),Stack(1)
  /*!
    Standard constructor
  */
{}

Code::Code(const Code& A) :
  valid(A.valid),StackPtr(A.StackPtr),
  ByteCode(A.ByteCode),Labels(A.Labels),
  Immed(A.Immed),Stack(A.Stack)
  /*!
    Standard copy constructor
    \param A :: Code item to copy
  */
{}

Code&
Code::operator=(const Code& A)
  /*!
    Standard assignment operator
    \param A :: Code Object to copy
    \return *this
  */ 
{
  if (&A!=this)
    {
      valid=A.valid;
      StackPtr=A.StackPtr;
      ByteCode=A.ByteCode;
      Labels=A.Labels;
      Immed=A.Immed;
      Stack=A.Stack;
    }
  return *this;
}

Code::~Code()
  /*!
    Standard destructor
  */
{}

void
Code::clear()
  /*!
    Clear the stack,Immed and Byte code sections
  */
{
  valid=0;
  StackPtr=0;
  ByteCode.clear();
  Labels.erase(Labels.begin(),Labels.end());
  Immed.clear();
  Stack.clear();
  return;
}

void
Code::addByte(const int B) 
  /*!
    Adds a new byte code to the list
    \param B :: byte code value
  */
{
  ByteCode.push_back(B); 
  StackPtr--; 
  return;
}

int
Code::popByte() 
  /*!
    Gets a byte code off the stack
    \retval byte code
  */
{
  const int out=ByteCode.back();
  ByteCode.pop_back();
  StackPtr++;          // why??????? 
  return out;
}

void
Code::addFunctionOpcode(const int B) 
  /*!
    Adds a new byte code to the list
    \param B :: byte code value
  */
{
  ByteCode.push_back(B); 
  return;
}

size_t
Code::incStackPtr()
  /*!
    Increase the stack pointer by one
    and return the value
    \return new stackPtr value
  */
{
  StackPtr++;
  if (StackPtr>=Stack.size())
    Stack.resize(StackPtr+1);
  return StackPtr;
}

size_t
Code::addStackPtr(const size_t AS)
  /*!
    Increase the stack pointer by AS
    \param AS :: amount to increase stack pointer.
    \return new stackPtr value
  */
{
  StackPtr+=AS;
  if (StackPtr>=Stack.size())
    Stack.resize(StackPtr+1);
  return StackPtr;
}

size_t
Code::subStackPtr(const size_t AS)
  /*!
    Increase the stack pointer by AS
    \param AS :: amount to increase stack pointer.
    \return new stackPtr value
  */
{
  if (AS>StackPtr)
    throw ColErr::IndexError<size_t>(AS,StackPtr,"Code::subStackPtr");
  StackPtr-=AS;
  return StackPtr;
}

double 
Code::Eval(varList* Vars)
  /*!
    The function that evaluates everything
    \param Vars :: Vector of variable pointers
    \returns value of Function expression
  */
{
  const size_t ByteCodeSize = ByteCode.size();
  size_t IP(0);       // Bytecode Pointer
  size_t DP(0);       // Immediated points (data)
  size_t SP(0);      // Stack pointer  
  SP--;

  while (IP<ByteCodeSize)
    {
      switch(ByteCode[IP])
        {
// Functions:
	case  Opcodes::cAbs: 
	  Stack[SP] = fabs(Stack[SP]); 
	  break;
	case  Opcodes::cAcos: 
	  if(Stack[SP] < -1 || Stack[SP] > 1)
	    {
	      //	      evalErrorType=4; 
	      return 0; 
	    }
	  Stack[SP] = acos(Stack[SP]); 
	  break;
	case Opcodes::cAcosh: 
	  Stack[SP] = acosh(Stack[SP]); 
	  break;
	case Opcodes::cAsin: 
	  if(Stack[SP] < -1 || Stack[SP] > 1)
	    { 
	      //	      evalErrorType=4; 
	      return 0; 
	    }
	  Stack[SP] = asin(Stack[SP]); 
	  break;
	case Opcodes::cAsinh: 
	  Stack[SP] = asinh(Stack[SP]); 
	  break;
	case  Opcodes::cAtan: 
	  Stack[SP] = atan(Stack[SP]); 
	  break;
	case Opcodes::cAtan2: 
	  Stack[SP-1] = atan2(Stack[SP-1], Stack[SP]);
	  SP--; 
	  break;
	case Opcodes::cAtanh: 
	  Stack[SP] = atanh(Stack[SP]); 
	  break;
	case Opcodes::cCeil: 
	  Stack[SP] = ceil(Stack[SP]); 
	  break;
	case Opcodes::cCos: 
	  Stack[SP] = cos(Stack[SP]); 
	  break;
	case Opcodes::cCosd: 
	  Stack[SP] = cos(M_PI*Stack[SP]/180.0); 
	  break;
	case  Opcodes::cCosh: 
	  Stack[SP] = cosh(Stack[SP]); 
	  break;
	case  Opcodes::cCot:
	  {
	    const double t = tan(Stack[SP]);
	    if(t == 0) 
	      return 0; 
	    Stack[SP] = 1/t; 
	    break;
	  }
	case  Opcodes::cCotd:
	  {
	    const double t = tan(M_PI*Stack[SP]/180.0);
	    if(t == 0) 
	      return 0; 
	    Stack[SP] = 1/t; 
	    break;
	  }
	case Opcodes::cCsc:
	  {
	    const double s = sin(Stack[SP]);
	    if(s == 0) 
		return 0; 
	    Stack[SP] = 1/s; 
	    break;
	  }
	case Opcodes::cCscd:
	  {
	    const double s = sin(M_PI*Stack[SP]/180.0);
	    if(s == 0) 
		return 0; 
	    Stack[SP] = 1/s; 
	    break;
	  }
	case Opcodes::cExp: 
	  Stack[SP] = exp(Stack[SP]); 
	  break;
	case Opcodes::cFloor: 
	  Stack[SP] = floor(Stack[SP]); 
	  break;

	case Opcodes::cInt: 
	  Stack[SP] = floor(Stack[SP]+.5); 
	  break;
	case Opcodes::cLog: 
	  if(Stack[SP] <= 0) 
	      return 0; 
	  Stack[SP] = log(Stack[SP]); 
	  break;
	case Opcodes::cLog10: 
	  if(Stack[SP] <= 0) 
	      return 0; 
	  Stack[SP] = log10(Stack[SP]);
	  break;
	case Opcodes::cMax: 
	  Stack[SP-1] = (Stack[SP-1]>Stack[SP]) ? Stack[SP-1] : Stack[SP];
	  SP--;
	  break;
	case Opcodes::cMin: 
	  Stack[SP-1] = (Stack[SP-1]<Stack[SP]) ? Stack[SP-1] : Stack[SP];
	  SP--; 
	  break;
	case Opcodes::cSec:
	  {
	    const double c = cos(Stack[SP]);
	    if (c == 0) 
	      return 0; 
	    Stack[SP] = 1.0/c; 
	    break;
	  }
	case Opcodes::cSecd:
	  {
	    const double c = cos(M_PI*Stack[SP]/180.0);
	    if (c == 0) 
		return 0; 
	    Stack[SP] = 1/c; 
	    break;
	  }
	case Opcodes::cSin: 
	  Stack[SP] = sin(Stack[SP]); 
	  break;
	case Opcodes::cSind: 
	  Stack[SP] = sin(M_PI*Stack[SP]/180.0); 
	  break;
	case Opcodes::cSinh: 
	  Stack[SP] = sinh(Stack[SP]); 
	  break;
	case Opcodes::cSqrt: 
	  if(Stack[SP] < 0) 
	      return 0; 
	  Stack[SP] = sqrt(Stack[SP]); 
	  break;
	case Opcodes::cTan: 
	  Stack[SP] = tan(Stack[SP]);
	  break;
	case Opcodes::cTand: 
	  Stack[SP] = tan(M_PI*Stack[SP]/180.0); 
	  break;
	case Opcodes::cTanh: 
	  Stack[SP] = tanh(Stack[SP]); 
	  break;
// Misc:
          case Opcodes::cImmed: 
	    SP++;
	    Stack[SP] = Immed[DP]; 
	    DP++;
	    break;

// Operators:
	case Opcodes::cNeg: 
	  Stack[SP] = -Stack[SP]; 
	  break;
	case Opcodes::cAdd: 
	  Stack[SP-1] += Stack[SP]; 
	  SP--; 
	  break;
	case Opcodes::cSub: 
	  Stack[SP-1] -= Stack[SP];
	  SP--; 
	  break;
	case Opcodes::cMul: 
	    Stack[SP-1] *= Stack[SP];
	    SP--; 
	    break;
	case Opcodes::cDiv: 
	  if(Stack[SP] == 0) 
	      return 0; 
	  Stack[SP-1] /= Stack[SP];
	  SP--; 
	  break;
	case Opcodes::cMod: 
	  if(Stack[SP] == 0) 
	    return 0;
	  Stack[SP-1] = fmod(Stack[SP-1], Stack[SP]);
	  SP--; 
	  break;
	case Opcodes::cPow: 
	  Stack[SP-1] = pow(Stack[SP-1], Stack[SP]);
	  SP--; 
	  break;

// Degrees-radians conversion:
	case  Opcodes::cDeg: 
	  Stack[SP] = 180.0*Stack[SP]/M_PI; 
	  break;
	case  Opcodes::cRad: 
	  Stack[SP] = M_PI*Stack[SP]/180.0; 
	  break;
	case Opcodes::cInv:
	  if(Stack[SP] == 0.0) 
	    return 0; 
	  Stack[SP] = 1.0/Stack[SP];
	  break;
	case Opcodes::cEqual: 
	  printByteCode(std::cout);
	  Vars->setValue(ByteCode[IP+1]-Opcodes::varBegin,Stack[SP]);
	  IP++;
	  break;

// User-defined function calls:
/*	case funcList::cFCall:
	  {
	    int index = ByteCode[++IP];
	    int params = data->FuncPtrs[index].params;
	    double retVal =
	      data->FuncPtrs[index].ptr(&Stack[SP-params+1]);
	    SP -= params-1;
	    Stack[SP] = retVal;
	    break;
	  }
*/
// Other function parser
/*
	case funcList::cPCall:
	  {
	    IP++;
	    int index = ByteCode[IP];
	    int params = data->FuncParsers[index]->data->varAmount;
	    double retVal =
	      data->FuncParsers[index]->Eval(&Stack[SP-params+1]);
	    SP -= params-1;
	    Stack[SP] = retVal;
	    break;
	  }
	case   funcList::cVar: 
	  break; // Paranoia. These should never exist
	case   funcList::cDup: 
	  Stack[SP+1] = Stack[SP]; 
	  SP++; 
	  break;
       */
// Variables (push onto stack): 
	default:
	  if (ByteCode[IP]<0)
	    {
	      if (SP>=Stack.size()) SP=0;
	      throw ColErr::InContainerError<size_t>
		(SP,"Code::Eval has looped");
//	      Stack[SP]=CList[1+ByteCode[IP]]->execute();
	    }
	  else
	    {
	      SP++;
	      Stack[SP] = Vars->getValue<double>(ByteCode[IP]-Opcodes::varBegin);
	    }
        }
      // UPDATE LOOP
      IP++;
    }

  return Stack[SP];
}

void
Code::writeCompact(std::ostream& OX) const
  /*!
    Write system in a way that can be read later
    \param OX :: Output stream
  */
{

  OX<<"B:";
  copy(ByteCode.begin(),ByteCode.end(),
       std::ostream_iterator<int>(OX," "));
  OX<<"I:";
  copy(Immed.begin(),Immed.end(),
       std::ostream_iterator<double>(OX," "));
  return;
}

void
Code::printByteCode(std::ostream& OFS) const
  /*!
    Debug function to find out how the string
    was passed 
    \param OFS :: output stream to write data too
  */
{  
  // Copy state:
  std::ios state(0);
  state.copyfmt(OFS);

  size_t DP(0);
  for(size_t IP=0;IP<ByteCode.size();IP++)
    {
      OFS << IP << " : ";
      int opcode = ByteCode[IP];

      const FuncDefinition& FItem=
	funcList::Instance()->findFunction(opcode);      
      switch(opcode)
        {
          case Opcodes::cImmed:
              OFS.precision(10);
              OFS << "push\t" << Immed[DP++] << std::endl;
              break;
          case Opcodes::cNeg:
              OFS << "neg "<< std::endl;
	      break;
          case Opcodes::cAdd:
              OFS << " + "<< std::endl;
              break;
          case Opcodes::cSub:
              OFS << " - "<< std::endl;
	      break;
          case Opcodes::cMul:
              OFS << " * "<< std::endl;
              break;
          case Opcodes::cDiv:
              OFS << " / "<< std::endl;
              break;
          case Opcodes::cMod:
              OFS << " % "<< std::endl;
              break;
          case Opcodes::cPow:
              OFS << " ^ "<< std::endl;
              break;
          case Opcodes::cDeg:
              OFS << "deg"<< std::endl;
              break;
          case Opcodes::cRad:
              OFS << "rad"<< std::endl;
              break;
          case Opcodes::cEqual:
              OFS << "="<< std::endl;
              break;

	      /*
          case Opcodes::cFCall:
	    {
	      IP++;
	      const int index = ByteCode[IP];
	      std::map<std::string,int>::const_iterator iter =
		data->FuncPtrNames.begin();
	      while(iter->second != index) 
		iter++;
	      OFS << "call\t" << iter->first << std::endl;
	      break;
	    }
	      */
	      /*
          case Opcodes::cPCall:
	    {
	      IP++;
	      int index = ByteCode[IP];
	      std::map<std::string,int>::const_iterator iter =
		data->FuncParserNames.begin();
	      while(iter->second != index) 
		iter++;
	      OFS << "call\t" << iter->first << std::endl;
	      break;
              }
	      */
          default:
	    if(opcode < Opcodes::varBegin)
              { 
		OFS <<FItem.name << std::endl;
              }
	    else
	      {
		OFS << "push\tVar" << opcode-Opcodes::varBegin << std::endl;
	      }
        }
    }
  OFS.copyfmt(state);
  return;
}
