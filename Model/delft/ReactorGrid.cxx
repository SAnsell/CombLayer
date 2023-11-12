/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delft/ReactorGrid.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#include <memory>

#include <string>
#include <algorithm>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "stringCombine.h"
#include "Vec3D.h"
#include "dataSlice.h"
#include "multiData.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
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
#include "FixedRotate.h" 
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"

#include "FuelLoad.h"
#include "RElement.h"
#include "DefElement.h"
#include "FuelElement.h"
#include "ControlElement.h"
#include "HfElement.h"
#include "BeElement.h"
#include "IrradElement.h"
#include "AirBoxElement.h"
#include "BeOElement.h"
#include "ReactorGrid.h"

namespace delftSystem
{

int
ReactorGrid::getMatElement(const FuncDataBase& Control,
			   const std::string& Name,
			   const size_t I, 
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
  ELog::RegMethod RegA("ReactorGrid","getMatElement");

  ModelSupport::DBMaterial& DB=ModelSupport::DBMaterial::Instance();
  const std::string EMat=getElement<std::string>(Control,Name,I,J);

  if (DB.createMaterial(EMat))
    return DB.getIndex(EMat);
  int ENumber;
  if (StrFunc::convert(EMat,ENumber) && DB.hasKey(ENumber))
    return ENumber;
    
  throw ColErr::InContainerError<std::string>
    (EMat,"Material not present:"+Name);
}

template<typename T>
T 
ReactorGrid::getElement(const FuncDataBase& Control,
			const std::string& Name,const size_t I,
			const size_t J) 
 /*!
   Check to convert a Name + a number string into a variable state.
   \tparam T :: return type (string/int)
   \param Control :: Function base to search
   \param Name :: Master name
   \param I :: Index (A-Z) type [A on viewed face]
   \param J :: Index (Number) type
   \return variable value
 */
{
  ELog::RegMethod RegA("ReactorGrid","getElement");

  std::array<std::string,2> Key;
  Key[0]=StrFunc::indexToAlpha(I);
  Key[1]=std::to_string(J);

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
  
  throw ColErr::InContainerError<std::string>(Name,"Variable name");
}

template<typename T>
T 
ReactorGrid::getDefElement(const FuncDataBase& Control,
			   const std::string& Name,const size_t I,
			   const size_t J,const std::string& defName) 
 /*!
   Check to convert a Name + a number string into a variable state:
   \param Control :: Function base to search
   \param Name :: Master name
   \param I :: Index (A-Z) type [A on viewed face]
   \param J :: Index (Number) type
   \param defName :: default Name for element value
   \return variable value order:[Name+IJ, Name+Ix, Name+xJ,  defName , Name]
 */
{
  ELog::RegMethod RegA("ReactorGrid","getElement");

  std::array<std::string,2> Key;
  Key[0]=StrFunc::indexToAlpha(I);
  Key[1]=std::to_string(J);

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
  // Check defkey
  if (Control.hasVariable(defName))
    return Control.EvalVar<T>(defName);

  // Finally All missing 
  if (Control.hasVariable(Name))
    return Control.EvalVar<T>(Name);

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
  ELog::RegMethod RegA("ReactorGrid","getElementName");
  
  std::string Out(Name);
  Out+=StrFunc::indexToAlpha(I);
  Out+=std::to_string(J);
  return Out;
}

std::pair<size_t,size_t>
ReactorGrid::getElementNumber(const std::string& Name)
 /*!
   Check to convert a Name + a number string into a variable state:
   \param Name :: Master name
   \return Index numbers [A1 ==> 0 0]
 */
{
  ELog::RegMethod RegA("ReactorGrid","getElementNumber");

  if (Name.size()!=2 && Name.size()!=3)
    throw ColErr::InvalidLine(Name,"Name wrong size",Name.size());
  std::pair<size_t,size_t> Out;
  Out.first=static_cast<size_t>(std::toupper(Name[0])-std::toupper('A'));
  if (!StrFunc::convert(Name.substr(1),Out.second))
    throw ColErr::InvalidLine(Name,"Name number not understood");
  return Out;
}


ReactorGrid::ReactorGrid(const std::string& Key) : 
  attachSystem::FixedOffset(Key,7),attachSystem::ContainedComp(),
  attachSystem::CellMap()
  /*!
    Constructor BUT ALL variable are left unpopulateed.
    \param Key :: KeyName
  */
{}

ReactorGrid::ReactorGrid(const ReactorGrid& A) : 
  attachSystem::FixedOffset(A),attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),
  NX(A.NX),NY(A.NY),Width(A.Width),Depth(A.Depth),Base(A.Base),
  Top(A.Top),plateThick(A.plateThick),plateRadius(A.plateRadius),
  plateMat(A.plateMat),waterMat(A.waterMat),GType(A.GType),
  Grid(A.Grid)
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
      attachSystem::FixedOffset::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      cellIndex=A.cellIndex;
      NX=A.NX;
      NY=A.NY;
      Width=A.Width;
      Depth=A.Depth;
      Base=A.Base;
      Top=A.Top;
      plateThick=A.plateThick;
      plateRadius=A.plateRadius;
      plateMat=A.plateMat;
      waterMat=A.waterMat;
      GType=A.GType;
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
ReactorGrid::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Function
  */
{
  ELog::RegMethod RegA("ReactorGrid","populate");

  FixedOffset::populate(Control);
  
  NX=Control.EvalVar<size_t>(keyName+"XSize");
  NY=Control.EvalVar<size_t>(keyName+"YSize");

  Width=Control.EvalVar<double>(keyName+"Width");
  Depth=Control.EvalVar<double>(keyName+"Depth");
  Top=Control.EvalVar<double>(keyName+"Top");
  Base=Control.EvalVar<double>(keyName+"Base");

  plateThick=Control.EvalVar<double>(keyName+"PlateThick");
  plateRadius=Control.EvalVar<double>(keyName+"PlateRadius");
  plateMat=ModelSupport::EvalMat<int>(Control,keyName+"PlateMat");
  waterMat=ModelSupport::EvalMat<int>(Control,keyName+"WaterMat");
  
  if ( (NX*NY)==0 || (NX*NY)>4000)
    throw ColErr::IndexError<size_t>(NX*NY,4000,
				     "NXYZ array size");

  GType.resize(NX,NY);
  Grid.resize(NX,NY);
  // GType.resize(boost::extents[static_cast<long int>(NX)]
  // 	      [static_cast<long int>(NY)]);
  // Grid.resize(boost::extents[static_cast<long int>(NX)]
  // 	      [static_cast<long int>(NY)]);
  
  // Populate grid type:
  for(size_t  i=0;i<NX;i++)
    for(size_t j=0;j<NY;j++)
      {
	GType.get()[i][j]=
	  getElement<std::string>(Control,keyName+"Type",i,j);
      }

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
  ModelSupport::buildPlane(SMap,buildIndex+1,
			   Origin-X*Width/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+2,
			   Origin+X*Width/2.0,X);
  

  ModelSupport::buildPlane(SMap,buildIndex+3,
			   Origin-Y*Depth/2.0,Y);
  ModelSupport::buildPlane(SMap,buildIndex+4,
			   Origin+Y*Depth/2.0,Y);


  ModelSupport::buildPlane(SMap,buildIndex+5,
			   Origin-Z*Base,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,
			   Origin+Z*Top,Z);

  // Plate Base:
  ModelSupport::buildPlane(SMap,buildIndex+5005,
			   Origin-Z*(Base+plateThick),Z);



  const double SArray[2]={Width,Depth};
  const size_t NArray[2]={NX,NY};
  const Geometry::Vec3D Axis[3]={X,Y};

  for(int dimI=0;dimI<2;dimI++)
    {
      Geometry::Vec3D layerO(Origin-Axis[dimI]*SArray[dimI]/2.0);
      const Geometry::Vec3D LStep
	(Axis[dimI]*(SArray[dimI]/static_cast<double>(NArray[dimI])));
      
      int sNum(buildIndex+7+dimI);

      SMap.addMatch(sNum,buildIndex+1+dimI*2);   // match 1,3,5 to 7,8,9
      for(size_t i=1;i<NArray[dimI];i++)
	{
	  sNum+=10; 
	  layerO+=LStep;
	  ModelSupport::buildPlane(SMap,sNum,layerO,Axis[dimI]);
	}
      
      sNum+=10;  // For exit
      SMap.addMatch(sNum,buildIndex+2+dimI*2);   // match 2,4 to x7,x8
    }

  // Create Plates centres:
  int cNum(buildIndex+5007);
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
  HeadRule HR;
  // Outer Layers
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 3 -4 5005 -6");
  addOuterSurf(HR);      

  const HeadRule ZPartHR=
    ModelSupport::getHeadRule(SMap,buildIndex,"5 -6");
  const HeadRule ZPlateHR=
    ModelSupport::getHeadRule(SMap,buildIndex,"-5 5005");

  
  // Note GridNumbers created at offset values:
  int cNum(buildIndex+5000);
  for(size_t i=0;i<NX;i++)
    {
      const int BI(buildIndex+static_cast<int>(i*10));
      const HeadRule XPartHR=
	ModelSupport::getHeadRule(SMap,BI,"7 -17");
      for(size_t j=0;j<NY;j++)
	{
	  const int JI(buildIndex+static_cast<int>(j*10));
	  const HeadRule YPartHR=
	    ModelSupport::getHeadRule(SMap,JI,"8 -18");
	  
	  const int MatN=getMatElement(Control,keyName+"Mat",i,j);
	  System.addCell(getCellNumber(i,j),MatN,0.0,XPartHR*YPartHR*ZPartHR);

	  // Plates:
	  const HeadRule HoleHR=
	    ModelSupport::getHeadRule(SMap,cNum," -7 ");
	  const HeadRule ExHoleHR=HoleHR.complement();

	  
	  System.addCell(getCellNumber(i,j)+5000,
			 plateMat,0.0,
			 XPartHR*YPartHR*ZPlateHR*ExHoleHR);

	  System.addCell(getCellNumber(i,j)+6000,
			 waterMat,0.0,ZPlateHR*HoleHR);
	  cNum+=10;
	}
    }
  
  return;
}

