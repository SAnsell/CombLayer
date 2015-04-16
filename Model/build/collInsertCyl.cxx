/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   build/collInsertCyl.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <complex>
#include <set>
#include <map>
#include <list>
#include <vector>
#include <string>
#include <boost/format.hpp>

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
#include "Quaternion.h"
#include "Surface.h"
#include "surfRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "collInsertBase.h"
#include "collInsertCyl.h"

namespace shutterSystem
{


collInsertCyl::collInsertCyl(const int N,const int SN,
				 const std::string& Key) :
  collInsertBase(N,SN,Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param N :: Index value of block
    \param SN :: surface Index value
    \param Key :: Name for item in search
  */
{}

collInsertCyl::collInsertCyl(const collInsertCyl& A) : 
  collInsertBase(A),
  radGap(A.radGap)
  /*!
    Copy constructor
    \param A :: collInsertCyl to copy
  */
{}

collInsertCyl&
collInsertCyl::operator=(const collInsertCyl& A)
  /*!
    Assignment operator
    \param A :: collInsertCyl to copy
    \return *this
  */
{
  if (this!=&A)
    {
      collInsertBase::operator=(A);
      radGap=A.radGap;
    }
  return *this;
}

collInsertCyl*
collInsertCyl::clone() const
  /*!
    Clone constructor
    \return copy of this
  */
{
  return new collInsertCyl(*this);
}

void
collInsertCyl::populate(const Simulation& System,
			const collInsertBase* sndBase)
  /*!
    Populate all the variables
    Requires that unset values are copied from previous block
    \param System :: Simulation to use
    \param sndBlock :: Standard block to use
  */
{
  ELog::RegMethod RegA("collInsertCyl","populate");
  const FuncDataBase& Control=System.getDataBase();

  const size_t Size(8);
  const size_t commonSize(7);
  const char* sndKey[Size]=
    {"FStep","CentX","CentZ","Len","Width","Height","Mat","RGap"};
  
  const collInsertCyl* cylPtr=
    dynamic_cast<const collInsertCyl*>(sndBase);

  for(size_t i=0;i<Size;i++)
    {
      const std::string KN=keyName+
	StrFunc::makeString(blockIndex+1)+sndKey[i];
      if (Control.hasVariable(KN))
	setVar(Control,i,KN);
      else if (cylPtr)
	setVar(i,cylPtr->getVar(i));	
      else if (sndBase && i<=commonSize)
	  setVar(i,sndBase->getVar(i));	
      else
	{
	  ELog::EM<<"sndBase == "<<
	    ((sndBase) ? sndBase->typeName() : "NULL")<<
	    ELog::endCrit;
	  throw ColErr::InContainerError<std::string>
	    (KN,"Failed to connect on first component");
	}
    }
  populated|=1;
  return;
}

void
collInsertCyl::setVar(const size_t Item,const double V)
  /*!
    Given a value set the item
    \param Item :: Index value to variable
    \param V :: Value
  */
{
  ELog::RegMethod RegA("collInsertCyl","setVar");
  double* VDPtr[8]={&fStep,&centX,&centZ,&length,
		   &width,&height,0,&radGap};
  int* VIPtr[8]={0,0,0,0,
		 0,0,&matN,0};
  if (Item>7)
    throw ColErr::IndexError<size_t>(Item,7,"Item");
  if (VDPtr[Item])
    *VDPtr[Item]=V;
  else if (VIPtr[Item])
    *VIPtr[Item]=static_cast<int>(V);
  else
    throw ColErr::InContainerError<size_t>(Item,"Item Index has no type");
  
  return;

}

void
collInsertCyl::setVar(const FuncDataBase& Control,
		      const size_t Item,const std::string& VarStr)
  /*!
    Convert a string into an item value
    \param Control :: DataBase to get value from
    \param Item :: Index value to variable
    \param VarStr :: String to convert into values
  */
{
  ELog::RegMethod RegA("collInsertCyl","setVar<Control>");
  if (Item==6)
    matN=ModelSupport::EvalMat<int>(Control,VarStr);
  else
    setVar(Item,Control.EvalVar<double>(VarStr));
  return;
}

double
collInsertCyl::getVar(const size_t Item) const
  /*!
    Get the value based on an index reference
    \param Item :: Index value to variable
    \return value
  */
{
  ELog::RegMethod RegA("collInsertCyl","getVar");
  switch(Item)
    {
    case 0:
      return fStep;
    case 1:
      return centX;
    case 2:
      return centZ;
    case 3:
      return length;
    case 4:
      return width;
    case 5:
      return height;
    case 6:
      return matN;
    case 7:
      return radGap;
    }
  throw ColErr::IndexError<size_t>(Item,8,"Item");
}

void
collInsertCyl::createSurfaces(const int startSurf)
  /*!
    Creates/duplicates the surfaces for this block
    \param startSurf :: First surface to use
  */
{  
  ELog::RegMethod RegA("collInsertCyl","createSurface");

  if (startSurf)
    SMap.addMatch(surfIndex+1,startSurf);
  else
    ModelSupport::buildPlane(SMap,surfIndex+1,Origin,Y);
  
  // Outer Surface 
  ModelSupport::buildPlane(SMap,surfIndex+2,Origin+Y*length,Y);
  ModelSupport::buildPlane(SMap,surfIndex+3,Origin-X*width,X);
  ModelSupport::buildPlane(SMap,surfIndex+4,Origin+X*width,X);
  ModelSupport::buildPlane(SMap,surfIndex+5,Origin-Z*height,Z);
  ModelSupport::buildPlane(SMap,surfIndex+6,Origin+Z*height,Z);
  
  // Inner surface
  ModelSupport::buildCylinder(SMap,surfIndex+17,
			      beamOrigin+beamX*centX+
			      beamZ*centZ,beamY,radGap);
  return;
}

void
collInsertCyl::createObjects(Simulation& System,
			     const std::string& fSurf,
			     const std::string& bSurf)
  /*!
    Create the objects
    \param System :: Simulation
    \param fSurf :: front surface 
    \param bSurf :: back Surface
  */
{
  ELog::RegMethod RegA("collInsertCyl","createObjects");

  std::string frontBack=fSurf.empty() ? 
    ModelSupport::getComposite(SMap,surfIndex,"1 ") : fSurf;
  frontBack+=bSurf.empty() ? 
    ModelSupport::getComposite(SMap,surfIndex,"-2 ") : bSurf;

  std::string Out;
  Out=ModelSupport::getComposite(SMap,surfIndex,"3 -4 5 -6 ");
  addOuterSurf(Out);

  // Centre void
  Out=ModelSupport::getComposite(SMap,surfIndex," -17 ")+
    frontBack;
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  // Outer metal
  Out=ModelSupport::getComposite(SMap,surfIndex,
				 "3 -4 5 -6 17 ")+
    frontBack;
  System.addCell(MonteCarlo::Qhull(cellIndex++,matN,0.0,Out));
  
  return;
}

int
collInsertCyl::exitWindow(const double Dist,
			  std::vector<int>& window,
			  Geometry::Vec3D& Pt) const
  /*!
    Get the exit window
    \param Dist :: Dist from exit point
    \param window :: window vector of paired planes
    \param Pt :: Output point for tally
    \return Master Surface
   */
{
  ELog::RegMethod RegA("collInsertCyl","exitWindow");

  window.clear();
  window.push_back(SMap.realSurf(surfIndex+3));
  window.push_back(SMap.realSurf(surfIndex+4));
  window.push_back(SMap.realSurf(surfIndex+5));
  window.push_back(SMap.realSurf(surfIndex+6));

  Pt=Origin+Y*Dist;
  return SMap.realSurf(surfIndex+1);
}

std::vector<Geometry::Vec3D> 
collInsertCyl::viewWindow(const collInsertBase*) const
  /*!
    Simple circle approximatin to points
    \return points:
   */
{
  ELog::RegMethod RegA("collInsertCyl","viewWindow");

  std::vector<Geometry::Vec3D> OutV;
  OutV.push_back(beamOrigin-beamX*radGap-beamZ*radGap);
  OutV.push_back(beamOrigin-beamX*radGap+beamZ*radGap);
  OutV.push_back(beamOrigin+beamX*radGap+beamZ*radGap);
  OutV.push_back(beamOrigin+beamX*radGap-beamZ*radGap);
 
  return OutV;
}

} // NAMESPACE shutterSystem
