/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   tally/surfaceTally.cxx
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
#include <fstream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <string>
#include <sstream>
#include <list>
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <iterator>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Tally.h"
#include "surfaceTally.h"

namespace tallySystem
{

surfaceTally::surfaceTally(const int ID) :
  Tally(ID)
  /*!
    Constructor
    \param ID :: Identity number of tally
  */
{}

surfaceTally::surfaceTally(const surfaceTally& A) :
  Tally(A),SurfList(A.SurfList),FSfield(A.FSfield),
  SDfield(A.SDfield),CellFlag(A.CellFlag),
  SurfFlag(A.SurfFlag)
  /*!
    Copy Constructor
    \param A :: surfaceTally object to copy
  */
{ }

surfaceTally*
surfaceTally::clone() const
  /*!
    Clone object
    \return new (this)
  */
{
  return new surfaceTally(*this);
}

surfaceTally&
surfaceTally::operator=(const surfaceTally& A) 
  /*!
    Assignment operator =
    \param A :: surfaceTally object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Tally::operator=(A);
      SurfList=A.SurfList;
      FSfield=A.FSfield;
      SDfield=A.SDfield;
      CellFlag=A.CellFlag;
      SurfFlag=A.SurfFlag;
    }
  return *this;
}

surfaceTally::~surfaceTally()
  /*!
    Destructor
  */
{}

int
surfaceTally::addLine(const std::string& LX)
  /*!
    Adds a string, if this fails
    it return Tally::addLine.
    Currently it only searches for:
    - f  : Initial line (then surface list)
    \param LX :: Line to search
    \retval -1 :: ID not correct
    \retval -2 :: Type object not correct
    \retval -3 :: Line not correct
    \retval 0 on success
    \retval Tally::addline(Line) if nothing found
  */
{
  ELog::RegMethod RegA("surfaceTally","addLine");

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
      int SN;
      std::string Lpart=LX;
      Tally::processParticles(Lpart);        // pick up particles
      SurfList.clear();
      while(StrFunc::section(Lpart,SN))
        {
	  SurfList.push_back(SN);
	}
      return 0;
    }

  if (FUnit.first=="fs")             /// field seperator
    {
      int SN;	  
      FSfield.clear();
      while(StrFunc::section(Line,SN))
	FSfield.push_back(SN);
    }

  if (FUnit.first=="sd")             /// Segment divisor
    return SDfield.processString(Line);

  if (FUnit.first=="cf")             /// Segment divisor
    return CellFlag.processString(Line);

  if (FUnit.first=="sf")             /// Segment divisor
    return SurfFlag.processString(Line);
  
  return Tally::addLine(LX);
}

void
surfaceTally::setCellDivider(const std::vector<int>& CF)
  /*!
    Set the cell flags
    \param CF :: Cell flags
   */
{
  CellFlag.clear();
  CellFlag.addComp(CF);
  return;
}

void
surfaceTally::setCellFlag(const std::vector<int>& CF)
  /*!
    Set the cell flags
    \param CF :: Cell flags
   */
{
  CellFlag.clear();
  CellFlag.addComp(CF);
  return;
}

void
surfaceTally::setSurfFlag(const std::vector<int>& SF)
  /*!
    Set the surface flags
    \param SF :: Surface Flags
   */
{
  SurfFlag.clear();
  SurfFlag.addComp(SF);
  return;
}

void
surfaceTally::setSurfDivider(const std::vector<int>& TS)
  /*!
    Set the surface division 
    \param TS :: Surface numbers
   */
{
  FSfield=TS;
  return;
}

void
surfaceTally::renumberCell(const int oldN,const int newN)
  /*!
    Renumber the cell based on the old/New numbers
    \param oldN :: Old number to find
    \param newN :: new number to replace the cell with
   */
{
  ELog::RegMethod RegA("surfaceTally","renumberCell");
  CellFlag.changeItem(oldN,newN);
  return;
}


void
surfaceTally::renumberSurf(const int oldN,const int newN)
  /*!
    Renumber the cell based on the old/New numbers
    \param oldN :: Old number to find
    \param newN :: new number to replace the surface with
   */
{
  ELog::RegMethod RegA("surfaceTally","renumberCell");

  SurfFlag.changeItem(oldN,newN);
  std::vector<int>::iterator vc=
    std::find(SurfList.begin(),SurfList.end(),oldN);
  if (vc!=SurfList.end())
    *vc=newN;


  vc=std::find(FSfield.begin(),FSfield.end(),oldN);
  if (vc!=FSfield.end())
    *vc=newN;

  vc=std::find(FSfield.begin(),FSfield.end(),-oldN);
  if (vc!=FSfield.end())
    *vc= -newN;

  return;
}

void
surfaceTally::write(std::ostream& OX) const
  /*!
    Writes out the surface tally depending on the 
    fields that have been set.
    \param OX :: Output Stream
  */
{
  if (!isActive())
    return;
  
  std::stringstream cx;
  if (IDnum)  // maybe default 
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
      if (SurfList.empty())
	throw ColErr::ExBase(IDnum,
			     "SurfList Empty for tally : "
			     "Mcnpx requires a particle");
      copy(SurfList.begin(),SurfList.end(),
	   std::ostream_iterator<int>(cx," "));
      StrFunc::writeMCNPX(cx.str(),OX);
      cx.str("");
    }
  if (!FSfield.empty())
    {
      cx<<"fs"<<IDnum<<" ";
      std::copy(FSfield.begin(),FSfield.end(),
		std::ostream_iterator<int>(cx," "));
      StrFunc::writeMCNPX(cx.str(),OX);
    }
  if (!CellFlag.empty())
    {
      cx<<"cf"<<IDnum<<" "<<CellFlag;
      StrFunc::writeMCNPX(cx.str(),OX);
    }
  if (!SurfFlag.empty())
    {
      cx<<"sf"<<IDnum<<" "<<SurfFlag;
      StrFunc::writeMCNPX(cx.str(),OX);
    }
  if (!SDfield.empty())
    {
      cx<<"sd"<<IDnum<<" "<<SDfield;
      StrFunc::writeMCNPX(cx.str(),OX);
    }

  writeFields(OX);
  return;
}

} // NAMESPACE tallySystem
