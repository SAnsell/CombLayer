/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonVar/LayerGenerator.cxx
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
#include <stack>
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
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "surfExpand.h"
#include "LayerGenerator.h"

namespace setVariable
{

layerItem::layerItem(const double T,const size_t N,
		     const std::string& M) :
  thick(T),nDiv(N),mat(M)
  /*!
    constructor
    \param T :: thickness
    \param N :: size
    \param M :: material
   */
{}

layerItem::layerItem(const layerItem& A) :
  thick(A.thick),nDiv(A.nDiv),mat(A.mat)
  /*!
    Copy construtor				
    \param A :: layer to copy
  */
{}

  
layerItem&
layerItem::operator=(const layerItem& A)
  /*!
    Assignment operator
    \param A :: layer to copy
    \return this
  */
{
  if (this!=&A)
    {
      thick=A.thick;
      nDiv=A.nDiv;
      mat=A.mat;
    }
  return *this;
}
  
  
LayerGenerator::LayerGenerator() :
  nWall(0),nRoof(0),nFloor(0)
  /*!
    Constructor and defaults
  */
{}


  
LayerGenerator::~LayerGenerator() 
 /*!
   Destructor
 */
{}

void
LayerGenerator::setLayers(std::vector<layerItem>& layer,
			  size_t& nLayer,
			  double& primThick,
			  const double voidThick,
			  const std::vector<double>& thick,
			  const std::vector<size_t>& nL,
			  const std::vector<std::string>& matName)
 /*!
   Given a set of values set the wall
   \param layer :: thickness of each layer
   \param primThick :: Total Thickness to set
   \param nLayer :: Fractional divisions of each layer
   \param voidThick :: Thickness of inner void [half]
   \param thick :: Layer thickness
   \param matName :: layer materials
 */
{
  ELog::RegMethod RegA("LayerGenerator","setLayers");


  if (thick.size()!=matName.size())
    throw ColErr::MisMatch<size_t>(thick.size(),matName.size(),"thick/matName");
  if (thick.size()!=nL.size())
    throw ColErr::MisMatch<size_t>(thick.size(),nL.size(),"thick/NL");
  
  primThick=std::accumulate(thick.begin(),thick.end(),0.0);
  layer.clear();
  nLayer=0;
  if (voidThick>Geometry::zeroTol)
    {
      primThick+=voidThick;
      layer.push_back(layerItem(voidThick,1,"Void"));
      nLayer++;
    }

  for(size_t i=0;i<thick.size();i++)
    { 
      layer.push_back(layerItem(thick[i],nL[i],matName[i]));
      nLayer+=nL[i];
    }  
  return;
}
  

void
LayerGenerator::setWall(const double voidThick,
			const std::vector<double>& thick,
			const std::vector<size_t>& nL,
			const std::vector<std::string>& matName)
/*!
   Given a set of values set the wall
   \param voidThick :: Thickness of inner void [half]
   \param thick :: Layer thickness of each master layer
   \param nL :: Number of layers in each master-layer
   \param matName :: master-layer materials
 */
{
  ELog::RegMethod RegA("LayerGenerator","setWall");

  setLayers(wall,nWall,wallThick,voidThick,thick,nL,matName);
  return;
}

  
void
LayerGenerator::setRoof(const double voidThick,
			const std::vector<double>& thick,
			const std::vector<size_t>& nL,
			const std::vector<std::string>& matName)
/*!
   Given a set of values set the roof
   \param voidThick :: Thickness of inner void [half]
   \param thick :: Layer thickness of each master layer
   \param nL :: Number of layers in each master-layer
   \param matName :: master-layer materials
 */
{
  ELog::RegMethod RegA("LayerGenerator","setRoof");

  setLayers(roof,nRoof,roofThick,voidThick,thick,nL,matName);  
  return;
}

void
LayerGenerator::setFloor(const double voidThick,
			const std::vector<double>& thick,
			const std::vector<size_t>& nL,
			const std::vector<std::string>& matName)
/*!
   Given a set of values set the floor
   \param voidThick :: Thickness of inner void [half]
   \param thick :: Layer thickness of each master layer
   \param nL :: Number of layers in each master-layer
   \param matName :: master-layer materials
 */
{
  ELog::RegMethod RegA("LayerGenerator","setFloor");

  setLayers(floor,nFloor,floorThick,voidThick,thick,nL,matName);  
  return;
}

  
void
LayerGenerator::processLUnit(FuncDataBase& Control,
			     const std::string& unitName,
			     const std::vector<layerItem>& LVec) 
  /*!
    Process a type
    \param Control :: DataBase for variables
    \param unitName :: keyname+"Wall/Roof/Floor"
    \param LVec :: layerItem vector
   */
{
  ELog::RegMethod RegA("LayerGenerator","processLUnit");
  
  size_t wIndex(0);
  for(const layerItem& LItem : LVec)
    {
      const double tStep(LItem.thick/static_cast<double>(LItem.nDiv));
      for(size_t i=0;i<LItem.nDiv;i++)
	{
	  Control.addVariable
	    (unitName+"Len"+StrFunc::makeString(wIndex+1),tStep);
	  Control.addVariable
	    (unitName+"Mat"+StrFunc::makeString(wIndex),LItem.mat);

	  wIndex++;
	}
    }
  return;
}
  

void
LayerGenerator::processLayers
(FuncDataBase& Control,const std::string& keyName) const
  /*!
    Set the individual layers
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
  */
{
  ELog::RegMethod RegA("LayerGenerator","processLayers");


  processLUnit(Control,keyName+"Wall",wall);
  processLUnit(Control,keyName+"Floor",floor);
  processLUnit(Control,keyName+"Roof",roof);

  return;
}
  
void
LayerGenerator::generateLayer(FuncDataBase& Control,
			      const std::string& keyName,
			      const double length,
			      const size_t NSeg)  const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param length :: overall length
    \param NSeg :: number of segments
  */
{
  ELog::RegMethod RegA("LayerGenerator","generatorSetLayer");
  
  Control.addVariable(keyName+"Length",length);

  Control.addVariable(keyName+"Left",wallThick);
  Control.addVariable(keyName+"Right",wallThick);
  Control.addVariable(keyName+"Height",roofThick);
  Control.addVariable(keyName+"Depth",floorThick);
  Control.addVariable(keyName+"DefMat","Void");
  Control.addVariable(keyName+"NSeg",NSeg);
  Control.addVariable(keyName+"NWallLayers",nWall);
  Control.addVariable(keyName+"NFloorLayers",nFloor);
  Control.addVariable(keyName+"NRoofLayers",nRoof);
  
  processLayers(Control,keyName);
  
  return;
}


}  // NAMESPACE setVariable
