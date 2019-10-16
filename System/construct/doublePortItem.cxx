/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/doublePortItem.cxx
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
#include <array>

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
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Cylinder.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Object.h"
#include "Line.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "AttachSupport.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "LineTrack.h"
#include "BaseMap.h"
#include "CellMap.h"

#include "portItem.h"
#include "doublePortItem.h"

namespace constructSystem
{

doublePortItem::doublePortItem(const std::string& baseKey,
				 const std::string& Key) :
  portItem(baseKey,Key),
  externPartLen(0.0),radiusB(0.0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param baseKey :: Base name
    \param Key :: Primary name
  */
{}

doublePortItem::doublePortItem(const std::string& Key) :
  portItem(Key),
  externPartLen(0.0),radiusB(0.0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}
  
doublePortItem::~doublePortItem() 
  /*!
    Destructor
  */
{}

void
doublePortItem::setLarge(const double L,const double R)
  /*!
    Set the port first radius part
    \param L :: Length of externPart
    \param R :: Main radius
   */
{
  externPartLen=L;
  radiusB=R;
  return;
}
  
void
doublePortItem::populate(const FuncDataBase& Control)
  /*!
    Populate variables
    \param Control :: Variable DataBase
   */
{
  ELog::RegMethod RegA("doublePortItem","populate");

  portItem::populate(Control);
  externPartLen=Control.EvalTail<double>(keyName,portBase,"PartLength");
  radiusB=Control.EvalTail<double>(keyName,portBase,"RadiusB");

  return;
}


void
doublePortItem::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("doublePortItem","createSurfaces");

  portItem::createSurfaces();
  ModelSupport::buildCylinder(SMap,buildIndex+1007,Origin,Y,radiusB);
  ModelSupport::buildCylinder(SMap,buildIndex+1017,Origin,Y,radiusB+wall);
  

  return;
}

void
doublePortItem::constructOuterFlange(Simulation& System,
			       const ModelSupport::LineTrack& LT,
			       const size_t startIndex,
			       const size_t lastIndex)
  /*!
    Find the length and outer fangge
    Copied from portItem -- needs refactoring
    \parma System :: Simulation to use
    \param LT :: Line track out of object
    \param startIndex :: index of last point
    \param lastIndex :: index of last point
  */
{
  ELog::RegMethod RegA("doublePortItem","constructOuterFlange");

  const Geometry::Vec3D exitPoint=LT.getPoint(lastIndex+1);

  // Final outer
  ModelSupport::buildPlane(SMap,buildIndex+2,
			   exitPoint+Y*externalLength,Y);
  // inner split
  ModelSupport::buildPlane(SMap,buildIndex+1002,
			   exitPoint+Y*externPartLen,Y);
  // inner split
  ModelSupport::buildPlane(SMap,buildIndex+1012,
			   exitPoint+Y*(externPartLen+wall),Y);

  ModelSupport::buildPlane(SMap,buildIndex+102,
			   exitPoint+Y*(externalLength-flangeLength),Y);

  if (capThick>Geometry::zeroTol)
    ModelSupport::buildPlane(SMap,buildIndex+202,
			     exitPoint+Y*(externalLength+capThick),Y);
  
  // determine start surface:
  std::string frontSurf,midSurf;
  if (startIndex!=0)
    frontSurf=std::to_string(LT.getSurfIndex()[startIndex-1]);
  if (startIndex+1<LT.getSurfIndex().size())
    midSurf=std::to_string(LT.getSurfIndex()[startIndex]);
  else
    midSurf=frontSurf;

  // construct inner volume:
  std::string Out;
  
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -1007 -1002 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out+frontSurf);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1002 -7 -2 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -1017 1007 -1002 ");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,Out+frontSurf);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1002 -17 7 -2 ");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1002 -1012 -1017 17 ");
  makeCell("MidWall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 102 -27 17 -2 ");
  makeCell("Flange",System,cellIndex++,wallMat,0.0,Out);

  if (capThick>Geometry::zeroTol)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex," -27 -202 2 ");
      makeCell("Plate",System,cellIndex++,capMat,0.0,Out);
    }

  if (outerFlag)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex,"1012 -1017 17 -102");
      makeCell("OutVoid",System,cellIndex++,0,0.0,Out);
      if (radiusB>flangeRadius+Geometry::zeroTol)  
	{
	  Out= (capThick>Geometry::zeroTol) ?
	    ModelSupport::getComposite(SMap,buildIndex," 102 27 -1017 -202") :
	    ModelSupport::getComposite(SMap,buildIndex," 102 27 -1017 -2");
	  makeCell("FlangeVoid",System,cellIndex++,0,0.0,Out);

	  Out= (capThick>Geometry::zeroTol) ?
	    ModelSupport::getComposite(SMap,buildIndex," -202 -1017  1 ") :
	    ModelSupport::getComposite(SMap,buildIndex," -2 -1017  1 ");
	}
      else
	{
	  Out=ModelSupport::getComposite(SMap,buildIndex," -102 -27 1017 1 ");
	  makeCell("FlangeVoid",System,cellIndex++,0,0.0,Out+midSurf);
	  Out= (capThick>Geometry::zeroTol) ?
	    ModelSupport::getComposite(SMap,buildIndex," -202 -27  1 ") :
	    ModelSupport::getComposite(SMap,buildIndex," -2 -27  1 ");
	}
      addOuterSurf(Out);
    }
  else  // no outer system
    {
      Out= (capThick>Geometry::zeroTol) ?
	ModelSupport::getComposite(SMap,buildIndex," -202 -27 102 ") :
	ModelSupport::getComposite(SMap,buildIndex," -2 -27 102 ");
      addOuterSurf(Out);
      Out=ModelSupport::getComposite(SMap,buildIndex," -1017 -1012 1 ");
      addOuterUnionSurf(Out+midSurf);

      Out=ModelSupport::getComposite(SMap,buildIndex," 1012 -17 -102 ");
      addOuterUnionSurf(Out);
    }
  
  // Mid port exclude
  const std::string tubeExclude=
    ModelSupport::getComposite(SMap,buildIndex," ( 1017 : -1 )");

  //  std::set<int> activeCell;
  const std::vector<MonteCarlo::Object*>& OVec=LT.getObjVec();
  const std::vector<double>& Track=LT.getSegmentLen();
  double T(0.0);   // extention base out point

  for(size_t i=startIndex;i<OVec.size() &&
	T<(externalLength-Geometry::zeroTol);i++)
    {
      MonteCarlo::Object* OPtr=OVec[i];
      const int OName=OPtr->getName();
      if (i>lastIndex)
	T+=Track[i];

      if (outerCell.find(OName)==outerCell.end())
	{
	  if (i>lastIndex)
	    OPtr->addSurfString(getExclude());
	  else 
	    OPtr->addSurfString(tubeExclude);
	}
    }
  if (externalLength<flangeLength+Geometry::zeroTol)
    {
      const Geometry::Surface* cylPtr=LT.getSurfVec()[lastIndex];
      insertComponent(System,"Flange",std::to_string(cylPtr->getName()));
    }
  // do essential outerCells
  for(const int ON : outerCell)
    {
      MonteCarlo::Object* OPtr=System.findObject(ON);
      if (!OPtr)
	throw ColErr::InContainerError<int>(ON,"Cell not found");
      OPtr->addSurfString(getExclude());
    }
  return;
}

}  // NAMESPACE constructSystem
