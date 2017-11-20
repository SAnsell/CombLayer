/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/LayerDivide3D.cxx
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
  divIndex(ModelSupport::objectRegister::Instance().cell(Key,20000)),
  cellIndex(divIndex+1),
  WallID({"Sector","Vert","Radial"}),DGPtr(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

LayerDivide3D::LayerDivide3D(const LayerDivide3D& A) : 
  attachSystem::FixedComp(A),attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  Centre(A.Centre),divIndex(A.divIndex),cellIndex(A.cellIndex),
  AFrac(A.AFrac),BFrac(A.BFrac),CFrac(A.CFrac),
  ALen(A.ALen),BLen(A.BLen),CLen(A.CLen),WallID(A.WallID),
  AWall(A.AWall),BWall(A.BWall),CWall(A.CWall),divider(A.divider),
  DGPtr((A.DGPtr) ? new DivideGrid(*A.DGPtr) : 0),
  objName(A.objName),
  loadFile(A.loadFile),outputFile(A.outputFile)
  /*!
    Copy constructor
    \param A :: LayerDivide3D to copy
  */
{}

LayerDivide3D&
LayerDivide3D::operator=(const LayerDivide3D& A)
  /*!
    Assignment operator
    \param A :: LayerDivide3D to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      Centre=A.Centre;
      cellIndex=A.cellIndex;
      AFrac=A.AFrac;
      BFrac=A.BFrac;
      CFrac=A.CFrac;
      ALen=A.ALen;
      BLen=A.BLen;
      CLen=A.CLen;
      WallID=A.WallID;
      AWall=A.AWall;
      BWall=A.BWall;
      CWall=A.CWall;
      divider=A.divider;
      delete DGPtr;
      DGPtr=(A.DGPtr) ? new DivideGrid(*A.DGPtr) : 0;
      objName=A.objName;
      loadFile=A.loadFile;
      outputFile=A.outputFile;
    }
  return *this;
}

LayerDivide3D::~LayerDivide3D() 
  /*!
    Destructor
  */
{
  delete DGPtr;
}


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
  surGroup[0]=StrFunc::indexToAlpha(Index);

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
    \param FV :: Fractions [0 - 1]
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
LayerDivide3D::setFractions(const size_t index,const size_t NF)
  /*!
    Set the fractions
    \param index :: Type index 0 to 2
    \param NF :: linear fractions
   */
{
  ELog::RegMethod RegA("LayerDivide3D","setFractions(size)");

  if (!NF)
    throw ColErr::SizeError<size_t>(NF,1,"NF cant be zero");

  std::vector<double> FV;
  const double gap(1.0/static_cast<double>(NF));
  double step(0.0);
  for(size_t i=0;i<NF;i++)
    {
      step+=gap;
      FV.push_back(step);
    }
  setFractions(index,FV);
  return;
}

void
LayerDivide3D::setIndexNames(const std::string& A,
			     const std::string& B,
			     const std::string& C)
  /*!
    Set the secton names
    \param A :: First Name
    \param B :: Second Name
    \param C :: Third Name
  */
{
  WallID[0]=A;
  WallID[1]=B;
  WallID[2]=C;
  return;
}
  
void
LayerDivide3D::setDivider(const std::string& SurfStr)
  /*!
    Set the divider string
    \param SurfStr :: Divider String
  */
{
  divider=SurfStr;
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
    throw ColErr::EmptyValue<int>("Section A not set");
  if (!(BWall.first*BWall.second))
    throw ColErr::EmptyValue<int>("Section B not set");
  if (!(CWall.first*CWall.second))
    throw ColErr::EmptyValue<int>("Section C not set");
  return;
}

void
LayerDivide3D::setMaterials(const std::string& DefMat)
  /*!
    Processes the material setting 
    \param DefMat :: Default Material						
  */
{
  ELog::RegMethod Rega("LayerDivide3D","setMaterials");
  
  if (!DGPtr)
    DGPtr=new DivideGrid(DefMat);
  DGPtr->setMaterial(0,0,0,DefMat);
  return;
}

void
LayerDivide3D::setMaterials(const size_t index,
			    const std::vector<std::string>& DefMatVec)
  /*!
    Processes the material setting 
    \param index :: [0-2] offset nubmer
    \param DefMatVec :: Default Material list
  */
{
  ELog::RegMethod Rega("LayerDivide3D","setMaterials(Vec)");

  if (index>2)
    throw ColErr::IndexError<size_t>(index,2,"index out of range");

  if (!DGPtr)
    DGPtr=new DivideGrid(DefMatVec.front());

  if (!index)
    {
      for(size_t i=0;i<DefMatVec.size();i++)
        DGPtr->setMaterial(i+1,0,0,DefMatVec[i]);
    }
  else if (index==1)
    {
      for(size_t i=0;i<DefMatVec.size();i++)
        DGPtr->setMaterial(0,i+1,0,DefMatVec[i]);
    }
  else
    {
      for(size_t i=0;i<DefMatVec.size();i++)
        DGPtr->setMaterial(0,0,i+1,DefMatVec[i]);
    }
  
  return;
}

int
LayerDivide3D::setMaterialXML(const std::string& LFile,
			      const std::string& ObjName,
			      const std::string& OutName,
			      const std::string& DefMat)
  /*!
    Processes the material setting 
    \param LFile :: Load file name
    \param ObjName :: XML component name
    \param OutName :: Output name
    \param DefMat :: Default material if no file
    \return 0 on no file and  1 on success
  */
{
  ELog::RegMethod Rega("LayerDivide3D","setMaterials(XML)");

  objName=ObjName;
  loadFile=LFile;
  outputFile=OutName;
  
  if (!DGPtr)
    DGPtr=new DivideGrid(DefMat);
  return DGPtr->loadXML(loadFile,objName);
}

  
void
LayerDivide3D::setDividerByExclude(const Simulation& System,const int cellN)
  /*!
    Set the divider
    \param System :: Simulation to use
    \param cellN :: Cell number
  */
{
  ELog::RegMethod RegA("LayerDivide3D","setDividerByExclude");
  const MonteCarlo::Object* CPtr=System.findQhull(cellN);
  if (!CPtr)
    throw ColErr::InContainerError<int>(cellN,"cellN");

  HeadRule CellRule= CPtr->getHeadRule();

  CellRule.removeItems(AWall.first);
  CellRule.removeItems(AWall.second);
  CellRule.removeItems(BWall.first);
  CellRule.removeItems(BWall.second);
  CellRule.removeItems(CWall.first);
  CellRule.removeItems(CWall.second);

  divider=CellRule.display();
  return;
}



void
LayerDivide3D::divideCell(Simulation& System,const int cellN)
  /*!
    Create a tesselated main wall
    \param System :: Simulation to use
    \param cellN :: Cell number
  */
{
  ELog::RegMethod RegA("LayerDivide3D","divideCell");

  checkDivide();
  
  const MonteCarlo::Object* CPtr=System.findQhull(cellN);
  if (!CPtr)
    throw ColErr::InContainerError<int>(cellN,"cellN");


  ALen=processSurface(0,AWall,AFrac);
  BLen=processSurface(1,BWall,BFrac);
  CLen=processSurface(2,CWall,CFrac);


  std::string Out;
  int aIndex(divIndex);
  for(size_t i=0;i<ALen;i++,aIndex++)
    {
      const std::string layerNum(StrFunc::makeString(i));
      const std::string ACut=
        ModelSupport::getComposite(SMap,aIndex,"1 -2");
      
      int bIndex(divIndex+1000);
      
      for(size_t j=0;j<BLen;j++,bIndex++)
	{
	  const std::string BCut=
	    ModelSupport::getComposite(SMap,bIndex," 1 -2 ")+ACut;
	  int cIndex(divIndex+2000);
	  
	  for(size_t k=0;k<CLen;k++,cIndex++)
	    {
	      const std::string CCut=
		ModelSupport::getComposite(SMap,cIndex," 1 -2 ")+BCut;
	      const int Mat=DGPtr->getMaterial(i+1,j+1,k+1);
	      
	      System.addCell(MonteCarlo::Qhull(cellIndex++,Mat,0.0,
					       CCut+divider));
	      attachSystem::CellMap::addCell
                ("LD3:"+layerNum,cellIndex-1);
      	    }
	}
    }

  System.removeCell(cellN);
  if (DGPtr && !outputFile.empty())
    DGPtr->writeXML(outputFile,objName,ALen,BLen,CLen);

  return;
}

}  // NAMESPACE ModelSupport
