/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   gammaBuild/NordBall.cxx
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
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "surfRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "NordBall.h"

namespace gammaSystem
{

NordBall::NordBall(const size_t Index,const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key+std::to_string(Index),6),
  detNumber(Index),baseName(Key)
  /*!
    Constructor
    \param Index :: Index number
    \param Key :: Name of construction key
  */
{}

NordBall::NordBall(const NordBall& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  detNumber(A.detNumber),baseName(A.baseName),
  nFace(A.nFace),
  faceWidth(A.faceWidth),backWidth(A.backWidth),
  frontLength(A.frontLength),backLength(A.backLength),
  wallThick(A.wallThick),plateThick(A.plateThick),
  plateRadius(A.plateRadius),supportThick(A.supportThick),
  elecRadius(A.elecRadius),elecThick(A.elecThick),mat(A.mat),
  wallMat(A.wallMat),plateMat(A.plateMat),
  supportMat(A.supportMat),elecMat(A.elecMat)
  /*!
    Copy constructor
    \param A :: NordBall to copy
  */
{}

NordBall&
NordBall::operator=(const NordBall& A)
  /*!
    Assignment operator
    \param A :: NordBall to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      nFace=A.nFace;
      faceWidth=A.faceWidth;
      backWidth=A.backWidth;
      frontLength=A.frontLength;
      backLength=A.backLength;
      wallThick=A.wallThick;
      plateThick=A.plateThick;
      plateRadius=A.plateRadius;
      supportThick=A.supportThick;
      elecRadius=A.elecRadius;
      elecThick=A.elecThick;
      mat=A.mat;
      wallMat=A.wallMat;
      plateMat=A.plateMat;
      supportMat=A.supportMat;
      elecMat=A.elecMat;
    }
  return *this;
}

NordBall::~NordBall()
  /*!
    Destructor
  */
{}

void
NordBall::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("NordBall","populate");

  FixedOffset::populate(Control);

  nFace=Control.EvalTail<size_t>(keyName,baseName,"NFace");
  faceWidth=Control.EvalTail<double>(keyName,baseName,"FaceWidth");
  backWidth=Control.EvalTail<double>(keyName,baseName,"BackWidth");
  frontLength=Control.EvalTail<double>(keyName,baseName,"FrontLength");
  backLength=Control.EvalTail<double>(keyName,baseName,"BackLength");
  wallThick=Control.EvalTail<double>(keyName,baseName,"WallThick");

  plateThick=Control.EvalTail<double>(keyName,baseName,"PlateThick");
  plateRadius=Control.EvalTail<double>(keyName,baseName,"PlateRadius");
  supportThick=Control.EvalTail<double>(keyName,baseName,"SupportThick");
  elecRadius=Control.EvalTail<double>(keyName,baseName,"ElecRadius");
  elecThick=Control.EvalTail<double>(keyName,baseName,"ElecThick");

  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat",baseName+"Mat");  
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat",
				     baseName+"WallMat");  
  plateMat=ModelSupport::EvalMat<int>(Control,keyName+"PlateMat",
				      baseName+"PlateMat");  
  supportMat=ModelSupport::EvalMat<int>(Control,keyName+"SupportMat",
					baseName+"SupportMat");  
  elecMat=ModelSupport::EvalMat<int>(Control,keyName+"ElecMat",
				     baseName+"ElecMat");  

  return;
}


void
NordBall::createSurfaces()
  /*!
    Create planes for the silicon and Polyethene layers
  */
{
  ELog::RegMethod RegA("NordBall","createSurfaces");

  // Front/Back plane
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*wallThick,Y);  
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+
			   Y*(frontLength),Y);  
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin+
			   Y*(frontLength+backLength),Y);  


  ModelSupport::buildPlane(SMap,buildIndex+101,Origin,Y);  
  // Loop over walls
  int DI(buildIndex);
  double FWidth(faceWidth);
  double BWidth(backWidth);
  for(int nWall=0;nWall<2;nWall++)
    {
      const double angleStep(2.0*M_PI/static_cast<double>(nFace));
      for(int i=0;i<static_cast<int>(nFace);i++)
	{
	  const double angle=angleStep*i;
	  const Geometry::Vec3D UDir(X*cos(angle)+Z*sin(angle));
	  Geometry::Vec3D SDir(UDir*frontLength-Y*(backWidth-faceWidth));
	  SDir.makeUnit();
	  const Geometry::Vec3D FacePt(Origin+UDir*FWidth);
	  const Geometry::Vec3D BackPt(Origin+UDir*BWidth+
				       Y*(frontLength+backLength));
	  ModelSupport::buildPlane(SMap,DI+11+i,BackPt,UDir);
	  ModelSupport::buildPlane(SMap,DI+31+i,FacePt,SDir);
	}
      FWidth+=wallThick;
      BWidth+=wallThick;
      DI+=50;
    }
  // Back plate

  double TLen(frontLength+backLength+plateThick);
  ModelSupport::buildCylinder(SMap,buildIndex+1107,Origin,Y,plateRadius);
  ModelSupport::buildPlane(SMap,buildIndex+1101,Origin+Y*TLen,Y);
  if (supportThick>Geometry::zeroTol)
    {
      TLen+=supportThick;
      ModelSupport::buildPlane(SMap,buildIndex+1201,Origin+Y*TLen,Y);
    }
  TLen+=elecThick;
  ModelSupport::buildCylinder(SMap,buildIndex+1307,Origin,Y,elecRadius);
  ModelSupport::buildPlane(SMap,buildIndex+1301,Origin+Y*TLen,Y);
  return; 
}

