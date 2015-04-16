/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   tally/heatTally.cxx
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
#include <sstream>
#include <cmath>
#include <string>
#include <list>
#include <map>
#include <vector>
#include <iterator>

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
#include "Triple.h"
#include "NList.h"
#include "NRange.h"
#include "Tally.h"
#include "heatTally.h"

namespace tallySystem
{

heatTally::heatTally(const int ID) :
  Tally(ID),plus(0)
  /*!
    Constructor
    \param ID :: Identity number of tally
  */
{}

heatTally::heatTally(const heatTally& A) :
  Tally(A),plus(A.plus),cellList(A.cellList)
  /*!
    Copy Constructore
    \param A :: heatTally object to copy
  */
{ }

heatTally*
heatTally::clone() const
  /*!
    Clone object
    \return new (this)
  */
{
  return new heatTally(*this);
}

heatTally&
heatTally::operator=(const heatTally& A) 
  /*!
    Assignment operator =
    \param A :: heatTally object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Tally::operator=(A);
      plus=A.plus;
      cellList=A.cellList;
    }
  return *this;
}

heatTally::~heatTally()
  /*!
    Destructor
  */
{}

int
heatTally::addLine(const std::string& LX)
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
  return Tally::addLine(LX);
}

void
heatTally::clearCells() 
  /*!
    Clears the cellList 
  */
{
  cellList.clear();
  return;
}

void
heatTally::addCells(const std::vector<int>& AVec)
  /*!
    Adds a list of individual cells
    \param AVec :: Cells to add
  */
{
  std::vector<int>::const_iterator vc;
  for(vc=AVec.begin();vc!=AVec.end();vc++)
    cellList.addComp(*vc);
  return;
}

void
heatTally::renumberCell(const int oldN,const int newN)
  /*!
    renumber the cell based on the old/New numbers
    \param oldN :: Old number to find
    \param newN :: new number to replace the cell with
   */
{
  ELog::RegMethod RegA("heatTally","renumberCell");
  cellList.changeItem(oldN,newN);
  return;
}

void
heatTally::write(std::ostream& OX)  const
  /*!
    Writes out the flux tally depending on the 
    fields that have been set.
    \param OX :: Output Stream
  */
{
   if (!isActive())
    return;

  std::stringstream cx;
  if (IDnum)                   // maybe default 
    {
      if (plus)
	cx<<"+f"<<IDnum;
      else
	cx<<"f"<<IDnum;
      if (!plus && !Particles.empty())
        {
	  cx<<":";
	  copy(Particles.begin(),Particles.end()-1,
	       std::ostream_iterator<std::string>(cx,","));
	  cx<<Particles.back();
	}
      cx<<" ";
      cx<<cellList;
      StrFunc::writeMCNPX(cx.str(),OX);
      cx.str("");
    }
  
  writeFields(OX);
  return;
}

}  // NAMESPACE tallySystem

