/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   src/RadNodes.cxx
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
#include <cmath>
#include <complex> 
#include <vector>
#include <set>
#include <list>
#include <map> 
#include <string>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iterator>
#include <memory>
#include <array>

#include "Exception.h"
#include "MersenneTwister.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "mathSupport.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Element.h"
#include "Zaid.h"
#include "MapSupport.h"
#include "MXcards.h"
#include "Material.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Tensor.h"
#include "Transform.h"
#include "Surface.h"
#include "Rules.h"
#include "Code.h"
#include "FItem.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "Radiation.h"
#include "NodePoint.h"
#include "NodeList.h"
#include "RadNodes.h"

MTRand RadNodes::RNG(12345UL);

RadNodes::RadNodes()  : Simulation(),
  aimMat(0),pScale(1.0),
  bestScore(0),curScore(0),temp(0)
  /*!
    Constructor
  */
{}

RadNodes::RadNodes(const RadNodes& A)  : Simulation(A),
  aimMat(A.aimMat),Grid(A.Grid),ND(A.ND),
  pScale(A.pScale),bestScore(A.bestScore),
  curScore(A.curScore),temp(A.temp)
  /*!
    Copy constructor
    \param A :: RadNode to copy
  */
{}

RadNodes&
RadNodes::operator=(const RadNodes& A)  
  /*!
    Assignment operator
    \param A :: RadNode item to copy
    \return *this
   */
{
  if (this!=&A)
    {
      Simulation::operator=(A);
      aimMat=A.aimMat;
      Grid=A.Grid;
      ND=A.ND;
      pScale=A.pScale;
      bestScore=A.bestScore;
      curScore=A.curScore;
      temp=A.temp;
    }
  return *this;
}

RadNodes::~RadNodes()
  /*!
    Destructor
  */
{}

void
RadNodes::readRadiation(const std::string& Fname)
  /*!
    Reads a Radiation file 
    of type grid.total
    \param Fname :: FileName of the grid file.
   */
{
  if (Grid.readFile(Fname))
    throw ColErr::FileError(0,Fname,"RadNopes::readRadiation");

//  return Field.readFile(Fname);
  return ;
}

void
RadNodes::processNodes(const std::string& TDis,
		       const std::string& TRot) 
  /*! 
    Applies a transform to each of the nodes.
    \param TDis :: displacement vector (string form) to add
    \param TRot :: Rotation to apply 
  */
{
  ELog::RegMethod RegA("RadNodes","processNodes");

  Geometry::Transform Tl;
  int result;
  if ((result=Tl.setTransform("tr9 "+TDis,TRot)))
    {
      ELog::EM<<"Error with Node:transform"<<result<<std::endl;
      ELog::EM<<"Dis == "<<TDis<<std::endl;
      ELog::EM<<"Rot == "<<TRot<<ELog::endErr;
      return;
    }
  ND.setTransform(Tl);
  ND.processTrans();
  return;
}

void
RadNodes::scaleNodes(const double Scale)
  /*!
    Multiplies the nodes points by scale
    \param Scale :: scaling parameter
  */
{
  ND.scale(Scale);
  return;
}


void
RadNodes::setNodes(const std::vector<int>& CellN,const Geometry::Vec3D& startPt,
		   const Geometry::Vec3D& endPt,const double spc) 
  /*!
    Fills node points from Cell type CellN, starting at startPt
    and contining until endPt
    \param CellN :: list of cell IDs to erase 
    \param startPt :: Start point (low corner of cube) 
    \param endPt :: End point (top corner of cube) 
    \param spc :: Gap in x,y,z 
  */
{
  ELog::RegMethod RegA("RadNodes","setNodes");

  ND.cubeNodes(startPt,endPt,spc);
  ELog::EM<<"Number of nodes =="<<ND.size()<<ELog::endTrace;
  makeNodeType();
  ELog::EM<<"Number of nodes =="<<ND.size()<<ELog::endTrace;
  ELog::EM<<"Erased "<<ND.eraseCellID(CellN)<<ELog::endTrace;
  ELog::EM<<"Number of nodes =="<<ND.size()<<ELog::endTrace;
  return;
}


