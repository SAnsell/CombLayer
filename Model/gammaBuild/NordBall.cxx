/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   gammaBuild/NordBall.cxx
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

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
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
#include "support.h"
#include "stringCombine.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "NordBall.h"

namespace gammaSystem
{

NordBall::NordBall(const size_t Index,const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key+StrFunc::makeString(Index),6),
  detNumber(Index),baseName(Key),
  detIndex(ModelSupport::objectRegister::Instance().cell(keyName)),
  cellIndex(detIndex+1)
  /*!
    Constructor
    \param Index :: Index number
    \param Key :: Name of construction key
  */
{}

NordBall::NordBall(const NordBall& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  detNumber(A.detNumber),baseName(A.baseName),
  detIndex(A.detIndex),cellIndex(A.cellIndex),nFace(A.nFace),
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
      cellIndex=A.cellIndex;
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

  nFace=Control.EvalPair<size_t>(keyName,baseName,"NFace");
  faceWidth=Control.EvalPair<double>(keyName,baseName,"FaceWidth");
  backWidth=Control.EvalPair<double>(keyName,baseName,"BackWidth");
  frontLength=Control.EvalPair<double>(keyName,baseName,"FrontLength");
  backLength=Control.EvalPair<double>(keyName,baseName,"BackLength");
  wallThick=Control.EvalPair<double>(keyName,baseName,"WallThick");

  plateThick=Control.EvalPair<double>(keyName,baseName,"PlateThick");
  plateRadius=Control.EvalPair<double>(keyName,baseName,"PlateRadius");
  supportThick=Control.EvalPair<double>(keyName,baseName,"SupportThick");
  elecRadius=Control.EvalPair<double>(keyName,baseName,"ElecRadius");
  elecThick=Control.EvalPair<double>(keyName,baseName,"ElecThick");

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
NordBall::createUnitVector(const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    Create the unit vectors.
    Note angle shift carried out first so easy YStep system
    \param FC :: Fixed Component
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("NordBall","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

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
  ModelSupport::buildPlane(SMap,detIndex+1,Origin-Y*wallThick,Y);  
  ModelSupport::buildPlane(SMap,detIndex+2,Origin+
			   Y*(frontLength),Y);  
  ModelSupport::buildPlane(SMap,detIndex+3,Origin+
			   Y*(frontLength+backLength),Y);  


  ModelSupport::buildPlane(SMap,detIndex+101,Origin,Y);  
  // Loop over walls
  int DI(detIndex);
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
  ModelSupport::buildCylinder(SMap,detIndex+1107,Origin,Y,plateRadius);
  ModelSupport::buildPlane(SMap,detIndex+1101,Origin+Y*TLen,Y);
  if (supportThick>Geometry::zeroTol)
    {
      TLen+=supportThick;
      ModelSupport::buildPlane(SMap,detIndex+1201,Origin+Y*TLen,Y);
    }
  TLen+=elecThick;
  ModelSupport::buildCylinder(SMap,detIndex+1307,Origin,Y,elecRadius);
  ModelSupport::buildPlane(SMap,detIndex+1301,Origin+Y*TLen,Y);
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
    ModelSupport::getComposite(SMap,detIndex," 101 -2 ");

  for(int i=0;i<static_cast<int>(nFace);i++)
    cx<<-(31+i)<<" ";
  CompUnit.procString(cx.str());
  CompUnit.makeComplement();
  Out=ModelSupport::getComposite(SMap,detIndex,cx.str());
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out+divide));

  Out=ModelSupport::getComposite(SMap,detIndex+50,cx.str());
  Out+=ModelSupport::getComposite(SMap,detIndex,CompUnit.display());
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out+divide));

  Out=ModelSupport::getComposite(SMap,detIndex+50,cx.str());
  Out+=ModelSupport::getComposite(SMap,detIndex," -101 1 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));

  // Edge [void]
  Out=ModelSupport::getComposite(SMap,detIndex,"1 -2 -1107 ");
  Out+=ModelSupport::getComposite(SMap,detIndex+50,CompUnit.display());
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));


  //MID
  const std::string midDivide=
    ModelSupport::getComposite(SMap,detIndex," 2 -3 ");
  cx.str("");
  for(int i=0;i<static_cast<int>(nFace);i++)
    cx<<-(11+i)<<" ";
  CompUnit.procString(cx.str());
  CompUnit.makeComplement();

  // Inner
  Out=ModelSupport::getComposite(SMap,detIndex,cx.str());
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out+midDivide));

  // Wall
  Out=ModelSupport::getComposite(SMap,detIndex+50,cx.str());
  Out+=ModelSupport::getComposite(SMap,detIndex,CompUnit.display());
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out+midDivide));


  // Edge [void]
  Out=ModelSupport::getComposite(SMap,detIndex,"2 -3 -1107 ");
  Out+=ModelSupport::getComposite(SMap,detIndex+50,CompUnit.display());
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));


  // Back plate
  Out=ModelSupport::getComposite(SMap,detIndex,"3 -1101 -1107 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,plateMat,0.0,Out));

  // support plate
  int eIndex(detIndex);
  if (supportThick>Geometry::zeroTol)
    {
      Out=ModelSupport::getComposite(SMap,detIndex,"1101 -1201 -1107 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,supportMat,0.0,Out));
      eIndex+=100;
    }
  Out=ModelSupport::getComposite(SMap,detIndex,eIndex,"1101M -1301 -1307 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,elecMat,0.0,Out));
  
  Out=ModelSupport::getComposite(SMap,detIndex,eIndex,"1101M -1301 1307 -1107");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  
  
  Out=ModelSupport::getComposite(SMap,detIndex,"1 -1301 -1107 ");
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
  FixedComp::setLinkSurf(0,SMap.realSurf(detIndex+1));

  FixedComp::setConnect(1,Origin+Y*(frontLength+backLength),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(detIndex+2));

  const double angleStep(2.0*M_PI/static_cast<double>(nFace));
  for(size_t i=0;i<nFace;i++)
    {
      const int ii(static_cast<int>(i));
      const double angle=angleStep*static_cast<double>(i);
      const Geometry::Vec3D UDir(X*cos(angle)+Z*sin(angle));
      const Geometry::Vec3D BackPt(Origin+UDir*backWidth+Y*(frontLength));
      FixedComp::setConnect(2+i,BackPt,UDir);
      FixedComp::setLinkSurf(2+i,SMap.realSurf(detIndex+11+ii));
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
