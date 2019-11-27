/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flukaProcess/flukaDefPhysics.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "inputParam.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "AttachSupport.h"
#include "LinkSupport.h"
#include "Object.h"
#include "ObjectAddition.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimMCNP.h"
#include "SimFLUKA.h"

#include "flukaImpConstructor.h"
#include "flukaProcess.h"
#include "cellValueSet.h"
#include "pairValueSet.h"
#include "flukaPhysics.h"
#include "magnetUnit.h"
#include "magnetDipole.h"
#include "magnetQuad.h"
#include "magnetHexapole.h"
#include "magnetOctopole.h"
#include "flukaDefPhysics.h"


namespace flukaSystem
{  

  void setMagneticExternal(SimFLUKA&,const mainSystem::inputParam&);
  
void
setUserFlags(SimFLUKA& System,
	      const mainSystem::inputParam& IParam)
   /*!
    Currently very simple system to get additional flag
    \param System :: Simulation
    \param IParam :: Input parameters
  */
{
  ELog::RegMethod RegA("flukaDefPhysics[F]","setUserFlags");


  if (IParam.flag("userWeight"))  // only one
    {
      const size_t NIndex=IParam.itemCnt("userWeight");

      const std::string extra =
	IParam.getDefValue<std::string>("3","userWeight");

      System.addUserFlags("userWeight",extra);
    }
  return;
}

void
setMagneticPhysics(SimFLUKA& System,
		   const mainSystem::inputParam& IParam)
  /*!
    Currently very simple but expect to get complex.
    \param System :: Simulation
    \param IParam :: Input parameters
  */
{
  ELog::RegMethod Rega("flukaDefPhysics","setMagneticPhysics");

  if (IParam.flag("MAG"))
    {
      const Geometry::Vec3D MF=
	IParam.getValue<Geometry::Vec3D>("MAG",0);  
      System.setMagField(MF);
    }

  const size_t nSet=IParam.setCnt("MagField");
  for(size_t setIndex=0;setIndex<nSet;setIndex++)
    {
      const size_t NIndex=IParam.itemCnt("MagField",setIndex);
      
      for(size_t index=0;index<NIndex;index++)
	{
	  const std::string objName=
	    IParam.getValueError<std::string>
	    ("MagField",setIndex,index,"No objName for MagField");
	  const std::vector<int> Cells=System.getObjectRange(objName);
	  if (Cells.empty())
	    throw ColErr::InContainerError<std::string>
	      (objName,"Empty cell");
	  
	  Simulation::OTYPE& CellObjects=System.getCells();
	  // Special to set cells in OBJECT  [REMOVE]
	  for(const int CN : Cells)
	    {
	      Simulation::OTYPE::iterator mc=
		CellObjects.find(CN);
	      if (mc!=CellObjects.end())
		mc->second->setMagFlag();
	    }
	}
    }
  if (nSet) setMagneticExternal(System,IParam);
  return;
}

  
void
setMagneticExternal(SimFLUKA& System,
		    const mainSystem::inputParam& IParam)
  /*!
    Sets the external magnetic fields in object(s) 
    \param System :: Simulation
    \param IParam :: Input parameters
  */
{
  ELog::RegMethod Rega("flukaDefPhysics[F]","setMagneticExternal");


  if (IParam.flag("MagUnit"))
    {
      const size_t nSet=IParam.setCnt("MagUnit");
      for(size_t setIndex=0;setIndex<nSet;setIndex++)
	{
	  Geometry::Vec3D AOrg;
	  Geometry::Vec3D AY;
	  Geometry::Vec3D AZ;
	  
	  // General form is ::  Type : location : Param
	  size_t index(1);
	  const std::string typeName=IParam.getValueError<std::string>
	    ("MagUnit",setIndex,0,"typeName");
	  ModelSupport::getObjectAxis(System,"MagUnit",IParam,setIndex,index,AOrg,AY,AZ);
	  const Geometry::Vec3D Extent=IParam.getCntVec3D("MagUnit",setIndex,index,"Extent");
	  const double KV=
	    IParam.getValueError<double> ("MagUnit",setIndex,index,"K Value");


	  if (typeName=="Octopole")
	    {
	      std::shared_ptr<flukaSystem::magnetOctopole>
		OPtr(new magnetOctopole("Octo",setIndex));
	      OPtr->createAll(System,AOrg,AY,AZ,Extent,KV);
	      System.addMagnetObject(OPtr);
	    }
	  else if (typeName=="Hexapole")
	    {
	      std::shared_ptr<flukaSystem::magnetHexapole>
		OPtr(new magnetHexapole("Hexa",setIndex));
	      OPtr->createAll(System,AOrg,AY,AZ,Extent,KV);
	      System.addMagnetObject(OPtr);
	    }

	  else if (typeName=="Quad")
	    {
	      std::shared_ptr<flukaSystem::magnetQuad>
		QPtr(new magnetQuad("Quad",setIndex));
	      QPtr->createAll(System,AOrg,AY,AZ,Extent,KV);
	      System.addMagnetObject(QPtr);
	    }

	  else if (typeName=="Dipole")
	    {
	      const double Radius=IParam.getValueError<double>
		("MagUnit",setIndex,index,"Radius");
	      std::shared_ptr<flukaSystem::magnetDipole>
		QPtr(new magnetDipole("Dipl",setIndex));
	      QPtr->createAll(System,AOrg,AY,AZ,Extent,KV,Radius);
	      System.addMagnetObject(QPtr);
	    }
	  else
	    {
	      throw ColErr::InContainerError<std::string>
		(typeName,"Magnet type not known");
	    }
	}
    }
  return;
}

  
void
setModelPhysics(SimFLUKA& System,
		const mainSystem::inputParam& IParam)
  /*!
    Set the physics that needs a model
    \param System :: Simulation
    \param IParam :: Input parameters
  */
{
  ELog::RegMethod RegA("flukaDefPhysics","setModelPhysics");
  
  setXrayPhysics(System,IParam);
  setMagneticPhysics(System,IParam);
  setUserFlags(System,IParam);
  
  size_t nSet=IParam.setCnt("wMAT");
  if (nSet)
    {
      flukaSystem::flukaImpConstructor A;
      for(size_t index=0;index<nSet;index++)
	A.processMAT(System,IParam,index);
    }

  nSet=IParam.setCnt("wIMP");
  if (nSet)
    {
      flukaSystem::flukaImpConstructor A;
      for(size_t index=0;index<nSet;index++)
	A.processUnit(System,IParam,index);
    }
  
  nSet=IParam.setCnt("wCUT");    
  if (nSet)
    {
      flukaSystem::flukaImpConstructor A;
      for(size_t index=0;index<nSet;index++)
	A.processCUT(System,IParam,index);
    }
  
  nSet=IParam.setCnt("wEMF");
  if (nSet)
    {
      flukaSystem::flukaImpConstructor A;
      for(size_t index=0;index<nSet;index++)
	A.processEMF(System,IParam,index);
    }
  
  nSet=IParam.setCnt("wEXP");
  if (nSet)
    {
      flukaSystem::flukaImpConstructor A;
      for(size_t index=0;index<nSet;index++)
	A.processEXP(System,IParam,index);
    }

  nSet=IParam.setCnt("wLAM");    
  if (nSet)
    {
      flukaSystem::flukaImpConstructor A;
      for(size_t index=0;index<nSet;index++)
	A.processLAM(System,IParam,index);
    }
  
  return; 
}

  
void 
setXrayPhysics(SimFLUKA& System,
	       const mainSystem::inputParam& IParam)
  /*!
    Set the neutron Physics for FLUKA run on a reactor
    \param System :: Fluke sim
    \param IParam :: Input stream
  */
{
  ELog::RegMethod RegA("DefPhysics","setXrayPhysics");

  flukaPhysics& PC= *System.getPhysics();
  const std::string PModel=IParam.getValue<std::string>("physModel");
  // typedef std::tuple<size_t,std::string,std::string,
  // 		     std::string,std::string> unitTYPE;

  // CELL emfs
  const std::set<int> activeCell=getActiveUnit(System,0,"all");
  const std::set<int> activeMat=getActiveUnit(System,1,"all");
  for(const int MN : activeMat)
    {
      //Turn pair-bremstrauhlung on 
      PC.setFlag("photonuc",MN);
      // Muon photon
      PC.setFlag("muphoton",MN);
      // electronuclear
      PC.setFlag("elecnucl",MN);
      // muon pair
      PC.setFlag("mupair",MN);
      
      // Compton PhotoElectic GammaPairProductuion      
      PC.setTHR("photthr",MN,"1e-3","1e-3","1.0");
	
      // Interaction threshold : Brem-e+/e- moller scatter photonuclear"
      PC.setTHR("elpothr",MN,"1e-2","1e-2","1.0");

      // Turn off multiple scattering [not a good idea]
      //      PC.setTHR("mulsopt",MN,"0","0","3");

      // Production Cut for e/e+ and photon 
      PC.setEMF("prodcut",MN,"1","1e-3");
      // Rayleigh photonuc-iteratcion
      PC.setEMF("pho2thr",MN,"1e-3","1");

      // Pairbrem
      PC.setEMF("pairbrem",MN,"0.0","0.1");
    }
  //  const std::set<int> activeMat=getActiveUnit(1,"all");

  for(const int CN : activeCell)
    { 
      PC.setEMF("emfcut",CN,"0.035","0.005");
    }
  // SPECIAL:
  PC.setIMP("partthr","neutron","1e-9");
  return; 
}


} // NAMESPACE flukaSystem
