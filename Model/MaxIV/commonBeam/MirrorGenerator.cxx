/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/MirrorGenerator.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"

#include "CFFlanges.h"
#include "MirrorGenerator.h"

namespace setVariable
{

MirrorGenerator::MirrorGenerator() :
  radius(0.0),length(80.0),thick(1.0),width(5.0),
  baseTop(0.1),baseDepth(2.0),baseGap(0.5),baseOutWidth(1.0),
  mirrMat("Silicon300K"),baseMat("Copper")
  /*!
    Constructor and defaults
  */
{}

MirrorGenerator::MirrorGenerator(const MirrorGenerator& A) : 
  radius(A.radius),length(A.length),thick(A.thick),
  width(A.width),baseTop(A.baseTop),baseDepth(A.baseDepth),
  baseGap(A.baseGap),baseOutWidth(A.baseOutWidth),
  mirrMat(A.mirrMat),baseMat(A.baseMat)
  /*!
    Copy constructor
    \param A :: MirrorGenerator to copy
  */
{}

MirrorGenerator&
MirrorGenerator::operator=(const MirrorGenerator& A)
  /*!
    Assignment operator
    \param A :: MirrorGenerator to copy
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

MirrorGenerator::~MirrorGenerator() 
 /*!
   Destructor
 */
{}

void
MirrorGenerator::setPlate(const double L,const double T,
			  const double W)

  /*!
    Set the mirror plate
    \param L :: Length of mirror [Y]
    \param T :: Thickness [Z] of mirror plate
    \param W :: Widht [X]  mirror plate
   */
{
  length=L;
  thick=T;
  width=W;
  return;
}


void
MirrorGenerator::setSupport(const double top,const double depth,
			    const double gap,const double extra)
  /*!
    Set support sizes
    \param top :: Extra on top
    \param depth :: Full depth [> gap]
  */
{
  ELog::RegMethod RegA("MirrorGenerator","setSupport");

  if (gap>=depth)
    throw ColErr::OrderError<double>(gap,depth,"Depth must be > gap");
  baseTop=top;
  baseDepth=depth;
  baseGap=gap;
  baseOutWidth=extra;
  return;
}

void
MirrorGenerator::setRadius(const double R)

  /*!
    Set the surface radius
    \param R :: surface radius
   */
{
  radius=R;
  return;
}

void
MirrorGenerator::setMaterial(const std::string& MMat,
			     const std::string& BMat)
  /*!
    Set the materials
    \param MMat :: Mirror Material
    \param BMat :: Base Material
  */
{
  mirrMat=MMat;
  baseMat=BMat;
  return;
}

				  
void
MirrorGenerator::generateMirror(FuncDataBase& Control,
				const std::string& keyName,
				const double yStep,
				const double zStep,
				const double theta,
				const double phi,
				const double radius) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: Step along beam centre
    \param zStep :: Vertical lift from beamCentre
    \param theta :: theta angle
    \param phi :: phi angle
    \param radius ;: bending radius / focus distance
  */
{
  ELog::RegMethod RegA("MirrorGenerator","generatorMount");
  
  Control.addVariable(keyName+"YStep",yStep);
  Control.addVariable(keyName+"ZStep",zStep);
  Control.addVariable(keyName+"Theta",theta);
  Control.addVariable(keyName+"Phi",phi);

  Control.addVariable(keyName+"Radius",radius);
  Control.addVariable(keyName+"Thick",thick);
  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"Width",width);
  Control.addVariable(keyName+"Radius",radius);

  Control.addVariable(keyName+"BaseTop",baseTop);
  Control.addVariable(keyName+"BaseDepth",baseDepth);
  Control.addVariable(keyName+"BaseGap",baseGap);
  Control.addVariable(keyName+"BaseOutWidth",baseOutWidth);

  
  Control.addVariable(keyName+"MirrorMat",mirrMat);
  Control.addVariable(keyName+"BaseMat",baseMat);
       
  return;

}

  
}  // NAMESPACE setVariable
