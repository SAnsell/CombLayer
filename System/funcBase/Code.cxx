/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   funcBase/Code.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
  valid(0),StackPtr(0),StackSize(1)
  /*!
    Standard constructor
  */
{}

Code::Code(const Code& A) :
  valid(A.valid),StackPtr(A.StackPtr),
  StackSize(A.StackSize),ByteCode(A.ByteCode),
  Labels(A.Labels),Immed(A.Immed),      
  ImmedVec(A.ImmedVec)
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
      StackSize=A.StackSize;
      ByteCode=A.ByteCode;
      Labels=A.Labels;
      Immed=A.Immed;
      ImmedVec=A.ImmedVec;
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
  StackSize=0;
  ByteCode.clear();
  Labels.erase(Labels.begin(),Labels.end());
  Immed.clear();
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
  if (StackPtr>=StackSize)
    StackSize++;
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
  if (StackPtr>=StackSize)
    StackSize=StackPtr+1;
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



template<typename T>
T
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
  size_t DPV(0);      // Immediated Vec3D (data vec)
  size_t SP(0);       // Stack pointer  
  SP--;

  std::vector<int> stackType(StackSize);
  std::vector<double> Stack(StackSize);
  std::vector<Geometry::Vec3D> StackVec(StackSize);
  while (IP<ByteCodeSize)
    {
      switch(ByteCode[IP])
	{
	  // Functions:
	case  Opcodes::cAbs: 
	  if (stackType[SP]==1)  // Vector
	    {
	      Stack[SP] = StackVec[SP].abs(); 
	      stackType[SP]=0;
	    }
	  else if (stackType[SP]==0)  // double
	    Stack[SP] = fabs(Stack[SP]); 
	  else
	    return zeroType<T>();
	  break;
	  
	case  Opcodes::cAcos: 
	  if(stackType[SP]==1 || 
	     (Stack[SP] < -1 || Stack[SP] > 1))
	    return zeroType<T>();
	  Stack[SP] = acos(Stack[SP]); 
	  break;

	case Opcodes::cAcosh: 
	  if(stackType[SP]==0)
	    Stack[SP] = acosh(Stack[SP]); 
	  else 
	    return zeroType<T>();
	  break;

	case Opcodes::cAsin: 
	  if(stackType[SP]==1 || 
	     Stack[SP] < -1 || Stack[SP] > 1)
	    return zeroType<T>();
	  Stack[SP] = asin(Stack[SP]); 
	  break;
	  
	case Opcodes::cAsinh: 
	  if(stackType[SP]==0)
	    Stack[SP] = asinh(Stack[SP]); 
	  else 
	    return zeroType<T>();
	  break;

	case  Opcodes::cAtan: 
	  if(stackType[SP]==1)
	    return zeroType<T>();
	  Stack[SP] = atan(Stack[SP]); 
	  break;


	case Opcodes::cAtan2: 
	  if(!SP || stackType[SP-1]==1 || stackType[SP]==1)
	    return zeroType<T>();

	  Stack[SP-1] = atan2(Stack[SP-1], Stack[SP]);
	  SP--; 
	  break;

	case Opcodes::cAtanh: 
	  if(stackType[SP]==0)
	    Stack[SP] = atanh(Stack[SP]); 
	  else 
	    return zeroType<T>();
	  break;

	case Opcodes::cCeil: 
	  if(stackType[SP]==0)
	    Stack[SP] = ceil(Stack[SP]); 
	  else 
	    return zeroType<T>();
	  break;

	case Opcodes::cCos: 
	  if(stackType[SP]==0)
	    Stack[SP] = cos(Stack[SP]); 
	  else 
	    return zeroType<T>();
	  break;
	
	case Opcodes::cCosd: 
	  if(stackType[SP]==0)
	    Stack[SP] = cos(M_PI*Stack[SP]/180.0); 
	  else 
	    return zeroType<T>();
	  break;

	case  Opcodes::cCosh: 
	  if(stackType[SP]==0)
	    Stack[SP] = cosh(Stack[SP]); 
	  else 
	    return zeroType<T>();
	  break;

	case  Opcodes::cCot:
	  if(stackType[SP]==0)
	    {
	      const double t = tan(Stack[SP]);
	      if(t == 0) zeroType<T>();
	      Stack[SP] = 1/t; 
	    }
	  else 
	    return zeroType<T>();
	  break;

	case  Opcodes::cCotd:
	  if(stackType[SP]==0)
	    {
	      const double t = tan(M_PI*Stack[SP]/180.0);
	      if(t == 0) zeroType<T>();
	      Stack[SP] = 1/t; 
	    }
	  else 
	    return zeroType<T>();
	  break;

	case  Opcodes::cCsc:
	  if(stackType[SP]==0)
	    {
	      const double t = sin(Stack[SP]);
	      if(t == 0) zeroType<T>();
	      Stack[SP] = 1/t; 
	    }
	  else 
	    return zeroType<T>();
	  break;

	case  Opcodes::cCscd:
	  if(stackType[SP]==0)
	    {
	      const double t = sin(M_PI*Stack[SP]/180.0);
	      if(t == 0) zeroType<T>();
	      Stack[SP] = 1/t; 
	    }
	  else 
	    return zeroType<T>();
	  break;

	case Opcodes::cDot: 
	  if(SP && stackType[SP]==1 && stackType[SP-1]==1)
	    {
	      Stack[SP-1] = StackVec[SP].dotProd(StackVec[SP-1]);
	      stackType[SP-1]=0;
	      SP--;
	    }
	  else 
	    return zeroType<T>();
	  break;

	case Opcodes::cExp: 
	  if(stackType[SP]==0)
	    Stack[SP] = exp(Stack[SP]); 
	  else 
	    return zeroType<T>();
	  break;

	case Opcodes::cFloor: 
	  if(stackType[SP]==0)
	    Stack[SP] = floor(Stack[SP]); 
	  else 
	    return zeroType<T>();
	  break;
	      
	case Opcodes::cInt: 
	  if(stackType[SP]==0)
	    Stack[SP] = int(Stack[SP]+0.5); 
	  else 
	    return zeroType<T>();
	  break;

	case Opcodes::cLog: 
	  if(stackType[SP]==0 && Stack[SP] <= 0.0)
	    Stack[SP] = log(Stack[SP]); 
	  else 
	    return zeroType<T>();
	  break;

	case Opcodes::cLog10: 
	  if(stackType[SP]==0 && Stack[SP] <= 0.0)
	    Stack[SP] = log10(Stack[SP]); 
	  else 
	    return zeroType<T>();
	  break;

	case Opcodes::cMax: 
	  if(!SP || stackType[SP-1]==1 || stackType[SP]==1)
	    return zeroType<T>();
	  Stack[SP-1] = (Stack[SP-1]>Stack[SP]) ? Stack[SP-1] : Stack[SP];
	  SP--; 
	  break;

	case Opcodes::cMin: 
	  if(!SP || stackType[SP-1]==1 || stackType[SP]==1)
	    return zeroType<T>();
	  Stack[SP-1] = (Stack[SP-1]<Stack[SP]) ? Stack[SP-1] : Stack[SP];
	  SP--; 
	  break;

	case  Opcodes::cSec:
	  if(stackType[SP]==0)
	    {
	      const double t = cos(Stack[SP]);
	      if(t == 0) zeroType<T>();
	      Stack[SP] = 1/t; 
	    }
	  else 
	    return zeroType<T>();
	  break;

	case  Opcodes::cSecd:
	  if(stackType[SP]==0)
	    {
	      const double t = cos(M_PI*Stack[SP]/180.0);
	      if(t == 0) zeroType<T>();
	      Stack[SP] = 1/t; 
	    }
	  else 
	    return zeroType<T>();
	  break;


	case Opcodes::cSin: 
	  if(stackType[SP]==0)
	    Stack[SP] = sin(Stack[SP]); 
	  else 
	    return zeroType<T>();
	  break;

	case Opcodes::cSind: 
	  if(stackType[SP]==0)
	    Stack[SP] = sin(M_PI*Stack[SP]/180.0); 
	  else 
	    return zeroType<T>();
	  break;

	case Opcodes::cSinh: 
	  if(stackType[SP]==0)
	    Stack[SP] = sinh(Stack[SP]); 
	  else 
	    return zeroType<T>();
	  break;

	case Opcodes::cSqrt: 
	  if(stackType[SP]==0)
	    Stack[SP] = sqrt(Stack[SP]); 
	  else 
	    return zeroType<T>();
	  break;

	case Opcodes::cTan: 
	  if(stackType[SP]==0)
	    Stack[SP] = tan(Stack[SP]); 
	  else 
	    return zeroType<T>();
	  break;

	case Opcodes::cTand: 
	  if(stackType[SP]==0)
	    Stack[SP] = tan(M_PI*Stack[SP]/180.0); 
	  else 
	    return zeroType<T>();
	  break;

	case Opcodes::cTanh: 
	  if(stackType[SP]==0)
	    Stack[SP] = tanh(Stack[SP]); 
	  else 
	    return zeroType<T>();
	  break;

	case Opcodes::cVec3D: 
	  if(SP>=2 && stackType[SP]==0 && 
	     stackType[SP-1]==0 && stackType[SP-2]==0)
	    {
	      StackVec[SP-2] = 
		Geometry::Vec3D(Stack[SP-2],Stack[SP-1],Stack[SP]);
	      stackType[SP-2]=1;
	      SP-=2;
	    }
	  else 
	    return zeroType<T>();
	  break;

	case Opcodes::cImmed: 
	  SP++;
	  Stack[SP] = Immed[DP]; 
	  stackType[SP]=0;
	  DP++;
	  break;

	case Opcodes::cImmedVec: 
	  SP++;
	  StackVec[SP] = ImmedVec[DPV]; 
	  DPV++;
	  stackType[SP]=1;
	  break;
	  
	  // Operators:
	case Opcodes::cNeg: 
	  if (stackType[SP]==1)
	    StackVec[SP] *= -1.0;
	  else if  (stackType[SP]==0)
	    Stack[SP] = -Stack[SP]; 
	  else 
	    return zeroType<T>();
	  break;

	case Opcodes::cAdd: 
	  if (SP && stackType[SP]==1 && stackType[SP-1]==1)
	    StackVec[SP-1] += StackVec[SP]; 
	  else if (SP && stackType[SP]==0 && stackType[SP-1]==0)
	    Stack[SP-1] += Stack[SP]; 
	  else 
	    return zeroType<T>();
	  SP--;
	  break;
	  
	case Opcodes::cSub: 
	  if (SP && stackType[SP]==1 && stackType[SP-1]==1)
	    StackVec[SP-1] -= StackVec[SP]; 
	  else if (SP && stackType[SP]==0 && stackType[SP-1]==0)
	    Stack[SP-1] -= Stack[SP]; 
	  else 
	    return zeroType<T>();
	  SP--;
	  break;

	case Opcodes::cMul: 
	  if (SP && stackType[SP]==1 && stackType[SP-1]==1)
	    StackVec[SP-1] *= StackVec[SP]; 
	  else if (SP && stackType[SP]==0 && stackType[SP-1]==0)
	    Stack[SP-1] *= Stack[SP]; 
	  else if (SP && stackType[SP]==1 && stackType[SP-1]==0)
	    {
	      StackVec[SP-1] = StackVec[SP]*Stack[SP-1];
	      stackType[SP-1]=1;
	    }
	  else if (SP && stackType[SP]==0 && stackType[SP-1]==1)
	    StackVec[SP-1] *= Stack[SP];
	  else 
	    return zeroType<T>();
	  SP--;
	  break;
	  
	case Opcodes::cDiv: 
	  if (SP && stackType[SP]==0 && stackType[SP-1]==0 &&
	      Stack[SP]!=0.0)
	    Stack[SP-1] /= Stack[SP]; 
	  else if (SP && stackType[SP]==0 && stackType[SP-1]==1 &&
		   Stack[SP]!=0.0)
	    StackVec[SP-1] /= Stack[SP];
	  else 
	    return zeroType<T>();
	  SP--;
	  break;
	  
	case Opcodes::cMod: 
	  if (SP && stackType[SP]==0 && 
	      stackType[SP-1]==0 && Stack[SP]!=0)
	    Stack[SP-1] = fmod(Stack[SP-1], Stack[SP]);
	  else
	    return zeroType<T>();
	  SP--; 
	  break;

	case Opcodes::cPow: 
	  if (SP && stackType[SP]==0 && stackType[SP-1]==0)
	    Stack[SP-1] = pow(Stack[SP-1], Stack[SP]);
	  else
	    return zeroType<T>();
	  SP--; 
	  break;
	      
	  // Degrees-radians conversion:
	case  Opcodes::cDeg: 
	  if(stackType[SP]==0)
	    Stack[SP] = 180.0*Stack[SP]/M_PI; 
	  else 
	    return zeroType<T>();
	  break;
	  
	case  Opcodes::cRad: 
	  if(stackType[SP]==0)
	    Stack[SP] = M_PI*Stack[SP]/180.0; 
	  else 
	    return zeroType<T>();
	  break;

	case Opcodes::cInv:
	  if(stackType[SP]==0 && Stack[SP]!=0)
	    Stack[SP] = 1.0/Stack[SP];
	  else 
	    return zeroType<T>();
	  break;

	case Opcodes::cEqual: 
	  printByteCode(std::cout);
	  Vars->setValue(ByteCode[IP+1]-Opcodes::varBegin,Stack[SP]);
	  IP++;
	  break;

	default:
	  if (ByteCode[IP]<0)
	    {
	      if (SP>=Stack.size())
		throw ColErr::InContainerError<size_t>
		  (SP,"Code::Eval has looped");
	    }
	  else
	    {
	      SP++;
	      stackType[SP]=Vars->selectValue
		(ByteCode[IP]-Opcodes::varBegin,StackVec[SP],Stack[SP]);
	    }
	}
      // UPDATE LOOP
      IP++;
    }

  return (stackType[SP]) ? 
    typeConvert<T>(StackVec[SP]) : 
    typeConvert<T>(Stack[SP]);
}

