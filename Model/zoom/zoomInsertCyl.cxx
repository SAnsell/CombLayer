/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   zoom/zoomInsertCyl.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "surfDIter.h"
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
#include "shutterBlock.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "zoomInsertBase.h"
#include "zoomInsertCyl.h"

namespace shutterSystem
{


zoomInsertCyl::zoomInsertCyl(const int N,const int SN,
				 const std::string& Key) :
  zoomInsertBase(N,SN,Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param N :: Index value of block
    \param SN :: surface Index value
    \param Key :: Name for item in search
  */
{}

zoomInsertCyl::zoomInsertCyl(const zoomInsertCyl& A) : 
  zoomInsertBase(A),
  radGap(A.radGap)
  /*!
    Copy constructor
    \param A :: zoomInsertCyl to copy
  */
{}

zoomInsertCyl&
zoomInsertCyl::operator=(const zoomInsertCyl& A)
  /*!
    Assignment operator
    \param A :: zoomInsertCyl to copy
    \return *this
  */
{
  if (this!=&A)
    {
      zoomInsertBase::operator=(A);
      radGap=A.radGap;
    }
  return *this;
}

zoomInsertCyl*
zoomInsertCyl::clone() const
  /*!
    Clone constructor
    \return copy of this
  */
{
  return new zoomInsertCyl(*this);
}

void
zoomInsertCyl::populate(const Simulation& System,
			const zoomInsertBase* sndBase)
  /*!
    Populate all the variables
    Requires that unset values are copied from previous block
    \param System :: Simulation to use
    \param sndBlock :: Standard block to use
  */
{
  ELog::RegMethod RegA("zoomInsertCyl","populate");
  const FuncDataBase& Control=System.getDataBase();

  const int Size(8);
  const int commonSize(7);
  const char* sndKey[Size]=
    {"FStep","CentX","CentZ","Len","Width","Height","Mat","RGap"};
  
  const zoomInsertCyl* sndBlock=
    dynamic_cast<const zoomInsertCyl*>(sndBase);

  for(int i=0;i<Size;i++)
    {
      std::ostringstream cx;
      cx<<keyName<<blockIndex+1<<sndKey[i];
      if (Control.hasVariable(cx.str()))
	setVar(Control,i,cx.str());
      else if (sndBlock)
	setVar(i,sndBlock->getVar(i));	
      else 
	{
	  ELog::EM<<"Failed to connect on first component:"
		  <<blockIndex+1<<" :: "<<cx.str()<<ELog::endCrit;
	}
    }  
  populated|=1;
  return;
}

void
zoomInsertCyl::setVar(const int Item,const double V)
  /*!
    Given a value set the item
    \param Item :: Index value to variable
    \param V :: Value
  */
{
  switch(Item)
    {
    case 0:
      fStep=V;
      return;
    case 1:
      centX=V;
      return;
    case 2:
      centZ=V;
      return;
    case 3:
      length=V;
      return;
    case 4:
      width=V;
      return;
    case 5:
      height=V;
      return;
    case 6:
      matN=static_cast<int>(V);
      return;
    case 7:
      radGap=V;
      return;
    default:
      throw ColErr::IndexError<int>(Item,7,"Item");
    }
  return;
}

void
zoomInsertCyl::setVar(const FuncDataBase& Control,
			const int Item,const std::string& VarStr)
  /*!
    Convert a string into an item value
    \param Control :: DataBase to get value from
    \param Item :: Index value to variable
    \param VarStr :: String to convert into values
  */
{
  ELog::RegMethod RegA("zoomInsertCyl","setVar<Control>");
  setVar(Item,Control.EvalVar<double>(VarStr));
  return;
}

double
zoomInsertCyl::getVar(const int Item) const
  /*!
    Get the value based on an index reference
    \param Item :: Index value to variable
    \return value
  */
{
  ELog::RegMethod RegA("zoomInsertCyl","getVar");
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
  throw ColErr::IndexError<int>(Item,8,"Item");
}

void
zoomInsertCyl::createSurfaces(const int startSurf)
  /*!
    Creates/duplicates the surfaces for this block
    \param startSurf :: First surface to use
  */
{  
  ELog::RegMethod RegA("zoomInsertCyl","createSurface");

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
zoomInsertCyl::createObjects(Simulation& System,
			       const std::string& fSurf,
			       const std::string& bSurf)
  /*!
    Create the objects
    \param System :: Simulation
    \param fSurf :: front surface 
    \param bSurf :: back Surface
  */
{
  ELog::RegMethod RegA("zoomInsertCyl","createObjects");

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
zoomInsertCyl::exitWindow(const double Dist,
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
  ELog::RegMethod RegA("zoomInsertCyl","exitWindow");

  window.clear();
  window.push_back(SMap.realSurf(surfIndex+3));
  window.push_back(SMap.realSurf(surfIndex+4));
  window.push_back(SMap.realSurf(surfIndex+5));
  window.push_back(SMap.realSurf(surfIndex+6));

  Pt=Origin+Y*Dist;
  return SMap.realSurf(surfIndex+1);
}


} // NAMESPACE shutterSystem
