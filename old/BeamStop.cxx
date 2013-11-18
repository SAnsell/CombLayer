/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/BeamStop.cxx
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
#include <functional>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Tensor.h"
#include "Vec3D.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "surfEqual.h"
#include "MergeSurf.h"
#include "surfDivide.h"
#include "surfDIter.h"
#include "Quaternion.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "KGroup.h"
#include "Source.h"
#include "SimProcess.h"
#include "Simulation.h"
#include "ReadFunctions.h"
#include "ModelSupport.h"
#include "chipDataStore.h"
#include "LinearComp.h"
#include "Hutch.h"
#include "beamBlock.h"
#include "BeamStop.h"

namespace hutchSystem
{

BeamStop::BeamStop(const int N,const std::string& Key)  :
  zoomSystem::LinearComp(),stopIndex(N),keyName(Key),
  cellIndex(N+1),populated(0),insertCell(0),
  RBase(3,3),nLayers(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param N :: Index value
    \param Key :: Name for item in search
  */
{}

BeamStop::BeamStop(const BeamStop& A) : 
  zoomSystem::LinearComp(A),
  stopIndex(A.stopIndex),keyName(A.keyName),cellIndex(A.cellIndex),
  populated(A.populated),insertCell(A.insertCell),Axis(A.Axis),
  XAxis(A.XAxis),Centre(A.Centre),RBase(A.RBase),width(A.width),
  depth(A.depth),height(A.height),nLayers(A.nLayers),CDivideList(A.CDivideList)
  /*!
    Copy constructor
    \param A :: BeamStop to copy
  */
{}

BeamStop&
BeamStop::operator=(const BeamStop& A)
  /*!
    Assignment operator
    \param A :: BeamStop to copy
    \return *this
  */
{
  if (this!=&A)
    {
      zoomSystem::LinearComp::operator=(A);
      cellIndex=A.cellIndex;
      populated=A.populated;
      insertCell=A.insertCell;
      Axis=A.Axis;
      XAxis=A.XAxis;
      Centre=A.Centre;
      RBase=A.RBase;
      width=A.width;
      depth=A.depth;
      height=A.height;
      nLayers=A.nLayers;
      CDivideList=A.CDivideList;
    }
  return *this;
}

BeamStop::~BeamStop() 
  /*!
    Destructor
  */
{}

void
BeamStop::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("BeamStop","populate");

  // This sets group of objects within the shutter
  // They require that each unit is fully defined and 
  // that requires 6 items
  const FuncDataBase& Control=System.getDataBase();
  beamBlock Item;
  int flag(0);
  int itemNum(0);
  do 
    {
      if (flag==beamBlock::Size)
	{
	  BBlock.push_back(Item);
	  itemNum++;
	  if (itemNum>10)
	    ELog::EM<<"Error with all variables in a block totally generic"
		    <<ELog::endErr;
	}
      for(flag=0;flag<beamBlock::Size && 
	    Item.setFromControl(Control,"BeamStop",1,itemNum,flag);
	  flag++) ;
    } while (flag==beamBlock::Size);

  defMaterial=Control.EvalVar<int>(keyName+"Material");
  //  const FuncDataBase& Control=System.getDataBase();

  // width=Control.EvalVar<double>(keyName+"Width");
  // height=Control.EvalVar<double>(keyName+"Height");
  // depth=Control.EvalVar<double>(keyName+"Depth");

  // Layers
  // nLayers=Control.EvalVar<int>(keyName+"NLayers");
  // ModelSupport::populateDivide(Control,nLayers,
  // 			      keyName+"Frac_",cFrac);
  // ModelSupport::populateDivide(Control,nLayers,
  // 			       keyName+"Mat_",defMat,cMat);

  populated |= 1;
  
  return;
}

void
BeamStop::setAxis(const Geometry::Vec3D& A)
  /*!
    Set the movement axis			
    \param A :: Axis to move						
  */
{
  populated |=2;
  Axis=A.unit();
  return;
}

void
BeamStop::createUnitVector(const zoomSystem::LinearComp& LC)
  /*!
    Create the unit vectors
    \param LC :: LinearComponent the beamstop is attached to.
  */
{
  ELog::RegMethod RegA("BeamStop","createUnitVector");

//  const masterRotate& MR=masterRotate::Instance();
//  chipIRDatum::chipDataStore& CS=chipIRDatum::chipDataStore::Instance();

  LinearComp::createUnitVector(LC);
  Origin=Centre;
  XAxis=Y*Axis;
  
  for(int i=0;i<3;i++)
    {
      RBase[i][0]=X[i];
      RBase[i][1]=Y[i];
      RBase[i][2]=Z[i];
    }

//  CS.setENum(chipIRDatum::collExit,MR.calcRotate(ExitPoint));  
  return;
}

void
BeamStop::createSurfaces(const Simulation& System)
  /*!
    Create All the surfaces
    Goes throught the insertCell to get the outside surfaces
    registered as 1-6 using normal convension to determine x-y-z.
    \param System :: Current system
  */
{
  ELog::RegMethod RegA("BeamStop","createSurface");
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  const MonteCarlo::Qhull* beamCell=System.findQhull(insertCell);
  if (!beamCell)
    ELog::EM<<"Failed to find beamstop:"<<insertCell<<ELog::endErr;

  std::vector<int> sI=beamCell->getSurfaceIndex();
  std::vector<int>::const_iterator vc;
  const Geometry::Plane* TPlane;
  int surfCnt(0);
  // loop over surfaces : there should be 6

  int xyzFlag[3]={0};
  for(vc=sI.begin();vc!=sI.end();vc++)
    {
      const int oldSurf(surfCnt);
      TPlane=dynamic_cast<const Geometry::Plane*>(SurI.getSurf(*vc));
      if (TPlane)
	{
	  const int sign=beamCell->surfSign(*vc);
	  const Geometry::Vec3D& N=TPlane->getNormal();
	  if (fabs(N.dotProd(X))>0.8)
	    {
	      xyzFlag[0]++;
	      if (sign>0)
		SMap.addMatch(3+stopIndex,*vc);
	      else
		SMap.addMatch(4+stopIndex,*vc);
	      surfCnt++;
	    }
	  else if (fabs(N.dotProd(Y))>0.8)
	    {
	      xyzFlag[1]++;
	      if (sign>0)
		SMap.addMatch(1+stopIndex,*vc);
	      else
		SMap.addMatch(2+stopIndex,*vc);
	      surfCnt++;
	    }
	  else if (fabs(N.dotProd(Z))>0.8)
	    {
	      xyzFlag[2]++;
	      if (sign>0)
		SMap.addMatch(5+stopIndex,*vc);
	      else
		SMap.addMatch(6+stopIndex,*vc);
	      surfCnt++;
	    }
	}
      if (surfCnt==oldSurf)
	{
	  ELog::EM<<"Surface "<<*vc<<" not processed"<<ELog::endWarn;
	  ELog::EM<<"Normal == "<<TPlane->getNormal()<<ELog::endWarn;
	}
    }
      
  if (surfCnt!=6 || xyzFlag[0]!=2 || xyzFlag[1]!=2 || xyzFlag[2]!=2)
    {
      ELog::EM<<"Error : Incorrect number of surface "<<surfCnt<<ELog::endErr;
      exit(1);
    }

  const Geometry::Plane* A=SMap.realPtr<Geometry::Plane>(stopIndex+1);
  const Geometry::Plane* B=SMap.realPtr<Geometry::Plane>(stopIndex+2);
  depth=fabs(A->getDistance()-B->getDistance());
  A=SMap.realPtr<Geometry::Plane>(stopIndex+3);
  B=SMap.realPtr<Geometry::Plane>(stopIndex+4);
  width=fabs(A->getDistance()-B->getDistance());
  A=SMap.realPtr<Geometry::Plane>(stopIndex+5);
  B=SMap.realPtr<Geometry::Plane>(stopIndex+6);
  height=fabs(A->getDistance()-B->getDistance());
  
  Origin=Centre-Y*(depth/2.0);
  ExitPoint=Origin+Y*depth;  

  return;
}

void
BeamStop::createDefSurfaces()
  /*!
    Create a surface stack in the default [non-file reading case]
  */
{
  ELog::RegMethod RegA("BeamStop","createDefSurfaces");
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();

  double totalThick(0.0);
  Geometry::Plane* PX;    
  for(int i=0;i<static_cast<int>(BBlock.size());i++)
    {
      const beamBlock& BB=BBlock[i];
      totalThick+=BB.thickness;
      Geometry::Vec3D RAxis;
      if (fabs(BB.angle)>1e-3)
	{
	  PX=SurI.createUniqSurf<Geometry::Plane>(stopIndex+10*i+11);
	  RAxis= Y;
	  Geometry::Quaternion::calcQRotDeg(BB.angle,Z).rotate(RAxis);
	  PX->setPlane(Origin+Y*totalThick,RAxis);
	  SMap.registerSurf(stopIndex+10*i+11,PX);

	  PX=SurI.createUniqSurf<Geometry::Plane>(stopIndex+10*i+12);
	  RAxis= Y;
	  Geometry::Quaternion::calcQRotDeg(-BB.angle,Z).rotate(RAxis);
	  PX->setPlane(Origin+Y*totalThick,RAxis);
	  SMap.registerSurf(stopIndex+10*i+12,PX);
	  ELog::EM<<"Creating surface "<<stopIndex+10*i+12<<ELog::endDiag;
	}
      else
	{
	  PX=SurI.createUniqSurf<Geometry::Plane>(stopIndex+10*i+11);
	  PX->setPlane(Origin+Y*totalThick,Y);
	  SMap.registerSurf(stopIndex+10*i+11,PX);
	}
    }
  return;
}

void
BeamStop::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components : only called if 
    \param System :: Simulation system
  */
{
  ELog::RegMethod RegA("BeamStop","createObjects");

  std::string Out;
  std::ostringstream cx;
  std::ostringstream dx;
  std::string frontSurf=" 1 ";
  
  int materialNum;           // first is a void [at moment]
  double totalThick(0.0);
  for(int i=0;i<static_cast<int>(BBlock.size());i++)
    {
      const beamBlock& BB=BBlock[i];
      materialNum=BB.matN;
      totalThick+=BB.thickness;
      Geometry::Vec3D Axis;
      cx.str("");
      dx.str("");
      if (fabs(BB.angle)>1e-3)
	{
	  //	  cx<<"("<<-(10*i+11)<<" : "<<-(10*i+12)<<") ";
	  cx<<-(10*i+11)<<" "<<-(10*i+12)<<" ";
	  dx<<"#("+cx.str()<<") ";
	  cx<<frontSurf;
	  frontSurf=dx.str();
	}
      else
	{
	  cx<<-(10*i+11)<<" ";
	  dx<<(10*i+11)<<" ";
	  cx<<frontSurf;
	  frontSurf=dx.str();
	}
      
      if (BB.thickness>1e-3)
	{
	  Out=ModelSupport::getComposite(SMap,stopIndex,
					 "1 -2 3 -4 5 -6 "+cx.str());
	  System.addCell(MonteCarlo::Qhull(cellIndex++,materialNum,0.0,Out));
	}
    }      
  // Last cell:
  materialNum=defMaterial;
  Out=ModelSupport::getComposite(SMap,stopIndex,"1 -2 3 -4 5 -6 "+frontSurf);
  System.addCell(MonteCarlo::Qhull(cellIndex++,materialNum,0.0,Out));
  
  return;
}

void
BeamStop::insertObjects(Simulation& System)
  /*!
    Create outer virtual space that includes the beamstop etc
    \param System :: Simulation to add to 
  */
{
  ELog::RegMethod RegA("BeamStop","insertObjects");

  if (!System.removeCell(insertCell))
    ELog::EM<<"Failed to find outerObject: "<<insertCell<<ELog::endErr;
  
  return;
}

void 
BeamStop::setInsertCell(const int CN)
  /*!
    Sets the centre
    \param CN :: Cell number
  */
{
  populated |= 8;
  insertCell=CN;
  return;
}

void 
BeamStop::setCentre(const Geometry::Vec3D& C)
  /*!
    Sets the centre
    \param C :: Centre point
  */
{
  populated |= 4;
  Centre=C;
  return;
}

void 
BeamStop::layerProcess(Simulation& )
  /*!
    Processes the splitting of the surfaces into a multilayer system
    This has to deal with the three layers that invade cells:
  */
{
  ELog::RegMethod RegA("BeamStop","LayerProcess");
  
  
  return;
}


int
BeamStop::exitWindow(const double Dist,
		    std::vector<int>& window,
		    Geometry::Vec3D& Pt) const
  /*!
    Outputs a window
    \param Dist :: Dist from exit point
    \param window :: window vector of paired planes
    \param Pt :: Output point for tally
    \return Master Plane
  */
{
  window.clear();
  // window.push_back(SMap.realSurf(colIndex+3));
  // window.push_back(SMap.realSurf(colIndex+4));
  // window.push_back(SMap.realSurf(colIndex+5));
  // window.push_back(SMap.realSurf(colIndex+6));
  Pt=Origin+Y*(depth+Dist);  
  return 0;
  //  return SMap.realSurf(colIndex+2);
}


int
BeamStop::readFile(Simulation& System,const std::string& FName)
  /*!
    Process the reading of an MCNPX insert file
    \param System :: Simulation to update
    \param FName :: file to read
    \return 0 on success / -ve on failure
  */
{
  ELog::RegMethod RegA("BeamStop","readFile");


  ReadFunc::OTYPE OMap;
  std::ifstream IX;
  if (!FName.empty())
    IX.open(FName.c_str());
  if (FName.empty() || !IX.good())
    {
      ELog::EM<<"Failed to open file:"<<FName<<ELog::endErr;
      return -1;
    }
  FuncDataBase& DB=System.getDataBase();
  const int cellCnt=ReadFunc::readCells(DB,IX,cellIndex,OMap);
  
  ELog::EM<<"Read "<<cellCnt<<" extra cells for beamstop"<<ELog::endDebug;
  if (cellCnt<=0)
    return -2;
  cellIndex+=cellCnt;
  
  const int surfCnt=ReadFunc::readSurfaces(DB,IX,stopIndex);
  ELog::EM<<"Read "<<surfCnt<<" extra surfaces for beamstop"<<ELog::endDebug;
 
  // REBASE and recentre
  const ModelSupport::surfIndex::STYPE& SMap
    =ModelSupport::surfIndex::Instance().surMap();
  ModelSupport::surfIndex::STYPE::const_iterator sc;
  for(sc=SMap.begin();sc!=SMap.end();sc++)
    {
      if (sc->first>stopIndex && sc->first<stopIndex+10000)
	{
	  ELog::EM<<"Displacing :"<<sc->first<<" by "<<Centre<<ELog::endWarn;
	  sc->second->rotate(RBase);
	  sc->second->displace(Centre);
	}
    }
  reMapSurf(OMap);
  // READJUST 
  System.setMaterialDensity(OMap);
  ReadFunc::OTYPE& SysOMap=System.getCells();
  ReadFunc::mapInsert(OMap,SysOMap);
  
  populated |= 16;
  return 0; 
}

void
BeamStop::reMapSurf(ReadFunc::OTYPE& ObjMap) const
  /*!
    Given a set of Objects, remap the surfaces so they are 
    displaced by stopIndex.
    \param ObjMap :: Map of objects
  */
{
  ReadFunc::OTYPE::iterator mc;
  
  for(mc=ObjMap.begin();mc!=ObjMap.end();mc++)
    {
      std::string cellStr=mc->second->cellCompStr();
      cellStr=ModelSupport::getComposite(SMap,stopIndex,cellStr);
      mc->second->procString(cellStr);
    }
  return;
}
  
void
BeamStop::createAll(Simulation& System,
		    const hutchSystem::ChipIRHutch& Hut,
		    const std::string& FName)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param Hut :: ChipIRHutch to connect box to
    \param FName :: BeamStop file to be readin
  */
{
  ELog::RegMethod RegA("BeamStop","createAll");
  populate(System);

  setInsertCell(Hut.getBeamStopCell());
  setCentre(Hut.getBeamStopCentre());
  setAxis(Hut.getX());

  createUnitVector(Hut);

  createSurfaces(System);  
  if (FName.empty())
    {
      createDefSurfaces();
      createObjects(System);
    }
  else 
    readFile(System,FName);
  
  layerProcess(System);
  insertObjects(System);
 
  return;
}
  
}  // NAMESPACE shutterSystem
