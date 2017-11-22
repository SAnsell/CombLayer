/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tally/gridConstruct.cxx
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
#include <iterator>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "support.h"
#include "stringCombine.h"
#include "NList.h"
#include "NRange.h"
#include "Tally.h"
#include "TallyCreate.h"
#include "Transform.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Line.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "MainProcess.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "PositionSupport.h"
#include "LinkSupport.h"
#include "Simulation.h"
#include "inputParam.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "SurfLine.h"
#include "ContainedComp.h"

#include "MeshGrid.h"
#include "TallySelector.h" 
#include "SpecialSurf.h"
#include "gridConstruct.h" 


namespace tallySystem
{

gridConstruct::gridConstruct() 
  /// Constructor
{}

gridConstruct::gridConstruct(const gridConstruct&) 
  /// Copy Constructor
{}

gridConstruct&
gridConstruct::operator=(const gridConstruct&) 
  /// Assignment operator
{
  return *this;
}

void
gridConstruct::processGrid(Simulation& System,
			   const mainSystem::inputParam& IParam,
			   const size_t Index) const
  /*!
    Add grid tally as needed
    \param System :: Simulation to add tallies
    \param IParam :: Main input parameters
    \param Index :: index of the -T card
   */
{
  ELog::RegMethod RegA("gridConstruct","processGrid");

  const size_t NItems=IParam.itemCnt("tally",Index);
  if (NItems<4)
    throw ColErr::IndexError<size_t>(NItems,4,
				     "Insufficient items for tally");
  const size_t NItemsT=IParam.itemCnt("TGrid",0);
  if (NItemsT<2)
    throw ColErr::IndexError<size_t>(NItemsT,2,
				     "Insufficient TGrid for tally");

  
  ELog::EM<<"NItems = "<<NItems<<ELog::endDiag;
  ELog::EM<<"T = "<<NItemsT<<":"
	  <<IParam.flag("TGrid")<<":"
	  <<IParam.dataCnt("TGrid")<<ELog::endDiag;

  //    NItems<<ELog::endDiag;

  const std::string PType(IParam.getValue<std::string>("tally",Index,1));   
  const masterRotate& MR=masterRotate::Instance();

  // First point to used
  const size_t initNPD=IParam.getValue<size_t>("TGrid",0);
  const size_t NPD=IParam.getValue<size_t>("TGrid",1);       // Total PTS
  
  if (PType=="free")
    {
      size_t itemIndex(2);
      Geometry::Vec3D TOrigin=
	IParam.getCntVec3D("tally",Index,itemIndex,"Centre point detector");
      Geometry::Vec3D XVec=
	IParam.getCntVec3D("tally",Index,itemIndex,"XVector");
      Geometry::Vec3D YVec=
	IParam.getCntVec3D("tally",Index,itemIndex,"YVector");

      TOrigin=MR.reverseRotate(TOrigin);
      XVec=MR.reverseAxisRotate(XVec);
      YVec=MR.reverseAxisRotate(YVec);
      applyMultiGrid(System,initNPD,NPD,TOrigin,XVec,YVec);
    }
  
  else if (PType=="object")
    {
      size_t offsetIndex(4);
      const std::string place=
	IParam.getValueError<std::string>
	("tally",Index,2,"object name not given");
      
      const std::string snd=
	IParam.getValueError<std::string>
	("tally",Index,3,"front/back/side not give");

      Geometry::Vec3D TOrigin=
	IParam.getCntVec3D("tally",Index,offsetIndex,"Centre point offset");
      Geometry::Vec3D XVec=
	IParam.getCntVec3D("tally",Index,offsetIndex,"XVector scale");

      Geometry::Vec3D YVec=
	IParam.getCntVec3D("tally",Index,offsetIndex,"YVector scale");
      ELog::EM<<"TOgin == "<<XVec<<ELog::endDiag;
      ELog::EM<<"OI == "<<offsetIndex<<ELog::endDiag;


      const long int linkNumber=attachSystem::getLinkIndex(snd);
      if (!calcGlobalCXY(place,linkNumber,TOrigin,XVec,YVec))
	applyMultiGrid(System,initNPD,NPD,TOrigin,XVec,YVec);
    }

  else
    {
      ELog::EM<<"Grid TallyType "<<PType<<" ignored"<<ELog::endWarn;
    }
   return;
}

int
gridConstruct::calcGlobalCXY(const std::string& Place,
			     const long int linkNumber,
			     Geometry::Vec3D& Centre,
			     Geometry::Vec3D& XVec,
			     Geometry::Vec3D& YVec) const
/*!
  Calculate the global Center/XY axis based on C/X/Y which
  are currently in the local frame

  \param Place :: direction of the object
  \param linkNumber :: link number  
  \param Centre :: Centre of the tally
  \param XVec :: Direction in X [local -> global]
  \param YVec :: Direction in Y [local -> global]
  \return -ve on error
 */
{
  ELog::RegMethod RegA("gridConstruct","calcGlobalCXY");

  const ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  const attachSystem::FixedComp* FC=
    OR.getObjectThrow<attachSystem::FixedComp>(Place,"FixedComp");
  

  const Geometry::Vec3D O=FC->getLinkPt(linkNumber);

  Geometry::Vec3D X,Y,Z;
  FC->calcLinkAxis(linkNumber,X,Y,Z);

  Centre=O+X*Centre.X()+Y*Centre.Y()+Z*Centre.Z();
  ELog::EM<<"Centre == "<<Centre<<ELog::endDiag;
  ELog::EM<<"XVEC == "<<XVec<<ELog::endDiag;
  ELog::EM<<"YVEC == "<<YVec<<ELog::endDiag;
  XVec=X*XVec.X()+Y*XVec.Y()+Z*XVec.Z();
  YVec=X*YVec.X()+Y*YVec.Y()+Z*YVec.Z();

  return 0; 
}
			     

void
gridConstruct::applyMultiGrid(Simulation& System,
			      const size_t initNPD,
			      const size_t NPD,
			      const Geometry::Vec3D& TOrigin,
			      const Geometry::Vec3D& Xv,
			      const Geometry::Vec3D& Yv) const
  /*!
    Actually proces the grid system based on the input mesh
    \param System :: simulation to add tallies
    \param initNPD :: Initial number [1 normally]
    \param NPD :: Number of point detectors
    \param TOrigin :: Centre of the plane
    \param Xv :: Direction X [scaled]
    \param Yv :: Direction Y [scaled]
   */
{
  ELog::RegMethod RegA("gridConstruct","applyMultiGrid");

  const masterRotate& MR=masterRotate::Instance();
  Geometry::MeshGrid RT(TOrigin,Xv,Yv);
  
  for(size_t NI=1;NI<initNPD;NI++)
    RT.getNext();

  int level(RT.getLevel());
  ELog::EM<<"Tally Centre Point[AFTER OFFSET] == "
	  <<MR.calcRotate(TOrigin)<<ELog::endTrace;
  ELog::EM<<"XY == "<<MR.calcAxisRotate(Xv)<< "::"
	  <<MR.calcAxisRotate(Yv)<<ELog::endTrace;
  ELog::EM<<"Starting at level "<<level<<ELog::endTrace;
      
  int tNum(15);
  for(size_t NI=0;NI<NPD;NI++,tNum+=10)
    {
      Geometry::Vec3D TPos(RT.getNext());
      tallySystem::addF5Tally(System,tNum);      
      // SET CENTRE POSITION:
      tallySystem::setF5Position(System,tNum,TPos);
      tallySystem::setTallyTime(System,tNum,"1.0 8log 1e8");
	  
      ELog::FM<<"GRID TALLY POSITION "<<NI<<" : "
	      <<MR.calcRotate(TPos)<<ELog::endDiag;
	  
      ELog::EM<<"F"<<tNum<<" ["<<NI+initNPD<<"] Pos == "
	      <<MR.calcRotate(TPos)<<ELog::endDiag;
      if (RT.getLevel()!=level)
	{
	  level=RT.getLevel();
	  ELog::EM<<"Advancing to level "<<level<<ELog::endWarn;
	}
    }
  return;
}

  
void
gridConstruct::writeHelp(std::ostream& OX) const
  /*!
    Write out help
    \param OX :: Output stream
  */
{
  OX<<
    "gridTally option:\n"
    "free Vec3D[center] Vec3D[X] Vec3D[Y]-- grid at centre \n"
    "object ObjName linkUnit Vec3D[center] Vec3D[X] Vec3D[Y] "
    " using object X/Y/Z \n";
  return;
}


  
}  // NAMESPACE tallySystem
