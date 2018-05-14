/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/RotaryCollimator.cxx
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
#include "surfEqual.h"
#include "generateSurf.h"
#include "objectRegister.h"
#include "surfDivide.h"
#include "surfDIter.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
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
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedGroup.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurInter.h"
#include "HoleShape.h"
#include "RotaryCollimator.h"

namespace constructSystem
{

RotaryCollimator::RotaryCollimator(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedGroup(Key,"Main",16,"Beam",3,"Hole",0),
  attachSystem::CellMap(),
  colIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(colIndex+1),holeIndex(0),nHole(0),nLayers(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

RotaryCollimator::RotaryCollimator(const RotaryCollimator& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedGroup(A),
  attachSystem::CellMap(A),
  colIndex(A.colIndex),cellIndex(A.cellIndex),xyAngle(A.xyAngle),
  zAngle(A.zAngle),xStep(A.xStep),yStep(A.yStep),
  zStep(A.zStep),rotDepth(A.rotDepth),radius(A.radius),
  thick(A.thick),defMat(A.defMat),holeIndex(A.holeIndex),
  holeAngOffset(A.holeAngOffset),innerWall(A.innerWall),
  innerWallMat(A.innerWallMat),nHole(A.nHole),nLayers(A.nLayers),
  Holes(A.Holes),cFrac(A.cFrac),cMat(A.cMat)
  /*!
    Copy constructor
    \param A :: RotaryCollimator to copy
  */
{}

RotaryCollimator&
RotaryCollimator::operator=(const RotaryCollimator& A)
  /*!
    Assignment operator
    \param A :: RotaryCollimator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedGroup::operator=(A);
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      rotDepth=A.rotDepth;
      radius=A.radius;
      thick=A.thick;
      defMat=A.defMat;
      holeIndex=A.holeIndex;
      holeAngOffset=A.holeAngOffset;
      innerWall=A.innerWall;
      innerWallMat=A.innerWallMat;
      nHole=A.nHole;
      nLayers=A.nLayers;
      Holes=A.Holes;
      cFrac=A.cFrac;
      cMat=A.cMat;
    }
  return *this;
}

RotaryCollimator::~RotaryCollimator() 
  /*!
    Destructor
  */
{}

void
RotaryCollimator::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Function database
  */
{
  ELog::RegMethod RegA("RotaryCollimator","populate");

  // NEED TO REGISTER objects
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();  
  
  nHole=Control.EvalVar<size_t>(keyName+"NHole");
  
  xStep=Control.EvalDefVar<double>(keyName+"XStep",0.0);
  yStep=Control.EvalDefVar<double>(keyName+"YStep",0.0);
  zStep=Control.EvalDefVar<double>(keyName+"ZStep",0.0);
  xyAngle=Control.EvalDefVar<double>(keyName+"XYAngle",0.0);
  zAngle=Control.EvalDefVar<double>(keyName+"ZAngle",0.0);

  rotDepth=Control.EvalVar<double>(keyName+"RotDepth");
  radius=Control.EvalVar<double>(keyName+"Radius");
  thick=Control.EvalVar<double>(keyName+"Thick");
  
  defMat=ModelSupport::EvalMat<int>(Control,keyName+"DefMat");

  innerWall=Control.EvalVar<double>(keyName+"InnerWall");
  innerWallMat=ModelSupport::EvalMat<int>(Control,keyName+"InnerWallMat");

  // Layers
  nLayers=Control.EvalVar<size_t>(keyName+"NLayers");
  ModelSupport::populateDivideLen(Control,nLayers,
				  keyName+"Frac",thick,
				  cFrac);
  ModelSupport::populateDivide(Control,nLayers,
			       keyName+"Mat",defMat,cMat);

  for(size_t i=0;i<nHole;i++)
    {
      const std::string holeName=keyName+"Hole"+
	StrFunc::makeString(i);
      Holes.push_back(std::shared_ptr<HoleShape>(new HoleShape(holeName)));
      Holes.back()->populate(Control);
      OR.addObject(Holes.back());
    }

  // Rotation angle:
  holeIndex=Control.EvalVar<size_t>(keyName+"HoleIndex");
  holeAngOffset=Control.EvalVar<double>(keyName+"HoleAngOff");
  setHoleIndex();
  for(std::shared_ptr<HoleShape>& HH : Holes)
    HH->setMasterAngle(holeAngOffset);
  
  return;
}

void
RotaryCollimator::createUnitVector(const attachSystem::FixedComp& FC,
				   const long int sideIndex)
  /*!
    Create the unit vectors. The vectors are created
    relative to the exit point
    \param FC :: Centre beamline
    \param sideIndex :: link index
  */
{
  ELog::RegMethod RegA("RotaryCollimator","createUnitVector");

  // Origin is in the wrong place as it is at the EXIT:

  attachSystem::FixedComp& mainFC=FixedGroup::getKey("Main");
  attachSystem::FixedComp& beamFC=FixedGroup::getKey("Beam");

  mainFC.createUnitVector(FC,sideIndex);
  beamFC.createUnitVector(FC,sideIndex);

  beamFC.applyShift(0,yStep,0);  
  mainFC.applyShift(xStep,yStep,zStep-rotDepth);  
  mainFC.applyAngleRotate(xyAngle,zAngle);

  setDefault("Main");
  return;
}


void
RotaryCollimator::createSurfaces()
  /*!
    Create All the surfaces
   */
{
  ELog::RegMethod RegA("RotaryCollimator","createSurface");
  // INNER PLANES
  
  // Front/Back
  ModelSupport::buildPlane(SMap,colIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,colIndex+2,Origin+Y*thick,Y);
  // Master Cylinder
  ModelSupport::buildCylinder(SMap,colIndex+7,Origin,Y,radius);

  return;
}

void
RotaryCollimator::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("RotaryCollimator","createObjects");

  std::string Out;
  Out=ModelSupport::getComposite(SMap,colIndex,"1 -2 -7");
  addOuterSurf(Out);
  //
  // Sectors
  // All via centre / at Angle from normal
  //
  HeadRule HoleExclude;
  for(size_t i=0;i<nHole;i++)
    {
      std::shared_ptr<HoleShape>& HU=Holes[i];
      HU->setFaces(SMap.realSurf(colIndex+1),-SMap.realSurf(colIndex+2));
      HU->createAllNoPopulate(System,*this,0);  // Use THIS Origin (rot centre)
      if (HU->getShape())
	HoleExclude.addIntersection(HU->getExclude());
      addCell("Void"+StrFunc::makeString(i),HU->getCell("Void"));
    }
  Out+=" "+HoleExclude.display();
  System.addCell(MonteCarlo::Qhull(cellIndex++,defMat,0.0,Out));            
  addCell("Main",cellIndex-1);
  return;
}

void
RotaryCollimator::setHoleIndex()
  /*!
    Given a hole index determine and set the angle offset
  */
{
  ELog::RegMethod RegA("RotaryCollimator","setHoleIndex");
  if (holeIndex)
    {
      if (holeIndex>Holes.size())
	throw ColErr::IndexError<size_t>(holeIndex,Holes.size(),
					 "vector<Hole> :: holeIndex");
      holeAngOffset= Holes[holeIndex-1]->getCentreAngle();
    }
  return;
}

void
RotaryCollimator::createLinks()
  /*!
    Create the linkes
   */
{
  ELog::RegMethod RegA("RotaryCollimator","createLinks");

  
  attachSystem::FixedComp& mainFC=FixedGroup::getKey("Main");
  attachSystem::FixedComp& beamFC=FixedGroup::getKey("Beam");
  attachSystem::FixedComp& holeFC=FixedGroup::getKey("Hole");
  // 
  const std::string Out=getExclude();
  HeadRule HM(Out);
  std::pair<Geometry::Vec3D,int> PtA=
    SurInter::interceptRule(HM,beamFC.getCentre(),beamFC.getY());
  std::pair<Geometry::Vec3D,int> PtB=
    SurInter::interceptRule(HM,beamFC.getCentre()+beamFC.getY()*thick,
			    beamFC.getY());

  beamFC.setConnect(0,PtA.first,-beamFC.getY());
  beamFC.setConnect(1,PtB.first,beamFC.getY());
  beamFC.setConnect(2,(PtB.first+PtA.first)/2,
                    beamFC.getY());  // mid point [NO SURF]
  beamFC.setLinkSurf(0,-SMap.realSurf(colIndex+1));
  beamFC.setLinkSurf(1,SMap.realSurf(colIndex+2));

  mainFC.setConnect(0,Origin,-Y);
  mainFC.setConnect(1,Origin+Y*thick,Y);
  mainFC.setLinkSurf(0,-SMap.realSurf(colIndex+1));
  mainFC.setLinkSurf(1,SMap.realSurf(colIndex+2));

  const size_t nExtra(6);
  const double angleStep(2.0*M_PI/nExtra);
  double angle(0.0);
  for(size_t i=0;i<nExtra;i++)
    {
      const Geometry::Vec3D Axis(X*cos(angle)+Z*sin(angle));
      mainFC.setConnect(i+2,Origin+Y*(thick/2.0)+Axis*radius,Axis);
      mainFC.setLinkSurf(i+2,SMap.realSurf(colIndex+7));
      angle+=angleStep;
    }
  // Front/back points as well
  const Geometry::Vec3D ADir[4]{-X,X,-Z,Z};
  for(size_t i=0;i<4;i++)
    {
      // front
      mainFC.setConnect(i+8,Origin+ADir[i]*radius,-Y);
      mainFC.setLinkSurf(i+8,-SMap.realSurf(colIndex+1));
      // back
      mainFC.setConnect(i+12,Origin+Y*thick+ADir[i]*radius,Y);
      mainFC.setLinkSurf(i+12,SMap.realSurf(colIndex+2));
    }

  // Process holes:
  if (nHole)
    {
      holeFC.setNConnect(3*nHole);
      size_t index(0);
      for(size_t i=0;i<nHole;i++)
        {
          holeFC.setLinkSignedCopy(index,*Holes[i],1);
          holeFC.setLinkSignedCopy(index+1,*Holes[i],2);
          const Geometry::Vec3D midPt((Holes[i]->getLinkPt(1)+
                                      Holes[i]->getLinkPt(2))/2.0);
          holeFC.setConnect(index+2,midPt,Holes[i]->getLinkAxis(1));
          index+=3;
        }
    }
  
  return;
}
  
void 
RotaryCollimator::layerProcess(Simulation&)
  /*!
    Processes the splitting of the surfaces into a multilayer system
    This has to deal with the three layers that invade cells:
    
    \param  :: Simulation to work on
  */
{
  ELog::RegMethod RegA("RotaryCollimator","LayerProcess");
  
  return;
}

void
RotaryCollimator::createAll(Simulation& System,
			    const attachSystem::FixedComp& FC,
			    const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Link component
    \param sideIndex :: link index
  */
{
  ELog::RegMethod RegA("RotaryCollimator","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  layerProcess(System);
  createLinks();
  insertObjects(System);
  
  return;
}
  
}  // NAMESPACE constructSystem