void
NordBall::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("NordBall","createObjects");

  std::string Out;
  // First make conics:
  std::ostringstream cx;

  HeadRule CompUnit;

  // Front [inner]
  const std::string divide=
    ModelSupport::getComposite(SMap,buildIndex," 101 -2 ");

  for(int i=0;i<static_cast<int>(nFace);i++)
    cx<<-(31+i)<<" ";
  CompUnit.procString(cx.str());
  CompUnit.makeComplement();
  Out=ModelSupport::getComposite(SMap,buildIndex,cx.str());
  System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out+divide));

  Out=ModelSupport::getComposite(SMap,buildIndex+50,cx.str());
  Out+=ModelSupport::getComposite(SMap,buildIndex,CompUnit.display());
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out+divide));

  Out=ModelSupport::getComposite(SMap,buildIndex+50,cx.str());
  Out+=ModelSupport::getComposite(SMap,buildIndex," -101 1 ");
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));

  // Edge [void]
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 -1107 ");
  Out+=ModelSupport::getComposite(SMap,buildIndex+50,CompUnit.display());
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));


  //MID
  const std::string midDivide=
    ModelSupport::getComposite(SMap,buildIndex," 2 -3 ");
  cx.str("");
  for(int i=0;i<static_cast<int>(nFace);i++)
    cx<<-(11+i)<<" ";
  CompUnit.procString(cx.str());
  CompUnit.makeComplement();

  // Inner
  Out=ModelSupport::getComposite(SMap,buildIndex,cx.str());
  System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out+midDivide));

  // Wall
  Out=ModelSupport::getComposite(SMap,buildIndex+50,cx.str());
  Out+=ModelSupport::getComposite(SMap,buildIndex,CompUnit.display());
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out+midDivide));


  // Edge [void]
  Out=ModelSupport::getComposite(SMap,buildIndex,"2 -3 -1107 ");
  Out+=ModelSupport::getComposite(SMap,buildIndex+50,CompUnit.display());
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));


  // Back plate
  Out=ModelSupport::getComposite(SMap,buildIndex,"3 -1101 -1107 ");
  System.addCell(MonteCarlo::Object(cellIndex++,plateMat,0.0,Out));

  // support plate
  int eIndex(buildIndex);
  if (supportThick>Geometry::zeroTol)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex,"1101 -1201 -1107 ");
      System.addCell(MonteCarlo::Object(cellIndex++,supportMat,0.0,Out));
      eIndex+=100;
    }
  Out=ModelSupport::getComposite(SMap,buildIndex,eIndex,"1101M -1301 -1307 ");
  System.addCell(MonteCarlo::Object(cellIndex++,elecMat,0.0,Out));
  
  Out=ModelSupport::getComposite(SMap,buildIndex,eIndex,"1101M -1301 1307 -1107");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));
  
  
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -1301 -1107 ");
  addOuterSurf(Out);


  return; 
}

void
NordBall::createLinks()
  /*!
    Creates a full attachment set
  */
{  
  ELog::RegMethod RegA("NordBall","createLinks");

  setNConnect(2+nFace);
  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setLinkSurf(0,SMap.realSurf(buildIndex+1));

  FixedComp::setConnect(1,Origin+Y*(frontLength+backLength),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));

  const double angleStep(2.0*M_PI/static_cast<double>(nFace));
  for(size_t i=0;i<nFace;i++)
    {
      const int ii(static_cast<int>(i));
      const double angle=angleStep*static_cast<double>(i);
      const Geometry::Vec3D UDir(X*cos(angle)+Z*sin(angle));
      const Geometry::Vec3D BackPt(Origin+UDir*backWidth+Y*(frontLength));
      FixedComp::setConnect(2+i,BackPt,UDir);
      FixedComp::setLinkSurf(2+i,SMap.realSurf(buildIndex+11+ii));
    }

  return;
}

void
NordBall::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
    \param sideIndex :: link point index
   */
{
  ELog::RegMethod RegA("NordBall","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE gammaSystem