void
RadNodes::setNodes(const int cellNumber,const double) 
  /*!
    Fills node points within cell with id = cellNumber 
    \param cellNumber :: Cell ID number
    \param  :: place holder for setting node value
    \todo INCOMPLETE
  */
{
  MonteCarlo::Qhull* QH=findQhull(cellNumber);
  if (!QH)
    return;

  QH->calcVertex();  // return 0 on failure
  return;
}

void
RadNodes::readNodes(const std::string& Fname)
  /*!
    Read a NodeList Node file
    \param Fname :: File name to read
  */
{
  if (ND.readFile(Fname))
    throw ColErr::FileError(0,Fname,"RadNodes::readNodes");

  return;
}


double
RadNodes::getHeat(const Geometry::Vec3D& Pt) const
  /*!
    Prints out the Heat at a point (Test function) 
    \param Pt :: Point to get the heating
    \returns Head (Watt/cc)
  */
{
  return Grid.heat(Pt);
}

void
RadNodes::makeNodeType()
  /*!
    Loops over all the cells and get the cell
    point, set the material and cell number 
  */
{
  ELog::RegMethod RegA("RadNodes","makeNodeType");

  std::vector<NodePoint>::iterator vc;
  for(vc=ND.begin();vc!=ND.end();vc++)
    {
      // get cell ID from the current position (Use old cell id number as default start point )
      // but not necessarily in the same cell since we can morph the position
      const int cellNumber=findCellNumber(vc->getMorph(),vc->getCellName());
      // These two numbers are now the same .. need to change nodepts.
      ELog::EM<<"Cell Number == "<<vc->getMorph()<<" : "<<cellNumber<<ELog::endBasic;
      vc->setCellID(cellNumber);                   // reset cellID number and cell number
      vc->setCellName(cellNumber);                 // set the cell number 
      vc->setMatType(getCellMaterial(cellNumber));         // 
    }
  return;
}

void
RadNodes::setupMorphFit(const double Scale,const double Tp)
  /*! 
    initialise things to start a MC morph refinement
    \param Scale :: sets the pressure scale
    \param Tp :: Temperature of the fit (MaxEnt Temperature)
  */
  
{
  pScale=Scale;
  temp=Tp;
  bestScore=ND.nodeScore(aimMat);
  curScore=bestScore;
  return;
}

double 
RadNodes::NodeScore() const
  /*! 
    Calculates the Node score:: 
    number of nodes in the correct material type
    \return Score of the node
  */
{
  return ND.nodeScore(aimMat);
}

void
RadNodes::writeNodes(std::ostream& OX,const Geometry::Vec3D& Disp,
		     const Geometry::Matrix<double>& MR)
  /*! 
    Print out each node, its original and current 
    possition and the heat value 
    \param OX :: Output stream
    \param Disp :: Displacement
    \param MR :: Rotation matrix 
  */
{
  std::vector<NodePoint>::iterator vc;
  for(vc=ND.begin();vc!=ND.end();vc++)
    {
      vc->setHeat(Grid.heat(vc->getMorph()));
      Geometry::Vec3D X=vc->getMorph();
      X-=Disp;
      X.rotate(MR);
      OX<<X<<" "<<vc->getPoint()<<" "
	<<vc->getHeat()<<std::endl;
    }
  return;
}


double
RadNodes::heatSum() const
  /*!
    Sum the nodes to get the total heat
    \returns Heat (in the correct material type) [watts/cc]
  */
{
  double sum(0.0);
  std::vector<NodePoint>::const_iterator vc;
  for(vc=ND.begin();vc!=ND.end();vc++)
    {
      if (vc->getMatType()==aimMat || aimMat==0)
	sum+=Grid.heat(vc->getMorph());
    }
  return sum;
}

