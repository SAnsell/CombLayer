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
#include "SurfMap.h"
#include "MXcards.h"
#include "Zaid.h"
#include "Material.h"
#include "DBMaterial.h"
#include "SurInter.h"
#include "surfDBase.h"
#include "DivideGrid.h"
#include "LayerDivide3D.h"

namespace ModelSupport
{

LayerDivide3D::LayerDivide3D(const std::string& Key)  :
  FixedComp(Key,0),
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


size_t
LayerDivide3D::processSurface(const size_t Index,
			      const std::pair<int,int>& WallSurf,
			      const std::vector<double>& lenFraction)
  /*!
    Process the surfaces and convert them into merged layers
    \param Index :: A/B/C surface
    \param WallSurf :: Surface numbers
    \param lenFraction :: Lengths to divide into fractions 
    \return segment count
  */
{
  ELog::RegMethod RegA("LayerDivide3D","processSurface");
  //
  // Currently we only can deal with two types of surface [ plane/plane
  // and plane/cylinder
  
  Geometry::Surface* aSurf=SMap.realSurfPtr(WallSurf.first);
  Geometry::Surface* bSurf=SMap.realSurfPtr(WallSurf.second);
  //
  // EXTREME CARE :
  //  -- a/bSurf can be negative relative to wallSurf since
  //     the sign of first/second is not handled in realSurfPtr.
  // 
  // mirror planes only work with planes(!)
  std::string surGroup="ASurf";
  surGroup[0]+=Index;

  if (WallSurf.first<0)
    {
      Geometry::Surface* PX=
	SMap.createMirrorPlane(aSurf);
      if (PX)
	aSurf=PX;
    }
  if (WallSurf.second<0)
    {
      Geometry::Surface* PX=SMap.createMirrorPlane(bSurf);
      if (PX)
	bSurf=PX;
    }
  // -------------------------------------------------------------
  
  int surfN(divIndex+1000*static_cast<int>(Index)+1);
  SMap.addMatch(surfN,WallSurf.first);
  attachSystem::SurfMap::addSurf(surGroup,surfN);
  surfN++;
  size_t segCount(1);
  
  if (lenFraction.empty()) 
    {
      SMap.addMatch(surfN,WallSurf.second);
      attachSystem::SurfMap::addSurf(surGroup,surfN);
      return 1;
    }
  
  // inner index
  const size_t startIndex((lenFraction.front()>Geometry::zeroTol) ? 0 : 1);
  const size_t endIndex(((lenFraction.back()-1.0)<Geometry::zeroTol) ?
    lenFraction.size()+1 : lenFraction.size());
  for(size_t i=startIndex;i+1<endIndex;i++)
    {
      Geometry::Surface* PX=
	ModelSupport::surfDBase::generalSurf(aSurf,bSurf,lenFraction[i],surfN);
      SMap.addToIndex(surfN-1,PX->getName());
      attachSystem::SurfMap::addSurf(surGroup,surfN);
      //      SMap.registerSurf(surfN-1,PX);
      segCount++;

    }
  SMap.addMatch(surfN,WallSurf.second);
  attachSystem::SurfMap::addSurf(surGroup,surfN);

  return segCount;;
}

void
LayerDivide3D::addCalcPoint(const size_t i,const size_t j,
			    const size_t k)
  /*!
    Process the string to calculate the corner points 
    \param i :: Index 
    \param j :: Index 
    \param k :: Index 
   */
{
  ELog::RegMethod RegA("LayerDivide3D","addCalcPoint");

  const int Asurf(divIndex+static_cast<int>(i));
  const int Bsurf(divIndex+1000+static_cast<int>(j));
  const int Csurf(divIndex+2000+static_cast<int>(k));
  
  Geometry::Surface* APtr[2];
  Geometry::Surface* BPtr[2];
  Geometry::Surface* CPtr[2];

  APtr[0]=SMap.realSurfPtr(Asurf);
  APtr[1]=SMap.realSurfPtr(Asurf+1);
  BPtr[0]=SMap.realSurfPtr(Bsurf);
  BPtr[1]=SMap.realSurfPtr(Bsurf+1);
  CPtr[0]=SMap.realSurfPtr(Csurf);
  CPtr[1]=SMap.realSurfPtr(Csurf+1);

  std::vector<Geometry::Vec3D> OutPts;
  for(size_t i=0;i<2;i++)
    for(size_t j=0;j<2;j++)
      for(size_t k=0;k<2;k++)
	OutPts.push_back(SurInter::getPoint(APtr[0],BPtr[0],CPtr[0],Origin));

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
    \param BSurf :: second surf number
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
LayerDivide3D::setFractions(const size_t index,
			    const std::vector<double>& FV)
  /*!
    Set the fractions
    \param index :: Type index 0 to 2
    \param FV :: Fraction
   */
{
  ELog::RegMethod RegA("LayerDivide3D","setFractions");
  
  switch (index)
    {
    case 0:
      AFrac=FV;
      return;
    case 1:
      BFrac=FV;
      return;
    case 2:
      CFrac=FV;
      return;
    default:
      throw ColErr::InContainerError<size_t>(index,"Index out of range");
    }  
  return;
}

void
LayerDivide3D::setXMLdata(const std::string& xmlFile,
			  const std::string& xmlItem,
			  const std::string& xmlOut)
  /*!
    Simple setter and getter for LayerDivide3D
    \param xmlFile :: XML input file
    \param xmlFile :: XML item
    \param xmlOut :: XML output file name
  */
{
  loadFile=xmlFile;
  objName=xmlItem;
  outputFile=xmlOut;
  return;
}

  
void
LayerDivide3D::checkDivide() const
  /*!
    Throws exception is things are not good for dividing
   */
{
  ELog::RegMethod RegA("LayerDivide3D","checkDivide");
  if (!(AWall.first*AWall.second))
    throw ColErr::EmptyValue<int>("Wall A not set");
  if (!(BWall.first*BWall.second))
    throw ColErr::EmptyValue<int>("Wall B not set");
  if (!(CWall.first*BWall.second))
    throw ColErr::EmptyValue<int>("Wall C not set");
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

  checkDivide();
  
  ModelSupport::DBMaterial& DB=
    ModelSupport::DBMaterial::Instance();


  const MonteCarlo::Object* CPtr=System.findQhull(cellN);
  if (!CPtr)
    throw ColErr::InContainerError<int>(cellN,"cellN");


  ALen=processSurface(0,AWall,AFrac);
  BLen=processSurface(1,BWall,BFrac);
  CLen=processSurface(2,CWall,CFrac);

  if (!DGPtr)
    DGPtr=new DivideGrid(DB.getKey(CPtr->getMat()));
  DGPtr->loadXML(loadFile,objName);

  std::string Out;
  int aIndex(divIndex);
  for(size_t i=0;i<ALen;i++,aIndex++)
    {
      const std::string ACut=ModelSupport::getComposite(SMap,aIndex,"1 -2");
      int bIndex(divIndex+1000);
      
      for(size_t j=0;j<BLen;j++,bIndex++)
	{
	  const std::string BCut=
	    ModelSupport::getComposite(SMap,bIndex," 1 -2 ")+ACut;
	  int cIndex(divIndex+2000);
	  
	  for(size_t k=0;k<CLen;k++,cIndex++)
	    {
	      const std::string CCut=
		ModelSupport::getComposite(SMap,cIndex,"1 -2")+BCut;
	      const int Mat=DGPtr->getMaterial(i+1,j+1,k+1);
	      
	      System.addCell(MonteCarlo::Qhull(cellIndex++,Mat,0.0,
					       CCut+divider));
	      attachSystem::CellMap::addCell("LD3",cellIndex-1);
      	    }
	}
    }
  System.removeCell(cellN);
  if (DGPtr && !outputFile.empty())
    DGPtr->writeXML(outputFile,objName,ALen,BLen,CLen);

  return;
}

}  // NAMESPACE ModelSupport
