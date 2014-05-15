/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   tally/tallyConstruct.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/format.hpp>

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
#include "objectRegister.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "Simulation.h"
#include "inputParam.h"
#include "MeshGrid.h"
#include "NRange.h"
#include "pairRange.h"
#include "Tally.h"
#include "TallyCreate.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "meshTally.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"

#include "TallySelector.h" 
#include "basicConstruct.h" 
#include "pointConstruct.h" 
#include "meshConstruct.h" 
#include "fluxConstruct.h" 
#include "fissionConstruct.h" 
#include "heatConstruct.h" 
#include "itemConstruct.h" 
#include "surfaceConstruct.h" 
#include "tallyConstructFactory.h" 
#include "tallyConstruct.h" 


namespace tallySystem
{

// static definitions:
std::map<std::string,int> tallyConstruct::chipGridPos;

void
tallyConstruct::initStatic()
  /*!
    Initialize the static members
  */
{
  typedef std::map<std::string,int> MTYPE;
  chipGridPos.insert(MTYPE::value_type("chipTable1",0));
  chipGridPos.insert(MTYPE::value_type("chipTable2",1));
  chipGridPos.insert(MTYPE::value_type("chipPreCol",2));
  chipGridPos.insert(MTYPE::value_type("chipVCol",3));
  chipGridPos.insert(MTYPE::value_type("chipHCol",4));
  chipGridPos.insert(MTYPE::value_type("chipSeparator",5));

  return;
}

tallyConstruct::tallyConstruct(const tallyConstructFactory& FC) : 
  basicConstruct(),pointPtr(FC.makePoint()),
  meshPtr(FC.makeMesh()),fluxPtr(FC.makeFlux()),
  heatPtr(FC.makeHeat()),itemPtr(FC.makeItem()),
  surfPtr(FC.makeSurf()),fissionPtr(FC.makeFission())
  /*!
    Constructor
    \param FC :: Factory object to specialize constructors
   */
{
  initStatic();
}


tallyConstruct::~tallyConstruct()
  /*!
    Delete operator
   */
{
  delete pointPtr;
  delete meshPtr;
  delete fluxPtr;
  delete heatPtr;
  delete itemPtr;
  delete surfPtr;
  delete fissionPtr;
}

void
tallyConstruct::setPoint(pointConstruct* PPtr) 
  /*!
    Modify/assign the pointConstructor 
    \param PPtr :: New point Ptr [MANGAGED]
  */
{
  if (PPtr)
    {
      delete pointPtr;
      pointPtr=PPtr;
    }
  return;
}

void
tallyConstruct::setFission(fissionConstruct* PPtr) 
  /*!
    Modify/assign the pointConstructor 
    \param PPtr :: New point Ptr [MANGAGED]
  */
{
  if (PPtr)
    {
      delete fissionPtr;
      fissionPtr=PPtr;
    }
  return;
}

int
tallyConstruct::tallySelection(Simulation& System,
			       const mainSystem::inputParam& IParam) const
 /*!
    An amalgumation of values to determine what sort of tallies to put
    in the system.
    \param System :: Simulation to add tallies
    \param IParam :: Main input parameters
    \return flag to indicate that more work is required after renumbering
  */
{
  ELog::RegMethod RegA("tallyConstruct","tallySelection");

  int workFlag(0);  
  for(size_t i=0;i<IParam.grpCnt("tally");i++)
    {
      const std::string TType=
	IParam.getCompValue<std::string>("tally",i,0);

      if (TType=="help" || TType=="?")
	helpTallyType();

      else if (TType=="grid") 
	processGrid(System,IParam,i);
      else if (TType=="point")
	pointPtr->processPoint(System,IParam,i);
      else if (TType=="mesh")
	meshPtr->processMesh(System,IParam,i);
      else if (TType=="flux")
	workFlag+=fluxPtr->processFlux(System,IParam,i,0);
      else if (TType=="fission")
	workFlag+=fissionPtr->processPower(System,IParam,i,0);
      else if (TType=="heat")
	heatPtr->processHeat(System,IParam,i);
      else if (TType=="item")
	itemPtr->processItem(System,IParam,i);
      else if (TType=="surface")
	workFlag+=surfPtr->processSurface(System,IParam,i);
      else
	ELog::EM<<"Unable to understand tally type :"<<TType<<ELog::endErr;
    }
  if (IParam.flag("Txml"))
    tallySystem::addXMLtally(System,IParam.getValue<std::string>("Txml"));
      
  return workFlag;
}

int
tallyConstruct::tallyRenumber(Simulation& System,
			      const mainSystem::inputParam& IParam) const
 /*!
    An amalgumation of values to determine what sort of tallies to put
    in the system.
    \param System :: Simulation to add tallies
    \param IParam :: Main input parameters
    \return flag to indicate that more work is required after renumbering
  */
{
  ELog::RegMethod RegA("tallyConstruct","tallySelection");

  int workFlag(0);  
  for(size_t i=0;i<IParam.grpCnt("tally");i++)
    {
      const std::string TType=
	IParam.getCompValue<std::string>("tally",i,0);

      if (TType=="flux")
	fluxPtr->processFlux(System,IParam,i,1);
      else if (TType=="heat")
	heatPtr->processHeat(System,IParam,i);
    }

  // if (IParam.flag("Txml"))
  //   tallySystem::addXMLtally(System,IParam.getValue<std::string>("Txml"));
      
  return workFlag;
}

void
tallyConstruct::processGridFree(Simulation& System,const int initNPD,
				const int NPD) const
  /*!
    Processes a grid tally : Requires variables and informaton 
    \param System :: Simulation to add tallies
    \param initNPD :: Initial index number for tally
    \param NPD :: Number of point detectors 
  */
{
  ELog::RegMethod RegA("tallyConstruct","processFreeGrid");

  const FuncDataBase& Control=System.getDataBase();
  const masterRotate& MR=masterRotate::Instance();

  Geometry::Vec3D TOrigin;
  Geometry::Vec3D XVec;
  Geometry::Vec3D ZVec;

  const double HSize=Control.EvalVar<double>("gridTallyHSize");      
  const double VSize=Control.EvalVar<double>("gridTallyVSize");      
  TOrigin=Control.EvalVar<Geometry::Vec3D>("gridTallyOrigin");      
  XVec=Control.EvalVar<Geometry::Vec3D>("gridTallyXVec");      
  ZVec=Control.EvalVar<Geometry::Vec3D>("gridTallyZVec");      
  XVec.makeUnit();
  ZVec.makeUnit();
  XVec*=2.0*HSize;
  ZVec*=2.0*VSize;
  // Need to reverse since in output frame:
  TOrigin=MR.reverseRotate(TOrigin);
  XVec=MR.reverseAxisRotate(XVec);
  ZVec=MR.reverseAxisRotate(ZVec);
  
  applyMultiGrid(System,initNPD,NPD,TOrigin,XVec,ZVec);
  return;
}

void
tallyConstruct::processGridHelp() const
  /*!
    Simple print of the grid help
  */
{
  ELog::RegMethod RegA("tallyConstruct","processHelpGrid");
  
  ELog::EM<<"Requires variables:\n"
    "gridTallyHSize -- Half horrizontal width of grid\n" 
    "gridTallyVSize -- Half vertical width of grid\n" 
    "gridTallyAOffset -- Y (Axis) offset\n" 
    "gridTallyHOffset -- X offset\n" 
    "gridTallyVOffset -- Z offset\n" <<ELog::endBasic;
  
  ELog::EM<<"Options :\n";
  std::map<std::string,int>::const_iterator mc;
  for(mc=chipGridPos.begin();mc!=chipGridPos.end();mc++)
    ELog::EM<<mc->first<<" [front/back/offset_dist] \n";
  ELog::EM<<
    "free -- \n"
    "  Variables : gridTallyOrigin\n"
    "            : gridTallyXVec\n"
    "            : gridTallyZVec\n"
    "Object [front/back] offsetDist"
	  <<ELog::endBasic;
  return;
}

void
tallyConstruct::processGridObject(Simulation& System,
       const std::string& GType,const int linkPt,
       const int initNPD,const int NPD) const
  /*!
    If the job item is an object name process
    \param System :: Simulation object
    \param GType :: Object name
    \param linkPt :: Link point  [-ve for beam / +ve for linkUnit]
    \param initNPD :: Initial index number for tally
    \param NPD :: Number of point detectors 
  */
{
  ELog::RegMethod RegA("tallyConstruct","processObjectGrid");

  const FuncDataBase& Control=System.getDataBase();

  const double HSize=Control.EvalVar<double>("gridTallyHSize");      
  const double VSize=Control.EvalVar<double>("gridTallyVSize");      
  const double aOffset=Control.EvalDefVar<double>("gridTallyAOffset",0.0);      
  const double hOffset=Control.EvalDefVar<double>("gridTallyHOffset",0.0);      
  const double vOffset=Control.EvalDefVar<double>("gridTallyVOffset",0.0);      

  const ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  Geometry::Vec3D TOrigin;
  Geometry::Vec3D XVec,YVec,ZVec;
  

  // Process beam points
  if (linkPt<0)
    {
      const attachSystem::SecondTrack* TC=
	OR.getObject<attachSystem::SecondTrack>(GType);
      if (!TC)
	{
	  ELog::EM<<"Unable to convert to BeamTrack : "
		  <<GType<<ELog::endErr;
	  return;
	}
      TOrigin=(linkPt==-1) ? TC->getBeamStart() : TC->getBeamExit();
      XVec=TC->getBX();
      YVec=TC->getBY();
      ZVec=TC->getBZ();
    }
  else if (linkPt>0)
    {
      const attachSystem::FixedComp* FC=
	OR.getObject<attachSystem::FixedComp>(GType);

      if (!FC)
	{
	  ELog::EM<<"Unable to convert to Fixed Origin : "
		  <<GType<<ELog::endErr;
	  return;
	}
      const size_t fLinkIndex(static_cast<size_t>(linkPt-1));
      ELog::EM<<"GTYPE == "<<GType<<" "<<linkPt<<ELog::endTrace;
      TOrigin=FC->getLinkPt(fLinkIndex);
      ELog::EM<<"AXIS == "<<GType<<" "<<linkPt<<ELog::endTrace;
      FC->selectAltAxis(fLinkIndex,XVec,YVec,ZVec);
    }

  TOrigin+=XVec*hOffset+YVec*aOffset+ZVec*vOffset;      
  XVec*=2.0*HSize;
  ZVec*=2.0*VSize;  

  applyMultiGrid(System,initNPD,NPD,TOrigin,XVec,ZVec);

  return;

}

void 
tallyConstruct::processGrid(Simulation& System,
			    const mainSystem::inputParam& IParam,
			    const size_t Index) const
  /*!
    Processes a grid tally : Requires variables and informaton 
    \param System :: Simulation to add tallies
    \param IParam :: Main input parameters
    \param Index :: -T index number
  */
{
  ELog::RegMethod RegA("tallyConstruct","processGrid");
  
  const size_t NItems=IParam.itemCnt("tally",Index);
  if (NItems<2)
    throw ColErr::IndexError<size_t>(NItems,2,
				     "Insufficient items for tally");

  const std::string GType(IParam.getCompValue<std::string>("tally",Index,1));

  if (GType=="help")
    {
      processGridHelp();
      return;
    }
  // Size of grid:
  const int initNPD=IParam.getValue<int>("TGrid",0);   // First point to used
  const int NPD=IParam.getValue<int>("TGrid",1);       // Total 
  if (GType=="free")
    {
      processGridFree(System,initNPD,NPD);
      return;
    }
  // Set LinkPoint:
  const int linkPt=
    basicConstruct::getLinkIndex(IParam,Index,2);

  // Check objectRegister 
  const ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  if (OR.hasObject(GType)) 
    {  
      processGridObject(System,GType,linkPt,initNPD,NPD);
      return;
    }  
  
  ELog::EM<<"Failed to understand tally Grid component : "
	  <<GType<<ELog::endErr;
  return;
}



void
tallyConstruct::applyMultiGrid(Simulation& System,
			       const int initNPD,
			       const int NPD,
			       const Geometry::Vec3D& TOrigin,
			       const Geometry::Vec3D& Xv,
			       const Geometry::Vec3D& Zv) const
  /*!
    Actually proces the grid system based on the input mesh
    \param System :: simulation to add tallies
    \param initNPD :: Initial number [1 normally]
    \param NPD :: Number of point detectors
    \param TOrigin :: Centre of the plane
    \param Xv :: Direction X
    \param Zv :: Direction Z
   */
{
  ELog::RegMethod RegA("tallyConstruct","applyMultiGrid");

  const masterRotate& MR=masterRotate::Instance();
  Geometry::MeshGrid RT(TOrigin,Xv,Zv);
  
  for(int NI=1;NI<initNPD;NI++)
    RT.getNext();

  int level(RT.getLevel());
  ELog::EM<<"Tally Centre Point[AFTER OFFSET] == "
	  <<MR.calcRotate(TOrigin)<<ELog::endTrace;
  ELog::EM<<"Starting at level "<<level<<ELog::endWarn;
      
  int tNum(15);
  for(int NI=0;NI<NPD;NI++,tNum+=10)
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

  /*
void 
tallyConstruct::processSurface(Simulation& System,
			    const mainSystem::inputParam& IParam,
			    const size_t Index) const
  /*!
    Processes a surface tally : Requires variables and informaton 
    \param System :: Simulation to add tallies
    \param IParam :: Main input parameters
    \param Index :: -T index number
  * /
{
  ELog::RegMethod RegA("tallyConstruct","processSurface");
  
  const size_t NItems=IParam.itemCnt("tally",Index);
  if (NItems<2)
    throw ColErr::IndexError<size_t>(NItems,2,
				     "Insufficient items for tally");

  const std::string GType(IParam.getCompValue<std::string>("tally",Index,1));

  if (GType=="help")
    {
      processSurfaceHelp();
      return;
    }
  // Size of grid:
  const int initNPD=IParam.getValue<int>("TGrid",0);   // First point to used
  const int NPD=IParam.getValue<int>("TGrid",1);       // Total 
  if (GType=="free")
    {
      processGridFree(System,initNPD,NPD);
      return;
    }
  // Set LinkPoint:
  const int linkPt=basicConstruct::getLinkIndex(IParam,Index,2);

  // Check objectRegister 
  const ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  if (OR.hasObject(GType)) 
    {  
      processGridObject(System,GType,linkPt,initNPD,NPD);
      return;
    }  
  
  ELog::EM<<"Failed to understand tally Grid component : "
	  <<GType<<ELog::endErr;
  return;
}
  */

void  
tallyConstruct::helpTallyType() const
  /*!
    Simple help for types
   */
{
  ELog::RegMethod("TallyConstructor","helpTallyType");

  ELog::EM<<"Tally Types:\n\n";
  ELog::EM<<"-- grid : \n";
  ELog::EM<<"-- mesh : \n";
  ELog::EM<<"-- point : \n";
  ELog::EM<<"-- surface : \n";
  ELog::EM<<"-- flux : \n";
  ELog::EM<<"-- heat : \n";
  ELog::EM<<ELog::endBasic;
  return;
}

  



}  // NAMESPACE tallySystem
