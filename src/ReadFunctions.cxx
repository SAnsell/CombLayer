/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   src/ReadFunctions.cxx
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <cmath>
#include <complex> 
#include <vector>
#include <list> 
#include <map> 
#include <set>
#include <string>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iterator>
#include <memory>
#include <array>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "mathSupport.h"
#include "support.h"
#include "Element.h"
#include "Zaid.h"
#include "MapSupport.h"
#include "MXcards.h"
#include "Material.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Triple.h"
#include "NList.h"
#include "NRange.h"
#include "Transform.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FItem.h"
#include "FuncDataBase.h"
#include "SurInter.h"
#include "BnId.h"
#include "Acomp.h"
#include "Algebra.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "DBMaterial.h"
#include "ModeCard.h"
#include "PhysCard.h"
#include "LSwitchCard.h"
#include "PhysImp.h"
#include "PhysicsCards.h"
#include "ReadFunctions.h"

namespace ReadFunc
{

void
removeDollarComment(std::string& Line)
  /*!
    Remove a dollar comment
    \param Line :: Line to process
  */
{
  std::string::size_type pos=Line.find("$ ");
  if (pos!=std::string::npos)
    Line.erase(pos);
  return;
}

void
processDollarString(FuncDataBase& DB,std::string& Line)
  /*!
    Take a string of type px 43.4 $dx+$yl and
    produce a suitable transform using the FuncDataBase.
    It extracts the dollars and then corrects the line
    Each component must have no spaces but each 
    component only needs one $var.
    \param DB :: DataBase for function evaluation
    \param Line :: Line to process
    \throw ExBase::CompileError() on missed comile
  */
{
  ELog::RegMethod RegA("ReadFunctions","processDollarString");

  std::ostringstream cx;
  std::string Part;
  int first(1);
  // Avoid comments
  if (Line.length()<2 || 
      (tolower(Line[0])=='c' && isspace(Line[1])))
    return;

  while(StrFunc::section(Line,Part))
    {
      std::string::size_type pos=Part.find('$');
      if (pos != std::string::npos)
        {
	  /// process equals
	  std::string::size_type eqpos=Part.find("=");
	  if (eqpos!=std::string::npos)
	    {
	      cx<<" "<<Part.substr(0,eqpos+1);
	      Part.erase(0,eqpos+1);
	      pos=Part.find('$');
	      first=1;       // avoid spc
	    }
	  // Standard item
	  while(pos!=std::string::npos)
	    {
	      Part.erase(pos,1);
	      pos=Part.find('$');
	    }
	  int flag=DB.Parse(Part);
	  if (flag)
	    throw ColErr::InvalidLine(RegA.getBase(),Part,0);
	  if (!first)
	    cx<<" ";
	  cx<<DB.Eval<double>();
	  first=0;
	}
      else
        {
	  if (!first)
	    cx<<" ";
	  cx<<Part;
	  first=0;
	}
    } 
  Line=cx.str();
  return;
}

int
processSurface(const std::string& InputLine,const int offset)
  /*! 
    Job is to decide which type of surface the 
    current line is attached too. It also must decide if 
    there is enough information on the current line 
    \param InputLine :: Line to process (Stripped of comments on return)
    \param offset :: surface ID offset
    \retval 0 More information required
    \retval -1 failed
    \retval 1 good
    \retval 2 pure comment 
    \retval 3 valid but void surface
  */
{
  ELog::RegMethod RegItem("ReadFunctions","processSurface");

  std::string Line=StrFunc::fullBlock(InputLine);
  StrFunc::stripComment(Line);
  if (Line.size()<1 ||               // comments blank line, ^c or ^c<spc> 
      (tolower(Line[0])=='c' && 
       (Line.size()==1 || isspace(Line[1])) ))
    return 2;

  StrFunc::lowerString(Line);  
  int name;
  if (!StrFunc::section(Line,name) || Line.empty())
    return -1; 
  name+=offset;
  // Get transform if it exists.
  int transN(0);
  StrFunc::section(Line,transN);

  ModelSupport::surfIndex::Instance().
    createSurface(name,transN,Line);
  
  return 1;
}

int
readSurfaces(FuncDataBase& DB,std::istream& IX,const int offset)
  /*!
    Reads the surfaces
      - Section is teminated with an END comment or a 
         blank line
    \param DB :: DataBase for function evaluation
    \param IX :: input file aligned on the surface section
    \param offset :: Offset nubmer 
    \return Number of surfaces read
  */
{
  ELog::RegMethod RegItem("ReadFunctions","readSurfaces");

  std::string Line;
  int monoLine(1);        // Do we have and extention to add
  int Scount(0);
  int ignore(0);          // 
  std::vector<std::string> errLine;
  
  while(IX.good())
    {
      std::string InputLine = StrFunc::getLine(IX);           
      removeDollarComment(InputLine);
      // HANDLE Actions
      if (InputLine.find(" ENDIGNORE")!=std::string::npos) 
	ignore=0;
      else if (InputLine.find(" IGNORE")!=std::string::npos) 
	ignore=1;
      else if (!ignore && InputLine.find("END")!=std::string::npos) 
	{
	  ELog::EM<<"Read Surfaces == "<<Scount<<ELog::endDebug;
	  return Scount;
	}
      
      if (!ignore)
	{
	  processDollarString(DB,InputLine);
	  Line+=InputLine+" ";
	  monoLine=processSurface(Line,offset);
	  if (monoLine==1 || monoLine==2 || monoLine==3)      // Good line / comment
	    {
	      if (monoLine==1) Scount++;
	      Line="";
	      errLine.clear();
	    }
	  else if ((!errLine.empty() && monoLine>1) ||
		   errLine.size()>4)
	    {
	      ELog::EM<<"Error with line grp:\n";
	      for(size_t i=0;i<errLine.size();i++)
		ELog::EM<<" -- "<<errLine[i]<<"\n";
	      ELog::EM<<ELog::endErr;
	    }
	  else
	    {
	      errLine.push_back(InputLine);
	    }
	}
    }
  ELog::EM<<"File ended without END"<<ELog::endWarn;
  return Scount;
}

// ---------------------------------------------------
//                     CELLS
// ---------------------------------------------------

int
checkInsert(const MonteCarlo::Qhull& A,const int offset,OTYPE& ObjMap)
  /*!
    When a new Qhull object is to be inserted this
    checks to see if it can be done, returns 1 on 
    success and 0 on failure 
    \param A :: Hull to insert in the main list
    \param offset :: offset number to add to object index
    \param ObjMap :: Map to add the cells to.
    \returns 1 on success and 0 on overwrite
  */
{
  ELog::RegMethod RegItem("ReadFunctions","checkInsert");

  const int cellNumber=A.getName()+offset;

  OTYPE::mapped_type APtr=A.clone();
  APtr->setName(cellNumber);
  std::pair<OTYPE::iterator,bool> iPair=
    ObjMap.insert(OTYPE::value_type(cellNumber,APtr));
  if (!iPair.second)
    {
      ELog::EM<<"Over-writing Object ::"<<cellNumber<<ELog::endWarn;
      delete iPair.first->second;
      OTYPE::iterator mc=iPair.first;
      mc->second=APtr;
      return 0;
    }
  return 1;
}

int
mapInsert(const OTYPE& baseMap,OTYPE& ObjMap)
  /*!
    Inserts one map into the other map
    \param baseMap :: Map to add the cells to.
    \param ObjMap :: Map to add the cells to.
    \returns number inserted
  */
{
  ELog::RegMethod RegItem("ReadFunctions","mapInsert");

  int cnt(0);
  OTYPE::const_iterator mc;
  for(mc=baseMap.begin();mc!=baseMap.end();mc++)
    {
      OTYPE::mapped_type APtr=mc->second;
      if (APtr)
	{
	  const int cellNumber=APtr->getName();
	  // Does not change the map if object exists:
	  std::pair<OTYPE::iterator,bool> iPair=
	    ObjMap.insert(OTYPE::value_type(cellNumber,APtr));
	  if (!iPair.second)
	    {
	      ELog::EM<<"Over-writing Object ::"<<cellNumber<<ELog::endWarn;
	      delete iPair.first->second;
	      OTYPE::iterator mc=iPair.first;
	      mc->second=APtr;
	    }
	  else
	    cnt++;
	}
    }
  return cnt;
}

int
readCells(FuncDataBase& DB,std::istream& IX,
	  const int offset,OTYPE& ObjMap)
  /*!
    Reads the Cell definitions from MCNPX.
    \param DB :: DataBase for function evaluation
    \param IX :: Input stream
    \param offset :: Cell number offset to add the cell number
    \param ObjMap :: Map to place cells
    \retval Number of cell read
  */
{
  ELog::RegMethod RegA("ReadFunctions","readCells");

  std::string Line;        // Line with everything
  std::string ObjLine;     // Stripped line
  int Ccount(0);
  int last(0);
  int endActive(0);        // No active line
  int ignore(0);

  while(!last && IX.good())
    {
      Line=StrFunc::getAllLine(IX);                // Read Line [with comment]
      ReadFunc::removeDollarComment(Line);         // Strip comments
      if (Line.find(" ENDIGNORE")!=std::string::npos) 
	ignore=0;
      else if (Line.find(" IGNORE")!=std::string::npos) 
	ignore=1;
      else if (!ignore && Line.find("END")!=std::string::npos)  // If END exit (but process last comp)
        {
	  last=1;
	  endActive=1;
	}
      else if (StrFunc::isEmpty(Line) || 
	       tolower(Line[0]) == 'c' || Line[0]=='$')     // comment line
	endActive=1;
      else if (!ignore)
        {
	  ReadFunc::processDollarString(DB,Line);          // Process variables
	  if (MonteCarlo::Object::startLine(Line))        // Start of a line
	    endActive=2;
	  else
	    {
	      StrFunc::stripComment(Line);
	      ObjLine+=" "+StrFunc::fullBlock(Line);	  
	    }
	}
      
      if (endActive && !ObjLine.empty())
        {
	  MonteCarlo::Qhull Tx;
	  // This loop over the string making fake object
	  // for each #(xxx) 
	  // REMOVED To use new CompGrp object
	  // Now add object
	  Tx.setCreate(Ccount);
	  if (Tx.setObject(ObjLine) && 
	      checkInsert(Tx,offset,ObjMap))
	    {
	      Ccount++;
	    }
	  else
	    {
	      ELog::EM<<"Failed on Line:"
		      <<ObjLine<<ELog::endErr;
	      throw ColErr::ExitAbort(RegA.getFull());
	    }
	  ObjLine="";
	}

      // afterwards incase first/new line
      if (endActive==2)
        {
	  // Add line without comments + end <spc>
	  StrFunc::stripComment(Line);
	  ObjLine=StrFunc::fullBlock(Line);
	}
      endActive=0;
    }
  if (!last)
    ELog::EM<<"Over run end of Cells file"<<ELog::endErr;

  ELog::EM<<"Read Cells =="<<Ccount<<ELog::endDiag;
  return Ccount;
}


int
readMaterial(std::istream& IX)
/*! 
  Reads the Material components until
  it reaches an END. 
  \param IX :: Input stream
  \return number of materials read
  */
{
  ELog::RegMethod RegItem("Simulation","readMaterial");

  ModelSupport::DBMaterial& DBM=
    ModelSupport::DBMaterial::Instance();

  int MCount(0);          /// Number of materials found
  std::string Line;
  std::vector<std::string> MatLines;

  
  int currentMat(0);          // Current material
  Line = StrFunc::getLine(IX);           
  while(IX.good() && currentMat != -100)
    {
      const int lineType = MonteCarlo::Material::lineType(Line);
		      
      if (lineType==0 && !MatLines.empty())             // Continue line
	MatLines.back()+=" "+Line;
      else if (lineType>0 && lineType==currentMat)      // mt/mx etc line
	MatLines.push_back(Line);
      else if (lineType>0 || lineType==-100)            // newMat / END
	{
	  if (currentMat)
	    {
	      MonteCarlo::Material Mt;
	      if (!Mt.setMaterial(MatLines))     
		{
		  DBM.setMaterial(Mt);
		  MCount++;
		}
	      MatLines.clear();
	    }
	  MatLines.push_back(Line);
	  currentMat=lineType;
	}
      Line = StrFunc::getLine(IX);           
    }
  return MCount;
}

int
readPhysics(FuncDataBase& DB,std::istream& IX,
	    physicsSystem::PhysicsCards* PhysPtr)
  /*!
    Read until the end of the physics section
    which is denoted by a blank line or EOF
    \param DB :: Database for variables
    \param IX :: imput stream to read
    \param PhysPtr :: Physics card system
    
    \returns number of successfull reads
  */
{
  ELog::RegMethod RegA("ReadFunctions","readPhysics");

  std::string Line;
  int Pcount(0);

  while(IX.good())
    {
      Line = StrFunc::getLine(IX);
      if (Line.find("END")!=std::string::npos) 
	return Pcount;
      removeDollarComment(Line);
      processDollarString(DB,Line);
      const int flag=PhysPtr->processCard(Line);  
      if(flag<0)
        {
	  ELog::EM<<"Error on Phys line:: \n"
		  <<"=="<<Line<<ELog::endErr;
	  throw ColErr::ExitAbort(RegA.getFull());
	}
      Pcount+=flag;
    }
  return Pcount;
}


}  // NAMESPACE ReadFunc
