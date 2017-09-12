/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tally/tmeshTally.cxx
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
#include "tmeshTally.h"

namespace tallySystem
{

tmeshTally::tmeshTally(const int ID) :
  Tally(ID),typeID(1),keyWords(""),
  activeMSHMF(0),requireRotation(0)
  /*!
    Constructor
    \param ID :: Identity number of tally
  */
{}

tmeshTally::tmeshTally(const tmeshTally& A) : 
  Tally(A),
  typeID(A.typeID),keyWords(A.keyWords),kIndex(A.kIndex),
  activeMSHMF(A.activeMSHMF),mshmf(A.mshmf),
  requireRotation(A.requireRotation),
  Pts(A.Pts),minCoord(A.minCoord),maxCoord(A.maxCoord)
  /*!
    Copy constructor
    \param A :: tmeshTally to copy
  */
{}

tmeshTally&
tmeshTally::operator=(const tmeshTally& A)
  /*!
    Assignment operator
    \param A :: tmeshTally to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Tally::operator=(A);
      typeID=A.typeID;
      keyWords=A.keyWords;
      kIndex=A.kIndex;
      activeMSHMF=A.activeMSHMF;
      mshmf=A.mshmf;
      requireRotation=A.requireRotation;
      Pts=A.Pts;
      minCoord=A.minCoord;
      maxCoord=A.maxCoord;
    }
  return *this;
}

tmeshTally*
tmeshTally::clone() const
  /*!
    Clone object
    \return new (this)
  */
{
  return new tmeshTally(*this);
}

tmeshTally::~tmeshTally()
  /*!
    Destructor
  */
{}

void
tmeshTally::setType(const int T)
  /*!
    Set the mesh typeID values
    \param T :: Type to set [1,2,3]
  */
{
  if (T<1 || T>3)
    throw ColErr::RangeError<int>(T,1,4,"tmeshTally::setType");
  typeID=T;
  return;
}

void
tmeshTally::setKeyWords(const std::string& K)
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
tmeshTally::setIndexLine(std::string K)
  /*!
    Set the mesh index items [?]
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
tmeshTally::setIndex(const std::array<size_t,3>& IDX)
  /*!
    Sets the individual index for each x,y,z
    \param IDX :: array of three object
  */
{
  ELog::RegMethod RegA("tmeshTally","setIndex");

  for(size_t i=0;i<3;i++)
    {
      if (!IDX[i])
	throw ColErr::IndexError<size_t>(IDX[i],i,"IDX[index] zero");

      Pts[i]=IDX[i];
    }
  return;
}

void
tmeshTally::setCoordinates(const Geometry::Vec3D& A,
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
tmeshTally::setResponse(const std::string& Line)
  /*!
    Set the mesh response function based on the input line
    \param Line :: Line to process
  */
{
  ELog::RegMethod RegA("tmeshTally","setResponse");
  if (mshmf.processString(Line))
    {
      ELog::EM<<"Failed to set response line :"<<ELog::endCrit;
      throw ColErr::InvalidLine("mshmf reponse: ",Line,0);
    }
  activeMSHMF=1;
  return;
}

int
tmeshTally::addLine(const std::string& LX)
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
tmeshTally::rotateMaster()
  /*!
    Rotate the points [if required]
  */
{
  ELog::RegMethod RegA("tmeshTally","rotateMaster");
  
  if (requireRotation)
    {
      const masterRotate& MR=masterRotate::Instance(); 
      MR.applyFull(minCoord);
      MR.applyFull(maxCoord);
      requireRotation=0;
    }
  return;
}

void
tmeshTally::writeCoordinates(std::ostream& OX) const
  /*!
    Function to write out the coordinates for the user
    \param OX :: Oupt stream
  */
{
  static char abc[]="abc";
  
  for(size_t i=0;i<3;i++)
    OX<<"cor"<<abc[i]<<IDnum<<" "<<minCoord[i]<<" "
      <<(Pts[i]-1)<<"i "<<maxCoord[i]<<std::endl;
  return;
}
  
void
tmeshTally::write(std::ostream& OX) const
  /*!
    Write out the mesh tally into the tally region
    \param OX :: Output stream
  */
{
  masterWrite& MW=masterWrite::Instance();

  if (isActive())
    {
      const char typeLetter[]="rcs";

      std::ostringstream cx;
      
      cx<<typeLetter[typeID-1]<<"mesh"<<IDnum;
      writeParticles(cx);
      if (!activeMSHMF)
	{
	  // THIS IS JUNK as maybe two keywords and two value sets
	  cx<<keyWords<<" ";
	  for(const double V : kIndex)
	    cx<<MW.Num(V)<<" ";
	}
      else
	cx<<"DOSE "<<std::abs(activeMSHMF);

      StrFunc::writeMCNPX(cx.str(),OX);
      if (!getEnergy().empty())
	{
	  cx.str("");
	  cx<<"ergsh"<<IDnum<<" "<<getEnergy();
	  StrFunc::writeMCNPX(cx.str(),OX);
	}					 
      if (!mshmf.empty() && activeMSHMF>0)
	{
	  cx.str("");
	  cx<<"mshmf"<<activeMSHMF<<" "<<mshmf;
	  StrFunc::writeMCNPX(cx.str(),OX);
	}
      writeCoordinates(OX);
    }
  return;
}

}  // NAMESPACE tallySystem


