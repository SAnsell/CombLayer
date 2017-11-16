/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/RoofPillars.cxx
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
#include <array>

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
#include "SurInter.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ReadFunctions.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FrontBackCut.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "AttachSupport.h"

#include "World.h"
#include "Bunker.h"
#include "BunkerRoof.h"
#include "pillarInfo.h"
#include "RoofPillars.h"


namespace essSystem
{

RoofPillars::RoofPillars(const std::string& Key)  :
  attachSystem::FixedComp(Key,0),
  attachSystem::CellMap(),
  attachSystem::FrontBackCut(),
  rodIndex(ModelSupport::objectRegister::Instance().cell(Key,30000)),
  cellIndex(rodIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}


RoofPillars::~RoofPillars() 
  /*!
    Destructor
  */
{}

void
RoofPillars::insertBeamCells(Simulation& System,
			     const double halfWidth,
			     const Geometry::Vec3D& XAxis,
			     const std::array<Geometry::Vec3D,2>& EPts,
			     const std::string& innerCut)
  /*!
    Insert beam cell
    \param System :: Simulation system
    \param halfWidth :: Half beam width
    \param XAxis :: XAxis direction
    \param EPts :: End points
    \param innerCut :: cut string for beam void
  */
{
  ELog::RegMethod RegA("RoofPillars","insertBeamCells");

  // horrizontal points:
  const std::array<Geometry::Vec3D,4> CPts=
    {{  EPts[0]-XAxis*halfWidth,
        EPts[0]+XAxis*halfWidth,
	EPts[1]-XAxis*halfWidth,
        EPts[1]+XAxis*halfWidth
      }};  
  insertRoofCells(System,CPts,0.1,innerCut);

  return;
}			      

void
RoofPillars::insertPillarCells(Simulation& System,
			       const pillarInfo& pillarItem,
			       const std::string& innerCut)
  /*!
    Insert roof cell
    \param System :: Simulation system
    \param pillarInfo :: Pillar Info 
    \param innerCut :: cut string for pillar void
  */
{
  ELog::RegMethod RegA("RoofPillars","insertPillarCells");
  
  const Geometry::Vec3D& CP(Origin+pillarItem.centPoint);
  const Geometry::Vec3D& YAxis=pillarItem.YAxis;
  const Geometry::Vec3D& XAxis=Z*YAxis;

  // horrizontal points:
  const double WT=topFootWidth/2.0+topFootGap;
  const double DT=topFootDepth/2.0+topFootGap;
  const std::array<Geometry::Vec3D,4> CPts=
    {{  CP-XAxis*WT-YAxis*DT,
        CP+XAxis*WT-YAxis*DT,
        CP-XAxis*WT+YAxis*DT,
        CP+XAxis*WT+YAxis*DT }};
  
  insertRoofCells(System,CPts,0.1,innerCut);
  return;
}

void
RoofPillars::insertRoofCells(Simulation& System,
			     const std::array<Geometry::Vec3D,4>& CPts,
			     const double innerStep,
			     const std::string& innerCut)
  /*!
    Insert roof cells
    \param System :: Simulation system
    \param CPts :: plan points on layer of roof cross beam
    \param innerStep :: distance to move points inward to recalc
    \param innerCut :: cut string for pillar void
  */
{
  ELog::RegMethod RegA("RoofPillars","insertRoofCells");

  static int cnt(0);
  const Geometry::Vec3D ZBase=
    SurInter::getLinePoint(Origin,Z,getBackRule(),getBackBridgeRule());
  const Geometry::Vec3D ZTop=ZBase+Z*topFootHeight;

  System.populateCells();
  System.validateObjSurfMap();

  typedef std::map<int,MonteCarlo::Object*> OTYPE;

  OTYPE OMap;
  for(size_t i=0;i<4;i++)
    for(size_t j=0;j<4;j++)
      attachSystem::lineIntersect(System,CPts[i]+ZBase,CPts[j]+ZTop,OMap);

  if (std::abs(innerStep)>Geometry::zeroTol)
    {
      const std::array<Geometry::Vec3D,4> IPts=
	{{
	  CPts[0]+(CPts[3]-CPts[0])*innerStep,
	  CPts[1]+(CPts[2]-CPts[1])*innerStep,
	  CPts[2]+(CPts[1]-CPts[2])*innerStep,
	  CPts[3]+(CPts[0]-CPts[3])*innerStep
	  }};
      
      for(size_t i=0;i<4;i++)
	for(size_t j=0;j<4;j++)
	  attachSystem::lineIntersect(System,IPts[i]+ZBase,IPts[j]+ZTop,OMap);

      for(size_t i=0;i<2;i++)
	for(size_t j=0;j<2;j++)
	  {
	    attachSystem::lineIntersect(System,CPts[i]+ZBase,IPts[j]+ZTop,OMap);
	    attachSystem::lineIntersect(System,IPts[i]+ZBase,CPts[j]+ZTop,OMap);
	    attachSystem::lineIntersect(System,CPts[i+2]+ZBase,
					IPts[j+2]+ZTop,OMap);
	    attachSystem::lineIntersect(System,IPts[i+2]+ZBase,
					CPts[j+2]+ZTop,OMap);
	  }
    }
  
  if (cnt==84)
    {
      if (roofCells)
	{
	  // first make ordered set of possibles
	  std::set<int> cellName;
	  for(const OTYPE::value_type Vunit : OMap)
	    {
	      const int cellN(Vunit.second->getName());
	      if (roofCells->hasCell(cellN))
		cellName.insert(cellN);
	    }

	  // now check order components
	  int prevCellN=0;
	  for(const int cellN : cellName)
	    {
	      // check not consecutive and not in other group 
	      if (prevCellN && prevCellN+1!=cellN && prevCellN+10000>cellN)
		{
		  for(int newCN=prevCellN+1;newCN<cellN;newCN++)
		    {
		      MonteCarlo::Object* NPtr=System.findQhull(newCN);
		      if (NPtr)
			{
			  OMap.emplace(newCN,NPtr);
			  ELog::EM<<"EMPLACE == "<<NPtr->getName()
				  <<ELog::endDiag;
			}
		    }
		}
	      prevCellN=cellN;
	    }
	}
    }

  // make intersection:
  HeadRule IC(innerCut);
  IC.makeComplement();
  for(const OTYPE::value_type Vunit : OMap)
    Vunit.second->addSurfString(IC.display());

  cnt++;
  return;  
}
  
void
RoofPillars::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("RoofPillars","populate");

