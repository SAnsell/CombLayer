/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tally/cellFluxTally.cxx
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
#include <fstream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <iterator>
#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "NList.h"
#include "NRange.h"
#include "Tally.h"
#include "fissionTally.h"

namespace tallySystem
{ 

fissionTally::fissionTally(const int ID) :
  Tally(ID)
  /*!
    Constructor
    \param ID :: Identity number of tally
  */
{}

fissionTally::fissionTally(const fissionTally& A) :
  Tally(A),cellList(A.cellList),FSfield(A.FSfield)

  /*!
    Copy Constructor
    \param A :: fissionTally object to copy
  */
{ }

fissionTally*
fissionTally::clone() const
  /*!
    Clone object
    \return new (this)
  */
{
  return new fissionTally(*this);
}

fissionTally&
fissionTally::operator=(const fissionTally& A) 
  /*!
    Assignment operator =
    \param A :: fissionTally object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Tally::operator=(A);
      cellList=A.cellList;
      FSfield=A.FSfield;
    }
  return *this;
}

fissionTally::~fissionTally()
  /*!
    Destructor
  */
{}


std::vector<int>
fissionTally::getCells() const
  /*!
    Get a list of the cells
    \return Cell vector
  */
{
  return cellList.actualItems();
}

int
fissionTally::addLine(const std::string& LX)
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
      std::string Lpart=LX;
      Tally::processParticles(Lpart);        // pick up particles
      if (cellList.processString(Lpart))
	return -3;
      return 0;
    }

  if (FUnit.first=="fs")             /// Segment devisor
    return FSfield.processString(Line);

  if (FUnit.first=="sd")             /// Segment devisor
    return SDfield.processString(Line);

  return Tally::addLine(LX);
}

void
fissionTally::clearCells() 
  /*!
    Clears the cellList 
  */
{
  cellList.clear();
  return;
}

void
fissionTally::addCells(const std::vector<int>& AVec)
  /*!
    Add cells 
    \param AVec :: List of cells
  */
{
  cellList.addComp(AVec);
  return;
}

void
fissionTally::addIndividualCells(const std::vector<int>& AVec)
  /*!
    Add cells 
    \param AVec :: List of cells
  */
{
  cellList.addComp(AVec);
  return;
}

void
fissionTally::renumberCell(const int oldCell,const int newCell)
  /*!
    Renumbers a cell from the active list
    \param oldCell :: Old cell numb
    \param newCell :: New cell numb
  */
{
  cellList.changeItem(oldCell,newCell);
  return;
}

int
fissionTally::makeSingle()
  /*!
    Convert the cell flux into a set of single calculation
    \return 1 on success / 0 on failure [cell empty]
  */
{
  ELog::RegMethod RegA("fissionTally","makeSingle");
  const std::vector<int> cells=cellList.actualItems();
  std::vector<double> sd=SDfield.actualItems();
  cellList.clear();
  cellList.addUnits(cells);
  // Need to make sd cards etc plural
  if (!sd.empty() && sd.size()!=cells.size())
    {
      if (sd.size()>cells.size())
	sd.resize(cells.size());
      else 
	{
	  // Should be a fill algorithm for this
	  const double SDV=sd.back();
	  for(size_t i=sd.size();i<cells.size();i++)
	    sd.push_back(SDV);
	}
      SDfield.clear();
      SDfield.addUnits(sd);
    }
  
  return (!cells.empty()) ? 1 : 0;
}


void
fissionTally::write(std::ostream& OX)  const
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
      // PARTICLES ARE FORBIDDEN
      cx<<"f"<<IDnum;
      cx<<":n ";
      cx<<cellList;
      StrFunc::writeMCNPX(cx.str(),OX);
      cx.str("");
    }
  
  if (!FSfield.empty())
    {
      cx<<"fs"<<IDnum<<" "<<FSfield;
      StrFunc::writeMCNPX(cx.str(),OX);
      cx.str("");
    }

  writeFields(OX);
  return;
}

}  // NAMESPACE tallySystem
