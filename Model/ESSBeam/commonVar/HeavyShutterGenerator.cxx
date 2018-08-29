/****************************************************************************
  CombLayer : MCNP(X) Input builder
 
 * File:   commonVar/HeavyShutterGenerator.cxx
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>. 
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
#include "HeavyShutterGenerator.h"

namespace setVariable
{

HeavyShutterGenerator::HeavyShutterGenerator() :
  width(140.0),height(140.0),
  wallThick(5.0),guideLodging(10.0),separatorThick(2.0),slabThick({15.0,15.0,15.0,30.0,15.0}),
  mainMat("Aluminium"),slabMat({"Tungsten","Tungsten","Poly","Aluminium","Lead"})
  /*!
    Constructor and defaults
  */
{}

HeavyShutterGenerator::HeavyShutterGenerator(const HeavyShutterGenerator& A) : 
  width(A.width),height(A.height),
  wallThick(A.wallThick),separatorThick(A.separatorThick),slabThick(A.slabThick),
  mainMat(A.mainMat),slabMat(A.slabMat)
  /*!
    Copy constructor
    \param A :: HeavyShutterGenerator to copy
  */
{}

HeavyShutterGenerator&
HeavyShutterGenerator::operator=(const HeavyShutterGenerator& A)
  /*!
    Assignment operator
    \param A :: HeavyShutterGenerator to copy
    \return *this
  */
{
  if (this!=&A)
  {
    width=A.width,
    height=A.height,
    wallThick=A.wallThick;
    separatorThick=A.separatorThick;
    slabThick=A.slabThick;
    mainMat=A.mainMat;
    slabMat=A.slabMat;
  }
  return *this;
}

HeavyShutterGenerator::~HeavyShutterGenerator() 
 /*!
   Destructor
 */
{}

void
HeavyShutterGenerator::setWallThick(const double T)
  /*!
    Set external wall thickness
    \param T :: Wall thickness
   */
{
  wallThick=T;
  return;
}

void
HeavyShutterGenerator::setGuideLodging(const double T)
  /*!
    Set guide housing vertical space
    \param T :: Guide vertical space
   */
{
  guideLodging=T;
  return;
}

void
HeavyShutterGenerator::setSeparatorThick(const double T)
  /*!
    Set internal wall thickness
    \param T :: Internal wall thickness
   */
{
  separatorThick=T;
  return;
}

void
HeavyShutterGenerator::resetSlabThick()
  /*!
    Reset all the slab thicknesses
  */
{
  slabThick.clear();
  return;
}

void
HeavyShutterGenerator::resetSlabMat()
  /*!
    Reset all the slab materials
  */
{
  slabMat.clear();
  return;
}

void
HeavyShutterGenerator::setSlabThick(const std::vector<double>& T)
  /*!
    Set slabs thicknesses
    \param T :: Thickness vector
   */
{
  ELog::RegMethod RegA("HeavyShutterGenerator","setSlabThick");

  resetSlabThick();

  for(double s : T)
  {
    if (s<Geometry::zeroTol)
      throw ColErr::NumericalAbort("Slab thickness is too small: "+StrFunc::makeString(s));
    else
      slabThick.push_back(s);
  }

  return;
}

void
HeavyShutterGenerator::setMainMaterial(const std::string& M)
  /*!
    Set both external and internal wall material
    \param M :: Main material
  */
{
  mainMat=M;
  return;
}

void
HeavyShutterGenerator::setSlabMaterial(const std::vector<std::string>& M)
  /*!
    Set slab materials
    \param M :: Material vector [no checking]
   */
{
  ELog::RegMethod RegA("HeavyShutterGenerator","setSlabMat");
  resetSlabMat();
  
  for(std::string s : M)
    slabMat.push_back(s);
    
  return;
}

double
HeavyShutterGenerator::getLength()
  /*!
    Get total length/thickness
   */
{
  ELog::RegMethod RegA("HeavyShutterGenerator","getLength");

  double L=std::accumulate(slabThick.begin(),slabThick.end(),0.0);
  L+=separatorThick*static_cast<double>(slabThick.size()-1);
  L+=wallThick*2;
  
  return L;
}

void
HeavyShutterGenerator::generateHeavyShutter(FuncDataBase& Control,
                                            const std::string& keyName,
                                            const double yStep,
                                            const double width,
                                            const double height) const
  /*!
    Primary function for setting the variables
    \param Control :: Database to add variables
    \param keyName :: Head name for variable
    \param yStep :: y-offset
    \param width :: Width
    \param height :: Height
  */
{
  ELog::RegMethod RegA("HeavyShutterGenerator","generatorHeavyShutter");
  
  Control.addVariable(keyName+"XStep",0.0);
  Control.addVariable(keyName+"YStep",yStep);
  Control.addVariable(keyName+"ZStep",0.0);
  Control.addVariable(keyName+"XYangle",0.0);
  Control.addVariable(keyName+"Zangle",0.0);
  
  Control.addVariable(keyName+"Width",width);
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"SeparatorThick",separatorThick);
  Control.addVariable(keyName+"WallThick",wallThick);
  Control.addVariable(keyName+"GuideLodging",guideLodging);
  
  Control.addVariable(keyName+"NSlabs",slabThick.size());
  
  for(size_t i=0;i<slabThick.size();i++)
  {
    Control.addVariable(keyName+"SlabThick"+std::to_string(i),slabThick[i]);
    Control.addVariable(keyName+"SlabMat"+std::to_string(i),slabMat[i]);
  }
  
  Control.addVariable(keyName+"MainMat",mainMat);
  
  // Checks:
  if (slabThick.size()!=slabMat.size())
    ELog::EM<<"Failure: slabThick size ("<<slabThick.size()<<") != slabMat size ("<<slabMat.size()<<")"<<ELog::endErr;
  
  return;
}

}  // NAMESPACE setVariable