  width=Control.EvalVar<double>(keyName+"Width");
  depth=Control.EvalVar<double>(keyName+"Depth");
  thick=Control.EvalVar<double>(keyName+"Thick");
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  innerMat=ModelSupport::EvalMat<int>(Control,keyName+"InnerMat");

  topFootHeight=Control.EvalDefVar<double>(keyName+"TopFootHeight",0.0);
  topFootDepth=Control.EvalDefVar<double>(keyName+"TopFootDepth",0.0);
  topFootWidth=Control.EvalDefVar<double>(keyName+"TopFootWidth",0.0);
  topFootThick=Control.EvalDefVar<double>(keyName+"TopFootThick",0.0);
  topFootGap=Control.EvalDefVar<double>(keyName+"TopFootGap",0.0);

  beamWidth=Control.EvalDefVar<double>(keyName+"BeamWidth",0.0);
  beamWallThick=Control.EvalDefVar<double>(keyName+"BeamWallThick",0.0);
  beamRoofThick=Control.EvalDefVar<double>
    (keyName+"BeamRoofThick",beamWallThick);
  beamWallGap=Control.EvalDefVar<double>(keyName+"BeamWallGap",2.0);

  longWidth=Control.EvalDefVar<double>(keyName+"LongWidth",beamWidth);
  longWallThick=Control.EvalDefVar<double>
    (keyName+"LongWallThick",beamWallThick);
  longRoofThick=Control.EvalDefVar<double>
    (keyName+"LongRoofThick",beamRoofThick);
  longWallGap=Control.EvalDefVar<double>(keyName+"LongWallGap",beamWallGap);