template<>
double
Code::zeroType()
/*!
  Output a zero variable + warning
  \return nullObject
 */
{
  ELog::EM<<"Error with zero conversion [double]"<<ELog::endErr;
  return 0.0;
}

template<>
Geometry::Vec3D
Code::zeroType()
/*!
  Output a zero variable + warning
  \return nullObject
*/
{
  ELog::EM<<"Error with zero conversion [vec] "<<ELog::endErr;
  return Geometry::Vec3D(0,0,0);
}

template<typename T,typename U>
T
Code::typeConvert(const U& A)
/*!
  Convert between different types for Code::Eval output.
  \param A :: Input Object
  \throw TypeConv because T/U not equal
  \return [No-return]
 */
{
  throw ColErr::TypeConvError<U,T>(A,"Code convert");
}

template<>
Geometry::Vec3D
Code::typeConvert(const Geometry::Vec3D& D)
  /*!
    Convert between different types for Code::Eval output.
    \param D :: Input Object
    \return D [re-returns]
  */
{
  return D;
}

template<>
double
Code::typeConvert(const double& D)
 /*!
   Convert between different types for Code::Eval output.
   \param D :: Input Object
   \return D [re-returns]
 */
{
  return D;
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
  OX<<"\n";
  OX<<"I:";
  copy(Immed.begin(),Immed.end(),
       std::ostream_iterator<double>(OX," "));
  OX<<"\n";
  OX<<"IVEc:";
  copy(ImmedVec.begin(),ImmedVec.end(),
       std::ostream_iterator<Geometry::Vec3D>(OX," :: "));
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
  const std::streamsize ssPrec=
    OFS.precision();

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
  OFS.precision(ssPrec);
  OFS.copyfmt(state);
  return;
}


///\cond TEMPLATE
template double Code::Eval(varList*);
template Geometry::Vec3D Code::Eval(varList*);

template double Code::typeConvert(const Geometry::Vec3D&);
template Geometry::Vec3D Code::typeConvert(const double&);
///\endcond TEMPLATE
