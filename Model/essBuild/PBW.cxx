/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/PBW.cxx
 *
 * Copyright (c) 2004-2018 by Konstantin Batkov
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
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "inputParam.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ReadFunctions.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "SpaceCut.h"
#include "ContainedSpace.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "FixedOffset.h"
#include "surfDBase.h"
#include "surfDIter.h"
#include "surfDivide.h"
#include "SurInter.h"
#include "mergeTemplate.h"
#include "AttachSupport.h"

#include "CellMap.h"
#include "FrontBackCut.h"
#include "TelescopicPipe.h"
#include "PBW.h"

namespace essSystem
{

PBW::PBW(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,8),
  shield(new TelescopicPipe(Key+"Shield"))
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{
  ELog::RegMethod RegA("PBW","PBW");

  ModelSupport::objectRegister& OR = ModelSupport::objectRegister::Instance();
  OR.addObject(shield);

  return;
}

PBW::PBW(const PBW& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),

  engActive(A.engActive),
  plugLength1(A.plugLength1),
  plugLength2(A.plugLength2),
  plugWidth1(A.plugWidth1),
  plugWidth2(A.plugWidth2),
  plugHeight(A.plugHeight),
  plugDepth(A.plugDepth),
  plugMat(A.plugMat),
  plugVoidLength(A.plugVoidLength),
  plugVoidWidth(A.plugVoidWidth),
  plugVoidDepth(A.plugVoidDepth),
  plugVoidHeight(A.plugVoidHeight),
  plugAlLength(A.plugAlLength),
  plugAlGrooveRadius(A.plugAlGrooveRadius),
  plugAlGrooveDepth(A.plugAlGrooveDepth),
  plugAlGapHeight(A.plugAlGapHeight),
  plugAlGapWidth(A.plugAlGapWidth),
  flangeRadius(A.flangeRadius),
  flangeThick(A.flangeThick),
  flangeWaterRingRadiusIn(A.flangeWaterRingRadiusIn),
  flangeWaterRingRadiusOut(A.flangeWaterRingRadiusOut),
  flangeWaterRingThick(A.flangeWaterRingThick),
  flangeWaterRingOffset(A.flangeWaterRingOffset),
  flangeNotchDepth(A.flangeNotchDepth),
  flangeNotchThick(A.flangeNotchThick),
  flangeNotchOffset(A.flangeNotchOffset),
  foilThick(A.foilThick),
  foilRadius(A.foilRadius),
  foilOffset(A.foilOffset),
  foilCylOffset(A.foilCylOffset),
  foilWaterThick(A.foilWaterThick),
  foilWaterLength(A.foilWaterLength),
  pipeRad(A.pipeRad),
  pipeMatBefore(A.pipeMatBefore),
  pipeMatAfter(A.pipeMatAfter),
  coolingMat(A.coolingMat),mat(A.mat),
  shield(A.shield->clone())
  /*!
    Copy constructor
    \param A :: PBW to copy
  */
{}

PBW&
PBW::operator=(const PBW& A)
  /*!
    Assignment operator
    \param A :: PBW to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      cellIndex=A.cellIndex;
      engActive=A.engActive;
      plugLength1=A.plugLength1;
      plugLength2=A.plugLength2;
      plugWidth1=A.plugWidth1;
      plugWidth2=A.plugWidth2;
      plugHeight=A.plugHeight;
      plugDepth=A.plugDepth;
      plugMat=A.plugMat;
      plugVoidLength=A.plugVoidLength;
      plugVoidWidth=A.plugVoidWidth;
      plugVoidDepth=A.plugVoidDepth;
      plugVoidHeight=A.plugVoidHeight;
      plugAlLength=A.plugAlLength;
      plugAlGrooveRadius=A.plugAlGrooveRadius;
      plugAlGrooveDepth=A.plugAlGrooveDepth;
      plugAlGapHeight=A.plugAlGapHeight;
      plugAlGapWidth=A.plugAlGapWidth;
      flangeRadius=A.flangeRadius;
      flangeThick=A.flangeThick;
      flangeWaterRingRadiusIn=A.flangeWaterRingRadiusIn;
      flangeWaterRingRadiusOut=A.flangeWaterRingRadiusOut;
      flangeWaterRingThick=A.flangeWaterRingThick;
      flangeWaterRingOffset=A.flangeWaterRingOffset;
      flangeNotchDepth=A.flangeNotchDepth;
      flangeNotchThick=A.flangeNotchThick;
      flangeNotchOffset=A.flangeNotchOffset;
      foilThick=A.foilThick;
      foilRadius=A.foilRadius;
      foilOffset=A.foilOffset;
      foilCylOffset=A.foilCylOffset;
      foilWaterThick=A.foilWaterThick;
      foilWaterLength=A.foilWaterLength;
      pipeRad=A.pipeRad;
      pipeMatBefore=A.pipeMatBefore;
      pipeMatAfter=A.pipeMatAfter;
      coolingMat=A.coolingMat;
      mat=A.mat;
      *shield=*A.shield;
    }
  return *this;
}

PBW*
PBW::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new PBW(*this);
}

PBW::~PBW()
  /*!
    Destructor
  */
{}

void
PBW::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("PBW","populate");