  // LOAD PILLAR DATA:
  const size_t nRadius=Control.EvalVar<size_t>(keyName+"NRadius");
  int index(0);
  for(size_t i=0;i<nRadius;i++)
    {
      const std::string Num=StrFunc::makeString(i);
      const size_t nSector=Control.EvalPair<size_t>
        (keyName+"NSector"+Num,keyName+"NSector");
      const double rotRadius=Control.EvalPair<double>
        (keyName+"R"+Num,keyName+"R");
      
      for(size_t j=0;j<nSector;j++)
        {
          const std::string NSec=std::to_string(j);
          const int active=Control.EvalDefPair<int>
            (keyName+"R_"+Num+"S_"+NSec+"Active",
             keyName+"RS_"+NSec+"Active",1);

	  // degrees: 
	  const double angle=M_PI*Control.EvalPair<double>
	    (keyName+"R_"+Num+"S_"+NSec,keyName+"RS_"+NSec)/180.0;
	  const std::string CName("R_"+Num+"S_"+NSec);
	  
	  const Geometry::Vec3D
	    CPos(rotRadius*sin(angle),rotRadius*cos(angle),0.0);
	  
	  const Geometry::Vec3D YA(sin(angle),cos(angle),0.0);
	  PInfo.emplace(CName,pillarInfo(CName,i,j,index,active,CPos,YA));
	  index++;
        }          
    }

  // LOAD Long BEAM DATA
  if (longWidth>Geometry::zeroTol)
    populateBeamSet(Control,"LBeam",1,0,longLinks);
  // LOAD Cross BEAM DATA
  if (beamWidth>Geometry::zeroTol)
    populateBeamSet(Control,"XBeam",0,1,beamLinks);

  return;
}

