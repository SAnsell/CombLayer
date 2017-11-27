/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   build/collInsertBase.cxx
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
#include "LinearComp.h"
#include "ContainedComp.h"
#include "collInsertBase.h"

namespace shutterSystem
{

std::ostream& 
operator<<(std::ostream& OX,
	   const collInsertBase& A)
  /*!
    Calls collInsertBase method write to output class
    \param OX :: Output stream
    \param A :: Insert object to write
    \return Current state of stream
  */
{
  A.write(OX);
  return OX;
}


collInsertBase::collInsertBase(const int N,const int SN,
				 const std::string& Key) :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,4),
  blockIndex(N),surfIndex(SN),cellIndex(SN+1),
  populated(0),insertCell(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param N :: Index value of block
    \param SN :: surface Index value
    \param Key :: Name for item in search
  */
{}

collInsertBase::collInsertBase(const collInsertBase& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  blockIndex(A.blockIndex),surfIndex(A.surfIndex),
  cellIndex(A.cellIndex),populated(A.populated),
  beamOrigin(A.beamOrigin),beamX(A.beamX),beamY(A.beamY),
  beamZ(A.beamZ),insertCell(A.insertCell),fStep(A.fStep),
  centX(A.centX),centZ(A.centZ),length(A.length),
  width(A.width),height(A.height),matN(A.matN)
  /*!
    Copy constructor
    \param A :: collInsertBase to copy
  */
{}

collInsertBase&
collInsertBase::operator=(const collInsertBase& A)
  /*!
    Assignment operator
    \param A :: collInsertBase to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      populated=A.populated;
      beamOrigin=A.beamOrigin;
      beamX=A.beamX;
      beamY=A.beamY;
      beamZ=A.beamZ;
      insertCell=A.insertCell;
      fStep=A.fStep;
      centX=A.centX;
      centZ=A.centZ;
      length=A.length;
      width=A.width;
      height=A.height;
      matN=A.matN;
    }
  return *this;
}

void
collInsertBase::createUnitVector(const collInsertBase& ZB)
  /*!
    Create the unit vectors
    - Y Down the beamline
    \param ZB :: coll Block
  */
{
  ELog::RegMethod RegA("collInsertBase","createUnitVector<ZIB>");

  X=ZB.X;
  Y=ZB.Y;
  Z=ZB.Z;

  beamX=ZB.beamX;
  beamY=ZB.beamY;  
  beamZ=ZB.beamZ;

  Origin=ZB.getLinkPt(2)+Y*fStep;
  beamOrigin=ZB.getLinkPt(4)+beamY*fStep;

  return;
}

void
collInsertBase::createUnitVector(const FixedComp& FC)
  /*!
    Create the unit vectors
    - Y Down the beamline
    \param FC :: coll Block
  */
{
  ELog::RegMethod RegA("collInsertBase","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC);
  beamX=X;
  beamZ=Z;
  if (FC.NConnect()>1)
    {
      Origin=FC.getLinkPt(2)+Y*fStep;
      beamY=FC.getLinkAxis(2);
    }
  else
    {
      beamY=Y;  
      Origin=Y*fStep;
    }
  beamOrigin=Origin;
  
  return;
}

void
collInsertBase::createLinks()
  /*!
    Creates a full attachment set
    0 - 1 standard points
    2 - 3 beamaxis points
  */
{
  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setConnect(1,Origin+Y*length,Y);
 
  FixedComp::setLinkSurf(0,-SMap.realSurf(surfIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(surfIndex+2));

  FixedComp::setConnect(2,beamOrigin,-beamY);
  FixedComp::setConnect(3,beamOrigin+beamY*length,beamY);
 
  FixedComp::setLinkSurf(2,-SMap.realSurf(surfIndex+1));
  FixedComp::setLinkSurf(3,SMap.realSurf(surfIndex+2));

  return;
}

bool
collInsertBase::equalExternal(const collInsertBase& ZB) const
  /*!
    Tests if the external values of a block match
    Need to test both the Origin - the Y component
    \param ZB :: Second block to compare
    \return success if external values are equal
   */
{
  ELog::RegMethod RegA("collInsertBase","equalExternal");
  const Geometry::Vec3D ZBnonYOrg=ZB.Origin-Y*ZB.Origin.dotProd(ZB.Y);
  const Geometry::Vec3D nonYOrg=Origin-Y*Origin.dotProd(Y);
  
  return (std::abs(ZB.width-width)>Geometry::zeroTol ||
	  std::abs(ZB.height-height)>Geometry::zeroTol ||
	  ZBnonYOrg!=nonYOrg) ? 0 : 1;
}


void
collInsertBase::initialize(Simulation& System,
			    const attachSystem::FixedComp& FC)
  /*!
    Populate and create unit vectors
    \param System :: Simulation to use
    \param FC :: LinearComp to build vectors/Origin
  */
{
  ELog::RegMethod RegA("collInsertBase","initialize");
  populate(System,0);

  // Care here since origin is the original point:
  createUnitVector(FC);
  return;
}

void
collInsertBase::initialize(Simulation& System,
			 const collInsertBase& ZB)
  /*!
    Populate and create unit vectors
    \param System :: Simulation to use
    \param ZB :: similar item to copy
  */
{
  ELog::RegMethod RegA("collInsertBase","initialize");
  populate(System,&ZB);

  // Care here since origin is the original point:
  createUnitVector(ZB);
  return;
}

void
collInsertBase::setOrigin(const Geometry::Vec3D& OG,
			   const double xStart,
			   const double xAngle,
			   const double zStart,
			   const double zAngle)
  /*!
    Set the origin
    \param OG :: Origin
    \param xStart :: X step
    \param xAngle :: X angle
    \param zStart :: Z step
    \param zAngle :: Z angle
   */
{
  ELog::RegMethod RegA("collInsertBase","setOrigin");

  Origin=OG+Y*fStep;

  beamOrigin=Origin+X*xStart+Z*zStart;
  beamX=X;
  beamY=Y;
  beamZ=Z;
  const Geometry::Quaternion Qz=
    Geometry::Quaternion::calcQRot(zAngle,X);
  Qz.rotate(beamY);
  Qz.rotate(beamZ);

  const Geometry::Quaternion Qxy=
    Geometry::Quaternion::calcQRot(xAngle,Z);
  Qxy.rotate(beamX);
  Qxy.rotate(beamY);
  Qxy.rotate(beamZ);

  return;
}

Geometry::Vec3D
collInsertBase::getWindowCentre() const
  /*!
    Calculate the effective window in the void
    \return Centre 
  */
{ 
  return beamOrigin+beamX*centX+beamZ*centZ;
}

void
collInsertBase::createAll(Simulation& System,
			  const collInsertBase& ZB)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param ZB :: Linear object to component
  */
{
  ELog::RegMethod RegA("collInsertBase","createAll(collInsertBase)");
  populate(System,&ZB);

  createUnitVector(ZB);
  createSurfaces(ZB.getLinkSurf(2));
  createObjects(System,"","");
  createLinks();
  insertObjects(System);       

  return;
}

void
collInsertBase::createAll(Simulation& System,
			   const int startSurf,
			   const std::string& fSurf,
			   const std::string& bSurf)
   /*!
     Create all assuming that population has been done
     \param System :: Simulation to use
     \param startSurf :: initial front surface [if fSurf not give]
     \param fSurf :: Front surface(s)
     \param bSurf :: Back surface(s) [empty for module created]
   */
{
  ELog::RegMethod RegA("collInsertBase","createAll(int,string)");
  if (!populated)
    {
      ELog::EM<<"Failed to initialize item "<<blockIndex<<ELog::endErr;
      return;
    }
  
  createSurfaces(startSurf);  
  createObjects(System,fSurf,bSurf);
  createLinks();
  insertObjects(System);
  return;
}

void
collInsertBase::write(std::ostream& OX) const
  /*!
    Write stuff out
    \param OX :: Output stream
  */
{
  OX<<"Origin == "<<beamOrigin<<std::endl;
  OX<<"Width == "<<width<<" "<<beamX<<std::endl;
  OX<<"Height == "<<height<<":"<<beamZ<<std::endl;
}

} // NAMESPACE shutterSystem
