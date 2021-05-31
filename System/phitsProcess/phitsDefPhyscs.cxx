/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   phitsProcess/phitsDefPhysics.cxx
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "inputParam.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimPHITS.h"

#include "phitsPhysics.h"

namespace phitsSystem
{  

void
setModelPhysics(SimPHITS& System,
		const mainSystem::inputParam& IParam)
  /*!
    Set the physics that needs a model
    \param System :: Simulation
    \param IParam :: Input parameter
  */
{
  ELog::RegMethod RegA("phitsDefPhysics[F]","setModelPhysics");

  // This forces icntl : Should not be needed. 
  if (IParam.flag("icntl"))
    {
      ELog::EM<<"VAL "<<IParam.getValue<std::string>("icntl")<<ELog::endDiag;
      System.setICNTL(IParam.getValue<std::string>("icntl"));
    }
  
  phitsSystem::phitsPhysics* PC=System.getPhysics();
  if (!PC) return;

  // This is PHITS cards
  size_t nSet=IParam.setCnt("energyCut");
  for(size_t i=0;i<nSet;i++)
    {
      const size_t nElm=IParam.itemCnt("energyCut",i);
      size_t index(0);
      while(index<nElm)
	{
	  const std::string particle=
	    IParam.getValueError<std::string>
	    ("energyCut",i,index++,"particle name");
	  const double V=
	    IParam.getValueError<double>
	    ("energyCut",i,index++,"cutoff value");
	  PC->setECut(particle,V);
	}
    }

  // This is PHITS cards for energy range of charged particles
  if (IParam.flag("eRange"))
    {
      const double eLow=IParam.getValueError<double>
	("eRange",0,0,"eRange needs lower value (-ve for default)");
      const double eHigh=IParam.getDefValue<double>(-1.0,"eRange",0,1);
      PC->setERange(eLow,eHigh);
    }

  // This is PHITS cards for energy range of electron track
  if (IParam.flag("eTrack"))
    {
      const double eLow=IParam.getValueError<double>
	("eTrack",0,0,"eTrack needs lower value (-ve for default)");
      const double eHigh=IParam.getDefValue<double>(-1.0,"eTrack",0,1);
      PC->setETrack(eLow,eHigh);
    }
  
  // This is FLUKA cards but will use if useful
  // Card of type -wEMF prodCut  / -wCUT partthr
  nSet=IParam.setCnt("wEMF"); 
  for(size_t i=0;i<nSet;i++)
    {
      const std::string type=IParam.getValueError<std::string>
	("wEMF",i,0,"wEMF card needs type");
      if (type=="prodcut")
	{
	  const std::string region=IParam.getValueError<std::string>
	    ("wEMF",i,1,"wEMF card needs region");
	  if (region=="all" || region=="All")
	    {
	      const double eCut=IParam.getDefValue<double>
		(-1.0,"wEMF",i,2);
	      const double pCut=IParam.getDefValue<double>
		(-1.0,"wEMF",i,3);
	      PC->setECut("electron",eCut);
	      PC->setECut("positron",eCut);
	      PC->setECut("photon",pCut);
	    }
	}
    }
  
  nSet=IParam.setCnt("wCUT"); 
  for(size_t i=0;i<nSet;i++)
    {
      const std::string type=IParam.getValueError<std::string>
	("wCUT",i,0,"wCUT card needs type");
      if (type=="partthr")
	{
	  const std::string particle=IParam.getValueError<std::string>
	    ("wCUT",i,1,"wCUT particle type");
	  const double eCut=IParam.getDefValue<double>
	    (-1.0,"wCUT",i,2);
	  PC->setECut(particle,eCut);
	}
    }

  
  return; 
}

void 
setDefaultPhysics(SimPHITS& System,
		  const mainSystem::inputParam& IParam)
  /*!
    Set the default Physics for phits
    \param System :: Simulation
    \param IParam :: Input parameter
  */
{
  ELog::RegMethod RegA("DefPhysics[F]","setDefaultPhysics(phits)");

  // trick to allow 1e8 entries etc.
  System.setNPS(static_cast<size_t>(IParam.getValue<double>("nps")));
  System.setRND(IParam.getValue<long int>("random"));
  return;
}
  
void 
setXrayPhysics(phitsPhysics& PC,
	       const mainSystem::inputParam& IParam)
  /*!
    Set the xray Physics for PHITS run on a synchrotron
    \param PC :: Physics cards
    \param IParam :: Input stream
  */
{
  ELog::RegMethod RegA("phitsDefPhysics[F]","setXrayPhysics");

  const std::string PModel=IParam.getValue<std::string>("physModel");
  typedef std::tuple<size_t,std::string,std::string,
		     std::string,std::string> unitTYPE;

  ELog::EM<<"Call here "<<ELog::endDiag;
  
  return; 
}


} // NAMESPACE phitsSystem