void
RoofPillars::populateBeamSet(const FuncDataBase& Control,
			     const std::string& nameKey,
			     const long int dirR,
                             const long int dirX,
			     BeamTYPE& Links) const
  /*!
    Populate the variables for the cross/long beams 
    \param Control :: Variable data base
    \param nameKey :: Key name to use
    \param dirR :: Radial Count increase
    \param dirX :: Cross beam count increase 
    \param Links :: set to add/remove components
  */
{
  ELog::RegMethod RegA("RoofPillars","populateBeamSet");

  typedef std::map<std::string,pillarInfo> PMAP;

  Links.clear();
  // keyName+"NLBeam" / keyName+"NXBeam" 
  const long int nBeam=Control.EvalDefVar<long int>(keyName+"N"+nameKey,0);
  const size_t NB(static_cast<size_t>(std::abs(nBeam)));
  for(size_t i=0;i<NB;i++)
    {
      const std::string Num=std::to_string(i);
      const std::string AKey=
	Control.EvalVar<std::string>(keyName+nameKey+Num+"A");
      // find next item
      PMAP::const_iterator mc=PInfo.find(AKey);
      if (mc!=PInfo.end())
	{
	  const std::string BKey(mc->second.getNext(dirR,dirX));
	  Links.insert(std::pair<std::string,std::string>(AKey,BKey));
	}
      
    }
  
  // SPECIAL CASE to EXCLUDE
  if (nBeam<=0)  // can use zero here because width has been pre-tested
    {
      for(const PMAP::value_type& PItem : PInfo)
	{
	  const pillarInfo& PVal(PItem.second);
	  const std::string BKey(PVal.getNext(dirR,dirX));
	  if (PInfo.find(BKey)!=PInfo.end())
	    {
	      const std::pair<std::string,std::string> LL(PVal.Name,BKey);
	      BeamTYPE::iterator mc=Links.find(LL);
	      if (mc!=Links.end())
		Links.erase(mc);
	      else
		Links.insert(LL);
	    }
	}
    }
  return;
}

  
void
RoofPillars::createUnitVector(const attachSystem::FixedComp& MainCentre,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param MainCentre :: Main rotation centre
    \param sideIndex :: Side for linkage centre
  */
{
  ELog::RegMethod RegA("RoofPillars","createUnitVector");

  FixedComp::createUnitVector(MainCentre,sideIndex);
  for(std::map<std::string,pillarInfo>::value_type& pItem : PInfo)
    {
      const Geometry::Vec3D& Pt(pItem.second.centPoint);
      pItem.second.centPoint=X*Pt.X()+Y*Pt.Y()+Z*Pt.Z();
    }

  return;
}
  
void
RoofPillars::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("RoofPillars","createSurface");

  if (topFootHeight>Geometry::zeroTol)
    {
      int RI(rodIndex);
      const std::set<int> FS= FrontBackCut::getBackRule().getSurfSet();
      int footIndex(RI);  // in case multiple surfaces
      for(const int& SNum : FS)
        {
          const Geometry::Surface* SPtr=SMap.realSurfPtr(SNum);
          const Geometry::Plane* PPtr=
            dynamic_cast<const Geometry::Plane*>(SPtr);
          if (PPtr)
            {
              // top plate
              // Full height
	      buildSignedShiftedPlane(SMap,-SNum,footIndex+6,
				      PPtr,topFootHeight-topFootThick);
              topFootPlate.addIntersection(SMap.realSurf(footIndex+6));
              // Full height
	      buildSignedShiftedPlane(SMap,-SNum,footIndex+16,
				      PPtr,topFootHeight);	      
              topFoot.addIntersection(SMap.realSurf(footIndex+16));
              // Beam inner meta; 
	      buildSignedShiftedPlane(SMap,-SNum,footIndex+26,
				      PPtr,beamWallThick);	      
              baseBeam.addIntersection(SMap.realSurf(footIndex+26));
	      // Beam Top inner metal [down -wallThick]
	      buildSignedShiftedPlane(SMap,-SNum,footIndex+36,
				      PPtr,topFootHeight-beamRoofThick); 
              topBeam.addIntersection(SMap.realSurf(footIndex+36));

              // Beam inner meta; 
	      buildSignedShiftedPlane(SMap,-SNum,footIndex+46,
				      PPtr,longWallThick);	      
              baseLong.addIntersection(SMap.realSurf(footIndex+46));
	      // Beam Top inner metal [down -wallThick]
	      buildSignedShiftedPlane(SMap,-SNum,footIndex+56,
				      PPtr,topFootHeight-longRoofThick); 
              topLong.addIntersection(SMap.realSurf(footIndex+56));
	      
	      footIndex++;
            }
        }
    }
  

  for(const std::map<std::string,pillarInfo>::value_type& PItem : PInfo)
    {
      const int RI(rodIndex+50*PItem.second.RI);
      const Geometry::Vec3D& CP(PItem.second.centPoint);
      const Geometry::Vec3D& YAxis(PItem.second.YAxis);
      const Geometry::Vec3D& XAxis=Z*YAxis;
      double WT(width/2.0);
      double DT(depth/2.0);      
      ModelSupport::buildPlane(SMap,RI+1,CP-XAxis*WT,XAxis);
      ModelSupport::buildPlane(SMap,RI+2,CP+XAxis*WT,XAxis);
      ModelSupport::buildPlane(SMap,RI+3,CP-YAxis*DT,YAxis);
      ModelSupport::buildPlane(SMap,RI+4,CP+YAxis*DT,YAxis);

      WT+=thick;
      DT+=thick;      
      ModelSupport::buildPlane(SMap,RI+11,CP-XAxis*WT,XAxis);
      ModelSupport::buildPlane(SMap,RI+12,CP+XAxis*WT,XAxis);
      ModelSupport::buildPlane(SMap,RI+13,CP-YAxis*DT,YAxis);
      ModelSupport::buildPlane(SMap,RI+14,CP+YAxis*DT,YAxis);

      WT=topFootWidth/2.0;
      DT=topFootDepth/2.0;
      ModelSupport::buildPlane(SMap,RI+21,CP-XAxis*WT,XAxis);
      ModelSupport::buildPlane(SMap,RI+22,CP+XAxis*WT,XAxis);
      ModelSupport::buildPlane(SMap,RI+23,CP-YAxis*DT,YAxis);
      ModelSupport::buildPlane(SMap,RI+24,CP+YAxis*DT,YAxis);

      WT+=topFootGap;
      DT+=topFootGap;
      ModelSupport::buildPlane(SMap,RI+31,CP-XAxis*WT,XAxis);
      ModelSupport::buildPlane(SMap,RI+32,CP+XAxis*WT,XAxis);
      ModelSupport::buildPlane(SMap,RI+33,CP-YAxis*DT,YAxis);
      ModelSupport::buildPlane(SMap,RI+34,CP+YAxis*DT,YAxis);
    }
      
  return;
}
  
