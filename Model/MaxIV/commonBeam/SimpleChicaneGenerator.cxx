/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamVar/SimpleChicaneGenerator.cxx
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"

#include "CFFlanges.h"
#include "SimpleChicaneGenerator.h"

namespace setVariable
{

SimpleChicaneGenerator::SimpleChicaneGenerator() :
  width(50.0),height(20.0),clearGap(8.0),upStep(10.0),
  plateThick(0.2),barThick(4.0),plateMat("Stainless304")
  /*!
    Constructor and defaults
  */
{}


SimpleChicaneGenerator::SimpleChicaneGenerator
(const SimpleChicaneGenerator& A) : 
  width(A.width),height(A.height),clearGap(A.clearGap),
  upStep(A.upStep),plateThick(A.plateThick),barThick(A.barThick),
  plateMat(A.plateMat)
  /*!
    Copy constructor
    \param A :: SimpleChicaneGenerator to copy
  */
{}

SimpleChicaneGenerator&
SimpleChicaneGenerator::operator=(const SimpleChicaneGenerator& A)
  /*!
    Assignment operator
    \param A :: SimpleChicaneGenerator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      width=A.width;
      height=A.height;
      clearGap=A.clearGap;
      upStep=A.upStep;
      plateThick=A.plateThick;
      barThick=A.barThick;
      plateMat=A.plateMat;
    }
  return *this;
}
  
  
SimpleChicaneGenerator::~SimpleChicaneGenerator() 
 /*!
   Destructor
 */
{}

void
SimpleChicaneGenerator::setSize(const double G,
				const double W,
				const double H)
  /*!
    Set length/width/height
    \param G :: Gap 
    \param W :: width 
    \param H :: height
   */
{
  clearGap=G;
  width=W;
  height=H;    
  return;
}


void
SimpleChicaneGenerator::setWall(const double WT,const std::string& WMat)
/*!
    Set wall paramaters
    \param WT :: Wall thickness
    \param WMat :: Wall material
   */
{
  plateThick=WT;
  plateMat=WMat;
  return;
}
				  
void
SimpleChicaneGenerator::generateSimpleChicane(FuncDataBase& Control,
					  const std::string& keyName,
					  const double xStep,
					  const double zStep) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param xStep :: Step left/right fixed centre point
    \param zStep :: Step up/down fixed centre point
  */
{
  ELog::RegMethod RegA("SimpleChicaneGenerator","generateSimpleChicane");
  
  Control.addVariable(keyName+"XStep",xStep);
  Control.addVariable(keyName+"YStep",0.0);
  Control.addVariable(keyName+"ZStep",zStep);

  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"Width",width);
  Control.addVariable(keyName+"ClearGap",clearGap);
  Control.addVariable(keyName+"UpStep",upStep);
  
  Control.addVariable(keyName+"PlateThick",plateThick);
  
  Control.addVariable(keyName+"PlateMat",plateMat);
  Control.addVariable(keyName+"InnerXWidth",barThick);
  Control.addVariable(keyName+"InnerXDepth",barThick);

       
  return;

}

  
}  // NAMESPACE setVariable