  FixedOffset::populate(Control);
  engActive=Control.EvalPair<int>(keyName,"","EngineeringActive");

  plugLength1=Control.EvalVar<double>(keyName+"PlugLength1");
  plugLength2=Control.EvalVar<double>(keyName+"PlugLength2");
  plugWidth1=Control.EvalVar<double>(keyName+"PlugWidth1");
  plugWidth2=Control.EvalVar<double>(keyName+"PlugWidth2");
  plugHeight=Control.EvalVar<double>(keyName+"PlugHeight");
  plugDepth=Control.EvalVar<double>(keyName+"PlugDepth");
  plugMat=ModelSupport::EvalMat<int>(Control,keyName+"PlugMat");
  plugVoidLength=Control.EvalVar<double>(keyName+"PlugVoidLength");
  plugVoidWidth=Control.EvalVar<double>(keyName+"PlugVoidWidth");
  plugVoidDepth=Control.EvalVar<double>(keyName+"PlugVoidDepth");
  plugVoidHeight=Control.EvalVar<double>(keyName+"PlugVoidHeight");
  plugAlLength=Control.EvalVar<double>(keyName+"PlugAlLength");
  plugAlGrooveRadius=Control.EvalVar<double>(keyName+"PlugAlGrooveRadius");
  plugAlGrooveDepth=Control.EvalVar<double>(keyName+"PlugAlGrooveDepth");
  plugAlGapHeight=Control.EvalVar<double>(keyName+"PlugAlGapHeight");
  plugAlGapWidth=Control.EvalVar<double>(keyName+"PlugAlGapWidth");
  flangeRadius=Control.EvalVar<double>(keyName+"FlangeRadius");
  flangeThick=Control.EvalVar<double>(keyName+"FlangeThick");
  flangeWaterRingRadiusIn=Control.EvalVar<double>(keyName+"FlangeWaterRingRadiusIn");
  flangeWaterRingRadiusOut=Control.EvalVar<double>(keyName+"FlangeWaterRingRadiusOut");
  flangeWaterRingThick=Control.EvalVar<double>(keyName+"FlangeWaterRingThick");
  flangeWaterRingOffset=Control.EvalVar<double>(keyName+"FlangeWaterRingOffset");
  flangeNotchDepth=Control.EvalVar<double>(keyName+"FlangeNotchDepth");
  flangeNotchThick=Control.EvalVar<double>(keyName+"FlangeNotchThick");
  flangeNotchOffset=Control.EvalVar<double>(keyName+"FlangeNotchOffset");
  foilThick=Control.EvalVar<double>(keyName+"FoilThick");
  foilRadius=Control.EvalVar<double>(keyName+"FoilRadius");
  foilOffset=Control.EvalVar<double>(keyName+"FoilOffset");
  foilCylOffset=Control.EvalVar<double>(keyName+"FoilCylOffset");
  foilWaterThick=Control.EvalVar<double>(keyName+"FoilWaterThick");
  foilWaterLength=Control.EvalVar<double>(keyName+"FoilWaterLength");
  pipeRad=Control.EvalVar<double>(keyName+"PipeRadius");
  pipeMatBefore=ModelSupport::EvalMat<int>(Control,keyName+"PipeMatBefore");
  pipeMatAfter=ModelSupport::EvalMat<int>(Control,keyName+"PipeMatAfter");