void
RoofPillars::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("RoofPillars","createObjects");

  std::string Out;

  const std::string Base=frontRule()+backRule();
  const std::string footLevel=topFoot.complement().display();
  const std::string plateLevelComp=topFootPlate.complement().display();
  const std::string plateLevel=topFootPlate.display();
  const std::string roofLevel=getBackRule().complement().display();
    
  for(const std::map<std::string,pillarInfo>::value_type& PItem : PInfo)
    {
      const int RI(rodIndex+50*PItem.second.RI);
      if (PItem.second.active)
	{
	  Out=ModelSupport::getComposite(SMap,RI," 1 -2 3 -4 ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out+Base));
	  addCell("Pillar"+PItem.first,cellIndex-1);
	  Out=ModelSupport::getComposite(SMap,RI," 11 -12 13 -14 (-1:2:-3:4) ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out+Base));
	  addCell("Pillar"+PItem.first,cellIndex-1);
	}
      // Object must be added afterward otherwise we add to self
      // innser pillar part
      Out=ModelSupport::getComposite(SMap,RI," 31 -32 33 -34  ");
      insertPillarCells(System,PItem.second,Out+footLevel+roofLevel);
      
      Out=ModelSupport::getComposite(SMap,RI," 1 -2 3 -4 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,
				       Out+roofLevel+plateLevelComp));
      addCell("Pillar"+PItem.first,cellIndex-1);
      Out=ModelSupport::getComposite(SMap,RI," 11 -12 13 -14 (-1:2:-3:4) ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,
				       Out+roofLevel+plateLevelComp));
      addCell("Pillar"+PItem.first,cellIndex-1);
      
      // pillar in roof
      Out=ModelSupport::getComposite(SMap,RI,
				     " 31 -32 33 -34  (-11:12:-13:14) ");
      System.addCell(MonteCarlo::Qhull
		     (cellIndex++,0,0.0,Out+roofLevel+plateLevelComp));
      
      Out=ModelSupport::getComposite(SMap,RI," 21 -22 23 -24  ");
      System.addCell(MonteCarlo::Qhull
		     (cellIndex++,mat,0.0,Out+plateLevel+footLevel));
      
      Out=ModelSupport::getComposite(SMap,RI,
				     " 31 -32 33 -34  (-21:22:-23:24) ");
      System.addCell(MonteCarlo::Qhull
		     (cellIndex++,0,0.0,Out+plateLevel+footLevel));
      addCell("Foot"+PItem.first,cellIndex-1);
    }
        
  return;
}

HeadRule
RoofPillars::getCrossPillarBoundary(const Geometry::Vec3D& midPoint,
				    const Geometry::Vec3D& YVec,
				    const int aIR,const int bIR,
				    std::array<Geometry::Vec3D,2>& endPt) const
  /*!
    Compute and return the headrule that divides two pillars on 
    horrizontal (crossing)
    \param midPoint :: MidPoint between pillars
    \param YVec :: vector along axis
    \param aIR :: pillar index for A
    \param bIR :: pillar index for B
    \param endPt :: End points
   */
{
  ELog::RegMethod RegA("RoofPillar","getCrossPillarBoundary");
  return getInterPillarBoundary(23,24,midPoint,YVec,aIR,bIR,endPt);
}

