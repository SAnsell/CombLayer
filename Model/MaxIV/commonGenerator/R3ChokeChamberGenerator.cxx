/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/R3ChokeChamberGenerator.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <stack>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <numeric>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"

#include "CFFlanges.h"
#include "R3ChokeChamberGenerator.h"

namespace setVariable
{

R3ChokeChamberGenerator::R3ChokeChamberGenerator() :
  radius(0.0),length(80.0),thick(1.0),width(5.0),
  baseTop(0.1),baseDepth(2.0),baseGap(0.5),baseOutWidth(1.0),
  mirrMat("Silicon300K"),baseMat("Copper")
  /*!
    Constructor and defaults
  */
{}

R3ChokeChamberGenerator::R3ChokeChamberGenerator(const R3ChokeChamberGenerator& A) : 
  radius(A.radius),length(A.length),thick(A.thick),
  width(A.width),baseTop(A.baseTop),baseDepth(A.baseDepth),
  baseGap(A.baseGap),baseOutWidth(A.baseOutWidth),
  mirrMat(A.mirrMat),baseMat(A.baseMat)
  /*!
    Copy constructor
    \param A :: R3ChokeChamberGenerator to copy
  */
{}

R3ChokeChamberGenerator&
R3ChokeChamberGenerator::operator=(const R3ChokeChamberGenerator& A)
  /*!
    Assignment operator
    \param A :: R3ChokeChamberGenerator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      radius=A.radius;
      length=A.length;
      thick=A.thick;
      width=A.width;
      baseTop=A.baseTop;
      baseDepth=A.baseDepth;
      baseGap=A.baseGap;
      baseOutWidth=A.baseOutWidth;
      mirrMat=A.mirrMat;
      baseMat=A.baseMat;
    }
  return *this;
}

R3ChokeChamberGenerator::~R3ChokeChamberGenerator() 
 /*!
   Destructor
 */
{}

void
R3ChokeChamberGenerator::setPlate(const double L,const double T,
			  const double W)

  /*!
    Set the mirror plate
    \param L :: Length of mirror [Y]
    \param T :: Thickness [Z] of mirror plate
    \param W :: Width [X]  mirror plate
   */
{
  length=L;
  thick=T;
  width=W;
  return;
}


void
R3ChokeChamberGenerator::setSupport(const double top,const double depth,
			    const double gap,const double extra)
  /*!
    Set support sizes
    \param top :: Extra on top
    \param depth :: Full depth [> gap]
  */
{
  ELog::RegMethod RegA("R3ChokeChamberGenerator","setSupport");

  if (gap>=depth)
    throw ColErr::OrderError<double>(gap,depth,"Depth must be > gap");
  baseTop=top;
  baseDepth=depth;
  baseGap=gap;
  baseOutWidth=extra;
  return;
}

void
R3ChokeChamberGenerator::setRadius(const double R)

  /*!
    Set the surface radius
    \param R :: surface radius
   */
{
  radius=R;
  return;
}

void
R3ChokeChamberGenerator::setMaterial(const std::string& MMat,
				     const std::string& BMat)
  /*!
    Set the materials
    \param MMat :: R3ChokeChamber Material
    \param BMat :: Base Material
  */
{
  mirrMat=MMat;
  baseMat=BMat;
  return;
}

				  
void
R3ChokeChamberGenerator::generateChamber(FuncDataBase& Control,
					 const std::string& keyName)
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
  */
{
  ELog::RegMethod RegA("R3ChokeChamberGenerator","generateChamber");
  
       
  return;

}

  
}  // NAMESPACE setVariable
