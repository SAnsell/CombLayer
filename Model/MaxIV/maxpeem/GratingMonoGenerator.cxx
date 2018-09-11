/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   maxpeem/GratingMonoGenerator.cxx
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

#include "GratingMonoGenerator.h"

namespace setVariable
{

GratingMonoGenerator::GratingMonoGenerator() :
  rotCent(0,63.0,12.0),theta(0.0),
  mOffset(0,0,0),
  mWidth(11.0),mThick(4.0),mLength(64.0),
  gOffset(0,42.0,3.2),
  gWidth(4.0),gThick(2.0),gLength(12.0),
  mBaseThick(0.6),mBaseWidth(6.0),mBaseLength(6.18),
  gBaseThick(0.5),gBaseWidth(2.0),gBaseLength(11.8),
  xtalMat("Silicon300K"),baseMat("Copper")
 /*!
    Constructor and defaults
  */
{}

GratingMonoGenerator::GratingMonoGenerator(const GratingMonoGenerator& A) : 
  rotCent(A.rotCent),theta(A.theta),mOffset(A.mOffset),
  mWidth(A.mWidth),mThick(A.mThick),mLength(A.mLength),
  gOffset(A.gOffset),gWidth(A.gWidth),gThick(A.gThick),
  gLength(A.gLength),mBaseThick(A.mBaseThick),mBaseWidth(A.mBaseWidth),
  mBaseLength(A.mBaseLength),gBaseThick(A.gBaseThick),
  gBaseWidth(A.gBaseWidth),gBaseLength(A.gBaseLength),
  xtalMat(A.xtalMat),baseMat(A.baseMat)
  /*!
    Copy constructor
    \param A :: GratingMonoGenerator to copy
  */
{}

GratingMonoGenerator&
GratingMonoGenerator::operator=(const GratingMonoGenerator& A)
  /*!
    Assignment operator
    \param A :: GratingMonoGenerator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      rotCent=A.rotCent;
      theta=A.theta;
      mOffset=A.mOffset;
      mWidth=A.mWidth;
      mThick=A.mThick;
      mLength=A.mLength;
      gOffset=A.gOffset;
      gWidth=A.gWidth;
      gThick=A.gThick;
      gLength=A.gLength;
      mBaseThick=A.mBaseThick;
      mBaseWidth=A.mBaseWidth;
      mBaseLength=A.mBaseLength;
      gBaseThick=A.gBaseThick;
      gBaseWidth=A.gBaseWidth;
      gBaseLength=A.gBaseLength;
      xtalMat=A.xtalMat;
      baseMat=A.baseMat;
    }
  return *this;
}
  
GratingMonoGenerator::~GratingMonoGenerator() 
 /*!
   Destructor
 */
{}
				  
void
GratingMonoGenerator::generateGrating(FuncDataBase& Control,
				      const std::string& keyName,
				      const double yStep,
				      const double zStep) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: Step along beam centre
    \param zStep :: Step above beam
  */
{
  ELog::RegMethod RegA("GratingMonoGenerator","generateBox");
  
  Control.addVariable(keyName+"YStep",yStep);
  Control.addVariable(keyName+"ZStep",zStep);

  Control.addVariable(keyName+"RotCentre",rotCent);
  Control.addVariable(keyName+"Theta",theta);


  
  Control.addVariable(keyName+"MirrorOffset",mOffset);
  Control.addVariable(keyName+"MirrorWidth",mWidth);
  Control.addVariable(keyName+"MirrorThick",mThick);
  Control.addVariable(keyName+"MirrorLength",mLength);

  Control.addVariable(keyName+"GrateOffset",gOffset);
  Control.addVariable(keyName+"GrateWidth",gWidth);
  Control.addVariable(keyName+"GrateThick",gThick);
  Control.addVariable(keyName+"GrateLength",gLength);

  Control.addVariable(keyName+"MirrorBaseWidth",mBaseWidth);
  Control.addVariable(keyName+"MirrorBaseThick",mBaseThick);
  Control.addVariable(keyName+"MirrorBaseLength",mBaseLength);
  
  Control.addVariable(keyName+"GrateBaseWidth",gBaseWidth);
  Control.addVariable(keyName+"GrateBaseThick",gBaseThick);
  Control.addVariable(keyName+"GrateBaseLength",gBaseLength);


  Control.addVariable(keyName+"XtalMat",xtalMat);
  Control.addVariable(keyName+"BaseMat",baseMat);
       
  return;

}

  
}  // NAMESPACE setVariable