HeadRule
RoofPillars::getLongPillarBoundary(const Geometry::Vec3D& midPoint,
				   const Geometry::Vec3D& YVec,
				   const int aIR,const int bIR,
				   std::array<Geometry::Vec3D,2>& endPt) const
  /*!
    Compute and return the headrule that divides two pillars on
    long (going out in radius)
    \param midPoint :: MidPoint between pillars
    \param YVec :: vector along axis
    \param aIR :: pillar index for A
    \param bIR :: pillar index for B
    \param endPt :: End points
   */
{
  ELog::RegMethod RegA("RoofPillar","getCrossPillarBoundary");
  return getInterPillarBoundary(21,22,midPoint,YVec,aIR,bIR,endPt);
}

HeadRule
RoofPillars::getInterPillarBoundary(const int aSurf,const int bSurf,
				    const Geometry::Vec3D& midPoint,
				    const Geometry::Vec3D& YVec,
				    const int aIR,const int bIR,
				    std::array<Geometry::Vec3D,2>& endPt) const
  /*!
    Compute and return the headRule that divides two pillars
    \param aSurf :: offset to pillar surface
    \param bSurf :: offset to pillar surface
    \param midPoint :: MidPoint between pillars
    \param YVec :: vector along axis
    \param aIR :: pillar index for A
    \param bIR :: pillar index for B
    \param endPt :: End points
    \return Rule for overlap region
   */
{
  ELog::RegMethod RegA("RoofPillar","getInterPillarBoundary");

  const int RIvec[2] {rodIndex+aIR*50,rodIndex+bIR*50};
  const Geometry::Plane* FB[2];
  HeadRule Out;
  for(size_t i=0;i<2;i++)
    {
      const Geometry::Plane* AP=SMap.realPtr<Geometry::Plane>(RIvec[i]+aSurf);
      const Geometry::Plane* BP=SMap.realPtr<Geometry::Plane>(RIvec[i]+bSurf);
      const Geometry::Vec3D aPt=SurInter::getLinePoint(midPoint,YVec,AP);
      const Geometry::Vec3D bPt=SurInter::getLinePoint(midPoint,YVec,BP);
      if (aPt.Distance(midPoint) > bPt.Distance(midPoint))
	{
	  endPt[i]=bPt;
	  FB[i]=BP;
	}
      else
	{
	  endPt[i]=aPt;
	  FB[i]=AP;
	}
      const int sN((FB[i]->side(midPoint)>0.0) ? 1 : -1);
      Out.addIntersection(sN*FB[i]->getName());
    }
  return Out;
}


void
RoofPillars::getPillarPair(const std::string& AName,
			   const std::string& BName,
			   Geometry::Vec3D& midPoint,
			   Geometry::Vec3D& XVec,
			   Geometry::Vec3D& YVec,
			   int& aRI,int& bRI) const
  /*!
    Determine scalar value between different componet
    \param AName :: Name of object
    \param BName :: Name of object
    \param midPoint :: mid point between pillars
    \param XVec :: XVector [cross unit vector]
    \param YVec :: Main Axis vector
    \param aRI :: index for A
    \param bRI :: index for B
  */
{
  ELog::RegMethod RegA("RoofPillar","getPillarPair");

  typedef std::map<std::string,pillarInfo> MTYPE;
  
  MTYPE::const_iterator ac=PInfo.find(AName);
  MTYPE::const_iterator bc=PInfo.find(BName);

  if (ac==PInfo.end())
    throw ColErr::InContainerError<std::string>(AName,"Beamlink");
  if (bc==PInfo.end())
    throw ColErr::InContainerError<std::string>(BName,"Beamlink");
  if (ac==bc)
    throw ColErr::IndexError<std::string>(AName,BName,"Beamlink items equal");
  
  const pillarInfo& APillar(ac->second);
  const pillarInfo& BPillar(bc->second);
  YVec=(BPillar.centPoint-APillar.centPoint).unit();
  XVec=Z*YVec;
  midPoint=Origin+(BPillar.centPoint+APillar.centPoint)/2.0;
  aRI=APillar.RI;
  bRI=BPillar.RI;
  
  return;
}


