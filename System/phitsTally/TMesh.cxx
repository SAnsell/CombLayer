/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   phitsTally/TMesh.cxx
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
#include "Vec3D.h"
#include "MeshXYZ.h"
#include "phitsWriteSupport.h"

#include "eType.h"
#include "phitsTally.h"
#include "TMesh.h"

namespace phitsSystem
{

TMesh::TMesh(const int ID) :
  phitsTally("TMesh",ID),
  energy(eType("Linear",1UL,0.0,5e3)),
  axis("xy"),unit(1)                  //1/MeV/cm^3
  /*!
    Constructor
    \param ID :: Identity number of tally 
  */
{
  epsFlag=1;
}

TMesh::TMesh(const TMesh& A) : 
  phitsTally(A),
  gridXYZ(A.gridXYZ),energy(A.energy),
  unit(A.unit)
  /*!
    Copy constructor
    \param A :: TMesh to copy
  */
{}

TMesh&
TMesh::operator=(const TMesh& A)
  /*!
    Assignment operator
    \param A :: TMesh to copy
    \return *this
  */
{
  if (this!=&A)
    {
      phitsTally::operator=(A);
      gridXYZ=A.gridXYZ;
      energy=A.energy;
      axis=A.axis;
      unit=A.unit;
    }
  return *this;
}
  
TMesh*
TMesh::clone() const
  /*!
    Clone object
    \return new (this)
  */
{
  return new TMesh(*this);
}

TMesh::~TMesh()
  /*!
    Destructor
  */
{}

void
TMesh::setUnit(const std::string& unitName)
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
TMesh::write(std::ostream& OX,const std::string& fileHead) const
  /*!
    Write out the mesh tally into the tally region
    \param OX :: Output stream
   */
{
  ELog::RegMethod RegA("TMesh","write");

  OX<<"[T-Track]\n";

  StrFunc::writePHITS(OX,1,"part",particle);
  gridXYZ.write(OX);
  gridXYZ.writeAxis(OX,1);
  energy.write(OX);

  StrFunc::writePHITS(OX,1,"file",fileHead+keyName+".out");
  StrFunc::writePHITS(OX,1,"gshow",1);  // lines
  if (vtkout)
    {
      StrFunc::writePHITS(OX,1,"vtkout",vtkout);
      StrFunc::writePHITS(OX,1,"vtkfmt",vtkBinary);
    }

  StrFunc::writePHITS(OX,1,"epsout",epsFlag);

  if (vtkout)
    {
      StrFunc::writePHITS(OX,1,"vtkout",vtkout);
      StrFunc::writePHITS(OX,1,"vtkfmt",vtkBinary);
    }
	     
  OX.flush();
  return;
}

}  // NAMESPACE phitsSystem

