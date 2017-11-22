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
#include "surfRegister.h"
#include "objectRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
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
#include "support.h"
#include "stringCombine.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"

#include "LayerPlate.h"

namespace sinbadSystem
{

LayerPlate::LayerPlate(const std::string& Key) : 
  attachSystem::ContainedComp(),attachSystem::FixedOffset(Key,6),
  slabIndex(ModelSupport::objectRegister::Instance().cell(Key)), 
  cellIndex(slabIndex+1),frontShared(1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

LayerPlate::LayerPlate(const LayerPlate& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  slabIndex(A.slabIndex),cellIndex(A.cellIndex),
  frontShared(A.frontShared),
  width(A.width),height(A.height),nSlab(A.nSlab),
  thick(A.thick),mat(A.mat),matTemp(A.matTemp),
  radiusWindow(A.radiusWindow)
  /*!
    Copy constructor
    \param A :: LayerPlate to copy
  */
{}

LayerPlate&
LayerPlate::operator=(const LayerPlate& A)
  /*!
    Assignment operator
    \param A :: LayerPlate to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      cellIndex=A.cellIndex;
      frontShared=A.frontShared;
      width=A.width;
      height=A.height;
      nSlab=A.nSlab;
      thick=A.thick;
      mat=A.mat;
      matTemp=A.matTemp;
      radiusWindow=A.radiusWindow;
    }
  return *this;
}


LayerPlate*
LayerPlate::clone() const
  /*!
    Virtual copy constructor
    \return new(this)
  */
{
  return new LayerPlate(*this);
}

LayerPlate::~LayerPlate() 
  /*!
    Destructor
  */
{}

int
LayerPlate::getCellIndex(const size_t lNumber) const
  /*!
    Determine the cell index based on lNumber
    \param lNumber :: Layer number
    \return cellNumber
  */
{
  ELog::RegMethod RegA("LayerPlate","getCellIndex");

  if (lNumber >= nSlab)
    throw ColErr::IndexError<size_t>
      (lNumber,nSlab,"lNumber");
  
  return slabIndex+1+static_cast<int>(lNumber);
}

std::string
LayerPlate::getFrontSurface(const size_t layerIndex,
			const attachSystem::FixedComp& FC,
			const long int sideIndex) const
  /*!
    Get the front/back surfaces based on the link object 
    \param layerIndex :: Layer Number
    \param FC :: Link object
    \param sideIndex :: Connection point [signed for direction/ zero = centre]
    \return surface string 
   */
{
  ELog::RegMethod RegA("LayerPlate","getFrontSurface");

  if (layerIndex>0 || !sideIndex || !frontShared)
    {
      const int SI(slabIndex+static_cast<int>(layerIndex)*10);
      return ModelSupport::getComposite(SMap,SI," 1 ");
    }
  return FC.getLinkString(sideIndex);
}

std::string
LayerPlate::getBackSurface(const size_t layerIndex,
		       const attachSystem::FixedComp&,
		       const long int ) const
  /*!
    Get the front/back surfaces based on the link object 
    \param layerIndex :: Layer Number
    \param FC :: Link object
    \param sideIndex :: Connection point [signed for direction/ zero = centre]
    \return surface string 
   */
{
  ELog::RegMethod RegA("LayerPlate","getBackSurface");

  const int SI(slabIndex+static_cast<int>(layerIndex)*10);
  return ModelSupport::getComposite(SMap,SI," -11 ");  
}


void
LayerPlate::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Function data base to use
  */
{
  ELog::RegMethod RegA("LayerPlate","populate");
  attachSystem::FixedOffset::populate(Control);

  height=Control.EvalVar<double>(keyName+"Height");
  width=Control.EvalVar<double>(keyName+"Width");
  radiusWindow=Control.EvalDefVar<double>(keyName+"AlWindowRadius",0.0);

  nSlab=Control.EvalVar<size_t>(keyName+"NSlab");

  double Len,Tmp;
  int M;
  
  for(size_t i=0;i<nSlab;i++)
    {
      const std::string NStr(StrFunc::makeString(i));
      Len=Control.EvalVar<double>(keyName+"Thick"+NStr);
      Tmp=Control.EvalDefVar<double>(keyName+"Temp"+NStr,0.0);
      M=ModelSupport::EvalMat<int>(Control,keyName+"Mat"+NStr);

      thick.push_back(Len);   
      mat.push_back(M);   
      matTemp.push_back(Tmp);   
    }

  return;
}


void
LayerPlate::createUnitVector(const attachSystem::FixedComp& FC,
			 const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: FixedComponent for origin
    \param sideIndex :: Connection point [signed for direction/ zero = centre]
  */
{
  ELog::RegMethod RegA("LayerPlate","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  
  if (std::abs(yStep)>Geometry::zeroTol || 
      std::abs(xyAngle)>Geometry::zeroTol ||
      std::abs(zAngle)>Geometry::zeroTol)
    frontShared=0;
      
      
  return;
}


void
LayerPlate::createSurfaces()
  /*!
    Create All the surfaces
   */
{
  ELog::RegMethod RegA("LayerPlate","createSurface");

  // Special case for sideIndex 1
  //  

  ModelSupport::buildPlane(SMap,slabIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,slabIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,slabIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,slabIndex+6,Origin+Z*(height/2.0),Z);

  if (radiusWindow>Geometry::zeroTol)
    ModelSupport::buildCylinder(SMap,slabIndex+7,Origin,Y,radiusWindow);

  
  int SI(slabIndex);
  double totalThick(0.0);
  for(size_t i=0;i<=nSlab;i++)
   {
     ModelSupport::buildPlane(SMap,SI+1,Origin+Y*totalThick,Y);
     if (i!=nSlab)
       {
	 totalThick+=thick[i];
	 SI+=10;
       }
   }

  FixedComp::setConnect(1,Origin+Y*totalThick,Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(SI+1));

  return;
}


void
LayerPlate::createObjects(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int sideIndex)
  /*!
    Create all the objects
    \param System :: Simulation to create objects in
    \param FC :: FixedComp
    \param sideIndex :: SIGNED sideIndex offset by 1 [0 base origin]
  */
{
  ELog::RegMethod RegA("LayerPlate","createObjects");
  if (mat.empty()) return;

  const std::string FSurf=getFrontSurface(0,FC,sideIndex);
  std::string Out;

  // Front one uses FC/sideIndex 
  Out=FSurf+ModelSupport::getComposite(SMap,slabIndex," -11 3 -4 5 -6");
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat[0],matTemp[0],Out)); 
  
  int SI(slabIndex);
  for(size_t i=1;i<nSlab;i++)
   {
     SI+=10;
     Out=ModelSupport::getComposite(SMap,slabIndex,SI,"1M -11M 3 -4 5 -6");
     System.addCell(MonteCarlo::Qhull(cellIndex++,mat[i],matTemp[i],Out)); 
   }
  
  Out=ModelSupport::getComposite(SMap,slabIndex,SI," -11M 3 -4 5 -6");
  addOuterSurf(FSurf+Out);
  return;
}

void
LayerPlate::createLinks()
  /*!
    Create all the links
  */
{
  ELog::RegMethod RegA("LayerPlate","createLinks");

  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setLinkSurf(0,SMap.realSurf(slabIndex+1));
  // Attacth 1 : provided by createSurfaces

  FixedComp::setConnect(2,Origin-X*(width/2.0),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(slabIndex+3));
  FixedComp::setConnect(3,Origin+X*(width/2.0),-X);
  FixedComp::setLinkSurf(3,SMap.realSurf(slabIndex+4));
  FixedComp::setConnect(4,Origin-Z*(height/2.0),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(slabIndex+5));
  FixedComp::setConnect(5,Origin+Z*(height/2.0),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(slabIndex+6));

  return;
}

void
LayerPlate::createAll(Simulation& System,
		  const attachSystem::FixedComp& FC,
		  const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed Component for origin
    \param sideIndex :: Direction/type of side index [0 central origin]
  */
{
  ELog::RegMethod RegA("LayerPlate","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System,FC,sideIndex);
  createLinks();
  insertObjects(System);

  return;
}
  
} 