void
RoofPillars::createBeamSurfaces(const int RI,
				const Geometry::Vec3D& midPoint,
				const Geometry::Vec3D& XVec,
				const double Width,
				const double WallThick,
				const double WallGap)
  /*!
    Create the surfaces for the beamxs
    \param RI :: Index offset
    \param midPoint :: Mid point of beam
    \param XVec :: X coordinate value [unit]
    \param Width :: Full width of beam
    \param WallThick :: Thickness of wall
    \param WallGap :: Gap round wall 
  */
{
  ELog::RegMethod RegA("RoofPillar","createBeamSurfaces");
  
  // SURFACES:
  double W(Width/2.0);  // inner void
  ModelSupport::buildPlane(SMap,RI+3,midPoint-XVec*W,XVec);
  ModelSupport::buildPlane(SMap,RI+4,midPoint+XVec*W,XVec);
  W+=WallThick;          // metal wall
  ModelSupport::buildPlane(SMap,RI+13,midPoint-XVec*W,XVec);
  ModelSupport::buildPlane(SMap,RI+14,midPoint+XVec*W,XVec);
  W+=WallGap;          // clearance gap
  ModelSupport::buildPlane(SMap,RI+23,midPoint-XVec*W,XVec);
  ModelSupport::buildPlane(SMap,RI+24,midPoint+XVec*W,XVec);

  return;
}


void
RoofPillars::createBeamObjects(Simulation& System,
			       const int RI,const std::string& fbBStr,
			       const HeadRule& topBUnit,
			       const HeadRule& baseBUnit,
			       const double halfWidth,
			       const Geometry::Vec3D& XVec,
			       const std::array<Geometry::Vec3D,2>& EPts)
  /*!
    Create the objects for a beam
    \param RI :: Index offset
    \param fbBStr :: front/back cut string
    \param topBUnit :: HeadRule for top cut
    \param baseBUnit :: HeadRule for base cut
    \param halfWidth :: Half beam width
    \param XVec :: Cross direction
    \param EPts :: End points
  */
{
  ELog::RegMethod RegA("RoofPillars","createBeamObjects");

  std::string Out;
     
  HeadRule Outer(topFoot.complement());
  Outer.addIntersection(getBackRule().complement());
  
  HeadRule Inner(topBUnit.complement());
  Inner.addIntersection(baseBUnit);
  
  Out=ModelSupport::getComposite(SMap,RI," 23 -24 ");
  Out+=fbBStr+Outer.display();
  insertBeamCells(System,halfWidth,XVec,EPts,Out);
  
  // gap
  Out=ModelSupport::getComposite(SMap,RI," 23 -13  ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,
				   Out+Outer.display()+fbBStr));
  Out=ModelSupport::getComposite(SMap,RI," 14 -24  ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,
				   Out+Outer.display()+fbBStr));
  
  Out=ModelSupport::getComposite(SMap,RI," 3 -4 ");
  Out+=fbBStr+Inner.display();
  System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,0.0,Out));
  
  
  // steel box [NOTE CARE for INCLUSION]:
  Out=ModelSupport::getComposite(SMap,RI," 3 -4 ");
  Inner.addIntersection(Out);


  Out=ModelSupport::getComposite(SMap,RI," 13 -14 ");
  Out+=fbBStr+Outer.display();
  Inner.makeComplement();
  Out+=Inner.display();
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));

  return;
}

