/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/PreCollimator.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

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
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Tally.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "surfEqual.h"
#include "MergeSurf.h"
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
#include "KGroup.h"
#include "Source.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "chipDataStore.h"
#include "LinearComp.h"
#include "ContainedComp.h"
#include "PreCollimator.h"

namespace hutchSystem
{

PreCollimator::PreCollimator(const int N,const std::string& Key)  :
  zoomSystem::ContainedComp(),zoomSystem::LinearComp(),
  colIndex(N),keyName(Key),cellIndex(N+1),
  populated(0),shapeType(0),nLayers(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param N :: Index value
    \param Key :: Name for item in search
  */
{}

PreCollimator::PreCollimator(const PreCollimator& A) : 
  zoomSystem::ContainedComp(A),zoomSystem::LinearComp(A),
  colIndex(A.colIndex),keyName(A.keyName),cellIndex(A.cellIndex),
  populated(A.populated),insertCells(A.insertCells),Axis(A.Axis),
  XAxis(A.XAxis),ZAxis(A.ZAxis),shapeType(A.shapeType),
  shapeRadius(A.shapeRadius),
  fStep(A.fStep),xStep(A.xStep),zStep(A.zStep),Centre(A.Centre),
  width(A.width),height(A.height),depth(A.depth),defMat(A.defMat),
  innerWall(A.innerWall),innerWallMat(A.innerWallMat),
  nLayers(A.nLayers),cFrac(A.cFrac),cMat(A.cMat),CDivideList(A.CDivideList)
  /*!
    Copy constructor
    \param A :: PreCollimator to copy
  */
{}

PreCollimator&
PreCollimator::operator=(const PreCollimator& A)
  /*!
    Assignment operator
    \param A :: PreCollimator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      zoomSystem::ContainedComp::operator=(A);
      zoomSystem::LinearComp::operator=(A);
      cellIndex=A.cellIndex;
      populated=A.populated;
      insertCells=A.insertCells;
      Axis=A.Axis;
      XAxis=A.XAxis;
      ZAxis=A.ZAxis;
      shapeType=A.shapeType;
      shapeRadius=A.shapeRadius;
      fStep=A.fStep;
      xStep=A.xStep;
      zStep=A.zStep;
      Centre=A.Centre;
      width=A.width;
      height=A.height;
      depth=A.depth;
      defMat=A.defMat;
      innerWall=A.innerWall;
      innerWallMat=A.innerWallMat;
      nLayers=A.nLayers;
      cFrac=A.cFrac;
      cMat=A.cMat;
      CDivideList=A.CDivideList;
    }
  return *this;
}

PreCollimator::~PreCollimator() 
  /*!
    Destructor
  */
{}

void
PreCollimator::setShape(const int ST)
  /*!
    Set the output shape:
    Options are :
    - 0 : Nothing
    - 1 : Circle
    - 2 : Square
    - 3 : Hexagon
    - 4 : Octagon
    \param ST :: Shape type
  */
{
  ELog::RegMethod RegA("PreCollimator","setShape");

  if (ST<0 || ST>4)
    {
      ELog::EM<<"PreCollimator only has 5 shapes"<<ELog::endCrit;
      throw ColErr::IndexError<int>(ST,4,RegA.getBase()); 
    }
  shapeType=ST;

  return;
}

void
PreCollimator::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  const FuncDataBase& Control=System.getDataBase();

  if (Control.hasVariable(keyName+"ShapeType"))
    setShape(Control.EvalVar<int>(keyName+"ShapeType"));
  
  xyAngle=Control.EvalVar<double>(keyName+"XYAngle");
  zAngle=Control.EvalVar<double>(keyName+"ZAngle");
  fStep=Control.EvalVar<double>(keyName+"FStep");
  xStep=Control.EvalVar<double>(keyName+"XStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");

  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  shapeRadius=Control.EvalVar<double>(keyName+"Radius");
  defMat=Control.EvalVar<int>(keyName+"DefMat");

  innerWall=Control.EvalVar<double>(keyName+"InnerWall");
  innerWallMat=Control.EvalVar<int>(keyName+"InnerWallMat");

  // Layers
  nLayers=Control.EvalVar<int>(keyName+"NLayers");
  ModelSupport::populateDivideLen(Control,nLayers,
				  keyName+"Frac_",depth,
				  cFrac);
  ModelSupport::populateDivide(Control,nLayers,
			       keyName+"Mat_",defMat,cMat);

  populated |= 1;
  return;
}

void
PreCollimator::createUnitVector(const LinearComp& LC)
  /*!
    Create the unit vectors
    \param LC :: LinearComponent to attach to
  */
{
  ELog::RegMethod RegA("PreCollimator","createUnitVector");

  //  const masterRotate& MR=masterRotate::Instance();
//  chipIRDatum::chipDataStore& CS=chipIRDatum::chipDataStore::Instance();

  // Origin is in the wrong place as it is at the EXIT:
  LinearComp::createUnitBeamVector(LC);

  Origin+=X*xStep+Y*fStep+Z*zStep;
  const Geometry::Quaternion Qz=
    Geometry::Quaternion::calcQRotDeg(zAngle,X);
  const Geometry::Quaternion Qxy=
    Geometry::Quaternion::calcQRotDeg(xyAngle,Z);

  // Move centre before rotation
  if (!(populated & 2))
    Centre=Origin+Y*(depth/2.0);
 
  Axis=Y;
  XAxis=X;
  ZAxis=Z;
  Qz.rotate(Axis);
  Qz.rotate(ZAxis);
  Qxy.rotate(Axis);
  Qxy.rotate(XAxis);
  Qxy.rotate(ZAxis);
  
  ExitPoint=Origin+Axis*depth;
  return;
}

void
PreCollimator::createCircleSurfaces()
  /*!
    Construct inner shape with a circle.
    Initially a straight cyclinder cut along the axis
    later will have a cone cut.
  */
{
  ELog::RegMethod RegA("PreCollimator","createCircleSurfaces");
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  
  // inner cyclinder:
  Geometry::Cylinder* CX;
  CX=SurI.createUniqSurf<Geometry::Cylinder>(colIndex+31);  
  CX->setCylinder(Centre,Axis,shapeRadius);
  SMap.registerSurf(colIndex+31,CX);
  return;
}

std::string
PreCollimator::createCircleObj(Simulation& System) 
  /*!
    Create the void and liner objects.
    Note that the returned object will be passed via
    getComposite later.
    \param System :: Simulation to create objects in
    \return the exclusion surface object / set
  */
{
  ELog::RegMethod RegA("PreCollimator","createCircleObj");
  
  std::string Out=ModelSupport::getComposite(SMap,colIndex,"1 -2 -31");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  
  return ModelSupport::getComposite(SMap,colIndex,"31");
}

void
PreCollimator::createSquareSurfaces()
  /*!
    Construct inner shape with a square.
    Initially a straight cyclinder cut along the axis
    later will have a cone cut.
  */
{
  ELog::RegMethod RegA("PreCollimator","createSquareSurfaces");
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  
  // inner square
  Geometry::Plane* PX;
  PX=SurI.createUniqSurf<Geometry::Plane>(colIndex+33);  
  PX->setPlane(Centre-XAxis*shapeRadius,XAxis);
  SMap.registerSurf(colIndex+33,PX);

  PX=SurI.createUniqSurf<Geometry::Plane>(colIndex+34);  
  PX->setPlane(Centre+XAxis*shapeRadius,XAxis);
  SMap.registerSurf(colIndex+34,PX);

  PX=SurI.createUniqSurf<Geometry::Plane>(colIndex+35);  
  PX->setPlane(Centre-ZAxis*shapeRadius,ZAxis);
  SMap.registerSurf(colIndex+35,PX);

  PX=SurI.createUniqSurf<Geometry::Plane>(colIndex+36);  
  PX->setPlane(Centre+ZAxis*shapeRadius,ZAxis);
  SMap.registerSurf(colIndex+36,PX);
  return;
}

std::string
PreCollimator::createSquareObj(Simulation& System) 
  /*!
    Create the square cutout
    \param System :: Simulation to create objects in
    \return the exclusion surface object / set
  */
{
  ELog::RegMethod RegA("PreCollimator","createSquareObj");
  
  std::string Out=ModelSupport::getComposite(SMap,colIndex,"1 -2 33 -34 35 -36");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  
  return ModelSupport::getComposite(SMap,colIndex,"(-33 : 34 : -35 : 36)");
}

void
PreCollimator::createHexagonSurfaces()
  /*!
    Construct inner shape with a hexagon.
    Initially a straight Hexagon cut along the axis
    later will have a cone cut.
  */
{
  ELog::RegMethod RegA("PreCollimator","createHexagonSurfaces");
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  
  // inner hexagon
  Geometry::Plane* PX;
  double theta=0.0;
  for(int i=0;i<6;i++)
    {
      PX=SurI.createUniqSurf<Geometry::Plane>(colIndex+31+i);  
      const Geometry::Vec3D Norm=XAxis*sin(theta)+ZAxis*cos(theta);
      PX->setPlane(Centre+Norm*shapeRadius,Norm);
      SMap.registerSurf(colIndex+31+i,PX);
      theta+=2.0*M_PI/6.0;
    }
  return;
}

std::string
PreCollimator::createHexagonObj(Simulation& System) 
  /*!
    Create the hexagon cutout
    \param System :: Simulation to create objects in
    \return the exclusion surface object / set
  */
{
  ELog::RegMethod RegA("PreCollimator","createHexagonObj");
  
  std::string Out=ModelSupport::getComposite(SMap,colIndex,
					     "1 -2 -31 -32 -33 -34 -35 -36");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  return ModelSupport::getComposite(SMap,colIndex,
				    "(31 : 32 : 33 : 34 : 35 : 36)");
}

void
PreCollimator::createOctagonSurfaces()
  /*!
    Construct inner shape with a octagon.
    Initially a straight Octagon cut along the axis
    later will have a cone cut.
  */
{
  ELog::RegMethod RegA("PreCollimator","createOctagonSurfaces");
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  
  // inner Octagon
  Geometry::Plane* PX;
  double theta=0.0;
  for(int i=0;i<8;i++)
    {
      PX=SurI.createUniqSurf<Geometry::Plane>(colIndex+31+i);  
      const Geometry::Vec3D Norm=XAxis*sin(theta)+ZAxis*cos(theta);
      PX->setPlane(Centre+Norm*shapeRadius,Norm);
      SMap.registerSurf(colIndex+31+i,PX);
      theta+=2.0*M_PI/8.0;
    }
  return;
}

std::string
PreCollimator::createOctagonObj(Simulation& System) 
  /*!
    Create the octagon cutout
    \param System :: Simulation to create objects in
    \return the exclusion surface object / set
  */
{
  ELog::RegMethod RegA("PreCollimator","createOctagonObj");
  
  std::string Out=ModelSupport::getComposite(SMap,colIndex,
		     "1 -2 -31 -32 -33 -34 -35 -36 -37 -38 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  return ModelSupport::getComposite(SMap,colIndex,
		    "(31 : 32 : 33 : 34 : 35 : 36 : 37 : 38)");
}

void
PreCollimator::createSurfaces()
  /*!
    Create All the surfaces
   */
{
  ELog::RegMethod RegA("PreCollimator","createSurface");
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
 
  // INNER PLANES
  
  // Front
  Geometry::Plane* PX;
  PX=SurI.createUniqSurf<Geometry::Plane>(colIndex+1);  
  PX->setPlane(Centre-Axis*depth/2.0,Axis);
  SMap.registerSurf(colIndex+1,PX);
  // Back
  PX=SurI.createUniqSurf<Geometry::Plane>(colIndex+2);  
  PX->setPlane(Centre+Axis*depth/2.0,Axis);
  SMap.registerSurf(colIndex+2,PX);
  exitSurf=SMap.realSurf(colIndex+2);

  // up [dependent on Axis]
  PX=SurI.createUniqSurf<Geometry::Plane>(colIndex+6);  
  PX->setPlane(Centre+ZAxis*(height/2.0),ZAxis);
  SMap.registerSurf(colIndex+6,PX);

  // down
  PX=SurI.createUniqSurf<Geometry::Plane>(colIndex+5);  
  PX->setPlane(Centre-ZAxis*height/2.0,ZAxis);
  SMap.registerSurf(colIndex+5,PX);

  // left [Dept. on Axis]
  PX=SurI.createUniqSurf<Geometry::Plane>(colIndex+3);  
  PX->setPlane(Centre-XAxis*(width/2.0),XAxis);
  SMap.registerSurf(colIndex+3,PX);

  // right [Dept. on Axis]
  PX=SurI.createUniqSurf<Geometry::Plane>(colIndex+4);  
  PX->setPlane(Centre+XAxis*(width/2.0),XAxis);
  SMap.registerSurf(colIndex+4,PX);

  
  switch (shapeType)
    {
    case 0:   // No hole:
      break;
    case 1:   // Circle
      createCircleSurfaces();
      break;
    case 2:   // Square
      createSquareSurfaces();
      break;
    case 3:   // Hexagon
      createHexagonSurfaces();
      break;
    case 4:   // Octagon
      createOctagonSurfaces();
      break;
    default:
      ELog::EM<<"Unwritten bit"<<ELog::endErr;
    }

  return;
}

void
PreCollimator::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("PreCollimator","createObjects");

