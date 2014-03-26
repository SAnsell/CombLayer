/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   src/SimValid.cxx
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
#include <boost/shared_ptr.hpp>
#include <boost/format.hpp>
#include <boost/multi_array.hpp>

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
#include "localRotate.h"
#include "masterRotate.h"
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
#include "Simulation.h"
#include "SimValid.h"

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

int
SimValid::run(const Simulation& System,const size_t N) const
  /*!
    Calculate the tracking
    \param System :: Simulation to use
    \param N :: Number of points to test
    \return true if valid
  */
{
  ELog::RegMethod RegA("SimValid","run");
  
  const ModelSupport::ObjSurfMap* OSMPtr =System.getOSM();
  MonteCarlo::Object* InitObj(0);
  const Geometry::Surface* SPtr;          // Output surface
  double aDist;       

  // Note for sphere that you can use X,Y,Z in any orthogonal 
  // directiron
  double phi,theta;

  // Find Initial cell [Store for next time]
  //  Centre+=Geometry::Vec3D(0.001,0.001,0.001);
  InitObj=System.findCell(Centre,InitObj);  
  const int initSurfNum=InitObj->isOnSide(Centre);

  ELog::EM<<"Init Object nubmer == "<<InitObj->getName()<<ELog::endDiag;      
  ELog::EM<<"Initial surface [if on surf] == "<<initSurfNum<<ELog::endDiag; 
     
  // check surfaces
  for(size_t i=0;i<N;i++)
    {
      std::vector<simPoint> Pts;
      // Get random starting point on edge of volume
      phi=RNG.rand()*M_PI;
      theta=2.0*RNG.rand()*M_PI;
      Geometry::Vec3D D(cos(theta)*sin(phi),
			sin(theta)*sin(phi),
			cos(phi));
      MonteCarlo::neutron TNeut(1,Centre,D);

      MonteCarlo::Object* OPtr=InitObj;
      int SN(-initSurfNum);

      Pts.push_back(simPoint(TNeut.Pos,OPtr->getName(),SN,OPtr));
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
	  Pts.push_back(simPoint(TNeut.Pos,OPtr->getName(),SN,OPtr));
	  OPtr=(SN) ?
	    OSMPtr->findNextObject(SN,TNeut.Pos,OPtr->getName()) : 0;	    
	}

      if (!OPtr)
	{
	  ELog::EM<<"------------"<<ELog::endCrit;
	  ELog::EM<<"I/SN == "<<i<<" "<<SN<<ELog::endCrit;
	  for(size_t j=0;j<Pts.size();j++)
	    {
	      ELog::EM<<"Pos["<<j<<"]=="<<Pts[j].Pt<<" :: "
		      <<Pts[j].objN<<" "<<Pts[j].surfN<<ELog::endDiag;
	    }

	  const size_t index(Pts.size()-2);
	  ELog::EM<<"Base Obj == "<<*Pts[index].OPtr
		  <<ELog::endDiag;
	  ELog::EM<<"Next Obj == "<<*Pts[index+1].OPtr
		  <<ELog::endDiag;
	  OPtr=Pts[index].OPtr;
	  // RESET:
	  TNeut.Pos=Pts[index].Pt;  // Reset point
	  OPtr=Pts[index].OPtr;

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
	  TNeut.Pos+=D*0.00001;

	  MonteCarlo::Object* NOPtr=System.findCell(TNeut.Pos,0);
	  if (NOPtr)
	    {
	      ELog::EM<<"Neutron == "<<TNeut<<ELog::endDiag;
	      ELog::EM<<"Actual object == "<<*NOPtr<<ELog::endDiag;
	      ELog::EM<<" IMP == "<<NOPtr->getImp()<<ELog::endDiag;
	    }
	  ELog::EM<<"Failed to calculate cell correctly: "<<i<<ELog::endCrit;
	  return 0;
	}
    }
  ELog::EM<<"Finished Validation check"<<ELog::endDiag;
  return 1;
}

} // NAMESPACE ModelSupport
