/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   delft/ReactorGrid.cxx
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
#include <boost/bind.hpp>
#include <boost/multi_array.hpp>

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
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfDIter.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "Rules.h"
#include "surfFunctors.h"
#include "SurInter.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "SimProcess.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"  
#include "FixedComp.h" 
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "RElement.h"
#include "DefElement.h"
#include "FuelElement.h"
#include "ControlElement.h"
#include "HfElement.h"
#include "BeElement.h"
#include "IrradElement.h"
#include "AirBoxElement.h"
#include "ReactorGrid.h"

namespace delftSystem
{

template<typename T>
T 
ReactorGrid::getElement(const FuncDataBase& Control,
			const std::string& Name,const size_t I,
			const size_t J) 
 /*!
   Check to convert a Name + a number string into a variable state:
   \param Control :: Function base to search
   \param Name :: Master name
   \param I :: Index (A-Z) type [A on viewed face]
   \param J :: Index (Number) type
   \return variable value
 */
{
  ELog::RegMethod RegA("ReactorGrid","getElement");

  const std::string Key[2]=
    { 
      std::string(1,static_cast<char>(I)+'A'),
      StrFunc::makeString(J),
    };
  // Completely specified:
  if (Control.hasVariable(Name+Key[0]+Key[1]))
    return Control.EvalVar<T>(Name+Key[0]+Key[1]);

  // One Missing
  for(size_t i=0;i<2;i++)
    {
      const std::string KN(Name+Key[i]);
      if (Control.hasVariable(KN))
	return Control.EvalVar<T>(KN);
    }
  // Finally All missing 
  if (Control.hasVariable(Name))
    return Control.EvalVar<T>(Name);
  
  ELog::EM<<"NO variable:"<<Name<<ELog::endTrace;
  throw ColErr::InContainerError<std::string>(Name,"Variable name");
}

std::string
ReactorGrid::getElementName(const std::string& Name,const size_t I,
			    const size_t J) 
 /*!
   Check to convert a Name + a number string into a variable state:
   \param Name :: Master name
   \param I :: I Index (Letter)
   \param J :: J Index (Number) 
   \return Full resolved anme
 */
{
  ELog::RegMethod RegA("ReactorGrid","getElement");
  
  std::string Out(Name);
  Out+=std::string(1,static_cast<char>(I)+'A');
  Out+=StrFunc::makeString(J);
  return Out;
}


ReactorGrid::ReactorGrid(const std::string& Key) : 
  attachSystem::FixedComp(Key,7),attachSystem::ContainedComp(),
  gridIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(gridIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

ReactorGrid::ReactorGrid(const ReactorGrid& A) : 
  attachSystem::FixedComp(A),attachSystem::ContainedComp(A),
  gridIndex(A.gridIndex),cellIndex(A.cellIndex),
  NX(A.NX),NY(A.NY),Width(A.Width),Depth(A.Depth),
  Base(A.Base),Top(A.Top),Grid(A.Grid)
  /*!
    Copy constructor
    \param A :: ReactorGrid to copy
  */
{}

ReactorGrid&
ReactorGrid::operator=(const ReactorGrid& A)
  /*!
    Assignment operator
    \param A :: ReactorGrid to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedComp::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      cellIndex=A.cellIndex;
      NX=A.NX;
      NY=A.NY;
      Width=A.Width;
      Depth=A.Depth;
      Base=A.Base;
      Top=A.Top;
      Grid=A.Grid;
    }
  return *this;
}

ReactorGrid::~ReactorGrid() 
  /*!
    Destructor
  */
{}

void
ReactorGrid::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("ReactorGrid","populate");

  const FuncDataBase& Control=System.getDataBase();
  
  NX=Control.EvalVar<size_t>(keyName+"XSize");
  NY=Control.EvalVar<size_t>(keyName+"YSize");

  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYangle");
  zAngle=Control.EvalVar<double>(keyName+"Zangle");

  Width=Control.EvalVar<double>(keyName+"Width");
  Depth=Control.EvalVar<double>(keyName+"Depth");
  Top=Control.EvalVar<double>(keyName+"Top");
  Base=Control.EvalVar<double>(keyName+"Base");

  plateThick=Control.EvalVar<double>(keyName+"PlateThick");
  plateRadius=Control.EvalVar<double>(keyName+"PlateRadius");
  plateMat=Control.EvalVar<int>(keyName+"PlateMat");
  waterMat=Control.EvalVar<int>(keyName+"WaterMat");

  if (!(NX*NY) || (NX*NY)>4000)
    throw ColErr::IndexError<size_t>(NX*NY,4000,
				     "NXYZ array size");

  
  GType.resize(boost::extents[static_cast<long int>(NX)]
	      [static_cast<long int>(NY)]);
  Grid.resize(boost::extents[static_cast<long int>(NX)]
	      [static_cast<long int>(NY)]);
  
  // Populate grid type:
  for(size_t  i=0;i<NX;i++)
    for(size_t j=0;j<NY;j++)
      {
	const long int li(static_cast<long int>(i));
	const long int lj(static_cast<long int>(j));
	GType[li][lj]=getElement<int>(Control,keyName+"Type",i,j);
      }

  return;
}

void
ReactorGrid::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: Orign object
  */
{
  ELog::RegMethod RegA("ReactorGrid","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC);
  attachSystem::FixedComp::applyShift(xStep,yStep,zStep);
  attachSystem::FixedComp::applyAngleRotate(xyAngle,zAngle);
  return;
}

void
ReactorGrid::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("ReactorGrid","createSurface");
  // Outside surfaces
  ModelSupport::buildPlane(SMap,gridIndex+1,
			   Origin-X*Width/2.0,X);
  ModelSupport::buildPlane(SMap,gridIndex+2,
			   Origin+X*Width/2.0,X);
  

  ModelSupport::buildPlane(SMap,gridIndex+3,
			   Origin-Y*Depth/2.0,Y);
  ModelSupport::buildPlane(SMap,gridIndex+4,
			   Origin+Y*Depth/2.0,Y);


  ModelSupport::buildPlane(SMap,gridIndex+5,
			   Origin-Z*Base,Z);
  ModelSupport::buildPlane(SMap,gridIndex+6,
			   Origin+Z*Top,Z);

  // Plate Base:
  ModelSupport::buildPlane(SMap,gridIndex+5005,
			   Origin-Z*(Base+plateThick),Z);



  const double SArray[2]={Width,Depth};
  const size_t NArray[2]={NX,NY};
  const Geometry::Vec3D Axis[3]={X,Y};

  for(int dimI=0;dimI<2;dimI++)
    {
      Geometry::Vec3D layerO(Origin-Axis[dimI]*SArray[dimI]/2.0);
      const Geometry::Vec3D LStep(Axis[dimI]*(SArray[dimI]/NArray[dimI]));
      int sNum(gridIndex+7+static_cast<int>(dimI));

      SMap.addMatch(sNum,gridIndex+1+dimI*2);   // match 1,3,5 to 7,8,9
      for(size_t i=1;i<NArray[dimI];i++)
	{
	  sNum+=10; 
	  layerO+=LStep;
	  ModelSupport::buildPlane(SMap,sNum,layerO,Axis[dimI]);

	}
      sNum+=10;  // For exit
      SMap.addMatch(sNum,gridIndex+2+dimI*2);   // match 2,4 to x7,x8
    }

  // Create Plates centres:
  int cNum(gridIndex+5007);
  for(size_t i=0;i<NX;i++)
    for(size_t j=0;j<NY;j++)
      {
	ModelSupport::buildCylinder(SMap,cNum,getCellOrigin(i,j),
				    Z,plateRadius);
	cNum+=10;
      }
  return;
}

void
ReactorGrid::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("ReactorGrid","createObjects");
  const FuncDataBase& Control=System.getDataBase();

