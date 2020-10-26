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
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "writeSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "MeshXYZ.h"

#include "eType.h"
#include "aType.h"
#include "phitsTally.h"
#include "TCross.h"

namespace phitsSystem
{

TCross::TCross(const int ID) :
  phitsTally(ID),fluxFlag(0),
  energy(eType("Linear",1UL,0.0,5e3)),
  angle(aType("Linear",1UL,0.0,5e3))
  /*!
    Constructor
    \param ID :: Identity number of tally 
  */
{
  epsFlag=1;
}

TCross::TCross(const TCross& A) : 
  phitsTally(A),
  fluxFlag(A.fluxFlag),
  energy(A.energy),angle(A.angle)
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
      energy=A.energy;
      angle=A.angle;
      title=A.title;
      xTxt=A.xTxt;
      yTxt=A.yTxt;
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
TCross::write(std::ostream& OX) const
  /*!
    Write out the mesh tally into the tally region
    \param OX :: Output stream
   */
{
  ELog::RegMethod RegA("TCross","write");

  
  OX<<"[T-cross]\n";

  OX<<"  axis = eng \n";
  OX<<"  flux =" << ((fluxFlag) ? "flux" : "current")<<"\n";
  energy.write(OX);
  angle.write(OX);
  
  
  if (!title.empty())
    OX<<"  title = "<<title<<"\n";
  if (!xTxt.empty())  OX<<"  x-txt = "<<xTxt<<"\n";
  if (!yTxt.empty())  OX<<"  y-txt = "<<yTxt<<"\n";
  OX<<"  epsout = "<<epsFlag<<"\n";
  OX<<"  vtkout = "<<vtkFlag<<"\n";
  OX<<"  vtkfmt = "<<vtkFormat<<"\n";
  OX<<"  file = "<<keyName<<"\n";
  OX.flush();
  return;
}

}  // NAMESPACE phitsSystem