void
RoofPillars::createCrossBeams(Simulation& System)
  /*!
    A system for construction long beams
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("RoofPillars","createCrossBeams");


  typedef std::pair<std::string,std::string> SPAIR;
  ELog::EM<<"Cross BEAM:"<<beamLinks.size()<<ELog::endDiag;
  int RI(rodIndex+10000);
  for(const SPAIR& BItem : beamLinks)
    {
      Geometry::Vec3D midPoint;
      Geometry::Vec3D XVec;
      Geometry::Vec3D YVec;
      int aRI,bRI;
		  
      getPillarPair(BItem.first,BItem.second,midPoint,
		    XVec,YVec,aRI,bRI);
      
  // Calculate closest surface to line (3/4):

      std::array<Geometry::Vec3D,2> EPts;
      const HeadRule fbBoundary=
	getCrossPillarBoundary(midPoint,YVec,aRI,bRI,EPts);

      createBeamSurfaces(RI,midPoint,XVec,beamWidth,beamWallThick,beamWallGap);
      
      // OBJECTS:
      const double halfWidth=beamWidth/2.0+beamWallThick+beamWallGap;
      createBeamObjects(System,
			RI,fbBoundary.display(),topBeam,baseBeam,
			halfWidth,XVec,EPts);

      RI+=100;
    }
  return;
}

void
RoofPillars::createLongBeams(Simulation& System)
  /*!
    A system for construction long beams
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("RoofPillars","createLongBeams");


  typedef std::pair<std::string,std::string> SPAIR;
  ELog::EM<<"Long BEAM:"<<longLinks.size()<<ELog::endDiag;
  int RI(rodIndex+20000);
  for(const SPAIR& BItem : longLinks)
    {
      Geometry::Vec3D midPoint;
      Geometry::Vec3D XVec;
      Geometry::Vec3D YVec;
      int aRI,bRI;
		  
      getPillarPair(BItem.first,BItem.second,midPoint,
		    XVec,YVec,aRI,bRI);
      
  // Calculate closest surface to line (3/4):

      std::array<Geometry::Vec3D,2> EPts;
      const HeadRule fbBoundary=
	getLongPillarBoundary(midPoint,YVec,aRI,bRI,EPts);

      createBeamSurfaces(RI,midPoint,XVec,longWidth,longWallThick,longWallGap);
      
      // OBJECTS:
      const double halfWidth=longWidth/2.0+longWallThick+longWallGap;
      createBeamObjects(System,RI,fbBoundary.display(),topLong,baseLong,
			halfWidth,XVec,EPts);

      RI+=100;
    }
  return;
}
  
void
RoofPillars::insertPillars(Simulation& System,
                           const attachSystem::CellMap& bObj)
  /*!
    Insert the pillers into the bunker void
    \param System :: Simulation 
    \param bObj :: Bunker void [cells]
   */
{
  ELog::RegMethod RegA("RoofPillars","insertPillars");

  const std::string Base=frontRule()+backRule();    
  std::string Out;
  
  for(const std::map<std::string,pillarInfo>::value_type& PItem : PInfo)
    {
      if (PItem.second.active)
	{
	  const int RI(rodIndex+50*PItem.second.RI);
	  Out=ModelSupport::getComposite(SMap,RI," 11 -12 13 -14 ");
	  HeadRule HR(Out);
	  HR.makeComplement();
	  bObj.insertComponent(System,"MainVoid",HR);
	}
    }
  return;
}

  
void
RoofPillars::createLinks()
  /*!
    Create all the linkes [OutGoing]
  */
{
  ELog::RegMethod RegA("RoofPillars","createLinks");

  return;
}
  
void
RoofPillars::createAll(Simulation& System,
                       const Bunker& bunkerObj)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param bunkerObj :: Bunker object 
  */
{
  ELog::RegMethod RegA("RoofPillars","createAll");

  populate(System.getDataBase());
  createUnitVector(bunkerObj,7);

  setFront(bunkerObj,11);    // floor
  setBack(bunkerObj,12);     // roof [inner]

  createSurfaces();
  createObjects(System);
  insertPillars(System,bunkerObj);

  roofCells=bunkerObj.getRoofObj();
  createCrossBeams(System);
  createLongBeams(System);
  return;
}

}  // NAMESPACE essSystem