  std::string Out;
  // Outer Layers
  Out=ModelSupport::getComposite(SMap,gridIndex,"1 -2 3 -4 5005 -6 ");
  addOuterSurf(Out);      

  const std::string ZPart=
    ModelSupport::getComposite(SMap,gridIndex," 5 -6 ");
  const std::string ZPlate=
    ModelSupport::getComposite(SMap,gridIndex," -5 5005 ");

  
  // Note GridNumbers created at offset values:
  int cNum(gridIndex+5000);
  for(int i=0;i<static_cast<int>(NX);i++)
    {
      const std::string XPart=
	ModelSupport::getComposite(SMap,gridIndex+i*10," 7 -17 ");
      for(int j=0;j<static_cast<int>(NY);j++)
	{
	  const std::string YPart=
	    ModelSupport::getComposite(SMap,gridIndex+j*10," 8 -18 ");
	  
	  const size_t si=static_cast<size_t>(i);
	  const size_t sj=static_cast<size_t>(j);
	  const int MatN=getElement<int>(Control,keyName+"Mat",si,sj);
	  System.addCell(MonteCarlo::Qhull(getCellNumber(i,j),MatN,0.0,
					   XPart+YPart+ZPart));

	  // Plates:
	  const std::string Hole=
	    ModelSupport::getComposite(SMap,cNum," -7 ");
	  const std::string ExHole=
	    ModelSupport::getComposite(SMap,cNum," 7 ");
	  System.addCell(MonteCarlo::Qhull(getCellNumber(i,j)+5000,
					   plateMat,0.0,
					   XPart+YPart+ZPlate+ExHole));

	  System.addCell(MonteCarlo::Qhull(getCellNumber(i,j)+6000,
					   waterMat,0.0,ZPlate+Hole));
	  cNum+=10;
	}
    }
  
