/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   imat/IMatPrimGuide.cxx
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
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
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "IMatGuide.h"
#include "IMatPrimGuide.h"

namespace imatSystem
{

IMatPrimGuide::IMatPrimGuide(const std::string& Key)  :
  IMatGuide(Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}


IMatPrimGuide::~IMatPrimGuide() 
 /*!
   Destructor
 */
{}

void
IMatPrimGuide::populate(const Simulation& System)
 /*!
   Populate all the variables
   \param System :: Simulation to use
 */
{
  ELog::RegMethod RegA("IMatPrimGuide","populate");

  IMatGuide::populate(System);
  
  const FuncDataBase& Control=System.getDataBase();

  feCut=Control.EvalVar<double>(keyName+"FeCut");
  feCutLen=Control.EvalVar<double>(keyName+"FeCutLen");

  wallCut=Control.EvalVar<double>(keyName+"WallCut");
  wallCutLen=Control.EvalVar<double>(keyName+"WallCutLen");

  return;
}
  
void
IMatPrimGuide::createSurfaces()
  /*!
    Create All the surfaces
    -- Outer plane is going 
  */
{
  ELog::RegMethod RegA("IMatPrimGuide","createSurfaces");
  
  IMatGuide::createSurfaces();

  // Inner void layers
  double xside(width/2.0+feSide+voidSide);
  // Corners:
  ModelSupport::buildPlane(SMap,guideIndex+47,
			   Origin-bX*(xside-feCut),
			   Origin-bX*(xside-feCut)+bZ,
			   Origin-bX*xside+bY*feCutLen,
			   bX);

  xside+=wallSide;
  ModelSupport::buildPlane(SMap,guideIndex+57,
			   Origin-bX*(xside-wallCut),
			   Origin-bX*(xside-wallCut)+bZ,
			   Origin-bX*xside+bY*wallCutLen,
			   bX);

  return;
}

void
IMatPrimGuide::createObjects(Simulation& System,
			 const attachSystem::FixedComp& FC)
  /*!
    Adds the BeamLne components
    \param System :: Simulation to add beamline to
    \param FC :: Fixed component (insert boundary)
  */
{
  ELog::RegMethod RegA("IMatPrimGuide","createObjects");
  
  const std::string insertEdge=FC.getLinkString(2);
  std::string Out;
  Out=ModelSupport::getComposite(SMap,guideIndex," -2 33 -34 35 -36 ");
  addOuterSurf("Inner",Out);
  Out=ModelSupport::getComposite(SMap,guideIndex," -2 57 53 -54 55 -56 ");
  addOuterSurf("Wall",Out+insertEdge);

  // Inner void cell:
  Out=ModelSupport::getComposite(SMap,guideIndex," -2 3 -4 5 -6 ")+insertEdge;
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  // Glass layer:
  Out=ModelSupport::getComposite(SMap,guideIndex,
				 "-2 13 -14 15 -16 (-3:4:-5:6) ")+insertEdge;
  System.addCell(MonteCarlo::Qhull(cellIndex++,glassMat,0.0,Out));
  // Box layer:
  Out=ModelSupport::getComposite(SMap,guideIndex,"-2 23 -24 25 -26 "
				 "(-13:14:-15:16) ")+insertEdge;
  System.addCell(MonteCarlo::Qhull(cellIndex++,boxMat,0.0,Out));

  // Void layer:
  Out=ModelSupport::getComposite(SMap,guideIndex,"-2 33 -34 35 -36 "
				 "(-23:24:-25:26) ")+insertEdge;
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  // Fe layer:
  Out=ModelSupport::getComposite(SMap,guideIndex,"-2 47 43 -44 45 -46 "
				 "(-33:34:-35:36) ")+insertEdge;
  System.addCell(MonteCarlo::Qhull(cellIndex++,feMat,0.0,Out));

  // Wall layer:
  Out=ModelSupport::getComposite(SMap,guideIndex,"-2 57 53 -54 55 -56 "
				 "(-43:44:-45:46:-47) ")+insertEdge;
  System.addCell(MonteCarlo::Qhull(cellIndex++,feMat,0.0,Out));
  
  return;
}

void
IMatPrimGuide::createLinks()
  /*!
    Create All the links:
    - 0 : First surface
    - 1 : Exit surface
    - 2 : Inner face
  */
{
  ELog::RegMethod RegA("IMatPrimGuide","createLinks");
  IMatGuide::createLinks();
  return;
}

void
IMatPrimGuide::createAll(Simulation& System,const attachSystem::TwinComp& TC)
  /*!
    Global creation of the vac-vessel
    \param System :: Simulation to add vessel to
    \param TC :: BulkInsert/IMatInsert
  */
{
  ELog::RegMethod RegA("IMatPrimGuide","createAll");
  populate(System);

  createUnitVector(TC);
  createSurfaces();
  createObjects(System,TC);
  insertObjects(System); 
  createLinks();

  return;
}

  
}  // NAMESPACE imatSystem
