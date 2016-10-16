/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Build/TubeCollimator.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "SurInter.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "LineIntersectVisit.h"
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
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "TubeCollimator.h"

namespace photonSystem
{

TubeCollimator::TubeCollimator(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,6),
  rodIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(rodIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

TubeCollimator::~TubeCollimator() 
  /*!
    Destructor
  */
{
}


void
TubeCollimator::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("VacuumVessel","populate");

  FixedOffset::populate(Control);

  radius=Control.EvalVar<double>(keyName+"Radius");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  centSpc=Control.EvalVar<double>(keyName+"CentSpc");
  length=Control.EvalVar<double>(keyName+"Length");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  layoutType=Control.EvalVar<std::string>(keyName+"LayoutType");
  boundaryType=Control.EvalVar<std::string>(keyName+"BoundaryType");
  
  setBoundary(Control);
  setLayout(Control);
  return;
}

void
TubeCollimator::setLayout(const FuncDataBase& Control)
  /*!
    Sets the layout vectors
    \param Control :: Database for variables
  */
{
  ELog::RegMethod RegA("TubeCollimator","setLayout");


  if (layoutType=="square" || layoutType=="Square")
    {
      AAxis=X;
      BAxis=Z;
    }
  else if (layoutType=="hexagon" || layoutType=="Hexagon")
    {
      AAxis=X;
      BAxis=X*cos(M_PI/3.0)+Z*sin(M_PI/3.0);
    }
  else
    {
      throw ColErr::InContainerError<std::string>
	(layoutType,"layout type not supported");
    }


  const double rotAngle=
    Control.EvalDefPair<double>(keyName+"LayoutRotAngle",
				keyName+"RotAngle",0.0);
  const Geometry::Quaternion Qab=
    Geometry::Quaternion::calcQRotDeg(rotAngle,Y);
  Qab.rotate(AAxis);
  Qab.rotate(BAxis);

  return;
}

  
void
TubeCollimator::setBoundary(const FuncDataBase& Control)
  /*!
    Set the boundary
    \param Control :: Database for variables
   */
{
  ELog::RegMethod RegA("TubeCollimator","setBoundary");

  std::string Out;
  const double rotAngle=
    Control.EvalDefPair<double>(keyName+"BoundaryRotAngle",
				keyName+"RotAngle",0.0);

  if (boundaryType=="Rectangle" || boundaryType=="rectangle")
    {
      const double width=
	Control.EvalVar<double>(keyName+"Width");
      const double height=
	Control.EvalVar<double>(keyName+"Height");
      Geometry::Vec3D AX(X);
      Geometry::Vec3D BZ(Z);
       const Geometry::Quaternion Qab=
	 Geometry::Quaternion::calcQRotDeg(rotAngle,Y);
       Qab.rotate(AX);
       Qab.rotate(BZ);

       ModelSupport::buildPlane(SMap,rodIndex+5003,Origin-AX*(width/2.0),AX);
       ModelSupport::buildPlane(SMap,rodIndex+5004,Origin+AX*(width/2.0),AX);
       ModelSupport::buildPlane(SMap,rodIndex+5005,Origin-BZ*(height/2.0),BZ);
       ModelSupport::buildPlane(SMap,rodIndex+5006,Origin+BZ*(height/2.0),BZ);

       // created for outer boundary
       ModelSupport::buildPlane(SMap,rodIndex+6003,Origin-AX*((width+centSpc)/2.0),AX);
       ModelSupport::buildPlane(SMap,rodIndex+6004,Origin+AX*((width+centSpc)/2.0),AX);
       ModelSupport::buildPlane(SMap,rodIndex+6005,Origin-BZ*((height+centSpc)/2.0),BZ);
       ModelSupport::buildPlane(SMap,rodIndex+6006,Origin+BZ*((height+centSpc)/2.0),BZ);

       
       Out=ModelSupport::getComposite(SMap,rodIndex," 5003 -5004 5005 -5006 ");
       boundary.procString(Out);
       boundary.populateSurf();
    }
  else if (boundaryType=="Circle" || boundaryType=="circle")
    {
      const double radius=
	Control.EvalVar<double>(keyName+"Radius");
      ModelSupport::buildCylinder(SMap,rodIndex+5007,Origin,Y,radius);
      // Outer boundary
      ModelSupport::buildCylinder(SMap,rodIndex+5007,Origin,Y,radius+centSpc/2.0);
      
      Out=ModelSupport::getComposite(SMap,rodIndex," -5007 ");
      boundary.procString(Out);
      boundary.populateSurf();
    }
  else if (boundaryType=="Free" || boundaryType=="free")
    {
      nTubeLayers=Control.EvalVar<long int>(keyName+"NTubeLayers");
      if (nTubeLayers<=0)
	throw ColErr::RangeError<long int>
	  (0,50,nTubeLayers,"boundaryType:Free > 0");
    }
  else
    {
      throw ColErr::InContainerError<std::string>
	(boundaryType,"boundary type not supported");
    }  
  return;
}

  
void
TubeCollimator::createUnitVector(const attachSystem::FixedComp& FC,
				 const long int sideIndex)
/*!
    Create the unit vectors
    \param FC :: Fixed Component
    \param sideIndex :: Link point surface to use as origin/basis.
  */
{
  ELog::RegMethod RegA("TubeCollimator","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  // Default layout [rectangular]
  AAxis=X;
  BAxis=Z;
  
  return;
}
  

void
TubeCollimator::createTubes(Simulation& System)
  /*!
    Create simple structures
    \param System :: simluation to use
  */
{
  ELog::RegMethod RegA("VacuumVessel","createSurfaces");

  ModelSupport::buildPlane(SMap,rodIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,rodIndex+2,Origin+Y*(length/2.0),Y);

  const std::string FBStr=
    ModelSupport::getComposite(SMap,rodIndex," 1 -2 ");

  
  std::string Out;
  int RI(rodIndex);

  // Outer boundary:
  const std::string OutBoundary=boundaryString();
  std::string HoleStr;
  
  bool acceptFlag(1);
  long int step(0);   
  while(acceptFlag || (nTubeLayers>0 && step<nTubeLayers))
    {
      acceptFlag=0;
      for(long int i=-step;i<=step;i++)
	for(long int j=-step;j<=step;j++)
	  {
	    if (std::abs(i)==step || std::abs(j)==step)
	      {
		const Geometry::Vec3D CPoint=AAxis*i+BAxis*j;
		if (boundary.isValid(CPoint))
		  {
		    acceptFlag=1;
		    ModelSupport::buildCylinder(SMap,RI+7,CPoint,Y,radius);
		    ModelSupport::buildCylinder(SMap,RI+8,CPoint,Y,radius+wallThick);

		    Out=ModelSupport::getComposite(SMap,rodIndex," -7 ");
		    System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out+FBStr));

		    Out=ModelSupport::getComposite(SMap,rodIndex," 7 -17 ");
		    System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out+FBStr));

		    Out=ModelSupport::getComposite(SMap,rodIndex," 17 ");
		    HoleStr+=Out;
		    RI+=10;
		  }
	      }
	  }
    }
  
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,OutBoundary+FBStr+HoleStr));
  addOuterSurf(OutBoundary+FBStr);
  
  return; 
}


std::string
TubeCollimator::boundaryString() const
  /*!
    Create the boundary string
    \param compFlag :: Complementary
    \return boundary string
  */
{
  ELog::RegMethod RegA("TubeCollimator","createBoundary");

  if (boundaryType=="Rectangle" || boundaryType=="rectangle")
    return ModelSupport::getComposite(SMap,rodIndex," 6003 -6004 6005 -6006");
  
  if (boundaryType=="Circle" || boundaryType=="circle")
    return ModelSupport::getComposite(SMap,rodIndex," -6007");

  return "";
}


void
TubeCollimator::createLinks()
  /*!
    Creates a full attachment set
  */
{  
  ELog::RegMethod RegA("VacuumVessel","createLinks");
  
  FixedComp::setConnect(0,Origin-Y*(length/2.0),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(rodIndex+1));

  FixedComp::setConnect(1,Origin+Y*(length/2.0),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(rodIndex+2));
  return;
}

void
TubeCollimator::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
    \param sideIndex :: surface offset
   */
{
  ELog::RegMethod RegA("TubeCollimator","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createTubes(System);
  createLinks();
  insertObjects(System);       



  return;
}


}
