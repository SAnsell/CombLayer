/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   Process/Process.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cmath>
#include <complex>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <functional>
#include <memory>
#include <array>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "inputParam.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Zaid.h"
#include "MXcards.h"
#include "Material.h"
#include "DBMaterial.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "particleConv.h"

#include "Simulation.h"
#include "Process.h"

namespace ModelSupport
{

void
setWImp(Simulation& System,const mainSystem::inputParam& IParam)
  /*!
    Set imp weigths for individual cells and regions
    \param System :: Simulation to use
    \param IParam :: Input param
  */
{
  ELog::RegMethod RegA("Process[F]","setWImp");

  const particleConv& pConv=particleConv::Instance();

  const size_t nIMP=IParam.setCnt("wIMP");

  std::string pType,cellName;
  for(size_t setIndex=0;setIndex<nIMP;setIndex++)
    {
      double impValue(1.0);

      pType=IParam.getValueError<std::string>
	("wIMP",setIndex,0,"No cell/object for wIMP ");

      if (pConv.hasName(pType))
	{
	  cellName=IParam.getValueError<std::string>
	    ("wIMP",setIndex,1,"No cell/object for wIMP ");
	  impValue=IParam.getValueError<double>
	    ("wIMP",setIndex,2," IMP value for wIMP");
	}
      else
	{
	  cellName=pType;
	  pType="all";
	  impValue=IParam.getValueError<double>
	    ("wIMP",setIndex,1," IMP value for wIMP");
	}
      
      const std::set<int> cells=
	ModelSupport::getActiveCell(System,cellName);
      for(const int CN : cells)
	    System.setImp(CN,pType,impValue);
    }
  return;
}
  
std::set<int>
getActiveMaterial(const Simulation& System,
		  std::string material)
  /*!
    Given a material find the active cells
    \param material : material name to use
    \return set of active materials (matching material)
  */
{
  ELog::RegMethod RegA("Process[F]","getActiveMaterial");

  const ModelSupport::DBMaterial& DB=
    ModelSupport::DBMaterial::Instance();
  
  std::set<int> activeMat=System.getActiveMaterial();
  if (material=="All" || material=="all")
    return activeMat;
  
  bool negKey(0);
  if (material.size()>1 && material.back()=='-')
    {
      negKey=1;
      material.pop_back();
    }
  
  if (!DB.hasKey(material))
    throw ColErr::InContainerError<std::string>
      (material,"Material not in material database");
  const int matID=DB.getIndex(material);

  if (negKey)
    {
      activeMat.erase(matID);
      return activeMat;
    }
  if (activeMat.find(matID)==activeMat.end())
    throw ColErr::InContainerError<std::string>
      (material,"Not present in model");
  
  std::set<int> activeOut;
  activeOut.insert(matID);
  return activeOut;
}

std::set<int>
getActiveCell(const objectGroups& OGrp,
	      const std::string& cell)
  /*!
    Given a cell find the active cells
    \param OGrp :: Active group						
    \param cell : cell name to use
    \return set of active cell numbers 
  */
{
  ELog::RegMethod RegA("Process[F]","getActiveCell");

  const std::vector<int> Cells=OGrp.getObjectRange(cell);
  std::set<int> activeCell(Cells.begin(),Cells.end());

  activeCell.erase(1);
  return activeCell;
}
  
void
setDefRotation(const objectGroups& OGrp,
	       const mainSystem::inputParam& IParam)
  /*!
    Apply a standard rotation to the simulation
    \param OGrp :: Object group
    \param IParam :: Parameter set
   */
{
  ELog::RegMethod RegA("Process[F]","setDefRotation");

  masterRotate& MR = masterRotate::Instance();
  if (IParam.flag("axis"))
    {
      // Move X to Z:
      MR.addRotation(Geometry::Vec3D(0,1,0),
		     Geometry::Vec3D(0,0,0),
		     90.0);
      //Move XY to -X-Y 
      MR.addRotation(Geometry::Vec3D(0,0,1),
		     Geometry::Vec3D(0,0,0),
		     -90.0);
      MR.addMirror(Geometry::Plane
		   (1,0,Geometry::Vec3D(0,0,0),
		    Geometry::Vec3D(1,0,0)));
    }

  if (IParam.flag("offset"))
    {
      const size_t nP=IParam.setCnt("offset");
      for(size_t i=0;i<nP;i++)
        procOffset(OGrp,IParam,"offset",i);
    }
  if (IParam.flag("angle"))
    {
      const size_t nP=IParam.setCnt("angle");
      for(size_t i=0;i<nP;i++)
        procAngle(OGrp,IParam,i);
    }
  if (IParam.flag("postOffset"))
    {
      const size_t nP=IParam.setCnt("postOffset");
      for(size_t i=0;i<nP;i++)
        procOffset(OGrp,IParam,"postOffset",i);
    }
  return;
}

void
procAngle(const objectGroups& OGrp,
	  const mainSystem::inputParam& IParam,
          const size_t index)
  /*!
    Process an angle unit
    \param OGrp :: Object group
    \param IParam :: Input param
    \param index :: set index
  */
{
  ELog::RegMethod RegA("Process[F]","procAngle");
  
  masterRotate& MR = masterRotate::Instance();

  const std::string AItem=
    IParam.getValue<std::string>("angle",index,0);
  const std::string BItem=(IParam.itemCnt("angle",index)>1) ?
    IParam.getValue<std::string>("angle",index,1) : "";

  if (AItem=="object" || AItem=="Object")
    {
      const attachSystem::FixedComp* GIPtr=
        OGrp.getObjectThrow<attachSystem::FixedComp>(BItem,"FixedComp");
      const std::string CItem=
        IParam.getDefValue<std::string>("2","angle",index,2);
      const int ZFlag=IParam.getDefValue<int>(1,"angle",index,3);
      const long int axisIndex=GIPtr->getSideIndex(CItem);

      const Geometry::Vec3D AxisVec=
        GIPtr->getLinkAxis(axisIndex);

      // Align item such that we put the object linkPt at +ve X
      const Geometry::Vec3D ZRotAxis=GIPtr->getZ();

      const double angle=180.0*acos(AxisVec[0])/M_PI;
      MR.addRotation(GIPtr->getZ(),
                     Geometry::Vec3D(0,0,0),ZFlag*angle);
      // Z rotation.
      const double angleZ=90.0-180.0*acos(-AxisVec[2])/M_PI;
      MR.addRotation(GIPtr->getX(),Geometry::Vec3D(0,0,0),-angleZ);
      ELog::EM<<"ROTATION AXIS["<<ZFlag<<"] == "
              <<AxisVec<<ELog::endDiag;

    }
  else  if (AItem=="objPoint" || AItem=="ObjPoint")
    {
      const attachSystem::FixedComp* GIPtr=
        OGrp.getObjectThrow<attachSystem::FixedComp>(BItem,"FixedComp");
      const std::string CItem=
        IParam.getDefValue<std::string>("2","angle",index,2);

      const long int sideIndex=GIPtr->getSideIndex(CItem);
          
      Geometry::Vec3D LP=GIPtr->getLinkPt(sideIndex);
      LP=LP.cutComponent(Geometry::Vec3D(0,0,1));
      LP.makeUnit();

      double angleZ=180.0*acos(LP[0])/M_PI;
      if (LP[1]>0.0) angleZ*=-1;
      MR.addRotation(Geometry::Vec3D(0,0,1),
                     Geometry::Vec3D(0,0,0),angleZ);
    }
  else  if (AItem=="objAxis" || AItem=="ObjAxis" ||
	    AItem=="objYAxis" || AItem=="ObjYAxis")
    {
      const attachSystem::FixedComp* GIPtr=
        OGrp.getObjectThrow<attachSystem::FixedComp>(BItem,"FixedComp");
      const std::string CItem=
        IParam.getDefValue<std::string>("2","angle",index,2);
      
      const long int sideIndex=GIPtr->getSideIndex(CItem);
      
      Geometry::Vec3D XRotAxis,YRotAxis,ZRotAxis;
      GIPtr->selectAltAxis(sideIndex,XRotAxis,YRotAxis,ZRotAxis);

      if (AItem=="objYAxis" || AItem=="ObjYAxis")
	{
	  const Geometry::Quaternion QR=Geometry::Quaternion::calcQVRot
	    (Geometry::Vec3D(0,1,0),YRotAxis,ZRotAxis);
	  
	  MR.addRotation(QR.getAxis(),Geometry::Vec3D(0,0,0),
			 -180.0*QR.getTheta()/M_PI);
	}
      else
	{
	  const Geometry::Quaternion QR=Geometry::Quaternion::calcQVRot
	    (Geometry::Vec3D(1,0,0),YRotAxis,ZRotAxis);
	  
	  MR.addRotation(QR.getAxis(),Geometry::Vec3D(0,0,0),
			 -180.0*QR.getTheta()/M_PI);
	}

    }
  else if (AItem=="free" || AItem=="FREE")
    {
      const double rotAngle=
        IParam.getValue<double>("angle",index,1);
      MR.addRotation(Geometry::Vec3D(0,0,1),Geometry::Vec3D(0,0,0),
                     -rotAngle);
      ELog::EM<<"ADDING ROTATION "<<rotAngle<<ELog::endDiag;
    }
  else if (AItem=="freeAxis" || AItem=="FREEAXIS")
    {
      size_t itemIndex(1);
      const Geometry::Vec3D rotAxis=
        IParam.getCntVec3D("angle",index,itemIndex,"Axis need [Vec3D]");
      const double rotAngle=
        IParam.getValue<double>("angle",index,itemIndex);
      MR.addRotation(rotAxis,Geometry::Vec3D(0,0,0),
                     -rotAngle);		  
    }
  else if (AItem=="help" || AItem=="Help")
    {
      ELog::EM<<"Angle help ::\n"
              <<"  free rotAngle :: Rotate about Z axis \n"
              <<"  freeAxis Vec3D rotAngle :: Rotate about Axis \n"
              <<"  objPoint  FC link :: Rotate linkPt to (X,0,0) \n"
              <<"  objAxis  FC link :: Rotate link-axit to X \n"
              <<"  object  FC link :: Rotate Axis about Z to "
              <<ELog::endDiag;
    }
  else
    throw ColErr::InContainerError<std::string>(AItem,"angle input error");
      
  return;
}


void
procOffset(const objectGroups& OGrp,
	   const mainSystem::inputParam& IParam,
	   const std::string& keyID,
           const size_t index)
  /*!
    Process an offset unit
    \param OGrp :: Object group
    \param IParam :: Input param
    \param index :: set index
  */
{
  ELog::RegMethod RegA("Process[F]","procOffset");
  masterRotate& MR = masterRotate::Instance();  

  const std::string AItem=
    IParam.getValue<std::string>(keyID,index);
  const std::string BItem=(IParam.itemCnt(keyID,index)>1) ?
    IParam.getValue<std::string>(keyID,index,1) : "";

  if (AItem=="object" || AItem=="Object")
    {
      const attachSystem::FixedComp* GIPtr=
        OGrp.getObjectThrow<attachSystem::FixedComp>(BItem,"FixedComp");
      const std::string CItem=
        IParam.getDefValue<std::string>("0",keyID,index,2);
      const long int sideIndex=GIPtr->getSideIndex(CItem);
      ELog::EM<<"Main Offset at "<<GIPtr->getLinkPt(sideIndex)
              <<ELog::endDiag;
      MR.addDisplace(-GIPtr->getLinkPt(sideIndex));
    }
  else if (AItem=="free" || AItem=="FREE")
    {
      size_t itemIndex(1);
      const Geometry::Vec3D OffsetPos=
        IParam.getCntVec3D(keyID,index,itemIndex,keyID+" need vec3D");
      MR.addDisplace(-OffsetPos);
    }
  else
    throw ColErr::InContainerError<std::string>(AItem,keyID+": input error");

  return;
}
  
  

} // NAMESPACE ModelSupport