int
ReactorGrid::getCellNumber(const size_t i,const size_t j) const
  /*!
    Get Cell number
    \param i :: index of X
    \param j :: index of Y
    \return Cell number
   */
{
  return buildIndex+static_cast<int>((i+1)*100+j);
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

  const double halfWidth=2.0*static_cast<double>(i+1)-
    static_cast<double>(NX+1);
  const double halfDepth=2.0*static_cast<double>(j+1)-
    static_cast<double>(NY+1);
  
  return Origin+
    X*(Width*halfWidth/static_cast<double>(2*NX))+
    Y*(Depth*halfDepth/static_cast<double>(2*NY));
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
	if (GType.get()[i][j]=="Null")
	  Grid.get()[i][j]=RTYPE(new DefElement(i,j,"delftElement"));

	else if (GType.get()[i][j]=="Fuel")
	  Grid.get()[i][j]=RTYPE(new FuelElement(i,j,"delftElement"));

	else if (GType.get()[i][j]=="Control")
	  Grid.get()[i][j]=
	    RTYPE(new ControlElement(i,j,"delftElement","delftControl"));

	else if (GType.get()[i][j]=="IRad")
	    Grid.get()[i][j]=
	      RTYPE(new IrradElement(i,j,"delftIrrad"));

	else if (GType.get()[i][j]=="HfControl")
	    Grid.get()[i][j]=
	      RTYPE(new HfElement(i,j,"delftElement","delftHf"));

	else if (GType.get()[i][j]=="Be")
	    Grid.get()[i][j]=
	      RTYPE(new BeElement(i,j,"delftBe"));

	else if (GType.get()[i][j]=="BeO")
	    Grid.get()[i][j]=
	      RTYPE(new BeOElement(i,j,"delftBeO"));

	else if (GType.get()[i][j]=="Air")
	  Grid.get()[i][j]=
	    RTYPE(new AirBoxElement(i,j,"delftAirBox",waterMat));

	else
	  {
	    throw ColErr::InContainerError<std::string>
	      (GType.get()[i][j],"GType");
	  }
	Grid.get()[i][j]->addInsertCell(getCellNumber(i,j));
	Grid.get()[i][j]->setFuelLoad(FuelSystem);
	Grid.get()[i][j]->setCutSurf("BasePlate",SMap.realSurf(buildIndex+5));
	Grid.get()[i][j]->createAll(System,*this,getElementName("Grid",i,j));
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
  

  FixedComp::setLinkSurf(0,SMap.realSurf(buildIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+3));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));
  FixedComp::setLinkSurf(4,SMap.realSurf(buildIndex+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));
  FixedComp::setLinkSurf(6,SMap.realSurf(buildIndex+7));


  const size_t NGrid(NX*NY);

  FixedComp::setNConnect(NGrid+8);
  size_t FCIndex(6);
  for(size_t i=0;i<NX;i++)
    for(size_t j=0;j<NY;j++)
      {
	FixedComp::nameSideIndex(FCIndex,getElementName("Grid",i,j));
	FixedComp::setConnect(FCIndex,getCellOrigin(i,j),Y);
	FCIndex++;
      }
  
  return;
}

