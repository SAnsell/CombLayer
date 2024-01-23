/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   modelProcess/Volumes.cxx
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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
#include <cmath>
#include <complex> 
#include <vector>
#include <map> 
#include <list> 
#include <set>
#include <string>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "Exception.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimMCNP.h"
#include "inputParam.h"
#include "support.h"
#include "mathSupport.h"
#include "volUnit.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "World.h"
#include "Importance.h"
#include "Object.h"
#include "dataSlice.h"
#include "multiData.h"
#include "VolSum.h"
#include "Volumes.h"

namespace ModelSupport
{

void
calcVolumes(Simulation* SimPtr,const mainSystem::inputParam& IParam)
  /*!
    Calculate the volumes for all f4 tallies
    \param SimPtr :: Simulation to use
    \param IParam :: Simulation to use
  */
{
  ELog::RegMethod RegA("Volumes[F]","calcVolumes");

  if (SimPtr && IParam.flag("volume"))
    {
      SimPtr->createObjSurfMap();
      Geometry::Vec3D Org=IParam.getValue<Geometry::Vec3D>("volume");

      const Geometry::Vec3D XYZ=IParam.getValue<Geometry::Vec3D>("volume",1);

      const size_t NP=IParam.getValue<size_t>("volNum");
      VolSum VTally(Org,XYZ);
      if (IParam.flag("volCells") )
	populateCells(*SimPtr,IParam,VTally);
      else
	{
	  const SimMCNP* SMPtr=dynamic_cast<const SimMCNP*>(SimPtr);
	  VTally.populateTally(*SMPtr);
	}

      VTally.pointRun(*SimPtr,NP);
      ELog::EM<<"Volume == "<<Org<<" : "<<XYZ<<" : "<<NP<<ELog::endDiag;
      VTally.write("volumes");
    }

  return;
}

void
readHeat(const std::string& heatFile,
	 std::vector<double>& xCoord,
	 std::vector<double>& yCoord,
	 std::vector<double>& zCoord,
	 multiData<double>& heat)
  /*!
    Given a heat file (node : x,y,z : value) : CombLayer values
    calclulate the heat in the coordinates 
   */
{
  ELog::RegMethod RegA("Volumes[F]","readHeat");

  if (heatFile.empty())
    throw ColErr::EmptyValue<std::string>("Heat fileName");

  std::ifstream IX(heatFile.c_str());
  xCoord.clear();
  yCoord.clear();
  zCoord.clear();
  std::vector<double> values;
  int oldX(-1),oldY(-1),oldZ(-1);

  double integral(0.0);
  while(IX.good())
    {
      int a,b,c;
      double x,y,z,val;
      std::string line = StrFunc::getLine(IX,512);

      if (StrFunc::section(line,a) &&
	  StrFunc::section(line,b) &&
	  StrFunc::section(line,c) &&
	  StrFunc::section(line,x) &&
	  StrFunc::section(line,y) &&
	  StrFunc::section(line,z) &&
	  StrFunc::section(line,val))
	{
	  if (oldX<a)
	    {
	      xCoord.push_back(x);
	      oldX=a;
	    }
	  if (oldY<b)
	    {
	      yCoord.push_back(y);
	      oldY=b;
	    }
	  if (oldZ<c)
	    {
	      zCoord.push_back(z);
	      oldZ=c;
	    }
	  val*=1000000.0;   // m^3 from cm^3
	  val*=5.03681e+18;    // GeV per second
	  val*=1.6021766e-10;  // GeV to Joules
	  values.push_back(val);
	  integral+=val;
	}
    }
  ELog::EM<<"XYZ size= "<<xCoord.size()<<" "<<yCoord.size()<<" "
	  <<zCoord.size()<<" "<<
	  zCoord.size()*yCoord.size()*zCoord.size()
	  <<ELog::endDiag;
  ELog::EM<<"Values size= "<<values.size()<<ELog::endDiag;

  // pixels are 0.1*0.1*0.1 cm there divide by 
  ELog::EM<<"Integral = "<<integral/1e9<<ELog::endDiag;
  heat.setData(xCoord.size(),yCoord.size(),zCoord.size(),
	       values);
  return;
}

void
readPts(const std::string& pointName,
	const Geometry::Vec3D& centre,
	const Geometry::Vec3D& X,
	const Geometry::Vec3D& Y,
	const Geometry::Vec3D& Z,
	std::vector<int>& OrgIndex,
	std::vector<Geometry::Vec3D>& OrgPts,
	std::vector<Geometry::Vec3D>& Pts)
  /*!
    Given a mesh file convert to main system
   */
{
  ELog::RegMethod RegA("Volumes[F]","readPts");
  
  if (pointName.empty())
    throw ColErr::EmptyValue<std::string>("Point Name");

  std::ifstream IX(pointName.c_str());
  
  while(IX.good())
    {
      int index;
      double x,y,z;
      std::string line = StrFunc::getLine(IX,512);
      if (StrFunc::section(line,index) &&
	  StrFunc::section(line,x) &&
	  StrFunc::section(line,y) &&
	  StrFunc::section(line,z))
	{
	  OrgIndex.push_back(index);
	  OrgPts.push_back(Geometry::Vec3D(x,y,z));
	  Geometry::Vec3D testPt(x,z,y);
	  testPt*=100.0;
	  testPt=testPt.getInBasis(X,Y,Z);
	  testPt+=centre;
	  Pts.push_back(testPt);
	}
    }
  IX.close();
  ELog::EM<<"XYZ Axis == "<<X<<ELog::endDiag;
  ELog::EM<<"XYZ Axis == "<<Y<<ELog::endDiag;
  ELog::EM<<"XYZ Axis == "<<Z<<ELog::endDiag;

  if (Pts.empty())
    throw ColErr::FileError(0,pointName,"Point file empty");
  return;
}
  
void 
materialHeat(const Simulation& System,
	     const mainSystem::inputParam& IParam)

