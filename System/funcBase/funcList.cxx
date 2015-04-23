/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   funcBase/funcList.cxx
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
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>
#include <tuple>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Triple.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Code.h"
#include "FItem.h"
#include "funcList.h"

FuncDefinition::FuncDefinition(const std::string& Str,
			       const int A,const size_t B) :
  name(Str),nameLength(Str.length()),
  opCode(A),Np(B)
  /*!
    Constructor 
    \param Str :: name of function
    \param A :: OpCode (needs to be unique)
    \param B :: Number of parameters
   */
{ }

FuncDefinition::FuncDefinition(const FuncDefinition& A) :
  name(A.name),nameLength(A.nameLength),
  opCode(A.opCode),Np(A.Np)
  /*!
    Copy constructor 
    \param A :: FuncDefinition to copy
   */
{ }

FuncDefinition&
FuncDefinition::operator=(const FuncDefinition& A)
  /*!
    Assignment operator 
    \param A :: FuncDefinition to copy
    \return *this
   */
{
  if (this!=&A)
    {
      name=A.name;
      nameLength=A.nameLength;
      opCode=A.opCode;
      Np=A.Np;
    }
  return *this;
}

FuncDefinition::~FuncDefinition() 
  /*!
    Destructor
   */
{}

funcList::funcList() :
  nullItem("",0,0)
  /*!
    Initialise function for the main map
  */
{
  typedef std::tuple<std::string,size_t,int> FTYPE;
  const FTYPE Functions[]=
     {
       FTYPE( "abs",   1 ,Opcodes::cAbs),
       FTYPE( "acos",  1 ,Opcodes::cAcos),
       FTYPE( "acosh", 1 ,Opcodes::cAcosh),
       FTYPE( "asin",  1 ,Opcodes::cAsin),
       FTYPE( "asinh", 1 ,Opcodes::cAsinh),
       FTYPE( "atan",  1 ,Opcodes::cAtan),
       FTYPE( "atan2", 2 ,Opcodes::cAtan2),
       FTYPE( "atanh", 1 ,Opcodes::cAtanh),
       FTYPE( "ceil",  1 ,Opcodes::cCeil),
       FTYPE( "cos",   1 ,Opcodes::cCos),
       FTYPE( "cosd",  1 ,Opcodes::cCosd),
       FTYPE( "cosh",  1 ,Opcodes::cCosh),
       FTYPE( "cot",   1 ,Opcodes::cCot),
       FTYPE( "cotd",  1 ,Opcodes::cCotd),
       FTYPE( "csc",   1 ,Opcodes::cCsc),
       FTYPE( "cscd",  1 ,Opcodes::cCscd),
       FTYPE( "deg2rad", 1 ,Opcodes::cDeg),
       FTYPE( "dot",   2 ,Opcodes::cDot),
       FTYPE( "exp",   1 ,Opcodes::cExp),
       FTYPE( "floor", 1 ,Opcodes::cFloor),
       FTYPE( "int",   1 ,Opcodes::cInt),
       FTYPE( "inv",   1 ,Opcodes::cInv),
       FTYPE( "log",   1 ,Opcodes::cLog),
       FTYPE( "log10", 1 ,Opcodes::cLog10),
       FTYPE( "max",   2 ,Opcodes::cMax),
       FTYPE( "min",   2 ,Opcodes::cMin),
       FTYPE( "sec",   1 ,Opcodes::cSec),
       FTYPE( "secd",  1 ,Opcodes::cSecd),
       FTYPE( "sin",   1 ,Opcodes::cSin),
       FTYPE( "sind",  1 ,Opcodes::cSind),
       FTYPE( "sinh",  1 ,Opcodes::cSinh),
       FTYPE( "sqrt",  1 ,Opcodes::cSqrt),
       FTYPE( "tan",   1 ,Opcodes::cTan),
       FTYPE( "tand",  1 ,Opcodes::cTand),
       FTYPE( "tanh",  1 ,Opcodes::cTanh),
       FTYPE( "vec3d", 3 ,Opcodes::cVec3D)
     };

    const size_t FuncAmount = sizeof(Functions)/sizeof(Functions[0]);
    for(size_t i=0;i<FuncAmount;i++)
      {
	mathFunc.insert(std::pair<std::string,FuncDefinition>
			(std::get<0>(Functions[i]),
			 FuncDefinition(std::get<0>(Functions[i]),
					std::get<2>(Functions[i]),
					std::get<1>(Functions[i]))));
      }
}


funcList*
funcList::Instance()
  /*!
    Hand back a pointer / reference to the class
    \return ptr to the class.
  */
{
  static funcList MasterFuncList;

  return &MasterFuncList;
}

const FuncDefinition&
funcList::findFunction(const std::string& Key) const  
  /*!
    Returns a pointer to the FuncDefinition instance which 'name' is
    the same as the one given by 'F'. 
    \param Key :: name of function to find
    \retval nullItem :: If no such function name exists,
    \retval FuncDefinition if valid name
  */
{
  std::map<std::string,FuncDefinition>::const_iterator vc;
  vc=mathFunc.find(Key);
  if (vc!=mathFunc.end())
    return vc->second;

  return nullItem;
}

const FuncDefinition&
funcList::findFunction(const int Key) const  
  /*!
    Returns a pointer to the FuncDefinition instance which Key is
    the same as the one given by 'F'. 
    \param Key :: opcode index to use for the search
    \retval nullItem :: If no such function key exists,
    \retval FuncDefinition if item exists
  */
{
  std::map<std::string,FuncDefinition>::const_iterator vc;
  for(vc=mathFunc.begin();vc!=mathFunc.end();vc++)
    {
      if (vc->second.opCode==Key)
	return vc->second;
    }
  return nullItem;
}