std::vector<Geometry::Vec3D>
ReactorGrid::fuelCentres() const 
  /*!
    Get the fuel centers 
    \return Centre vector
   */
{
  ELog::RegMethod RegA("ReactorGrid","FuelCentres");

  std::vector<Geometry::Vec3D> CPos;
  for(size_t i=0;i<NX;i++)
    for(size_t j=0;j<NY;j++)
       {
	 std::shared_ptr<FuelElement>
	   FPtr=std::dynamic_pointer_cast<FuelElement>
	   (Grid.get()[i][j].get());
	 if (FPtr)
	   {
	     const std::vector<Geometry::Vec3D>& CV=
	       FPtr->getFuelCentre();
	     CPos.insert(CPos.end(),CV.begin(),CV.end());
	   }
       }
  return CPos;
}

std::vector<int>
ReactorGrid::getFuelCells(const Simulation& System,
			  const size_t zaid) const
  /*!
    Get a comprehensive list of all cells
    \param System :: Simualation to check cell existance
    \param zaid :: isotope to check for in zaid
    \return All cell numbers of fuel
   */
{
  ELog::RegMethod RegA("ReactorGrid","getFuelCells");
  
  std::vector<int> cellOut;
  for(size_t i=0;i<NX;i++)
    {
      for(size_t j=0;j<NY;j++)
	{
	  const std::set<int> cellVec=
	    System.getObjectRange(Grid.get()[i][j]->getItemKeyName());
	  for(const int index : cellVec)
	    {
	      const MonteCarlo::Object* OPtr=
		System.findObject(index);
	      if (OPtr)
		{
		  const MonteCarlo::Material* matPtr=OPtr->getMatPtr();
		  if (matPtr->hasZaid(zaid,0,0))
		    cellOut.push_back(index);
		}
	    }
	}
    }
  return cellOut;
}