  coolingMat=ModelSupport::EvalMat<int>(Control,keyName+"CoolingMat");
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  return;
}

void
PBW::createUnitVector(const attachSystem::FixedComp& FC,const long int& sideIndex)
  /*!
    Create the unit vectors
    \param FC :: object for origin
  */
{
  ELog::RegMethod RegA("PBW","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,zAngle);

  return;
}

void
PBW::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("PBW","createSurfaces");

  // plug
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(plugLength2),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(plugLength1),Y);

  const double alpha = atan((plugWidth1-plugWidth2)/2.0/(plugLength1+plugLength2))*180/M_PI;

  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+3,
				  Origin-X*(plugWidth1/2.0)+Y*(plugLength1),
				  X,Z,alpha);
  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+4,
				  Origin+X*(plugWidth1/2.0)+Y*(plugLength1),
				  X,Z,-alpha);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(plugDepth),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(plugHeight),Z);

  // proton tube rad
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,pipeRad);

  // plug void
  ModelSupport::buildPlane(SMap,buildIndex+11,Origin-Y*(plugVoidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(plugVoidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(plugVoidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(plugVoidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(plugVoidDepth),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(plugVoidHeight),Z);

  // flanges
  //         water rings
  ModelSupport::buildShiftedPlane(SMap,buildIndex+21,
				  SMap.realPtr<Geometry::Plane>(buildIndex+11),
				  flangeWaterRingOffset);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+22,
				  SMap.realPtr<Geometry::Plane>(buildIndex+21),
				  flangeWaterRingThick);

  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Y,flangeRadius);
  const double cyl28rad = flangeRadius+flangeThick; // used as half height of plug Al plate
  ModelSupport::buildCylinder(SMap,buildIndex+28,Origin,Y,cyl28rad);

  ModelSupport::buildCylinder(SMap,buildIndex+29,Origin,Y,flangeWaterRingRadiusIn);
  ModelSupport::buildCylinder(SMap,buildIndex+30,Origin,Y,flangeWaterRingRadiusOut);

  ModelSupport::buildShiftedPlane(SMap,buildIndex+31,
				  SMap.realPtr<Geometry::Plane>(buildIndex+12),
				  -flangeWaterRingOffset);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+32,
				  SMap.realPtr<Geometry::Plane>(buildIndex+31),
				  -flangeWaterRingThick);

  //         1st flange notch
  ModelSupport::buildShiftedPlane(SMap,buildIndex+41,
				  SMap.realPtr<Geometry::Plane>(buildIndex+11),
				  flangeNotchOffset);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+42,
				  SMap.realPtr<Geometry::Plane>(buildIndex+41),
				  flangeNotchThick);
  ModelSupport::buildCylinder(SMap,buildIndex+47,Origin,Y,
			      flangeRadius+flangeThick-flangeNotchDepth);
  //          2nd flange notch
  ModelSupport::buildShiftedPlane(SMap,buildIndex+61,
				  SMap.realPtr<Geometry::Plane>(buildIndex+12),
				  -flangeNotchOffset);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+62,
				  SMap.realPtr<Geometry::Plane>(buildIndex+61),
				  -flangeNotchThick);

  // Plug Al plate
  ModelSupport::buildPlane(SMap,buildIndex+71,Origin-Y*(plugAlLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+72,Origin+Y*(plugAlLength/2.0),Y);

  ModelSupport::buildPlane(SMap,buildIndex+73,Origin-X*(cyl28rad),X);
  ModelSupport::buildPlane(SMap,buildIndex+74,Origin+X*(cyl28rad),X);

  ModelSupport::buildPlane(SMap,buildIndex+75,Origin-Z*(cyl28rad),Z);
  ModelSupport::buildPlane(SMap,buildIndex+76,Origin+Z*(cyl28rad),Z);

  // Plug Al plate - groove
  ModelSupport::buildPlane(SMap,buildIndex+81,
			   Origin-Y*(plugAlLength/2.0-plugAlGrooveDepth),Y);
  ModelSupport::buildPlane(SMap,buildIndex+82,
			   Origin+Y*(plugAlLength/2.0-plugAlGrooveDepth),Y);
  ModelSupport::buildCylinder(SMap,buildIndex+87,Origin,Y,plugAlGrooveRadius);

  // Plug Al plate - gap
  ModelSupport::buildPlane(SMap,buildIndex+93,Origin-X*(plugAlGapWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+94,Origin+X*(plugAlGapWidth/2.0),X);

  ModelSupport::buildPlane(SMap,buildIndex+95,Origin-Z*(plugAlGapHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+96,Origin+Z*(plugAlGapHeight/2.0),Z);

  // PBW foil
  ModelSupport::buildShiftedPlane(SMap,buildIndex+101,
				  SMap.realPtr<Geometry::Plane>(buildIndex+82),
				  -foilOffset-foilThick);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+102,
				  SMap.realPtr<Geometry::Plane>(buildIndex+101),
				  foilThick);
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin+Y*(foilCylOffset),X,foilRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+108,Origin+Y*(foilCylOffset),X,foilRadius+foilThick);

  // PBW foil - water
  ModelSupport::buildPlane(SMap,buildIndex+115,Origin-Z*(foilWaterLength/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+116,Origin+Z*(foilWaterLength/2.0),Z);
  ModelSupport::buildCylinder(SMap,buildIndex+117,Origin+Y*(foilCylOffset),X,
			      (foilRadius+foilRadius+foilThick-foilWaterThick)/2.0);
  ModelSupport::buildCylinder(SMap,buildIndex+118,Origin+Y*(foilCylOffset),X,
			      (foilRadius+foilRadius+foilThick+foilWaterThick)/2.0);

  return;
}

void
PBW::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("PBW","createObjects");

  std::string Out;

  // plug
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 11 -12 13 -14 15 -16 28 (-71:72:-73:74:-75:76) "); // outer void
  System.addCell(MonteCarlo::Qhull(cellIndex++,pipeMatAfter,0.0,Out));

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 1 -2 3 -4 5 -6 7 (-11:12:-13:14:-15:16) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,plugMat,0.0,Out));

  // proton tube void
  Out=ModelSupport::getComposite(SMap,buildIndex, " -7 12 -2");
  System.addCell(MonteCarlo::Qhull(cellIndex++,pipeMatAfter,0.0,Out));
  Out=ModelSupport::getComposite(SMap,buildIndex, " -7 1 -11");
  System.addCell(MonteCarlo::Qhull(cellIndex++,pipeMatBefore,0.0,Out));

  // flange cylinder
  // inner steel
  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -71 27 -29 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,plugMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,buildIndex," 72 -12 27 -29 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,plugMat,0.0,Out));

  // flange cylinder - inner layer
  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -21 29 -30 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,plugMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,buildIndex," 21 -22 29 -30 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,coolingMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,buildIndex," 22 -41 29 -30 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,plugMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,buildIndex," 41 -42 29 -47 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,plugMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,buildIndex," 42 -71 29 -28 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,plugMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,buildIndex," 72 -62 29 -28 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,plugMat,0.0,Out));


  Out=ModelSupport::getComposite(SMap,buildIndex," 62 -61 29 -47 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,plugMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,buildIndex," 61 -32 29 -30 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,plugMat,0.0,Out));


  Out=ModelSupport::getComposite(SMap,buildIndex," 32 -31 29 -30 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,coolingMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,buildIndex," 31 -12 29 -30 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,plugMat,0.0,Out));

  // flange - outer steel
  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -41 30 -28 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,plugMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,buildIndex," 41 -42 47 -28 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,pipeMatAfter,0.0,Out));

  Out=ModelSupport::getComposite(SMap,buildIndex," 62 -61 47 -28 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,pipeMatAfter,0.0,Out));
  Out=ModelSupport::getComposite(SMap,buildIndex," 61 -12 30 -28 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,plugMat,0.0,Out));

  // void inside PBW main cell
  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -81 -27 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,pipeMatBefore,0.0,Out));
  Out=ModelSupport::getComposite(SMap,buildIndex," 82 -12 -27 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,pipeMatAfter,0.0,Out));

  // PBW Al plate
  Out=ModelSupport::getComposite(SMap,buildIndex," 71 -81 27 -87 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,plugMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,buildIndex," 71 -81 87 73 -74 75 -76 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 81 -82 73 -74 75 -76 (-93:94:-95:96)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,buildIndex," 82 -72 27 -87 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,plugMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,buildIndex," 82 -72 87 73 -74 75 -76 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));

  // PBW foil
  Out=ModelSupport::getComposite(SMap,buildIndex, " 81 -101 108 93 -94 95 -96 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,pipeMatBefore,0.0,Out));

  Out=ModelSupport::getComposite(SMap,buildIndex, " -107 -102 93 -94 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,pipeMatAfter,0.0,Out));

  Out=ModelSupport::getComposite(SMap,buildIndex, " 101 -102 93 -94 95 -96 107");
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,buildIndex, " 102 -82 93 -94 95 -96 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,pipeMatAfter,0.0,Out));

  // cylindrical segment:
  //                      sides
  Out=ModelSupport::getComposite(SMap,buildIndex, " 107 -108 -101 116 93 -94 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,buildIndex, " 107 -108 -101 -115 93 -94 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));
  // central cylindrical segment with water:
  if (std::abs(foilThick-foilWaterThick)>Geometry::zeroTol)
    {
    Out=ModelSupport::getComposite(SMap,buildIndex, " 107 -117 -101 115 -116 93 -94 ");
    System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));
    Out=ModelSupport::getComposite(SMap,buildIndex, " 117 -118 -101 115 -116 93 -94 ");
    System.addCell(MonteCarlo::Qhull(cellIndex++,coolingMat,0.0,Out));
    Out=ModelSupport::getComposite(SMap,buildIndex, " 118 -108 -101 115 -116 93 -94 ");
    System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));
    }
  else // no side Al layers
    {
    Out=ModelSupport::getComposite(SMap,buildIndex, " 107 -108 -101 115 -116 93 -94 ");
    System.addCell(MonteCarlo::Qhull(cellIndex++,coolingMat,0.0,Out));
    }

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 3 -4 5 -6 ");
  addOuterSurf(Out);

  return;
}


