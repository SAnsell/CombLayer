/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/LayerDivide3D.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include "Simulation.h"
#include "ReadFunctions.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "MXcards.h"
#include "Zaid.h"
#include "Material.h"
#include "DBMaterial.h"
#include "surfDBase.h"
#include "surfDIter.h"
#include "surfDivide.h"
#include "SurInter.h"
#include "DivideGrid.h"
#include "LayerDivide3D.h"

namespace ModelSupport
{

LayerDivide3D::LayerDivide3D(const std::string& Key)  :
  keyName(Key),
  divIndex(ModelSupport::objectRegister::Instance().cell(Key,-1,20000)),
  cellIndex(divIndex+1),DGPtr(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}


LayerDivide3D::~LayerDivide3D() 
  /*!
    Destructor
  */
{}


void
LayerDivide3D::processSurface(const size_t Index,
			      const std::pair<int,int>& WallSurf,
			      const std::vector<double>& lenFraction)
  /*!
    Process the surfaces and convert them into merged layers
    \param WallSurf :: Surface numbers
    \param lenFraction :: Lengths to divide between FRACtions 
  */
{
  ELog::RegMethod RegA("LayerDivide3D","processSurface");
  //
  // Currently we only can deal with two types of surface [ plane/plane
  // and plane/cylinder

  const Geometry::Surface* aSurf=SMap.realSurfPtr(WallSurf.first);
  const Geometry::Surface* bSurf=SMap.realSurfPtr(WallSurf.second);
  
  int surfN(divIndex+1000*static_cast<int>(Index)+1);
  for(const double L : lenFraction)
    {
      ModelSupport::surfDBase::generalSurf(aSurf,bSurf,L,surfN);
    }	
  return;
}

void
LayerDivide3D::addCalcPoint(const size_t i,const size_t j,
			    const size_t k,
			    std::string OrderSurf)
  /*!
    Process the string to calculate the corner points 
    \param i :: Index 
    \param j :: Index 
    \param k :: Index 
    \param OrderSurf :: Very highly order list of surface
   */
{
  ELog::RegMethod RegA("LayerDivide3D","addCalcPoint");
  
  int side[2];
  int cyl[2];
  int vert[2];
  
  StrFunc::section(OrderSurf,side[0]);
  StrFunc::section(OrderSurf,side[1]);
  StrFunc::section(OrderSurf,vert[0]);
  StrFunc::section(OrderSurf,vert[1]);
  StrFunc::section(OrderSurf,cyl[0]);
  StrFunc::section(OrderSurf,cyl[1]);

  Geometry::Plane* SPtr[2];
  Geometry::Cylinder* CPtr[2];
  Geometry::Plane* VPtr[2];

  SPtr[0]=SMap.realPtr<Geometry::Plane>(side[0]);
  SPtr[1]=SMap.realPtr<Geometry::Plane>(side[1]);
  CPtr[0]=SMap.realPtr<Geometry::Cylinder>(cyl[0]);
  CPtr[1]=SMap.realPtr<Geometry::Cylinder>(cyl[1]);
  VPtr[0]=SMap.realPtr<Geometry::Plane>(vert[0]);
  VPtr[1]=SMap.realPtr<Geometry::Plane>(vert[1]);

  std::vector<Geometry::Vec3D> OutPts;
  for(size_t i=0;i<2;i++)
    for(size_t j=0;j<2;j++)
      for(size_t k=0;k<2;k++)
	OutPts.push_back(SurInter::getPoint(SPtr[0],VPtr[0],CPtr[0],Centre));

  DGPtr->setPoints(i,j,k,OutPts);
  
  return; 
}

void
LayerDivide3D::setSurfPair(const size_t index,const int ASurf,
			   const int BSurf)
  /*!
    Set a give pair of surfaces  for division
    \param index :: Type index 0 to 2
    \param ASurf :: surface nubmer
   */
{
  ELog::RegMethod RegA("LayerDivide3D","setSurfPair");

  switch (index)
    {
    case 0:
      AWall=std::pair<int,int>(ASurf,BSurf);
      return;
    case 1:
      BWall=std::pair<int,int>(ASurf,BSurf);
      return;
    case 2:
      CWall=std::pair<int,int>(ASurf,BSurf);
      return;
    default:
      throw ColErr::InContainerError<size_t>(index,"Index out of range");
    }  
  return;
}

void
LayerDivide3D::divideCell(Simulation& System,const int cellN)
  /*!
    Create a tesselated main wall
    \param System ::Simution to use
    \param cellN :: Cell number
  */
{
  ELog::RegMethod RegA("LayerDivide3D","divideCell");

  ModelSupport::DBMaterial& DB=
    ModelSupport::DBMaterial::Instance();


  const MonteCarlo::Object* CPtr=System.findQhull(cellN);
  if (!CPtr)
    throw ColErr::InContainerError<int>(cellN,"cellN");
     
  processSurface(0,AWall,ALen);
  processSurface(1,BWall,BLen);
  processSurface(2,CWall,CLen);
  
  if (!DGPtr)
    DGPtr=new DivideGrid(DB.getKey(CPtr->getMat()));
  DGPtr->loadXML(loadFile,objName);

  /*  
  const std::string divider=
    ModelSupport::getComposite(SMap,divIndex," 1 ");
  std::string Out;
  int secIndex(divIndex+1000);

  for(size_t i=0;i<nSectors;i++)
    {
      const std::string ACut=(!i) ?
	ModelSupport::getComposite(SMap,lwIndex," 3 ") :
	ModelSupport::getComposite(SMap,secIndex-1," 1 ");
      const std::string BCut=(i+1 == nSectors) ?
	ModelSupport::getComposite(SMap,rwIndex," -4 ") :
	ModelSupport::getComposite(SMap,secIndex," -1 ");
      secIndex++;

      int vertIndex(divIndex+2000);
      for(size_t j=0;j<nVert;j++)
	{
	  const std::string AVert=(!j) ?
	    ModelSupport::getComposite(SMap,divIndex," 5 ") :
	    ModelSupport::getComposite(SMap,vertIndex-1," 1 ");
	  const std::string BVert=(j+1 == nVert) ?
	    ModelSupport::getComposite(SMap,divIndex," -6 ") :
	    ModelSupport::getComposite(SMap,vertIndex," -1 ");
	  vertIndex++;  // +1 already in system due to 1M

	  
	  int wallIndex(divIndex+3000);
	  for(size_t k=0;k<nLayers;k++)
	    {
	      const std::string AWall=(!k) ?
		ModelSupport::getComposite(SMap,divIndex," 7 ") :
		ModelSupport::getComposite(SMap,wallIndex-1," 1 ");
	      const std::string BWall=(k+1 == nLayers) ?
		ModelSupport::getComposite(SMap,divIndex," -17 ") :
		ModelSupport::getComposite(SMap,wallIndex," -1 ");
	      wallIndex++;
	      	      
	      Out=ACut+BCut+AVert+BVert+AWall+BWall;
	      const int Mat=BMWPtr->getMaterial(i+1,j+1,k+1,wallMat);
	      System.addCell(MonteCarlo::Qhull(cellIndex++,Mat,0.0,
					       Out+divider));
	      addCell("MainWall",cellIndex-1);
	    }
	}
    }

  if (!outFile.empty())
    BMWPtr->writeXML(outFile,nSectors,nVert,nLayers);
*/  
  return;
}

}  // NAMESPACE ModelSupport
