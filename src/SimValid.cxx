/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   src/SimValid.cxx
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
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <complex>
#include <string>
#include <sstream>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "MersenneTwister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "mathSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "SimProcess.h"
#include "SurInter.h"
#include "ObjSurfMap.h"
#include "neutron.h"
#include "objectRegister.h"
#include "surfRegister.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "Simulation.h"
#include "SimValid.h"

#include "debugMethod.h"

extern MTRand RNG;

namespace ModelSupport
{

SimValid::SimValid() :
  Centre(Geometry::Vec3D(0.15,-0.45,0.15))
  /*!
    Constructor
  */
{}

SimValid::SimValid(const SimValid& A) : 
  Centre(A.Centre)
  /*!
    Copy constructor
    \param A :: SimValid to copy
  */
{}

SimValid&
SimValid::operator=(const SimValid& A)
  /*!
    Assignment operator
    \param A :: SimValid to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Centre=A.Centre;
    }
  return *this;
}

void
SimValid::diagnostics(const Simulation& System,
		     const std::vector<simPoint>& Pts) const
  /*!
    Write out some diagnostic information
    \param System :: simuation to sued
   */
{
  ELog::RegMethod RegA("SimValid","diagnostics");

  const ModelSupport::ObjSurfMap* OSMPtr =System.getOSM();
    
  ELog::EM<<"------------"<<ELog::endCrit;

  for(size_t j=0;j<Pts.size();j++)
    {
      ELog::EM<<"Pos["<<j<<"]=="<<Pts[j].Pt<<" :: Obj:"
	      <<Pts[j].objN<<" Surf:"<<Pts[j].surfN<<ELog::endDiag;
    }
  

  if (Pts.size()>=3)
    {
      double aDist;
      const Geometry::Surface* SPtr;          // Output surface
      const size_t index(Pts.size()-3);
      MonteCarlo::neutron TNeut(1,Pts[index].Pt,Pts[index].Dir);
                                      
      ELog::EM<<"Base Obj == "<<*Pts[index].OPtr
	      <<ELog::endDiag;
      ELog::EM<<"Next Obj == "<<*Pts[index+1].OPtr
	      <<ELog::endDiag;
      // RESET:
      TNeut.Pos=Pts[index].Pt;  // Reset point
      const MonteCarlo::Object* OPtr=Pts[index].OPtr;
      const int SN=Pts[index].surfN;

      ELog::EM<<"RESET POS== "<<TNeut.Pos<<ELog::endDiag;
      ELog::EM<<"RESET Obj== "<<OPtr->getName()<<ELog::endDiag;
      ELog::EM<<"RESET SurfN== "<<Pts[index].surfN<<ELog::endDiag;
      ELog::EM<<"----------------------------------"<<ELog::endDiag ;
      OPtr=OSMPtr->findNextObject(Pts[index].surfN,
				  TNeut.Pos,OPtr->getName());	    
      if (OPtr)
	{
	  ELog::EM<<"Found Obj == "<<*OPtr<<" :: "<<Pts[index].Pt<<" "
		  <<OPtr->pointStr(Pts[index].Pt)<<ELog::endDiag;
	  ELog::EM<<OPtr->isValid(Pts[index].Pt)<<ELog::endDiag;
	}
      else
	ELog::EM<<"No object "<<ELog::endDiag;

      TNeut.Pos += TNeut.uVec*0.00001;
      
      MonteCarlo::Object* NOPtr=System.findCell(TNeut.Pos,0);
      if (NOPtr)
	{
	  ELog::EM<<"Neutron == "<<TNeut<<ELog::endDiag;
	  ELog::EM<<"Actual object == "<<*NOPtr<<ELog::endDiag;
	  ELog::EM<<" IMP == "<<NOPtr->getImp()<<ELog::endDiag;
	}
      ELog::EM<<"TRACK to NEXT"<<ELog::endDiag;
      ELog::EM<<"--------------"<<ELog::endDiag;
      
      OPtr->trackOutCell(TNeut,aDist,SPtr,abs(SN));
    }

  return;
}

  
int
SimValid::runPoint(const Simulation& System,
		   const Geometry::Vec3D& CP,
		   const size_t  nAngle) const
  /*!
    Calculate the tracking
    \param System :: Simulation to use
    \param CP :: Centre point
    \param nAngle :: Number of points to test
    \return true if valid
  */
{
  ELog::RegMethod RegA("SimValid","run");
  ELog::debugMethod DebA;
  
  const ModelSupport::ObjSurfMap* OSMPtr =System.getOSM();
  MonteCarlo::Object* InitObj(0);
  const Geometry::Surface* SPtr;          // Output surface
  double aDist;       

  // Note for sphere that you can use X,Y,Z in any orthogonal 
  // directiron
  double phi,theta;

  // Find Initial cell [Store for next time]
  //  Centre+=Geometry::Vec3D(0.001,0.001,0.001);
  InitObj=System.findCell(CP,InitObj);  
  const int initSurfNum=InitObj->isOnSide(CP);

  // check surfaces
  for(size_t i=0;i<nAngle;i++)
    {
      std::vector<simPoint> Pts;
      // Get random starting point on edge of volume
      phi=RNG.rand()*M_PI;
      theta=2.0*RNG.rand()*M_PI;
      Geometry::Vec3D uVec(cos(theta)*sin(phi),
			     sin(theta)*sin(phi),
			     cos(phi));
      MonteCarlo::neutron TNeut(1,Centre,uVec);

      MonteCarlo::Object* OPtr=InitObj;
      int SN(-initSurfNum);

      Pts.push_back(simPoint(TNeut.Pos,TNeut.uVec,OPtr->getName(),SN,OPtr));
      while(OPtr && OPtr->getImp())
	{
	  // Note: Need OPPOSITE Sign on exiting surface
	  SN= OPtr->trackOutCell(TNeut,aDist,SPtr,abs(SN));
	  if (aDist>1e30 && Pts.size()<=1)
	    {
	      ELog::EM<<"Fail on Pts==1 and aDist inf"<<ELog::endDiag;
	      ELog::EM<<"Index == "<<Pts.size()-2<<ELog::endDiag;
	      ELog::EM<<"Pts[0] == "<<Pts[0].Pt<<ELog::endDiag;
	      ELog::EM<<"SN == "<<SN<<ELog::endDiag;
	      aDist=1e-5;
	    }

	  TNeut.moveForward(aDist);
	  Pts.push_back(simPoint(TNeut.Pos,TNeut.uVec,OPtr->getName(),SN,OPtr));
	  OPtr=(SN) ?
	    OSMPtr->findNextObject(SN,TNeut.Pos,OPtr->getName()) : 0;	    
	}

      if (!OPtr)
	{
	  ELog::EM<<"Failed to calculate cell correctly: "<<i<<ELog::endCrit;
	  diagnostics(System,Pts);
	  return 0;
	}
    }
  return 1;
}

int
SimValid::runFixedComp(const Simulation& System,
		       const size_t N) const
  /*!
    Calculate the tracking from fixedcomp
    \param System :: Simulation to use
    \param N :: Number of points to test
    \return true if valid
  */
{
  ELog::RegMethod RegA("SimValid","runFixedComp");
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  typedef std::shared_ptr<attachSystem::FixedComp> CTYPE;
  typedef std::map<std::string,CTYPE> cMapTYPE;

  const cMapTYPE& CM=OR.getComponents();

  for(const cMapTYPE::value_type& FCitem : CM)
    {
      const CTYPE& FC = FCitem.second;
      const std::vector<Geometry::Vec3D> FCPts=
	FC->getAllLinkPts();
      for(const Geometry::Vec3D& Pt : FCPts)
	ELog::EM<<"PT == "<<FC->getKeyName()<<" :: "<<Pt<<ELog::endDiag;
    }
  
  ELog::EM<<"Finished Validation check"<<ELog::endDiag;
  return 1;
}

} // NAMESPACE ModelSupport
