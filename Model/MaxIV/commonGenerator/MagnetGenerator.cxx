 /*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/MagnetGenerator.cxx
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"

#include "MagnetGenerator.h"

namespace setVariable
{

MagnetGenerator::MagnetGenerator() :
  preName("L2SPF"),
  length(20.0),width(10.0),height(5.0),
  KFactor({0,0,0,0})
  /*!
    Constructor and defaults
  */
{}

MagnetGenerator::~MagnetGenerator()
 /*!
   Destructor
 */
{}

void
MagnetGenerator::setSize(const double L,const double W,
			 const double H)
  /*!
    Simple setter 
    \param L :: length
    \param W :: width (full)
    \param H :: height (full)
   */
{
  length=L;
  width=W;
  height=H;
  return;
}

void
MagnetGenerator::setOffset(const Geometry::Vec3D& deltaXYZ)

  /*!
    Set the onetime offset
    \param deltaXYZ :: step in x,x,z
  */
{
  xyzStep=deltaXYZ;
  return;
}

void
MagnetGenerator::setOffset(const double xStep,const double yStep,
			   const double zStep)
  /*!
    Set the onetime offset
    \param xStep :: step in x
    \param yStep :: step in y
    \param zStep :: step in z
  */
{
  xyzStep=Geometry::Vec3D(xStep,yStep,zStep);
  return;
}

void
MagnetGenerator::setField(const double K0,const double K1,
			  const double K2,const double K3)
  /*!
    Simple setter 
    \param K0 :: Dipole monment
    \param K1 :: Quadrupole monment
    \param K2 :: Sexapole monment
    \param K3 :: Octopole monment
   */
{
  KFactor[0]=K0;
  KFactor[1]=K1;
  KFactor[2]=K2;
  KFactor[3]=K3;
  return;
}

void
MagnetGenerator::generateCorMag(FuncDataBase& Control,
				const size_t segNumber,
				const std::string& fcUnit,
				const double yAngle,
				const double QField) 
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: Head name for variable
    \param fcUnit :: Part name of FC magnet
    \param yAngle :: Angle
  */
{
  ELog::RegMethod RegA("MagnetGenerator","generateCorMag");
  
  setSize(12.0,2.5,2.5);
  setField(QField,0.0,0.0,0.0);
  generate(Control,
	   "Seg"+std::to_string(segNumber)+fcUnit,
	   preName+std::to_string(segNumber)+fcUnit,
	   "0",yAngle);

  return;
}

void
MagnetGenerator::generateDipole(FuncDataBase& Control,
				const size_t segNumber,
				const std::string& fcUnit,
				const double yAngle,
				const double QField) 
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: Head name for variable
    \param fcUnit :: Part name of FC magnet
    \param yAngle :: Angle
    \param QField :: K0 field
  */
{
  ELog::RegMethod RegA("MagnetGenerator","generateQuad");

  
  setSize(65.0,3.0,15.0);
  setField(QField,0.0,0.0,0.0);
  generate(Control,
	   "Seg"+std::to_string(segNumber)+fcUnit,
	   preName+std::to_string(segNumber)+fcUnit,
	   "0",yAngle);
  
  return;
}

void
MagnetGenerator::generateQuad(FuncDataBase& Control,
			      const size_t segNumber,
			      const std::string& fcUnit,
			      const double yAngle,
			      const double QField) 
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: Head name for variable
    \param fcUnit :: Part name of FC magnet
    \param yAngle :: Angle
    \param QField :: K1 field
  */
{
  ELog::RegMethod RegA("MagnetGenerator","generateQuad");
  
  setSize(18.0,2.5,2.5);
  setField(0.0,QField,0.0,0.0);
  generate(Control,
	   "Seg"+std::to_string(segNumber)+fcUnit,
	   preName+std::to_string(segNumber)+fcUnit,
	   "0",yAngle);
  
  return;
}

void
MagnetGenerator::generateSexupole(FuncDataBase& Control,
				  const size_t segNumber,
				  const std::string& fcUnit,
				  const double yAngle,
				  const double QField) 
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: Head name for variable
    \param fcUnit :: Part name of FC magnet
    \param yAngle :: Angle
    \param QField :: K1 field
  */
{
  ELog::RegMethod RegA("MagnetGenerator","generateSexupole");
  
  setSize(10.0,2.5,2.5);
  setField(0.0,0.0,QField,0.0);
  generate(Control,
	   "Seg"+std::to_string(segNumber)+fcUnit,
	   preName+std::to_string(segNumber)+fcUnit,
	   "0",yAngle);
  
  return;
}

void
MagnetGenerator::generate(FuncDataBase& Control,
			  const size_t segNumber,
			  const std::string& fcUnit,
			  const double yAngle) 
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: Head name for variable
    \param fcUnit :: Part name of FC magnet
    \param yAngle :: Angle
  */
{
  
  generate(Control,
	   "Seg"+std::to_string(segNumber)+fcUnit,
	   preName+std::to_string(segNumber)+fcUnit,
	   "0",yAngle);
  
  return;
}
  
void
MagnetGenerator::generate(FuncDataBase& Control,
			  const std::string& unitName,
			  const std::string& fcUnit,
			  const std::string& linkPt,
			  const double yAngle) 
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: Head name for variable
    \param Unit :: FixedComp for active units / origin
    \param yAngle :: Rotation of magnetic field
  */
{
  ELog::RegMethod RegA("MagnetGenerator","generate");

  const std::string keyName="MagUnit"+unitName;

  if (!xyzStep.nullVector())
    {
      Control.addVariable(keyName+"Offset",xyzStep);
      xyzStep=Geometry::Vec3D(0,0,0);
    }
  
  Control.addVariable(keyName+"YAngle",yAngle);
  Control.addVariable(keyName+"FixedComp",fcUnit);
  Control.addVariable(keyName+"LinkPt",linkPt);
       
  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"Width",width);
  Control.addVariable(keyName+"Height",height);

  Control.addVariable(keyName+"FixedComp",fcUnit);
  Control.addVariable(keyName+"LinkPt",linkPt);

  for(size_t i=0;i<4;i++)
    Control.addVariable(keyName+"KFactor"+std::to_string(i),KFactor[i]);

  Control.addVariable(keyName+"NActiveCell",activeCells.size());

  std::ostringstream cx;
  for(const std::string& CN : activeCells)
    cx<<" "<<CN;
  Control.addVariable(keyName+"ActiveCell",cx.str());
    
  return;

}

}  // namespace setVariable
