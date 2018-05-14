/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   insertUnit/insertGrid.cxx
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
#include "Zaid.h"
#include "MXcards.h"
#include "Material.h"
#include "DBMaterial.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h"
#include "ContainedComp.h"
#include "FrontBackCut.h"
#include "SurInter.h"
#include "AttachSupport.h"
#include "insertObject.h"
#include "insertGrid.h"

namespace insertSystem
{

insertGrid::insertGrid(const std::string& Key)  :
  insertObject(Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

insertGrid::insertGrid(const insertGrid& A) : 
  insertSystem::insertObject(A),
  innerWidth(A.innerWidth),innerHeight(A.innerHeight),
  innerDepth(A.innerDepth),plateAngle(A.plateAngle),
  nLayer(A.nLayer),wallThick(A.wallThick),wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: insertGrid to copy
  */
{}

insertGrid&
insertGrid::operator=(const insertGrid& A)
  /*!
    Assignment operator
    \param A :: insertGrid to copy
    \return *this
  */
{
  if (this!=&A)
    {
      insertSystem::insertObject::operator=(A);
      innerWidth=A.innerWidth;
      innerHeight=A.innerHeight;
      innerDepth=A.innerDepth;
      plateAngle=A.plateAngle;
      nLayer=A.nLayer;
      wallThick=A.wallThick;
      wallMat=A.wallMat;
    }
  return *this;
}



insertGrid::~insertGrid() 
  /*!
    Destructor
  */
{}

void
insertGrid::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Data Base
  */
{
  ELog::RegMethod RegA("insertGrid","populate");
  
  if (!populated)
    {
      insertObject::populate(Control);
      innerWidth=Control.EvalVar<double>(keyName+"Width");
      innerHeight=Control.EvalVar<double>(keyName+"Height");
      innerDepth=Control.EvalVar<double>(keyName+"Depth");
      plateAngle=Control.EvalDefVar<double>(keyName+"PlateAngle",0.0);
      
      nLayer=Control.EvalVar<size_t>(keyName+"NLayer");
      for(size_t i=0;i<nLayer;i++)
	{
	  const std::string NStr(StrFunc::makeString(i));
	  wallThick.push_back
	    (Control.EvalPair<double>(keyName+"Thick"+NStr,keyName+"Thick"));
	  wallMat.push_back(ModelSupport::EvalMat<int>
			    (Control,keyName+"Mat"+NStr,keyName+"Mat"));
	}
    }
  return;
}

void
insertGrid::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("insertGrid","createSurface");

  if (!frontActive())
    ModelSupport::buildPlane(SMap,ptIndex+1,Origin-Y*innerDepth/2.0,Y);
  if (!backActive())
    ModelSupport::buildPlane(SMap,ptIndex+2,Origin+Y*innerDepth/2.0,Y);

  const Geometry::Quaternion Qz=
    Geometry::Quaternion::calcQRotDeg(plateAngle,X);
  const Geometry::Quaternion Qx=
    Geometry::Quaternion::calcQRotDeg(plateAngle,Z);

  // build inner plate
  ModelSupport::buildPlane(SMap,ptIndex+3,Origin-X*(innerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,ptIndex+4,Origin+X*(innerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,ptIndex+5,Origin-Z*(innerHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,ptIndex+6,Origin+Z*(innerHeight/2.0),Z);

  setSurf("InnerLeft",SMap.realSurf(ptIndex+3));
  setSurf("InnerRight",SMap.realSurf(ptIndex+4));
  setSurf("InnerBase",SMap.realSurf(ptIndex+5));
  setSurf("InnerTop",SMap.realSurf(ptIndex+6));

  int PI(ptIndex+50);
  double TThick(0.0);
  for(const double T : wallThick)
    {
      TThick+=T;
      ModelSupport::buildPlane(SMap,PI+3,Origin-X*(T+innerWidth/2.0),X);
      ModelSupport::buildPlane(SMap,PI+4,Origin+X*(T+innerWidth/2.0),X);
      ModelSupport::buildPlane(SMap,PI+5,Origin-Z*(T+innerHeight/2.0),Z);
      ModelSupport::buildPlane(SMap,PI+6,Origin+Z*(T+innerHeight/2.0),Z); 
      PI+=50;
    }
  
  if (!frontActive())
    setSurf("Front",ptIndex+1);
  else
    setSurf("Front",getFrontRule().getPrimarySurface());

  if (!backActive())
    setSurf("Back",SMap.realSurf(ptIndex+2));
  else
    setSurf("Back",getBackRule().getPrimarySurface());
  return;
}

void
insertGrid::createLinks()
  /*!
    Create link points
  */
{
  ELog::RegMethod RegA("insertGrid","createLinks");

  FixedComp::setNConnect(14);
  if (frontActive())
    {
      setLinkSurf(0,getFrontRule());
      setLinkSurf(0,getFrontBridgeRule());
      FixedComp::setConnect
        (0,SurInter::getLinePoint(Origin,Y,getFrontRule(),
				  getFrontBridgeRule()),-Y);
    }
  else
    {
      FixedComp::setConnect(0,Origin-Y*(innerDepth/2.0),-Y);
      FixedComp::setLinkSurf(0,-SMap.realSurf(ptIndex+1));
    }

  if (backActive())
    {
      FixedComp::setLinkSurf(1,getBackRule());
      FixedComp::setBridgeSurf(1,getBackBridgeRule());
      FixedComp::setConnect
        (1,SurInter::getLinePoint(Origin,Y,getBackRule(),
				  getBackBridgeRule()),Y);  
    }
  else
    {
      FixedComp::setConnect(1,Origin+Y*(innerDepth/2.0),Y);
      FixedComp::setLinkSurf(1,SMap.realSurf(ptIndex+2));
    }

  const double TT=std::accumulate(wallThick.begin(),wallThick.end(),0.0);
  const double fullW(innerWidth/2.0+TT);
  const double fullH(innerHeight/2.0+TT);
  const int PI(ptIndex+50*static_cast<int>(nLayer));
  
  FixedComp::setConnect(2,Origin-X*fullW,-X);
  FixedComp::setConnect(3,Origin+X*fullW,X);
  FixedComp::setConnect(4,Origin-Z*fullH,-Z);
  FixedComp::setConnect(5,Origin+Z*fullH,Z);

  FixedComp::setLinkSurf(2,-SMap.realSurf(ptIndex+3));
  FixedComp::setLinkSurf(3,SMap.realSurf(ptIndex+4));
  FixedComp::setLinkSurf(4,-SMap.realSurf(ptIndex+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(ptIndex+6));

  // corners
  
  const Geometry::Vec3D frontPt=getLinkPt(1);
  FixedComp::setConnect(6,frontPt-X*fullW-Z*fullH,-X-Z);
  FixedComp::setConnect(7,frontPt+X*fullW-Z*fullH,X-Z);
  FixedComp::setConnect(8,frontPt-X*fullW+Z*fullH,-X+Z);
  FixedComp::setConnect(9,frontPt+X*fullW+Z*fullH,X+Z);

  // Back corner:
  const Geometry::Vec3D backPt=getLinkPt(2);
  FixedComp::setConnect(10,backPt-X*fullW-Z*fullH,-X-Z);
  FixedComp::setConnect(11,backPt+X*fullW-Z*fullH,X-Z);
  FixedComp::setConnect(12,backPt-X*fullW+Z*fullH,-X+Z);
  FixedComp::setConnect(13,backPt+X*fullW+Z*fullH,X+Z);

  FixedComp::setLinkSurf(6,-SMap.realSurf(PI+3));
  FixedComp::setLinkSurf(7,SMap.realSurf(PI+4));
  FixedComp::setLinkSurf(8,-SMap.realSurf(PI+3));
  FixedComp::setLinkSurf(9,SMap.realSurf(PI+4));

  FixedComp::addLinkSurf(6,-SMap.realSurf(PI+5));
  FixedComp::addLinkSurf(7,-SMap.realSurf(PI+5));
  FixedComp::addLinkSurf(8,SMap.realSurf(PI+6));
  FixedComp::addLinkSurf(9,SMap.realSurf(PI+6));

  FixedComp::setLinkSurf(10,-SMap.realSurf(PI+3));
  FixedComp::setLinkSurf(11,SMap.realSurf(PI+4));
  FixedComp::setLinkSurf(12,-SMap.realSurf(PI+3));
  FixedComp::setLinkSurf(13,SMap.realSurf(PI+4));

  FixedComp::addLinkSurf(10,-SMap.realSurf(PI+5));
  FixedComp::addLinkSurf(11,-SMap.realSurf(PI+5));
  FixedComp::addLinkSurf(12,SMap.realSurf(PI+6));
  FixedComp::addLinkSurf(13,SMap.realSurf(PI+6));

  return;
}

void
insertGrid::createObjects(Simulation& System)
  /*!
    Create the main volume
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("insertGrid","createObjects");

  std::string Out;
  const std::string FB=frontRule()+backRule();

  // Inner space
  Out=ModelSupport::getSetComposite(SMap,ptIndex," 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,defMat,0.0,Out+FB));
  addCell("Inner",cellIndex-1);

  int PI(ptIndex+50);
  for(size_t i=0;i<nLayer;i++)
    {
      Out=ModelSupport::getComposite
	(SMap,PI,PI-50," 3 -4 5 -6 (-3:4:-5:6) ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat[i],0.0,Out+FB));
      PI+=50;
    }

  Out=ModelSupport::getComposite(SMap,PI-50," 3 -4 5 -6 ");
  addOuterSurf(Out);
  return;
}

void
insertGrid::addLayer(const double T,const std::string& mat)
  /*!
    Add a layer to the system
    \param T :: thickness
    \param mat :: material name
  */
{
  ELog::RegMethod RegA("insertGrid","addLayer");

  wallThick.push_back(T);
  wallMat.push_back(ModelSupport::EvalMatName(mat));
  return;
}
  
void
insertGrid::setValues(const double XS,const double YS,
		      const double ZS,const int Mat)
  /*!
    Set the values and populate flag
    \param XS :: X-size [width]
    \param YS :: Y-size [depth] 
    \param ZS :: Z-size [height]
    \param Mat :: Material number
   */
{
  innerWidth=XS;
  innerDepth=YS;
  innerHeight=ZS;
  defMat=Mat;
  populated=1;
  return;
}

void
insertGrid::setValues(const double XS,const double YS,
		      const double ZS,const std::string& Mat)
  /*!
    Set the values and populate flag
    \param XS :: X-size [width]
    \param YS :: Y-size [depth] 
    \param ZS :: Z-size [height]
    \param Mat :: Material number
   */
{
  ELog::RegMethod RegA("insertGrid","setValues");

  setValues(XS,YS,ZS,ModelSupport::EvalMatName(Mat));
  return;
}

void
insertGrid::mainAll(Simulation& System)
  /*!
    Common part to createAll:
    Note: the strnage order -- create links and findObject
    before createObjects. This allows findObjects not to 
    find ourselves (and correctly to find whatever this object
    is in).
    
    \param System :: Simulation
   */
{
  ELog::RegMethod RegA("insertGrid","mainAll");
  
  createSurfaces();
  createLinks();

  if (!delayInsert)
    findObjects(System);
  createObjects(System);

  insertObjects(System);
  return;
}


void
insertGrid::createAll(Simulation& System,const Geometry::Vec3D& OG,
		       const attachSystem::FixedComp& FC)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param OG :: Offset origin							
    \param FC :: Linear component to set axis etc
  */
{
  ELog::RegMethod RegA("insertGrid","createAll(Vec,FC)");
  if (!populated) 
    populate(System.getDataBase());  
  createUnitVector(OG,FC);
  mainAll(System);
  return;
}

void
insertGrid::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int lIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Linear component to set axis etc
    \param lIndex :: link Index
  */
{
  ELog::RegMethod RegA("insertGrid","createAll(FC,index)");
  
  if (!populated) 
    populate(System.getDataBase());  
  createUnitVector(FC,lIndex);
  mainAll(System);
  return;
}

void
insertGrid::createAll(Simulation& System,
		       const Geometry::Vec3D& Orig,
                       const Geometry::Vec3D& YA,
                       const Geometry::Vec3D& ZA)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param Orig :: Origin al point 
    \param YA :: Origin al point 
    \param ZA :: ZAxis
  */
{
  ELog::RegMethod RegA("insertGrid","createAll");
  if (!populated) 
    populate(System.getDataBase());  
  createUnitVector(Orig,YA,ZA);
  mainAll(System);
  
  return;
}
   
}  // NAMESPACE insertSystem
