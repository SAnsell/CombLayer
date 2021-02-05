/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   phitsTally/TCross.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include <set>
#include <map>
#include <iterator>
#include <array>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "phitsWriteSupport.h"

#include "eType.h"
#include "aType.h"
#include "phitsTally.h"
#include "TCross.h"

namespace phitsSystem
{

TCross::TCross(const int ID) :
  phitsTally("TCross",ID),fluxFlag(0),
  energy(eType("Linear",1UL,0.0,5e3)),
  angle(aType("Cos",1UL,0.0,1.0)),
  axis("eng"),unit(1),                  //1/MeV/cm^3
  regionA(0),regionB(0)
  /*!
    Constructor
    \param ID :: Identity number of tally 
  */
{
  epsFlag=1;
}

TCross::TCross(const TCross& A) : 
  phitsTally(A),
  fluxFlag(A.fluxFlag),energy(A.energy),angle(A.angle),
  axis(A.axis),unit(A.unit),regionA(A.regionA),
  regionB(A.regionB)
  /*!
    Copy constructor
    \param A :: TCross to copy
  */
{}

TCross&
TCross::operator=(const TCross& A)
  /*!
    Assignment operator
    \param A :: TCross to copy
    \return *this
  */
{
  if (this!=&A)
    {
      phitsTally::operator=(A);
      fluxFlag=A.fluxFlag;
      energy=A.energy;
      angle=A.angle;
      axis=A.axis;
      unit=A.unit;
      regionA=A.regionA;
      regionB=A.regionB;
    }
  return *this;
}
  
TCross*
TCross::clone() const
  /*!
    Clone object
    \return new (this)
  */
{
  return new TCross(*this);
}

TCross::~TCross()
  /*!
    Destructor
  */
{}

void
TCross::setUnit(const std::string& unitName)
  /*!
    Set units
    \param unitName :: string
   */
{
  static const std::map<std::string,int> uConv
    ({
      {"1/cm^2/MeV/",1}
    });
  std::map<std::string,int>::const_iterator mc=uConv.find(unitName);

  if (mc!=uConv.end())
    throw ColErr::InContainerError<std::string>
      (unitName,"unitName not known");

  unit=mc->second;
  
  return;
}

void
TCross::renumberCell(const int oldCell,const int newCell)
  /*!
    Renumber cells
\
  */
{
  if (regionA==oldCell)
    {
      regionA=newCell;
      comments+="regionA:"+std::to_string(oldCell)+" ";
    }
  if (regionB==oldCell)
    {
      regionB=newCell;
      comments+="regionB:"+std::to_string(oldCell)+" ";
    }
  return;
}
  
void
TCross::write(std::ostream& OX,
	      const std::string& fileHead) const
  /*!
    Write out the mesh tally into the tally region
    \param OX :: Output stream
    \param fileHead :: fileheader
   */
{
  ELog::RegMethod RegA("TCross","write");

  
  OX<<"[T-cross]\n";
  StrFunc::writePHITSComment(OX,1,comments);

  StrFunc::writePHITS(OX,1,"output",((fluxFlag) ? "flux" : "current"));
  StrFunc::writePHITS(OX,1,"part",particle);
  
  energy.write(OX);
  angle.write(OX);

  StrFunc::writePHITS(OX,1,"unit",unit);
  StrFunc::writePHITS(OX,1,"mesh","reg");

  StrFunc::writePHITS(OX,2,"reg",1);
  StrFunc::writePHITSTableHead
    (OX,2,{"non","r-in","r-out","area"});
  StrFunc::writePHITSTable(OX,2,1,regionA,regionB,1.0);
     
  if (!title.empty()) StrFunc::writePHITS(OX,1,"title",1);
  if (!xTxt.empty()) StrFunc::writePHITS(OX,1,"x-txt",xTxt);
  if (!yTxt.empty()) StrFunc::writePHITS(OX,1,"y-txt",yTxt);

  StrFunc::writePHITS(OX,1,"axis",axis);

  StrFunc::writePHITS(OX,1,"epsout",epsFlag);
  StrFunc::writePHITS(OX,1,"file",fileHead+keyName+".out");
  if (vtkout)
    {
      StrFunc::writePHITS(OX,1,"vtkout",vtkout);
      StrFunc::writePHITS(OX,1,"vtkfmt",vtkBinary);
    }
  OX.flush();
  return;
}

}  // NAMESPACE phitsSystem

