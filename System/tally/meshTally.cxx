/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tally/meshTally.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include <cmath>
#include <string>
#include <sstream>
#include <list>
#include <vector>
#include <map>
#include <iterator>
#include <memory>
#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Triple.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "masterWrite.h"
#include "NList.h"
#include "NRange.h"
#include "pairRange.h"
#include "Tally.h"
#include "meshTally.h"

namespace tallySystem
{

meshTally::meshTally(const int ID) :
  Tally(ID),typeID(1),keyWords("flux"),
  requireRotation(0)
  /*!
    Constructor
    \param ID :: Identity number of tally
  */
{}

meshTally::meshTally(const meshTally& A) : 
  Tally(A),
  typeID(A.typeID),keyWords(A.keyWords),kIndex(A.kIndex),
  mshmf(A.mshmf),requireRotation(A.requireRotation),
  Pts(A.Pts),minCoord(A.minCoord),maxCoord(A.maxCoord)
  /*!
    Copy constructor
    \param A :: meshTally to copy
  */
{}

meshTally&
meshTally::operator=(const meshTally& A)
  /*!
    Assignment operator
    \param A :: meshTally to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Tally::operator=(A);
      typeID=A.typeID;
      keyWords=A.keyWords;
      kIndex=A.kIndex;
      mshmf=A.mshmf;
      requireRotation=A.requireRotation;
      Pts=A.Pts;
      minCoord=A.minCoord;
      maxCoord=A.maxCoord;
    }
  return *this;
}

meshTally*
meshTally::clone() const
  /*!
    Clone object
    \return new (this)
  */
{
  return new meshTally(*this);
}

meshTally::~meshTally()
  /*!
    Destructor
  */
{}

void
meshTally::setType(const int T)
  /*!
    Set the mesh typeID values
    \param T :: Type to set [1,2,3]
  */
{
  if (T<1 || T>3)
    throw ColErr::RangeError<int>(T,1,4,"meshTally::setType");
  typeID=T;
  return;
}

void
meshTally::setKeyWords(const std::string& K)
  /*!
    Set the mesh keyworkds
    \param K :: Keyword to add
  */
{
  if (!K.empty())
    keyWords=StrFunc::fullBlock(K);
  else
    keyWords="";
  return;
}

void
meshTally::setIndexLine(std::string K)
  /*!
    Set the mesh keyworkds
    \param K :: Line to process
  */
{
  double V;
  while(StrFunc::section(K,V))
    {
      kIndex.push_back(V);
    }
  return;
}

void
meshTally::setIndex(const size_t* IDX)
  /*!
    Sets the individual index for each x,y,z
    \param IDX :: array of three object
  */
{
  ELog::RegMethod RegA("meshTally","setIndex");

  for(size_t i=0;i<3;i++)
    {
      if (IDX[i]<1)
	throw ColErr::IndexError<int>(IDX[i],1,RegA.getBase());

      Pts[i]=IDX[i];
    }
  return;
}

void
meshTally::setCoordinates(const Geometry::Vec3D& A,
			  const Geometry::Vec3D& B)
  /*!
    Sets the min/max coordinates
    \param A :: First coordinate
    \param B :: Second coordinate
  */
{
  // Add some checking here
  minCoord=A;
  maxCoord=B;
  return;
}

void 
meshTally::setResponse(const std::string& Line)
  /*!
    Set the mesh response function based on the input line
    \param Line :: Line to process
  */
{
  ELog::RegMethod RegA("meshTally","setResponse");
  if (mshmf.processString(Line))
    {
      ELog::EM<<"Failed to set response line :"<<ELog::endCrit;
      ELog::EM<<Line<<ELog::endErr;
    }
  return;
}

int
meshTally::addLine(const std::string& LX)
  /*!
    Adds a string, if this fails
    it return Tally::addLine.
    Currently it only searches for:
    - f  : Initial line {:Particles} CellList
    \param LX :: Line to search
    \retval -1 :: ID not correct
    \retval -2 :: Type object not correct
    \retval -3 :: Line not correct
    \retval 0 on success
    \retval Tally::addline(Line) if nothing found
  */
{
  /*
    std::string Line(LX);
    std::string Tag;
    if (!StrFunc::section(Line,Tag))
    return -1;
    
    // get tag+ number
    std::pair<std::string,int> FUnit=Tally::getElm(Tag);
    if (FUnit.second<0)
    return -2;
    
    if (FUnit.first=="f")
      {
      plus=0;
      std::string Lpart=LX;
      Tally::processParticles(Lpart);        // pick up particles
      if (cellList.processString(Lpart))
      return -3;
      return 0;
      }
      
      if (FUnit.first=="+f")
      {
      plus=1;
      std::string Lpart=LX;
      Tally::processParticles(Lpart);        // pick up particles
      if (cellList.processString(Lpart))
      return -3;
      return 0;
      }
  */
  return Tally::addLine(LX);
}
  

void
meshTally::rotateMaster()
  /*!
    Rotate the points [if required]
  */
{
  ELog::RegMethod RegA("meshTally","rotateMaster");
  
  if (requireRotation)
    {
      const masterRotate& MR=masterRotate::Instance(); 
      MR.applyFull(minCoord);
      MR.applyFull(maxCoord);
    }
  return;
}

void
meshTally::write(std::ostream& OX) const
  /*!
    Write out the mesh tally into the tally region
    \param OX :: Output stream
   */
{
  masterWrite& MW=masterWrite::Instance();
  if (isActive())
    {
      const char abc[]="abc";
      const char typeLetter[]="rcs";
      OX<<"tmesh"<<std::endl;

      std::ostringstream cx;
      
      cx<<typeLetter[typeID-1]<<"mesh"<<IDnum;
      writeParticles(cx);
      cx<<keyWords<<" ";
      std::vector<double>::const_iterator vc;
      for(vc=kIndex.begin();vc!=kIndex.end();vc++)
	cx<<MW.Num(*vc)<<" ";
      
      StrFunc::writeMCNPX(cx.str(),OX);
      if (!getEnergy().empty())
	{
	  cx.str("");
	  cx<<"ergsh"<<IDnum<<" "<<getEnergy();
	  StrFunc::writeMCNPX(cx.str(),OX);
	}					 
      if (!mshmf.empty())
	{
	  cx.str("");
	  cx<<"mshmf"<<IDnum<<" "<<mshmf;
	  StrFunc::writeMCNPX(cx.str(),OX);
	}
      for(size_t i=0;i<3;i++)
	OX<<"cor"<<abc[i]<<IDnum<<" "<<minCoord[i]<<" "
	  <<(Pts[i]-1)<<"i "<<maxCoord[i]<<std::endl;
      OX<<"endmd"<<std::endl;
    }
  return;
}

}  // NAMESPACE tallySystem