std::vector<int>
ReactorGrid::getAllCells(const Simulation& System) const
  /*!
    Get a comprehensive list of all cells
    \param System :: Simualation to check cell existance
    \return All cell numbers
   */
{
  ELog::RegMethod RegA("ReactorGrid","getAllCells");

  std::vector<int> cellOut;
  for(size_t i=0;i<NX;i++)
    {
      for(size_t j=0;j<NY;j++)
	{
	  const std::set<int> cellVec=
	    System.getObjectRange(Grid.get()[i][j]->getItemKeyName());
	  for(const int index : cellVec)
	    {
	      if (System.existCell(index))
		cellOut.push_back(index);
	    }
	}
    }
  return cellOut;
}

void
ReactorGrid::loadFuelXML(const std::string& FName) 
  /*!
    Load the fuel Elements
    \param FName :: Output name
  */
{
   ELog::RegMethod RegA("ReactorGrid","loadFuelXML");

   FuelSystem.loadXML(FName);
   return;
  
}

void
ReactorGrid::writeFuelXML(const std::string& FName)  
  /*!
    Write out the fuel Elements
    \param FName :: Output name
  */
{
   ELog::RegMethod RegA("ReactorGrid","writeFuelXML");
   if (FName.empty())
     return;

   const ModelSupport::DBMaterial& DB=
     ModelSupport::DBMaterial::Instance();   
   FuelLoad OutSystem;
   for(size_t i=0;i<NX;i++)
     for(size_t j=0;j<NY;j++)
       {	 
	 std::shared_ptr<const FuelElement> FPtr=
	   std::dynamic_pointer_cast<const FuelElement>
	   (Grid.get()[i][j].get());
	 if (FPtr)
	   {
	     for(size_t nE=0;nE<FPtr->getNElements();nE++)
	       {
		 if (FPtr->isFuel(nE))
		   {
		     const int defMat=FPtr->getDefMat();
		     for(size_t nIndex=0;nIndex<FPtr->getNSections();nIndex++)
		       {
			 const int NF=
			   FuelSystem.getMaterial(i+1,j+1,nE+1,nIndex+1,defMat);
			 const std::string& MName=DB.getKey(NF);
			 OutSystem.setMaterial(i+1,j+1,nE+1,nIndex+1,MName);
		       }
		   }
	       }
	     OutSystem.addXML(i,j,FPtr->getNElements(),
			      FPtr->getNSections(),
			      FPtr->getRemovedSet());
	   }
       }
   OutSystem.writeXML(FName);
   return;
  
}

void
ReactorGrid::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Object for origin
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("ReactorGrid","createAll");
  
  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();

  createElements(System);
  insertObjects(System);   

  return;
}

///\cond TEMPLATE
  
template double 
ReactorGrid::getElement(const FuncDataBase&,const std::string&,
			const size_t,const size_t);

template int
ReactorGrid::getElement(const FuncDataBase&,const std::string&,
			const size_t,const size_t);
template size_t
ReactorGrid::getElement(const FuncDataBase&,const std::string&,
			const size_t,const size_t);

template double 
ReactorGrid::getDefElement(const FuncDataBase&,const std::string&,
			const size_t,const size_t,const std::string&);

template int
ReactorGrid::getDefElement(const FuncDataBase&,const std::string&,
			const size_t,const size_t,const std::string&);
template size_t
ReactorGrid::getDefElement(const FuncDataBase&,const std::string&,
			const size_t,const size_t,const std::string&);

///\endcond TEMPLATE

}  // NAMESPACE delftSystem
