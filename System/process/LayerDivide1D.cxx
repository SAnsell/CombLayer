/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/LayerDivide1D.cxx
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
#include "LayerDivide1D.h"

namespace ModelSupport
{

LayerDivide1D::LayerDivide1D(const std::string& Key)  :
  FixedComp(Key,0),
  divIndex(ModelSupport::objectRegister::Instance().cell(Key,20000)),
  cellIndex(divIndex+1),WallID("Split")
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}


LayerDivide1D::~LayerDivide1D() 
  /*!
    Destructor
  */
{
}


size_t
LayerDivide1D::processSurface(const std::pair<int,int>& WallSurf,
			      const std::vector<double>& lenFraction)
  /*!
    Process the surfaces and convert them into merged layers
    \param WallSurf :: Surface numbers
    \param lenFraction :: Lengths to divide into fractions 
    \return segment count
  */
{
  ELog::RegMethod RegA("LayerDivide1D","processSurface");
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
  
  int surfN(divIndex+1);
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
LayerDivide1D::setSurfPair(const int ASurf,const int BSurf)
  /*!
    Set a give pair of surfaces  for division
    \param ASurf :: surface nubmer
    \param BSurf :: second surf number
   */
{
  ELog::RegMethod RegA("LayerDivide1D","setSurfPair");
  
  AWall=std::pair<int,int>(ASurf,BSurf);
  return;
}

void
LayerDivide1D::setFractions(const std::vector<double>& FV)
  /*!
    Set the fractions
    \param FV :: Fraction
   */
{
  ELog::RegMethod RegA("LayerDivide1D","setFractions");
  
  AFrac=FV;
  return;
}

void
LayerDivide1D::setFractions(const size_t NF)
  /*!
    Set the fractions
    \param NF :: linear fractions
   */
{
  ELog::RegMethod RegA("LayerDivide1D","setFractions(size)");

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
  setFractions(FV);
  return;
}

void
LayerDivide1D::setIndexNames(const std::string& A)
  /*!
    Set the secton names
    \param A :: First Name
  */
{
  WallID=A;
  return;
}
  
void
LayerDivide1D::setDivider(const std::string& SurfStr)
  /*!
    Set the divider string
    \param SurfStr :: Divider String
  */
{
  divider=SurfStr;
  return;
}
  
void
LayerDivide1D::checkDivide() const
  /*!
    Throws exception is things are not good for dividing
   */
{
  ELog::RegMethod RegA("LayerDivide1D","checkDivide");
  if (!(AWall.first*AWall.second))
    throw ColErr::EmptyValue<int>("Section A not set");
  return;
}

void
LayerDivide1D::setMaterials(const std::string& DefMat)
  /*!
    Processes the material setting 
    \param DefMat :: Default Material						
  */
{
  ELog::RegMethod Rega("LayerDivide1D","setMaterials");

  AMat.resize(AFrac.size()+1);
  const int matN=ModelSupport::EvalMatName(DefMat);
  std::fill(AMat.begin(),AMat.end(),matN);
  return;
}

void
LayerDivide1D::setMaterials(const std::vector<std::string>& DefMatVec)
  /*!
    Processes the material setting 
    \param DefMatVec :: Default Material list
  */
{
  ELog::RegMethod Rega("LayerDivide1D","setMaterials(Vec)");

  AMat.resize(AFrac.size()+1);
  for(size_t i=0;i<DefMatVec.size() && AMat.size();i++)
    AMat[i]=ModelSupport::EvalMatName(DefMatVec[i]);
  
  return;
}

void
LayerDivide1D::setMaterials(const std::vector<int>& DefMatVec)
  /*!
    Processes the material setting 
    \param DefMatVec :: Default Material list
  */
{
  ELog::RegMethod Rega("LayerDivide1D","setMaterials(Vec)");

  AMat=DefMatVec;
  const int matN=(AMat.empty()) ? 0 : AMat.back();
  
  for(size_t i=AMat.size();i<AFrac.size()+1;i++)
    AMat.push_back(matN);
	
  return;
}

int
LayerDivide1D::setMaterialXML(const std::string& LFile,
			      const std::string& ObjName,
			      const std::string& OutName,
			      const std::string& DefMat)
  /*!
    Processes the material setting 
    \param LFile :: Load file name
    \param ObjName :: object to take value from
    \param OutName :: Output file naem 
    \param DefMat :: Default material if no file
    \return 0 on no file and  1 on success
  */
{
  ELog::RegMethod Rega("LayerDivide1D","setMaterials(XML)");

  objName=ObjName;
  loadFile=LFile;
  outputFile=OutName;

  ELog::EM<<"INCOMPLETE CODE"<<ELog::endErr;
  return 0;
}


  
void
LayerDivide1D::divideCell(Simulation& System,const int cellN)
  /*!
    Create a tesselated main wall
    \param System :: Simulation to use
    \param cellN :: Cell number
  */
{
  ELog::RegMethod RegA("LayerDivide1D","divideCell");

  checkDivide();
  
  ModelSupport::DBMaterial& DB=
    ModelSupport::DBMaterial::Instance();

  const MonteCarlo::Object* CPtr=System.findQhull(cellN);
  if (!CPtr)
    throw ColErr::InContainerError<int>(cellN,"cellN");

  ALen=processSurface(AWall,AFrac);

  std::string Out;
  int aIndex(divIndex);
  for(size_t i=0;i<ALen;i++,aIndex++)
    {
      const std::string layerNum(StrFunc::makeString(i));
      const std::string ACut=
        ModelSupport::getComposite(SMap,aIndex,"1 -2");
      
      const int Mat(AMat[i]);
      System.addCell(MonteCarlo::Qhull(cellIndex++,Mat,0.0,ACut+divider));
      attachSystem::CellMap::addCell("LD1:"+layerNum,cellIndex-1);
    }
  System.removeCell(cellN);

  return;
}

}  // NAMESPACE ModelSupport