void
RadNodes::printNodes() 
  /*! 
     Print out each node, its original and current 
     possition and the heat value and material type.
     Note:: This is the only point that the temperature
     of the grid point is accessed.
  */
{
  std::vector<NodePoint>::iterator vc;
  int Cnt(1);
  for(vc=ND.begin();vc!=ND.end();vc++,Cnt++)
    {
      vc->setHeat(Grid.heat(vc->getMorph()));
      std::cout<<"BF,"<<Cnt<<",HGEN,"
	 <<vc->getHeat()<<std::endl;

//      std::cout<<vc->getCellName()<<" "
//	 <<vc->getNodeID()<<" "
//	 <<vc->getMatType()<<" "
//	 <<" ("<<vc->getPoint()<<") "
//	 <<" ("<<vc->getMorph()<<") "
//	 <<vc->getHeat()<<std::endl;
//
//      std::cout<<vc->getPoint()<<
//	vc->getHeat()<<std::endl;

//      std::cout<<vc->getHeat()<<std::endl;
    }
  return;
}

int
RadNodes::applyMorph(const double radius)
  /*!
    Takes a random point within a sphere
    and applies a morph operations (either 
    to compress or attract points 
    The point must not lie in the object being
    morphed.
    \param radius :: Radius of random selection
    \retval 1 :: compression
    \retval 0 :: expansion
  */
{
  // get random point
  Geometry::Vec3D Cx(2.0*radius*(0.5-RNG.rand()),
		     2.0*radius*(0.5-RNG.rand()),
		     2.0*radius*(0.5-RNG.rand()));

  while(getCellMaterial(findCellNumber(Cx,0))==aimMat)
    {
      Cx[0]=2.0*radius*(0.5-RNG.rand());
      Cx[1]=2.0*radius*(0.5-RNG.rand());
      Cx[2]=2.0*radius*(0.5-RNG.rand());
    }
  const double Scale=pScale*(RNG.rand());
  ND.applyPressure(Cx,Scale);
  makeNodeType();
  if (!acceptNewPos(NodeScore()))
    {
      ND.applyPressure(Cx,-Scale);
      return 0; 
    }
  return 1;
}

int 
RadNodes::processMorph(const int Npts)
  /*!
    Carry out an MC type refinement of the Node
    doing Npts steps
    \param Npts :: Number of steps
    \return Number of bad nodes
  */
{
  ELog::RegMethod RegA("RadNodes","processMorph");
  const double RR=50.0;
  const double pb=bestScore;
  int ngood(0);
  const int pstop(Npts/100);
  for(int i=0;i<Npts;i++)
    {
      if (!(i % pstop))
        {
	  ELog::EM<<"ngood =="<<ngood<<ELog::endDiag;
	  ELog::EM<<"Best =="<<pb<<" "<<bestScore<<" "<<curScore<<ELog::endDiag;	
	}
      if (applyMorph(RR))
        ngood++;
    }
  ELog::EM<<"Best =="<<pb<<" "<<bestScore<<" "<<curScore<<ELog::endDiag;
  return static_cast<int>(curScore);
}

int 
RadNodes::acceptNewPos(const double Sc)
  /*!
    Accept/Reject a new score
    \param Sc :: New candidate score
    \retval -1 on new best score
    \retval 1 on ok score 
    \retval 0 on no acceptance
    
  */
{
  if (Sc<curScore)
    {
      curScore=Sc;
      if (Sc<bestScore)
        {
	  bestScore=Sc;
	  return -1;
	}
      return 1;
    }
  if (temp<=0.0)
    return 0;

  if (exp(curScore-Sc)>RNG.rand())
    {
      curScore=Sc;
      return 1;
    }
  return 0;
}

std::pair<Geometry::Vec3D,Geometry::Vec3D>
RadNodes::getMaxNodeSize() const
  /*! 
     Obtain the max and min coordinates, 
     can be independent 
     \returns Pair(Min Point, Max Point)
  */