 /*!
    Generate heat from heat file and for plot file
    \param centPoint :: Origin 
    \param xAxis :: full extent from left to right
    \param yAxis :: full extent from base to top
    \param zAxis :: volume acceptable
  */
{
  ELog::RegMethod RegA("Volumes[F]","generateHeat");

if (IParam.flag("materialHeat"))
    {
      const std::string ptsName=IParam.getValue<std::string>("materialHeat",0);
      const std::string heatName=IParam.getValue<std::string>("materialHeat",1);
      const std::string objName=
	IParam.getDefValue<std::string>("","materialHeat",2);
      const std::string linkName=
	IParam.getDefValue<std::string>("Origin","materialHeat",3);
      const Geometry::Vec3D linkOffset=
	IParam.getDefValue<Geometry::Vec3D>(Geometry::Vec3D(0,0,0),
					    "materialHeat",4);

      const attachSystem::FixedComp& FC=
	(objName.empty()) ?  World::masterOrigin() :
	*(System.getObjectThrow<attachSystem::FixedComp>
	  (objName,"Object not found"));
      
      const long int linkIndex=(linkName.empty()) ?  0 :
	FC.getSideIndex(linkName);

      const Geometry::Vec3D centre=FC.getLinkPt(linkIndex);
      Geometry::Vec3D X;
      Geometry::Vec3D Y;
      Geometry::Vec3D Z;
      FC.calcLinkAxis(linkIndex,X,Y,Z);

      std::vector<double> xCoord;
      std::vector<double> yCoord;
      std::vector<double> zCoord;
      multiData<double> heat;
      readHeat(heatName,xCoord,yCoord,zCoord,heat);

      std::vector<int> OrgIndex;
      std::vector<Geometry::Vec3D> OrgPts;
      std::vector<Geometry::Vec3D> Pts;
      readPts(ptsName,centre,X,Y,Z,OrgIndex,OrgPts,Pts);
      ELog::EM<<"Centere == "<<centre<<ELog::endDiag;
      std::ofstream OX("heatOut.txt");
      
      std::map<size_t,size_t> usageMap;
      double integral(0.0);
      multiData<int> useMesh(heat.shape());
      for(size_t i=0;i<Pts.size();i++)
	{
	  const Geometry::Vec3D& OrgPt(OrgPts[i]);
	  const Geometry::Vec3D& Pt(Pts[i]);
	  const double x=Pt[0];
	  const double y=Pt[1];
	  const double z=Pt[2];
	  const size_t ii=rangePos(xCoord,x);
	  const size_t jj=rangePos(yCoord,y);
	  const size_t kk=rangePos(zCoord,z);
	  //	  const size_t index=ii*1000000+jj*1000+kk;
	  
	  const std::vector<size_t>& shape=heat.shape();
	  // ELog::EM<<"CXY ="<<xCoord.size()<<" "
	  // 	  <<yCoord.size()<<" "<<zCoord.size()<<ELog::endDiag;
	  // ELog::EM<<"Shape ="<<shape[0]<<" "
	  // 	  <<shape[1]<<" "<<shape[2]<<ELog::endDiag;
	  double vMax(0.0);

	  size_t maxIndex(0);
	  const size_t aMinus((ii>10) ? 10 : ii);
	  const size_t bMinus((jj>2) ? 2 : jj);
	  const size_t cMinus((kk>2) ? 2 : kk);
	  for(size_t alpha=ii-aMinus;ii<ii+2 && alpha<shape[0];alpha++)
	    for(size_t beta=jj-bMinus;beta<jj+2 && beta<shape[1];beta++)
	      for(size_t gamma=kk-cMinus;gamma<kk+2 && gamma<shape[2];gamma++)
		{
		  const size_t index=ii*1000000+jj*1000+kk;
		  const double value=heat.get()[alpha][beta][gamma];
		  if (value>1e-4)
		    usageMap[index]++;
		  if (value>vMax)
		    {
		      vMax=value;
		      maxIndex=index;
		    }
		}
	  
	  // vMax=heat.get()[alpha][beta][gamma];
	  if (vMax>1e-12)
	    {
	      //	      usageMap[maxIndex]++;
	      if (usageMap[maxIndex]==1)
		{
		  integral+=vMax;
		}
	    }
	  
	  OX<<OrgIndex[i]<<" "
	    <<OrgPt[0]<<" "<<OrgPt[1]<<" "<<OrgPt[2]<<" "
	    <<vMax;
	    
	  OX<<std::endl;
	}
      OX.close();

      ELog::EM<<"Size == "<<heat.size()<<" "
	      <<usageMap.size()<<" "<<integral/1e9<<ELog::endDiag;

      double integralUsed(0.0);
      double integralMissing(0.0);
      double maxV(0.0);
      for(size_t i=0;i<xCoord.size();i++)
	for(size_t j=0;j<yCoord.size();j++)
	  for(size_t k=0;k<zCoord.size();k++)
	    {
	      const double value=heat.get()[i][j][k];
	      const size_t index=i*1000000+j*1000+k;
	      
	      if (usageMap.find(index)==usageMap.end())
		{
		  if (value>maxV && value>1e10)
		    {
		      maxV=value;

		      Geometry::Vec3D testX(xCoord[i],
					    yCoord[j],
					    zCoord[k]);
		      testX-=centre;
		      testX/=100.0;
		      Geometry::Vec3D testPt(-testX[0],testX[2],testX[1]);
		      
		      //			  integralUsed+=value;
		      ELog::EM<<"Point "<<i<<" "<<j<<" "<<k<<" : "
			      <<xCoord[i]<<" "
			      <<yCoord[j]<<" "
			      <<zCoord[k]<<" "
			      <<value<<":::"<<testPt<<ELog::endDiag;
		    }
		  integralMissing+=value;
		}
	      else
		integralUsed+=value;
	    }

      ELog::EM<<"Integral[Missing] == "<<integralMissing/1e9<<ELog::endDiag;
      ELog::EM<<"Integral[Good]     == "<<integralUsed/1e9<<ELog::endDiag;

    }

 
  return;
}

void 
generatePlot(const std::string& fName,
	     const Geometry::Vec3D& centPoint,
	     const Geometry::Vec3D& xAxis,
	     const Geometry::Vec3D& yAxis,
	     const Geometry::Vec3D& zAxis,
	     const size_t nX,
	     const size_t nY,
	     const std::vector<Geometry::Vec3D>& PtsA,
	     const std::vector<Geometry::Vec3D>& PtsB)
 /*!
    Generate a simple plane plot from a mesh of data points
    \param centPoint :: Origin 
    \param xAxis :: full extent from left to right
    \param yAxis :: full extent from base to top
    \param zAxis :: volume acceptable
    \param nX :: Number of x points
    \param nY :: Number of y points
    \param Pts :: coordinates to plot
  */
{
  ELog::RegMethod RegA("Volumes[F]","generatePlot");

  std::ofstream OX(fName.c_str());
  
  const double xLen=xAxis.abs();
  const double yLen=yAxis.abs();
  const double zLen=zAxis.abs();

  ELog::EM<<"X == "<<xAxis<<ELog::endDiag;
  ELog::EM<<"Y == "<<yAxis<<ELog::endDiag;
  ELog::EM<<"Z == "<<zAxis<<ELog::endDiag;
  int cnt(0);
  multiData<int> AreaMap(nX,nY);
  for(Geometry::Vec3D Pt : PtsA)
    {
      Pt-=centPoint;
      const double LX=xAxis.dotProd(Pt)/xLen;
      const double LY=yAxis.dotProd(Pt)/yLen;
      const double LZ=zAxis.dotProd(Pt)/zLen;
      if (LX>=-xLen/2.0 && LX<=xLen/2.0 &&
	  LY>=-yLen/2.0 && LY<=yLen/2.0 &&
	  LZ>=-zLen/2.0 && LZ<=zLen/2.0)
	{
	  const size_t iX=static_cast<size_t>(nX*(LX/xLen+0.5));
	  const size_t iY=static_cast<size_t>(nY*(LY/yLen+0.5));
	  AreaMap.get()[iX][iY]=10.0;
	  cnt++;
	  if (!(cnt % 5000))
	    ELog::EM<<"A Point == "<<Pt+centPoint<<ELog::endDiag;
	}
    }
  for(Geometry::Vec3D Pt : PtsB)
    {
      Pt-=centPoint;
      const double LX=xAxis.dotProd(Pt)/xLen;
      const double LY=yAxis.dotProd(Pt)/yLen;
      const double LZ=zAxis.dotProd(Pt)/zLen;
      if (LX>=-xLen/2.0 && LX<=xLen/2.0 &&
	  LY>=-yLen/2.0 && LY<=yLen/2.0 &&
	  LZ>=-zLen/2.0 && LZ<=zLen/2.0)
	{
	  const size_t iX=static_cast<size_t>(nX*(LX/xLen+0.5));
	  const size_t iY=static_cast<size_t>(nY*(LY/yLen+0.5));
	  AreaMap.get()[iX][iY]=5.0;
	  cnt++;
	  if (!(cnt % 5000))
	    ELog::EM<<"B Point == "<<Pt+centPoint<<" :"<<Pt
		    <<":"<<LX<<" "<<LZ<<" == "<<zLen/2.0<<ELog::endDiag;
	}
    }
  
  const double xStep(xLen/static_cast<double>(nX));
  const double yStep(yLen/static_cast<double>(nY));
  for(size_t i=0;i<nX;i++)
    for(size_t j=0;j<nY;j++)
      {
	const double xV=xStep*static_cast<double>(i);
	const double yV=yStep*static_cast<double>(j);
	OX<<xV<<" "<<yV<<" "<<AreaMap.get()[i][j]<<std::endl;
      }
  OX.close();
  return;
}
  
void
materialCheck(const Simulation& System,
	      const mainSystem::inputParam& IParam)
  /*!
    Check an external file for material values in the model
    \param SimPtr :: Simulation to use
    \param IParam :: Simulation to use
  */
{
  ELog::RegMethod RegA("Volumes[F]","materialCheck");

  if (IParam.flag("materialCheck"))
    {
      const std::string fileName=IParam.getValue<std::string>("materialCheck");
      const std::string objName=
	IParam.getDefValue<std::string>("","materialCheck",1);
      const std::string linkName=
	IParam.getDefValue<std::string>("Origin","materialCheck",2);
      const Geometry::Vec3D linkOffset=
	IParam.getDefValue<Geometry::Vec3D>(Geometry::Vec3D(0,0,0),
					    "materialCheck",3);

      const attachSystem::FixedComp& FC=
	(objName.empty()) ?  World::masterOrigin() :
	*(System.getObjectThrow<attachSystem::FixedComp>
	  (objName,"Object not found"));
      
      const long int linkIndex=(linkName.empty()) ?  0 :
	FC.getSideIndex(linkName);

      const Geometry::Vec3D centre=FC.getLinkPt(linkIndex);
      Geometry::Vec3D X;
      Geometry::Vec3D Y;
      Geometry::Vec3D Z;
      FC.calcLinkAxis(linkIndex,X,Y,Z);

      std::vector<Geometry::Vec3D> PtsGood;
      std::vector<Geometry::Vec3D> PtsBad;
      MonteCarlo::Object* OPtr(nullptr);
      std::ifstream IX;
      IX.open(fileName.c_str());

      std::map<int,int> activeMap;
      while(IX.good())
	{
	  int index;
	  double x,y,z;
	  std::string line = StrFunc::getLine(IX,512);
	  if (StrFunc::section(line,index) &&
	      StrFunc::section(line,x) &&
	      StrFunc::section(line,y) &&
	      StrFunc::section(line,z))
	    {
	      Geometry::Vec3D testPt(z,x,y);
	      testPt*=100.0;
	      testPt-=linkOffset;
	      testPt=testPt.getInBasis(X,Y,Z);
	      testPt+=centre;
	      OPtr=System.findCell(testPt,OPtr);
	      const int matID=OPtr->getMatID();
	      activeMap[matID]++;
	      if (!matID)
		PtsBad.push_back(testPt);
	      else
		PtsGood.push_back(testPt);
	    }
	}
      IX.close();
      ELog::EM<<"PTS == "<<PtsBad[0]<<ELog::endDiag;
      ELog::EM<<"Y == "<<Y<<ELog::endDiag;
      ELog::EM<<"Z == "<<Z<<ELog::endDiag;
      for(const auto& [i,n] : activeMap)
	ELog::EM<<"Mat ["<<i<<"] == "<<n<<ELog::endDiag;
      
      generatePlot("testA.out",centre,X*20.0,Z*20.0,Y*1.0,400,400,PtsGood,PtsBad);
      //      generatePlot("testB.out",centre,X*15.0,Z*15.0,Y,100,100,PtsBad);
    }
  return;
}

void
populateCells(const Simulation& System,
	      const mainSystem::inputParam& IParam,
	      VolSum& VTally)
  /*!
    Populate the Volume Tallys with cells defined of the input
    stream.
    \param System :: Simulation 
    \param IParam :: Input stream
    \param VTally :: Tally to populate
   */
{
  ELog::RegMethod RegA("Volumes[F]","populateCells");


  for(size_t i=0;i<IParam.setCnt("volCells");i++)
    {
      const size_t NItems=IParam.itemCnt("volCells",i);
      std::vector<std::string> CStr;
      for(size_t j=0;j<NItems;j++)
	CStr.push_back(IParam.getValue<std::string>("volCells",i,j));
      
      if (NItems && (CStr[0]=="all" || CStr[0]=="All"))
	VTally.populateAll(System);
      else if (NItems>=2 && (CStr[0]=="object" || CStr[0]=="Object"))
	{
	  std::vector<int> CNumbers;
	  for(size_t j=1;j<NItems;j++)
	    {
	      const std::set<int> CNumPlus=
		System.getObjectRange(CStr[j]);
	      if (CNumPlus.empty())
		throw ColErr::InContainerError<std::string>
		  (CStr[j]," Cell object not known");

	      ELog::EM<<"CNum == "<<CNumPlus.size()<<ELog::endDiag;
	      CNumbers.insert(CNumbers.end(),CNumPlus.begin(),CNumPlus.end());
	    }
	  ELog::EM<<"Cell size = "<<CNumbers.size()<<ELog::endDiag;
	  VTally.populateVSet(System,CNumbers);
	}
    }
  return;
}
  


} // NAMESPACE ModelSupport