  std::string Out;
  Out=ModelSupport::getComposite(SMap,colIndex,"1 -2 3 -4 5 -6");
  addOuterSurf(Out);
  // Create Master box:
  std::string InnerExclude;
  switch (shapeType)
    {
    case 0:  // No shape
      Out+=" ";
      break;
    case 1:  // circle
      Out+=" "+createCircleObj(System);
      break;
    case 2:  // square
      Out+=" "+createSquareObj(System);
      break;
    case 3:  // hexagon
      Out+=" "+createHexagonObj(System);
      break;
    case 4:  // octagon
      Out+=" "+createOctagonObj(System);
      break;
    default:
      ELog::EM<<"Unwritten object component"<<ELog::endErr;
    }
  Out+=" "+getContainer();
  System.addCell(MonteCarlo::Qhull(cellIndex++,defMat,0.0,Out));      
  CDivideList.push_back(cellIndex-1);
  return;
}

void
PreCollimator::insertObjects(Simulation& System)
  /*!
    Create outer virtual space that includes the beamstop etc
    \param System :: Simulation to add to 
  */
{
  ELog::RegMethod RegA("PreCollimator","insertObjects");
  
  if (!hasOuterSurf()) return;

  std::vector<int>::const_iterator vc;
  for(vc=insertCells.begin();vc!=insertCells.end();vc++)
    {
      MonteCarlo::Qhull* outerObj=System.findQhull(*vc);
      if (outerObj)
	outerObj->addSurfString(getExclude());
      else
	ELog::EM<<"Failed to find outerObject: "<<*vc<<ELog::endErr;
    }
  return;
}