{
  std::vector<NodePoint>::const_iterator vc=ND.begin();
  if (vc==ND.end())
    return std::pair<Geometry::Vec3D,Geometry::Vec3D>(Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,0,0));

  Geometry::Vec3D maxV=vc->getMorph();
  Geometry::Vec3D lowV=vc->getMorph();

  Geometry::Vec3D px;
  for(;vc!=ND.end();vc++)
    {
      px=vc->getMorph();
      for(int i=0;i<3;i++)
        {
	  if (px[i]>maxV[i])
	    maxV[i]=px[i];
	  if (px[i]<lowV[i])
	    lowV[i]=px[i];
	}
    }
  return std::pair<Geometry::Vec3D,Geometry::Vec3D>(lowV,maxV);
}

void
RadNodes::reZeroFromVertex(const int CellN,const unsigned int M,const unsigned int A,
			   const unsigned int B,const unsigned int C,
			   Geometry::Vec3D& Dis,Geometry::Matrix<double>& MR)
  /*! 
     Given points M , A (x) , B (y) ,C (z)  
     rezero the points matrix around the centre
     \param CellN :: Cell ID number
     \param M :: Vertex point in object for zero
     \param A :: Vertex point for x-axis (A-M)
     \param B :: Vertex point for y-axis (B-M)
     \param C :: Vertex point for z-axis (C-M)
     \param Dis :: Displacement vector 
     \param MR :: Rotation matrix that is required
   */
{
  const MonteCarlo::Qhull* QHX=findQhull(CellN);
  if (!QHX) 
    return;

  MR=QHX->getRotation(M,A,B,C);
  Dis=QHX->getDisplace(M);
  return;
}

double
RadNodes::sumNodes() const
  /*!
    Integrates over all the nodes to determine 
    the total heat load in the system
    \return Total heat (Watt/uAmp)
  */
{
  std::vector<NodePoint>::const_iterator vc;
  double sum(0.0);
  for(vc=ND.begin();vc!=ND.end();vc++)
    sum+=vc->getHeat();
  return sum;
}

void
RadNodes::sumCells(const int Nstep,const int Mat)
  /*! 
     Checks the heat in each cell 
     Use a MC integration to get all the cell volume integration 
     \param Nstep :: Number of MC step to use
     \param Mat :: material number (0 == all materials)
  */
{
  ELog::RegMethod RegA("RadNodes","sumCells");

  std::map<int,double> HeatMap;
  std::map<int,int> HeatCnt;

  double lowV[3],highV[3];
  Grid.setRange(lowV,highV);
  std::map<int,double>::const_iterator hm;
  std::map<int,int>::const_iterator hc;
  double totalHeat(0);

  for(int Iter=0;Iter<Nstep;Iter++)
    {
      Geometry::Vec3D X;
      for(int i=0;i<3;i++)
	X[i]=RNG.rand()*(highV[i]-lowV[i])+lowV[i];
      
      const int cellT=findCellNumber(X,0);
      if (!Mat || getCellMaterial(cellT)==Mat)
        {
	  const int cellName=cellT;
	  const double hX=Grid.heat(X);
	  
	  HeatMap[cellName]+=hX;
	  totalHeat+=hX;
	  HeatCnt[cellName]++;
	  // debug info
	}
      if (!(Iter % 100000) && Iter!=0)
        {
	  int icnt=0;
	  double heatX=0;
	  const double Vol(Grid.Volume());
	  std::ofstream OX;
	  OX.open("CellHeat.info");
	  OX<<"Zone "<<Iter<<std::endl;
	  for(hm=HeatMap.begin(),hc=HeatCnt.begin();
	      hm!=HeatMap.end();hc++,hm++)
	    {
	      OX<<"Cell "<<hm->first<<":"
		<<Vol*hm->second/Iter<<" "
		<<(Vol*hc->second)/Iter;
	      OX<<" :: "
		<<hm->second/Iter<<" "
		<<static_cast<double>(hc->second)/Iter
		<<std::endl; 
	      icnt  += hc->second;
	      heatX += hm->second;
	    }
	  OX<<"Total Heat == "<<Vol*totalHeat/Iter<<" "<<Vol*heatX/Iter<<std::endl;
	  ELog::EM<<"Total Heat == "<<Vol*totalHeat/Iter<<" "
		  <<Vol*heatX/Iter<<ELog::endBasic;
	  OX.close();
	}
    }

  return;
}
