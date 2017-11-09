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
#include "cellFluxTally.h"

namespace tallySystem
{ 

cellFluxTally::cellFluxTally(const int ID) :
  Tally(ID)
  /*!
    Constructor
    \param ID :: Identity number of tally
  */
{}

cellFluxTally::cellFluxTally(const cellFluxTally& A) :
  Tally(A),cellList(A.cellList),FSfield(A.FSfield)
  /*!
    Copy Constructore
    \param A :: cellFluxTally object to copy
  */
{ }

cellFluxTally*
cellFluxTally::clone() const
  /*!
    Clone object
    \return new (this)
  */
{
  return new cellFluxTally(*this);
}

cellFluxTally&
cellFluxTally::operator=(const cellFluxTally& A) 
  /*!
    Assignment operator =
    \param A :: cellFluxTally object to copy
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

cellFluxTally::~cellFluxTally()
  /*!
    Destructor
  */
{}


std::vector<int>
cellFluxTally::getCells() const
  /*!
    Get a list of the cells
    \return Cell vector
  */
{
  return cellList.actualItems();
}

int
cellFluxTally::addLine(const std::string& LX)
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

int
cellFluxTally::setSDField(const double V)
  /*!
    Sets a constant value for all FS fields.
    \param V :: Item to add
    \return 1 - success [only possible]
  */
{
  SDfield.clear();
  const int N=FSfield.count();
  for(int i=0;i<N || i<1;i++)
    SDfield.addComp(V);
  return 1;
}

  
void
cellFluxTally::clearCells() 
  /*!
    Clears the cellList 
  */
{
  cellList.clear();
  return;
}

void
cellFluxTally::addCells(const std::vector<int>& AVec)
  /*!
    Add cells 
    \param AVec :: List of cells
  */
{
  cellList.addComp(AVec);
  return;
}

void
cellFluxTally::addIndividualCells(const std::vector<int>& AVec)
  /*!
    Add cells 
    \param AVec :: List of cells
  */
{
  cellList.addComp(AVec);
  return;
}

void
cellFluxTally::renumberCell(const int oldCell,const int newCell)
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
cellFluxTally::mergeTally(const Tally& CT)
  /*!
    Join the cells of a tally to this tally
    \param CT :: Other tally to join
    \return 1 on success
  */
{
  ELog::RegMethod RegA("cellFluxTally","mergeTally");

  const cellFluxTally* CPtr=
    dynamic_cast<const cellFluxTally*>(&CT);
  if (!CPtr) return 0;
  
  //  const std::vector<int> cells=cellList.actualItems();
  //  const std::vector<int> otherCells=CPtr->actualItems();

  
  return 1;
}

  
int
cellFluxTally::makeSingle()
  /*!
    Convert the cell flux into a set of single calculation
    \return 1 on success / 0 on failure [cell empty]
  */
{
  ELog::RegMethod RegA("cellFluxTally","makeSingle");

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
cellFluxTally::writeHTape(const std::string& Name,
			  const std::string& tail) const
  /*!
    Write the HTapes : 
    Note this has to be called after 
    \param Name :: PrePart name
    \param tail :: Tail part of the output
  */
{
  boost::format FMT("%6d");
  boost::format FMT4("%4d");
  std::ofstream T08;
  std::ofstream T14;
  std::ofstream T15;

  const std::vector<int> cells=cellList.actualItems();

  T08.open((Name+"08"+tail).c_str());
  T14.open((Name+"14"+tail).c_str());
  T15.open((Name+"15"+tail).c_str());
  
  T08<<"HTAPE int deck: nuclide depletion for CINDER90 "<<std::endl;
  T14<<"HTAPE int deck: nuclide depletion for CINDER90 "<<std::endl;
  T15<<"HTAPE int deck: nuclide depletion for CINDER90 "<<std::endl;

  T08<<"nuclide depletion: tally:"<<IDnum<<" "<<getComment()<<std::endl;
  T14<<"nuclide depletion: tally:"<<IDnum<<" "<<getComment()<<std::endl;
  T15<<"nuclide depletion: tally:"<<IDnum<<" "<<getComment()<<std::endl;

  T08<<"108,,,,,"<<(FMT4 % cells.size())<<",,,,1/"<<std::endl;
  T14<<"114,,,,,"<<(FMT4 % cells.size())<<",,,,1/"<<std::endl;
  T15<<"115,,,,,"<<(FMT4 % cells.size())<<",,,,1/"<<std::endl;


  std::vector<int>::const_iterator vc;
  for(vc=cells.begin();vc!=cells.end();vc++)
    {
      T08<<(FMT % *vc);
      T14<<(FMT % *vc);      
      T15<<(FMT % *vc);
    }
  T08<<"/"<<std::endl;
  T14<<"/"<<std::endl;
  T15<<"/"<<std::endl;
  return;
}

void
cellFluxTally::write(std::ostream& OX)  const
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
      cx<<"f"<<IDnum;
      if (!Particles.empty())
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
