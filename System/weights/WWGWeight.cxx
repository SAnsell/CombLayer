/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weight/WWGWeight.cxx
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <cmath>
#include <complex> 
#include <vector>
#include <list>
#include <set>
#include <map> 
#include <string>
#include <algorithm>
#include <memory>
#include <boost/multi_array.hpp>
#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "mathSupport.h"
#include "Vec3D.h"
#include "BasicMesh3D.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "support.h"
#include "writeSupport.h"
#include "BaseMap.h"
#include "LineTrack.h"
#include "ObjectTrackAct.h"
#include "ObjectTrackPoint.h"
#include "ObjectTrackPlane.h"
#include "phitsWriteSupport.h"
#include "WWGWeight.h"

namespace WeightSystem
{

const double minLOG(-100.0);
  
WWGWeight::WWGWeight(const size_t index) :
  ID(index),
  WE(1),WX(0),WY(0),WZ(0),
  EBin({0.0,1e3})
  /*! 
    Constructor 
    \param index :: ID value [0 for non-writing]
  */
{}


WWGWeight::WWGWeight(const size_t index,
		       const WWGWeight& A) : 
  ID(index),
  WE(A.WE),WX(A.WX),WY(A.WY),WZ(A.WZ),
  particles(A.particles),EBin(A.EBin),Grid(A.Grid),
  WGrid(A.WGrid)
  /*!
    Copy constructor with pre-set index
    \param index :: new ID number
    \param A :: WWGWeight to copy
  */
{}

WWGWeight::WWGWeight(const WWGWeight& A) : 
  ID(A.ID),
  WE(A.WE),WX(A.WX),WY(A.WY),WZ(A.WZ),
  particles(A.particles),EBin(A.EBin),Grid(A.Grid),
  WGrid(A.WGrid)
  /*!
    Copy constructor
    \param A :: WWGWeight to copy
  */
{}

WWGWeight&
WWGWeight::operator=(const WWGWeight& A)
  /*!
    Assignment operator
    \param A :: WWGWeight to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ID=A.ID;
      WE=A.WE;
      WX=A.WX;
      WY=A.WY;
      WZ=A.WZ;
      particles=A.particles;
      EBin=A.EBin;
      Grid=A.Grid;
      if (WE*WX*WY*WZ>0)
	{
	  WGrid.resize(boost::extents[WE][WX][WY][WZ]);
	  WGrid=A.WGrid;
	}
    }
  return *this;
}

void
WWGWeight::setEnergy(const std::vector<double>& EVal)
  /*!
    Set the energy table
    \param EVal :: list of values
   */
{
  EBin.clear();
  for(const double E : EVal)
    {
      if (E>0.0 &&
	  (EBin.empty() || E-1e-6 > EBin.back()))   // meV set min:
	EBin.push_back(E);
    }
  if (EBin.size()<2)
    EBin=std::vector<double>{0.0,1e3};
      
  WE=static_cast<long int>(EBin.size())-1;
  resize();
  return;
}

void
WWGWeight::setMesh(const Geometry::Vec3D& lowPt,
		   const Geometry::Vec3D& highPt,
		   const long int LX,
		   const long int LY,
		   const long int LZ)
  /*!
    Resize the mesh value
    \param lowPt :: Low corner
    \param highPt :: hgih corner
    \param LX :: X coorindate size
    \param LY :: Y coorindate size
    \param LZ :: Z coorindate size
   */
{

  if (LX>0 && LY>0 && LZ>0)
    {
      std::array<size_t,3> Nxyz{static_cast<size_t>(LX),
				static_cast<size_t>(LY),
				static_cast<size_t>(LZ)};
      Grid.setMesh(lowPt,highPt,Nxyz);
      WX=LX;
      WY=LY;
      WZ=LZ;
      resize();
    }

  return;
}

void
WWGWeight::setMesh(const Geometry::Vec3D& lowPt,
		   const Geometry::Vec3D& highPt,
		   const std::array<size_t,3>& Nxyz)
  /*!
    Resize the mesh value
    \param lowPt :: Low corner
    \param highPt :: hgih corner
   */
{
  if ((Nxyz[0]*Nxyz[1]*Nxyz[2])!=0)
    {
      WX=static_cast<long int>(Nxyz[0]);
      WY=static_cast<long int>(Nxyz[1]);
      WZ=static_cast<long int>(Nxyz[2]);
      Grid.setMesh(lowPt,highPt,Nxyz);
      resize();
    }
  return;
}

void
WWGWeight::setMesh(const Geometry::BasicMesh3D& aGrid)
  /*!
    Set the mesh values
   */
{
  Grid=aGrid;
  WX=static_cast<long int>(aGrid.getXSize());
  WY=static_cast<long int>(aGrid.getYSize());
  WZ=static_cast<long int>(aGrid.getZSize());
  resize();

  return;
}
  
void
WWGWeight::resize()
  /*!
    Internal function to resize the main grid
   */
{
  if (WE*WX*WY*WZ>0)
    {
      WGrid.resize(boost::extents[WE][WX][WY][WZ]);
      zeroWGrid();
    }
  return;
}
  
void
WWGWeight::setDefault(const std::vector<double>& defVal)
  /*!
    Set default values
    \param defVal :: vector of main values
  */
{
  if (defVal.size()==static_cast<size_t>(WE))
    {
      for(long int e=0;e<WE;e++)
	{
	  double V=log(defVal[static_cast<size_t>(e)]);
	  if (V>0.0) V=0.0;
	  for(long int i=0;i<WX;i++)
	    for(long int j=0;j<WY;j++)
	      for(long int k=0;k<WZ;k++)
		{
		  WGrid[e][i][j][k]=V;
		}
	}
    }
  return;
}

bool
WWGWeight::isSized(const long int LX,const long int LY,
		   const long int LZ,const long int LE) const
  /*!
    Check the mesh value
    \param LX :: X coorindate size
    \param LY :: Y coorindate size
    \param LZ :: Z coorindate size
    \param LE :: Energy coorindate size
    \return true if properly sized
   */
{
  return ((LX==WX) && (LY==WY) && (LZ==WZ) && (LE==WE));
}

	
void
WWGWeight::zeroWGrid()
  /*!
    Zero WGrid and set the zeroflag 
  */
{
  for(long int e=0;e<WE;e++)
    for(long int i=0;i<WX;i++)
      for(long int  j=0;j<WY;j++)
	for(long int k=0;k<WZ;k++)
	  {
	    WGrid[e][i][j][k]=1.0;
          }
  
  return;
}

void
WWGWeight::scaleGrid(const double scaleFactor)
  /*!
    Multiply the grid by a scale factor 
    \param scaleFactor :: Scale value [NOT LOG]
  */
{
  ELog::RegMethod RegA("WWGWeight","scaleGrid");
  
  if (scaleFactor<minLOG || scaleFactor>-minLOG)
    throw ColErr::NumericalAbort
      ("ScaleFactor[log] too low/high:"+std::to_string(scaleFactor));

  double* TData=WGrid.data();
  const size_t NData=WGrid.num_elements();
  for(size_t i=0;i<NData;i++)
    {
      TData[i]+=scaleFactor;
      if (TData[i]>0.0) TData[i]=0.0;
      if (TData[i]<minLOG) TData[i]=minLOG;
    }
  return;
}

void
WWGWeight::scalePower(const double powerFactor)
  /*!
    Scale the grid by W^Power
    \param powerFactor :: Scale value [NOT LOG]  
  */
{
  ELog::RegMethod RegA("WWGWeight","scalePower");
  
  double* TData=WGrid.data();
  const size_t NData=WGrid.num_elements();
  for(size_t i=0;i<NData;i++)
    {
      TData[i]+=powerFactor;
      if (TData[i]>0.0) TData[i]=0.0;
      if (TData[i]>minLOG) TData[i]=minLOG;
    }
  
  return;
}
  
void
WWGWeight::addLogPoint(const long int index,
		       const long int IE,
		       const double V)
/*!
    Scale a point assuming x,y,z indexing and z fastest point
    \param index :: index for linearization
    \param IE :: Energy bin
    \param V :: value to set [LOG VALUE]
  */
{
  const long int IX=index/(WY*WZ);
  const long int IY=(index/WZ) % WY;
  const long int IZ=index % WZ;

  if (IX>=WX)
    throw ColErr::IndexError<long int>
      (index,WX,"WWGWeight::addLogPoint::Index out of range");

  if (IE>=WE)
    throw ColErr::IndexError<long int>
      (IE,WE,"WWGWeight::addLogPoint::eIndex out of range");

  const double VV= (WGrid[IE][IX][IY][IZ]>0.5) ?
    V :  mathFunc::logAdd(WGrid[IE][IX][IY][IZ],V);
  WGrid[IE][IX][IY][IZ]=(VV>0.0) ? 0.0 : VV;
  return;
}

double
WWGWeight::calcMaxAttn(const long int eIndex) const
  /*!
    Calculate the adjustment factor to get to the 
    max weight correction
    \param eIndex :: Index
    \return factor for exponent (not yet taken exp)
  */
{
  ELog::RegMethod RegA("WWGWeight","calcMaxAttn");
  
  double maxW(minLOG);
  if (WE<=eIndex)
    throw ColErr::IndexError<long int>(eIndex,WE,"WE/eIndex");
  
  for(long int i=0;i<WX;i++)
    for(long int  j=0;j<WY;j++)
      for(long int k=0;k<WZ;k++)
	{
	  if (WGrid[eIndex][i][j][k]>maxW)
	    maxW=WGrid[eIndex][i][j][k];
	}

  return maxW;
}

double
WWGWeight::calcMaxAttn() const
  /*!
    Calculate the adjustment factor to get to the 
    max weight correction
    \return factor for exponent (not yet taken exp)
  */
{
  double maxW(-100.0);
  for(long int eIndex=0;eIndex<WE;eIndex++)
    {
      const double W=calcMaxAttn(eIndex);
      if (W>maxW) maxW=W;
    }
  return maxW;
}

void
WWGWeight::setMinValue(const double MValue)
  /*!
    Set the minValue and the max value is trapped at 0 [ 1: in real]
    \param MValue :: Lowest acceptable attenuation fraction [-ve only]
  */
{
  ELog::RegMethod RegA("WWGWeight","setMinValue");
  
  if (MValue>0.0 || MValue<-100.0)
    throw ColErr::RangeError<double>(MValue,-100.0,0.0,"MValue > exp(0)");
  
  double* TData=WGrid.data();
  const size_t NData=WGrid.num_elements();

  for(size_t i=0;i<NData;i++)
    {
       if (TData[i] < MValue)
	 TData[i]= MValue;
       else if (TData[i]>0.0)
	 TData[i]=0.0;
    }
  return;
}

void
WWGWeight::scaleMeshItem(const long int I,const long int J,
			 const long int K,const long int EI,
			 const double AValue)
  /*!
    Scale a given mesh index [based on second index]
    \param I :: index for i,j,k
    \param J :: index for i,j,k
    \param K :: index for i,j,k
    \param EI :: energy bin
    \param AValue :: scaling vector for energy bins
  */

{
  ELog::RegMethod RegA("WWGWeight","scaleMeshItem");

  if (I>=WX || J>=WY || K>=WZ || EI>=WE ||
      I<0 || J<0 || K<0 || EI<0)
    throw ColErr::DimensionError<4,long int>
		      ({I,J,K,EI},{WX,WY,WZ,WE},"Index values");
  

  if (AValue<-100)
    throw ColErr::NumericalAbort("AValue to low"+std::to_string(AValue));

  
  const double V = (WGrid[EI][I][J][K]>0.0) ?  AValue :
    mathFunc::logAdd(WGrid[EI][I][J][K],AValue);
  
  if (V>0.0)
    WGrid[EI][I][J][K]=0.0;
  else if (V<minLOG)
    WGrid[EI][I][J][K]=minLOG;
  else
    WGrid[EI][I][J][K]=V;

  return;
}

void
WWGWeight::scaleRange(const size_t eIndex,
		      double AValue,double BValue,
		      const double fullRange,
		      const double weightMax)
  /*!
    Scale the mesh so all values fall in a range given by fullRange
    and groin from weightMax-fullRange : weightMax.
    \param eIndex :: Offset energy Index [offset by +1] / 0 for full range
    \param AValue :: [log] Min Value [assumed to be fullRange] (+ve use min)
    \param BValue :: [log] Max value [above assumed to be 1.0] (+ve to use max)
    \param fullRange :: Range of data output [+ve]
    \param weightMax :: Maximum value (so we get weightMax-fullRange : weightMax
  */
{
  ELog::RegMethod RegA("WWGWeight","scaleRange(full)");

  if (fullRange<1.0 || fullRange>100.0)
    throw ColErr::RangeError<double>(fullRange,1.0,100.0,"fullRange");

  if (weightMax<-100.0 || weightMax>0.0)
    throw ColErr::RangeError<double>(weightMax,-100.0,0.0,"weightMax");

  double* TData=WGrid.data();
  size_t NData=WGrid.num_elements();

  if (eIndex>0)
    {
      if (static_cast<long int>(eIndex)>WE)
	throw ColErr::IndexError<size_t>
	       (static_cast<long int>(eIndex),WE,"Energy out of range");
      NData/=static_cast<size_t>(WE);
      TData+= NData*(eIndex-1);
    }
      
  if (AValue > 0)
    AValue= *std::min_element(TData,TData+NData-1);
  
  if (BValue > 0)
    BValue= *std::max_element(TData,TData+NData-1);
  

  if (AValue>BValue) std::swap(AValue,BValue);
  
  double maxValue(-100);
  double minValue(100);
  if (AValue<BValue)
    {
      const double ABRange(BValue-AValue);

      for(size_t i=0;i<NData;i++)
	{
	  if (TData[i]<=AValue)
	    TData[i] = weightMax-fullRange;
	  else if (TData[i]>=BValue)
	    TData[i] = weightMax;
	  else
	    {
	      if (TData[i]>maxValue)
		maxValue=TData[i];
	      
	      if (TData[i]<minValue)
		minValue=TData[i];
	      // F is +ve
	      const double F=(TData[i]-AValue)/ABRange;
	      TData[i]= weightMax-(1.0-F)*fullRange;
	    }
	}
    }
  
  ELog::EM<<"Full range == "<<weightMax-fullRange<<":"<<weightMax
	  <<" Using basis:"<<AValue<<" "<<BValue
	  <<"\n"
	  <<"Scaled from ["<<minValue<<"]"<<exp(minValue)<<" ["
	  <<maxValue<<"] "<<exp(maxValue)<<ELog::endDiag;
  
  return;
}


template<typename T>
void
WWGWeight::wTrack(const Simulation& System,
		  const T& initPt,
		  const size_t eIndex,
		  const double densityFactor,
		  const double r2Length,
		  const double r2Power)
  /*!
    Calculate a specific track from sourcePoint to position
    \param System :: Simulation to use    
    \param initPt :: Point for outgoing track
    \param densityFactor :: Scaling factor for density
    \param r2Length :: scale factor for length
    \param r2Power :: power of 1/r^2 factor
  */
{
  ELog::RegMethod RegA("WWGWeight","wTrack");

  long int cN(0);

  //ELog::EM<<"Processing  "<<MidPt.size()<<" for WWG"<<ELog::endDiag;

  //  const long int NCut(WE*WX*WY*WZ/5);
  for(long int i=0;i<WX;i++)
    for(long int j=0;j<WY;j++)
      for(long int k=0;k<WZ;k++)
	{
	  const Geometry::Vec3D Pt=Grid.point(i,j,k);
	  for(long int index=0;index<WE;index++)
	    {
	      if (!eIndex || static_cast<size_t>(index+1)==eIndex)
		{
		  const size_t sIndex(static_cast<size_t>(index));
		  const double EVal=(EBin[sIndex]+EBin[sIndex+1])/2.0;
		  const double DT=
		    distTrack(System,initPt,EVal,Pt,
			      densityFactor,r2Length,r2Power);
		  // energy
		  addLogPoint(cN,index,DT);
		}
	    }
	  cN++;
	}
  return;
}


template<typename T>
double
WWGWeight::distTrack(const Simulation& System,
		     const T& aimPt,
		     const double E,
		     const Geometry::Vec3D& gridPt,
		     const double densityFactor,
		     const double r2Length,
		     const double r2Power) const
  /*!
    Calculate a specific track from sourcePoint to position
    \param System :: Simulation to use    
    \param aimPt :: Point for outgoing track
    \paramn E :: Central energy bin [MeV]
    \param gridPt :: Grid points
    \param densityFactor :: Scaling factor for density
    \param r2Length :: scale factor for length
    \param r2Power :: power of 1/r^2 factor
  */
{
  ELog::RegMethod RegA("WWGWeight","distTrack");

  typedef typename std::conditional<
    std::is_same<T,Geometry::Plane>::value,
    ModelSupport::ObjectTrackPlane,
    ModelSupport::ObjectTrackPoint>::type TrackType;
    
  TrackType OTrack(aimPt);
  
  OTrack.addUnit(System,1,gridPt);
  double DistT=OTrack.getDistance(1)*r2Length;
  if (DistT<1.0) DistT=1.0;
  // returns density * Dist * AtomicMass^0.66
  const double AT=OTrack.getAttnSum(1,E);
  return -densityFactor*AT-r2Power*log(DistT);
}


template<typename T,typename U>
void
WWGWeight::CADISnorm(const Simulation& System,
		     const long int mIndex,
		     const WWGWeight& Source,
		     const long int sIndex,
		     const WWGWeight& Adjoint,
		     const long int aIndex,
		     const T& sourcePt,
                     const U& tallyPt,
		     const double densityFactor,
		     const double r2Length,
		     const double r2Power)
  /*!
    Normalize this relative to an adjoint based on 
    the cadis formalism [M. Munk et al : Nucl Sci Eng (2017)]
    Sets the sourceFlux to to R/adjoint[i] flux
    \param System :: Simulation for tracking
    \param mIndex :: energy index for this
    \param Source :: Sourrce WWGWeight
    \param sIndex :: energy index for source
    \param Adjoint :: Adjoint WWGWeight
    \param aIndex :: energy index for adjoint
    \param sourcePt :: Source point [point/plane]
    \param tallyPt :: tally point   [point/plane]
  */
{
  ELog::RegMethod RegA("WWGWeight","CADISnorm");

  if (Source.WE!=Adjoint.WE ||
      Source.WX!=Adjoint.WX ||
      Source.WY!=Adjoint.WY ||
      Source.WZ!=Adjoint.WZ)
    throw ColErr::DimensionError<4,long int>
      ({Source.WX,Source.WY,Source.WZ,Source.WE},
       {Adjoint.WX,Adjoint.WY,Adjoint.WZ,Adjoint.WE},
       "Index values");
  
  if (Source.WE!=WE || Source.WX!=WX ||
      Source.WY!=WY || Source.WZ!=WZ)
    {
      Grid=Source.Grid;
      WE=Source.WE;
      WX=Source.WX;
      WY=Source.WY;
      WZ=Source.WZ;
      WGrid.resize(boost::extents[WE][WX][WY][WZ]);      
    }

  if (mIndex<0 || mIndex>WE)
    throw ColErr::IndexError<long int>(WE,mIndex,"Mesh E-Index");
  if (sIndex<0 || sIndex>WE)
    throw ColErr::IndexError<long int>(WE,sIndex,"Source E-Index");
  if (aIndex<0 || aIndex>WE)
    throw ColErr::IndexError<long int>(WE,aIndex,"Adjoint E-Index");
  

  
  double sumR=minLOG;
  double sumRA=minLOG;
  const double EVal=(EBin[mIndex]+EBin[mIndex])/2.0;  
  // STILL in log space
  for(long int i=0;i<WX;i++)
    for(long int j=0;j<WY;j++)
      for(long int k=0;k<WZ;k++)
	{
	  const Geometry::Vec3D gridPt=Grid.point(i,j,k);

	  const double W=
	    distTrack(System,sourcePt,EVal,gridPt,
		      densityFactor,r2Length,r2Power);
	      
	  sumR=(i*j*k != 0) ?
	    mathFunc::logAdd(sumR,Source.WGrid[sIndex][i][j][k]+W) :
	    Source.WGrid[sIndex][i][j][k]+W;
	  sumRA=(i*j*k != 0) ?
	    mathFunc::logAdd(sumRA,Adjoint.WGrid[aIndex][i][j][k]+W) :
	    Adjoint.WGrid[aIndex][i][j][k]+W;
	      
	}
  
  ELog::EM<<"sumR["<<EVal<<"]  == "<<sumR<<" "
	  <<exp(sumR)<<ELog::endDiag;
  ELog::EM<<"sumRA["<<EVal<<"]  == "<<sumRA<<" "
	  <<exp(sumRA)<<ELog::endDiag;

  
  // SETS THIS
  for(long int i=0;i<WX;i++)
    for(long int j=0;j<WY;j++)
      for(long int k=0;k<WZ;k++)
	{
	  WGrid[mIndex][i][j][k]=
	    Source.WGrid[sIndex][i][j][k]
	    +sumR
	    -Adjoint.WGrid[aIndex][i][j][k];
	}
  
  return;
}
  
void
WWGWeight::writeVTK(std::ostream& OX,
		    const long int EIndex,
		    const bool logFlag) const
  /*!
    Write out the VTK format [write log format]
    \param OX :: Output stream
    \param EIndex :: energy index
    \param logFlag :: Convert power to log space
  */
{
  ELog::RegMethod RegA("WWGWeight","writeVTK");
  
  if (EIndex<0 || EIndex>=WE)
    throw ColErr::IndexError<long int>(EIndex,WE,"index in WMesh.ESize");

  boost::format fFMT("%1$11.6g%|14t|");
  
  OX<<"POINT_DATA "<<WX*WY*WZ<<std::endl;
  OX<<"SCALARS cellID float 1.0"<<std::endl;
  OX<<"LOOKUP_TABLE default"<<std::endl;

  double wMin(1e80),wMax(-1e80);
  for(long int K=0;K<WZ;K++)
    for(long int J=0;J<WY;J++)
      {
	for(long int I=0;I<WX;I++)
	  {
	    if (!logFlag)
	      OX<<(fFMT % std::exp(WGrid[EIndex][I][J][K]));	
	    else
	      OX<<(fFMT % -WGrid[EIndex][I][J][K]);
	    if (WGrid[EIndex][I][J][K]<wMin)
	      wMin=WGrid[EIndex][I][J][K];
	    if (WGrid[EIndex][I][J][K]>wMax)
	      wMax=WGrid[EIndex][I][J][K];
	  }
	OX<<std::endl;
      }

  ELog::EM<<"WWG["<<ID<<"]["<<EIndex<<"] "<<wMin<<" "<<wMax<<ELog::endDiag;  

  /*
  for(long int I=0;I<WX;I++)
    for(long int J=0;J<WY;J++)
      {
	for(long int K=0;K<WX;J++)
	  OX<<(fFMT % std::exp(WGrid[EIndex][I][J][K]));
	OX<<std::endl;
      }
  */
  return;
}

void
WWGWeight::write(std::ostream& OX) const
  /*!
    Debug output
    \param OX :: Output stream
   */
{
  for(long int i=0;i<WX;i++)
    for(long int  j=0;j<WY;j++)
      for(long int k=0;k<WZ;k++)
        {
          OX<<i<<" "<<j<<" "<<k;
          for(long int e=0;e<WE;e++)
            {
              OX<<" "<<WGrid[e][i][j][k];
            }
          OX<<std::endl;
        }

  return;
}

void
WWGWeight::writePHITS(std::ostream& OX) const
  /*!
    Write out the PHITS format
    \param OX :: Output stream
   */
{
  ELog::RegMethod RegA("WWGWeight","writePHITS");
  
  std::ostringstream cx;
  
  Grid.writePHITS(OX);
  StrFunc::writePHITS(OX,1,"eng",EBin.size());
  StrFunc::writePHITSList(OX,2,EBin);
  
  OX<<"    xyz";
  for(long int i=0;i<WE;i++)
    OX<<"         ww"<<i+1;
  OX<<std::endl;

  for(long int I=0;I<WX;I++)
    for(long int J=0;J<WY;J++)
      for(long int K=0;K<WZ;K++)
	{
	  cx.str("");
	  cx<<"( "<<I+1<<" "<<J+1<<" "<<K+1<<" )";
	  for(long int EI=0;EI<WE;EI++)
	    cx<<" "<<std::exp(WGrid[EI][I][J][K]);
	  StrFunc::writePHITSTable<std::string>(OX,1,cx.str());
	}
  return;
}

void
WWGWeight::writeFLUKA(std::ostream& OX) const
  /*!
    Write out the FLUKA format
    \param OX :: Output stream
   */
{
  ELog::RegMethod RegA("WWGWeight","writeFLUKA");

  std::ostringstream cx;

  // convert energy to GeV
  cx<<"USRICALL "<<ID;
  for(const double E : EBin)
    cx<<" "<<E*1e-3; 
  for (size_t i=EBin.size();i<5;i++)
    cx<<" -";
  cx<<" wwgEng";
  StrFunc::writeFLUKA(cx.str(),OX);
  
  // This should be extended to allow multiple wwg meshes:
  // size : Must be infront of Low/High/wwg
  cx.str("");
  cx<<"USRICALL ";
  cx<<ID<<" "                  // INDEX not energy bins
    <<Grid.getXSize()<<" "
    <<Grid.getYSize()<<" "
    <<Grid.getZSize()<<" - - "
    <<"wwgSize";
  StrFunc::writeFLUKA(cx.str(),OX);

  cx.str("");
  cx<<"USRICALL "<<ID<<" "<<Grid.getLow()<<" - - "<<"wwgLow";
  StrFunc::writeFLUKA(cx.str(),OX);

  cx.str("");
  cx<<"USRICALL "<<ID<<" "<<Grid.getHigh()<<" - - "<<"wwgHigh";
  StrFunc::writeFLUKA(cx.str(),OX);
  
  for(long int EI=0;EI<WE;EI++)
    for(long int I=0;I<WX;I++)
      for(long int J=0;J<WY;J++)
	for(long int K=0;K<WZ;K++)
	  {
	    cx.str("");
	    cx<<"USRICALL "<<ID<<" "<<EI+1<<" "<<I+1<<" "<<J+1<<" "<<K+1;
	    const double logV(WGrid[EI][I][J][K]>0.0 ? 0.0 :
			      WGrid[EI][I][J][K]);
			   
	    if (logFlag)
	      cx<<" "<<logV;
	    else
	      cx<<" "<<std::exp(logV);
	    cx<<" wwg";
	    StrFunc::writeFLUKA(cx.str(),OX);
	  }
  return;
}


void
WWGWeight::writeWWINP(std::ostream& OX) const
  /*!
    Write out for INP file
   */
{
  
  boost::format TopFMT("%10i%10i%10i%10i%28s\n");
  boost::format neFMT("%10i");
  const std::string date("10/07/15 15:37:51");

  const size_t nParticle(particles.size());
  
  // IF[1] : timeIndependent : No. particleType : 10(rectangular) : date
  OX<<(TopFMT % 1 % 1  % nParticle % 10 % date);

  // particles:
  for(size_t i=0;i<nParticle;i++)
    {
      OX<<(neFMT % EBin.size());
      if ( ((i+1) % 7) == 0) OX<<std::endl;
    }
  if (nParticle % 7) OX<<std::endl;
  
  Grid.writeWWINP(OX);
  for(size_t i=0;i<nParticle;i++)
    {

      size_t itemCnt=0;
      for(const double& E : EBin)
	StrFunc::writeLine(OX,E,itemCnt,6);
      if (itemCnt!=0)
	OX<<std::endl;

      for(long int EI=0;EI<WE;EI++)
	{
	  itemCnt=0;
	  for(long int K=0;K<WZ;K++)
	    for(long int J=0;J<WY;J++)
	      for(long int I=0;I<WX;I++)
		StrFunc::writeLine(OX,std::exp(WGrid[EI][I][J][K]),itemCnt,6);
	  // final terminator if needed:
	  if (itemCnt)   OX<<std::endl;
	}
    }      
  return;
}

void
WWGWeight::writeGrid(std::ostream& OX) const
  /*!
    Write out the grid in an MCNP format
    \param OX :: Output group
   */
{
  Grid.write(OX);
  return;
}

void
WWGWeight::writeCHECK(const size_t index) const
  /*!
    Write out the data for debug
    \param index :: Data group to output 
   */
{
  const double* SData=WGrid.data();
  //  const size_t EnergyStride(static_cast<size_t>(WE));

  ELog::EM<<"WRITE CHECK:: S["<<index<<"] == "
	  <<SData[index]<<" "<<SData[index+1]<<ELog::endDiag;
  return;
}


///\cond TEMPLATE

template
double WWGWeight::distTrack(const Simulation&,const Geometry::Plane&,
			    const double,const Geometry::Vec3D&,
			    const double,const double,
			    const double) const;
template
double WWGWeight::distTrack(const Simulation&,const Geometry::Vec3D&,
			    const double,const Geometry::Vec3D&,
			    const double,const double,
			    const double) const;

template
void WWGWeight::wTrack(const Simulation&,const Geometry::Vec3D&,
		       const size_t,const double,const double,
		       const double);

template
void WWGWeight::wTrack(const Simulation&,const Geometry::Plane&,
		       const size_t,const double,const double,
		       const double);

template 
void WWGWeight::CADISnorm(const Simulation&,const long int,
			  const WWGWeight&,const long int,
			  const WWGWeight&,const long int,
                          const Geometry::Vec3D&,const Geometry::Vec3D&,
			  const double,const double,const double);

template 
void WWGWeight::CADISnorm(const Simulation&,const long int,
			  const WWGWeight&,const long int,
			  const WWGWeight&,const long int,
                          const Geometry::Plane&,const Geometry::Plane&,
			  const double,const double,const double);

///\endcond TEMPLATE

} // Namespace WeightSystem
