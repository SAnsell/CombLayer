/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   phitsTally/TGShow.cxx
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "phitsWriteSupport.h"
#include "Vec3D.h"
#include "MeshXYZ.h"

#include "phitsTally.h"
#include "TGShow.h"

namespace phitsSystem
{

TGShow::TGShow(const int ID) :
  phitsTally("TGShow",ID),output(6),
  lineWidth(0.5)
  /*!
    Constructor
    \param ID :: Identity number of tally 
  */
{
  epsFlag=1;
}

TGShow::TGShow(const TGShow& A) : 
  phitsTally(A),xyzGrid(A.xyzGrid),
  output(A.output),
  lineWidth(A.lineWidth),
  title(A.title),xTxt(A.xTxt),yTxt(A.yTxt)
  /*!
    Copy constructor
    \param A :: TGShow to copy
  */
{}

TGShow&
TGShow::operator=(const TGShow& A)
  /*!
    Assignment operator
    \param A :: TGShow to copy
    \return *this
  */
{
  if (this!=&A)
    {
      phitsTally::operator=(A);
      xyzGrid=A.xyzGrid;
      output=A.output;
      lineWidth=A.lineWidth;
      title=A.title;
      xTxt=A.xTxt;
      yTxt=A.yTxt;
    }
  return *this;
}

  
TGShow*
TGShow::clone() const
  /*!
    Clone object
    \return new (this)
  */
{
  return new TGShow(*this);
}

TGShow::~TGShow()
  /*!
    Destructor
  */
{}

void
TGShow::setIndex(const std::array<size_t,3>& A)
  /*!
    Sets the size but work should be done here to
    make one coordinate size 1 etc.
    \param A :: Array of X,Y,Z size
   */
{
  ELog::RegMethod RegA("TGShow","setIndex");

  xyzGrid.setSize(A[0],A[1],A[2]);
  return;
}

void
TGShow::setOutputStyle(const bool materialName,
		       const bool regionName)
  /*!
    Set the output name
    \param materialName :: Write material name
    \param regionName :: Write region name
   */
{
  if (regionName)
    output=6;
  else
    {
      output= (materialName) ? 4 : 2;
    }
  return;
}
    
  
void
TGShow::setCoordinates(const Geometry::Vec3D& A,const Geometry::Vec3D& B)
  /*!
    Sets the coordinates but work should be done here to
    make one coordinate size 1 etc.
    \param A :: Array of X,Y,Z size
   */
{
  ELog::RegMethod RegA("TGShow","setCoordinates");
  xyzGrid.setCoordinates(A,B);
  return;
}


void
TGShow::write(std::ostream& OX,const std::string& fileHead) const
  /*!
    Write out the mesh tally into the tally region
    \param OX :: Output stream
   */
{
  ELog::RegMethod RegA("TGShow","write");
  
  OX<<"[T-gshow]\n";
  xyzGrid.write(OX);
  xyzGrid.writeAxis(OX,1);
  if (!title.empty())
    StrFunc::writePHITS(OX,1,"title",title);
  if (!xTxt.empty()) StrFunc::writePHITS(OX,1,"x-txt",xTxt);
  if (!yTxt.empty()) StrFunc::writePHITS(OX,1,"y-txt",yTxt);
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