void 
PreCollimator::addInsertCell(const int CN)
  /*!
    Adds a cell to the insert list
    \param CN :: Cell number
  */
{
  insertCells.push_back(CN);
  return;
}

void 
PreCollimator::setCentre(const Geometry::Vec3D& C)
  /*!
    Sets the centre
    \param C :: Centre point
  */
{
  Centre=C;
  return;
}

void 
PreCollimator::layerProcess(Simulation& System)
  /*!
    Processes the splitting of the surfaces into a multilayer system
    This has to deal with the three layers that invade cells:
    
    \param System :: Simulation to work on
  */
{
  ELog::RegMethod RegA("PreCollimator","LayerProcess");
  
  if (!nLayers) return;
  ModelSupport::surfDivide DA;
  // Generic
  for(int i=0;i<nLayers-1;i++)
    {
      DA.addFrac(cFrac[i]);
      DA.addMaterial(cMat[i]);
    }
  DA.addMaterial(cMat[nLayers-1]);

  const int CSize=static_cast<int>(CDivideList.size());
  for(int i=0;i<CSize;i++)
    {
      DA.init();
      // Cell Specific:
      DA.setCellN(CDivideList[i]);
      DA.setOutNum(cellIndex,colIndex+201+100*i);

      DA.makePair<Geometry::Plane>(SMap.realSurf(colIndex+1),
				   -SMap.realSurf(colIndex+2));
      DA.activeDivide(System);
      cellIndex=DA.getCellNum();
    }
  return;
}

int
PreCollimator::exitWindow(const double Dist,
		    std::vector<int>& window,
		    Geometry::Vec3D& Pt) const
  /*!
    Outputs a window
    \param Dist :: Dist from exit point
    \param window :: window vector of paired planes
    \param Pt :: Output point for tally
    \return Master Plane
  */
{
  window.clear();
  window.push_back(SMap.realSurf(colIndex+3));
  window.push_back(SMap.realSurf(colIndex+4));
  window.push_back(SMap.realSurf(colIndex+5));
  window.push_back(SMap.realSurf(colIndex+6));
  Pt=Origin+Y*(depth+Dist);  
  return SMap.realSurf(colIndex+2);
}
  
void
PreCollimator::createAll(Simulation& System,
		      const zoomSystem::LinearComp& LC)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param LC :: Linear component to set axis etc
  */
{
  ELog::RegMethod RegA("PreCollimator","createAll");
  populate(System);

  createUnitVector(LC);
  createSurfaces();
  createObjects(System);
  layerProcess(System);
  insertObjects(System);
  
  return;
}
  
}  // NAMESPACE shutterSystem