  return;
}

int
ReactorGrid::getCellNumber(const long int i,const long int j) const
  /*!
    Get Cell number
    \param i :: index of X
    \param j :: index of Y
    \return Cell number
   */
{
  return gridIndex+static_cast<int>((i+1)*100+j);
}

Geometry::Vec3D 
ReactorGrid::getCellOrigin(const size_t i,const size_t j) const
  /*!
    Get the centre of the cell item
    \param i :: index of X
    \param j :: index of Y
    \return Middle of the cell
   */
{
  ELog::RegMethod RegA("ReactorGrid","getCellOrigin");
  if (i>=NX || j>=NY) 
    throw ColErr::IndexError<size_t>(i,j,"i/j in NX/NY");
  
  return Origin+X*(Width*(2.0*i-NX+1.0)/(2.0*NX))+
    Y*(Depth*(2.0*j-NY+1.0)/(2.0*NY));
}


void
ReactorGrid::createElements(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("ReactorGrid","createElements");
  for(size_t i=0;i<NX;i++)
    for(size_t j=0;j<NY;j++)
      {
	const long int li(static_cast<long int>(i));
	const long int lj(static_cast<long int>(j));
	switch (GType[li][lj])
	  {
	  case 0:   // NULL
	    Grid[li][lj]=RTYPE(new DefElement(i,j,"delftElement"));
	    break;
	  case 1:   // FUEL
	    Grid[li][lj]=RTYPE(new FuelElement(i,j,"delftElement"));
	    break;
	  case 2:   // CONTROL
	    Grid[li][lj]=
	      RTYPE(new ControlElement(i,j,"delftElement","delftControl"));
	    break;
	  case 3:   // IRAD
	    Grid[li][lj]=
	      RTYPE(new IrradElement(i,j,"delftIrrad"));
	    break;
	  case 4:   // HF Element
	    Grid[li][lj]=
	      RTYPE(new HfElement(i,j,"delftElement","delftHf"));
	    break;
	  case 5:   // Be Element
	    Grid[li][lj]=
	      RTYPE(new BeElement(i,j,"delftBe"));
	    break;
	  case 7:   // air box
	    Grid[li][lj]=
	      RTYPE(new AirBoxElement(i,j,"delftAirBox"));
	    break;
	  default:
	    ELog::EM<<"Unknown reactor element "<<GType[li][lj]<<ELog::endErr;
	    throw ColErr::InContainerError<int>(GType[li][lj],"GType");
	  }

	Grid[li][lj]->addInsertCell(getCellNumber(li,lj));
	Grid[li][lj]->createAll(System,*this,getCellOrigin(i,j));
      }
  return;
}

void 
ReactorGrid::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("ReactorGrid","createLinks");

  FixedComp::setConnect(0,Origin-X*Width/2.0,-X);     
  FixedComp::setConnect(1,Origin+X*Width/2.0,X);     
  FixedComp::setConnect(2,Origin-Y*Depth/2.0,-Y);       
  FixedComp::setConnect(3,Origin+Y*Depth/2.0,Y);     
  FixedComp::setConnect(4,Origin-Z*Base,-Z);     
  FixedComp::setConnect(5,Origin+Z*Top,Z);     
  FixedComp::setConnect(6,Origin+Y*Depth/2.0,Y);     

  for(size_t i=0;i<7;i++)
    FixedComp::setLinkSurf
      (i,SMap.realSurf(gridIndex+static_cast<int>(i)+1));
  
  return;
}

void
ReactorGrid::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Object for origin
  */
{
  ELog::RegMethod RegA("ReactorGrid","createAll");
  
  populate(System);
  createUnitVector(FC);
  createSurfaces();
  createObjects(System);
  createLinks();

  createElements(System);
  insertObjects(System);   

  return;
}

template double 
ReactorGrid::getElement(const FuncDataBase&,const std::string&,
			const size_t,const size_t);

template int
ReactorGrid::getElement(const FuncDataBase&,const std::string&,
			const size_t,const size_t);
template size_t
ReactorGrid::getElement(const FuncDataBase&,const std::string&,
			const size_t,const size_t);
  
}  // NAMESPACE shutterSystem