void
PBW::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("PBW","createLinks");

  FixedComp::setConnect(0,Origin-Y*(plugLength2),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));

  FixedComp::setConnect(1,Origin+Y*(plugLength1),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));

  FixedComp::setConnect(2,Origin-X*(plugWidth1/2.0)+Y*(plugLength1),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));

  FixedComp::setConnect(3,Origin-X*(plugWidth2/2.0)-Y*(plugLength2),-X);
  FixedComp::setLinkSurf(3,-SMap.realSurf(buildIndex+3));

  FixedComp::setConnect(4,Origin+X*(plugWidth1/2.0)+Y*(plugLength1),X);
  FixedComp::setLinkSurf(4,SMap.realSurf(buildIndex+4));

  FixedComp::setConnect(5,Origin+X*(plugWidth2/2.0)-Y*(plugLength2),X);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+4));

  FixedComp::setConnect(6,Origin-Z*(plugDepth),-Z);
  FixedComp::setLinkSurf(6,-SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(7,Origin+Z*(plugHeight),Z);
  FixedComp::setLinkSurf(7,SMap.realSurf(buildIndex+6));

  return;
}




void
PBW::createAll(Simulation& System,
	       const attachSystem::FixedComp& FC,const long int& lp,
	       const attachSystem::FixedComp& SB,const long int& sblp)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param lp :: FC link point
    \param SB :: shield end link point
    \param sblp :: SB link point
  */
{
  ELog::RegMethod RegA("PBW","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,lp);
  createSurfaces();
  createLinks();
  createObjects(System);
  insertObjects(System);

  if (engActive)
    {
      shield->setFront(*this,8);
      shield->setBack(SB,sblp);
      shield->createAll(System,*this,8);
      attachSystem::addToInsertSurfCtrl(System,SB,shield->getCC("Full"));
    }

  return;
}

}  // essSystem essSystem
