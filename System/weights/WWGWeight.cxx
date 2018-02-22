/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weight/WWGWeight.cxx
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
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "mathSupport.h"
#include "support.h"
#include "MapSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Mesh3D.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cone.h"
#include "support.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "Object.h"
#include "Qhull.h"
#include "weightManager.h"
#include "WForm.h"
#include "WItem.h"
#include "WCells.h"
#include "CellWeight.h"
#include "Simulation.h"
#include "LineTrack.h"
#include "ObjectTrackAct.h"
#include "ObjectTrackPoint.h"
#include "ObjectTrackPlane.h"
#include "weightManager.h"
#include "WWGItem.h"
#include "WWGWeight.h"

namespace WeightSystem
{
  
WWGWeight::WWGWeight(const size_t EB,
		     const Geometry::Mesh3D& Grid)  :
  WX(static_cast<long int>(Grid.getXSize())),
  WY(static_cast<long int>(Grid.getYSize())),
  WZ(static_cast<long int>(Grid.getZSize())),
  WE(static_cast<long int>(EB)),
  WGrid(boost::extents[WX][WY][WZ][WE])
  /*! 
    Constructor 
    \param EB :: Size of energy bin
    \param Grid :: 3D Mesh to build points on [boundary]
  */
{
  zeroWGrid();
}


WWGWeight::WWGWeight(const WWGWeight& A) : 
  zeroFlag(A.zeroFlag),WX(A.WX),WY(A.WY),WZ(A.WZ),WE(A.WE),
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
      zeroFlag=A.zeroFlag;
      WX=A.WX;
      WY=A.WY;
      WZ=A.WZ;
      WE=A.WE;
      WGrid=A.WGrid;
    }
  return *this;
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
WWGWeight::resize(const long int LX,const long int LY,
		  const long int LZ,const long int LE)
  /*!
    Resize the mesh value
    \param LX :: X coorindate size
    \param LY :: Y coorindate size
    \param LZ :: Z coorindate size
    \param LE :: Energy coorindate size
   */
{
  WGrid.resize(boost::extents[LX][LY][LZ][LE]);
  return;
}
	
void
WWGWeight::zeroWGrid()
  /*!
    Zero WGrid and set the zeroflag 
  */
{
  zeroFlag=1;
  for(long int i=0;i<WX;i++)
    for(long int  j=0;j<WY;j++)
      for(long int k=0;k<WZ;k++)
        for(long int e=0;e<WE;e++)
          {
            WGrid[i][j][k][e]=0.0;
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
  
  if (scaleFactor<1e-30)
    throw ColErr::NumericalAbort
      ("ScaleFactor too low:"+std::to_string(scaleFactor));

  if (std::abs(scaleFactor-1.0)>1e-5)
    {
      double* TData=WGrid.data();
      const size_t NData=WGrid.num_elements();
      const double LSF(log(scaleFactor));
      for(size_t i=0;i<NData;i++)
	{
	  TData[i]+=LSF;
	  if (TData[i]>0.0)
	    TData[i]=0.0;
	}
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
  
  if (std::abs(powerFactor-1.0)>1e-5)
    {
      if (!zeroFlag)
	{
	  double* TData=WGrid.data();
	  const size_t NData=WGrid.num_elements();
	  for(size_t i=0;i<NData;i++)
	    {
	      TData[i]*=powerFactor;
	      if (TData[i]>0.0)
		TData[i]=0.0;
	    }
	}
    }
  
  return;
}
  
void
WWGWeight::setLogPoint(const long int index,
		       const long int IE,
		       const double V)
   /*!
     Set a point assuming x,y,z indexing and z fastest point
     \param index :: index for linearization
     \param IE :: Energy bin
     \param V :: value to set
   */
{
  const long int IX=index/(WY*WZ);
  const long int IY=(index/WZ) % WY;
  const long int IZ=index % WZ;

  if (IX>=WX)
    throw ColErr::IndexError<long int>
      (index,WX,"setPoint::Index out of range");

  if (IE>=WE)
    throw ColErr::IndexError<long int>
      (IE,WE,"setPoint::eIndex out of range");
  
  WGrid[IX][IY][IZ][IE]=V;

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

  WGrid[IX][IY][IZ][IE]=mathFunc::logAdd(WGrid[IX][IY][IZ][IE],V);  
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
  double maxW(-1e38);
  if (WE>eIndex)
    {
      for(long int i=0;i<WX;i++)
        for(long int  j=0;j<WY;j++)
          for(long int k=0;k<WZ;k++)
            {
              if (WGrid[i][j][k][eIndex]>maxW)
                maxW=WGrid[i][j][k][eIndex];
            }
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
  double maxW(-1e38);
  for(long int eIndex=0;eIndex<WE;eIndex++)
    {
      const double W=calcMaxAttn(eIndex);
      if (W>maxW) maxW=W;
    }
  return maxW;
}

void
WWGWeight::setMinSourceValue(const double MValue)
  /*!
    Set the minValue and the max value is trapped at 0 [ 1: in real]
    \param MValue :: Lowest acceptable attenuation fraction [-ve only]
  */
{
  ELog::RegMethod RegA("WWGWeight","setMinSourceValue");
  
  if (MValue>0.0)
    throw ColErr::RangeError<double>(MValue,-1e38,0.0,"MValue > exp(0)");
  
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
		      ({I,J,K,EI,WX},{WY,WZ,WE},"Index values");
  

  if (AValue<1e-30)
    throw ColErr::NumericalAbort("AValue to low"+std::to_string(AValue));

  WGrid[I][J][K][EI]+=log(AValue);
  if (WGrid[I][J][K][EI]>0.0)
    WGrid[I][J][K][EI]=0.0;

  return;
}

void
WWGWeight::scaleRange(double AValue,double BValue,const double fullRange)
  /*!
    Convert a set of value [EXP not taken]
    into  a source. 
    - 0 is full beam 
    \param AValue :: [log] Min Value [assumed to be fullRange] (>0 to use min)
    \param BValue :: [log] Max value [above assumed to be 1.0] (>0 to use max)
    \param fullRange :: Range of data output
  */
{
  ELog::RegMethod RegA("WWGWeight","scaleRange");

  if (fullRange<1.0 || fullRange>400.0)
    throw ColErr::RangeError<double>(fullRange,1.0,400.0,"fullRange");

  double* TData=WGrid.data();
  const size_t NData=WGrid.num_elements();

  if (AValue > 1e-10)
    AValue= *std::min_element(TData,TData+NData-1);
  if (BValue > 1e-10)
    BValue= *std::max_element(TData,TData+NData-1);

  double maxValue(-1e30);
  double minValue(1e30);
  if (AValue<BValue)
    {
      const double ABRange(BValue-AValue);
      for(size_t i=0;i<NData;i++)
	{
	  if (TData[i]<=AValue)
	    TData[i] = -fullRange;
	  else if (TData[i]>=BValue)
	    TData[i] = 0.0;
	  else
	    {
	      if (TData[i]>maxValue)
		{
		  maxValue=TData[i];
		}
	      if (TData[i]<minValue)
		{
		  minValue=TData[i];
		}
	      // F is +ve
	      const double F=(TData[i]-AValue)/ABRange;
	      TData[i]= -(1.0-F)*fullRange;
	    }
	}
    }

  ELog::EM<<"Full range == "<<fullRange<<" "<<AValue<<" "<<BValue
	  <<"\n"
	  <<"Scaled from ["<<minValue<<"]"<<exp(minValue)<<" ["
	  <<maxValue<<"] "<<exp(maxValue)<<ELog::endDiag;

  
  return;
}

template<typename T>
void
WWGWeight::wTrack(const Simulation& System,
		  const T& initPt,
		  const std::vector<double>& EBand,
		  const std::vector<Geometry::Vec3D>& MidPt,
		  const double densityFactor,
		  const double r2Length,
		  const double r2Power)
  /*!
    Calculate a specific track from sourcePoint to position
    \param System :: Simulation to use    
    \param initPt :: Point for outgoing track
    \param MidPt :: Grid points
    \param densityFactor :: Scaling factor for density
    \param r2Length :: scale factor for length
    \param r2Power :: power of 1/r^2 factor
  */
{
  ELog::RegMethod RegA("WWGWeight","wTrack");

  long int cN(1);
  ELog::EM<<"Processing  "<<MidPt.size()<<" for WWG"<<ELog::endDiag;

  const long int NCut(static_cast<long int>(MidPt.size())/10);
  for(const Geometry::Vec3D& Pt : MidPt)
    {
      for(long int index=0;index<WE;index++)
	{
	  const double EVal=1e-6+EBand[static_cast<size_t>(index)];
	  const double DT=
	    distTrack(System,initPt,EVal,Pt,densityFactor,r2Length,r2Power);
                                                // energy
	  if (!((cN-1) % NCut))
	    ELog::EM<<"WTRAC["<<cN<<"] "<<DT<<ELog::endDiag;
	  
	  if (!zeroFlag)
	    addLogPoint(cN-1,index,DT);
	  else
	    setLogPoint(cN-1,index,DT);
	  
	  if (!(cN % NCut))
	    ELog::EM<<"Item[ "<<index<<"] == "
		    <<cN<<" "<<MidPt.size()<<" "<<densityFactor<<" "
		    <<r2Length<<ELog::endDiag;
	}
      cN++;
    }
  zeroFlag =0;
  
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
		     const WWGWeight& Adjoint,
		     const std::vector<double>& EBand,
		     const std::vector<Geometry::Vec3D>& gridPts,
		     const T& sourcePt,
                     const U& tallyPt)
  /*!
    Normalize this relative to an adjoint based on 
    the cadis formalism [M. Munk et al : Nucl Sci Eng (2017)]
    Sets the sourceFlux to to R/adjoint[i] flux
    \param System :: Simulation for tracking
    \param Adjoint :: Adjoint WWGWeight
    \param sourcePt :: Source point 
    \param tallyPt :: tally point
  */
{
  ELog::RegMethod RegA("WWGWeight","CADISnorm");

  double* SData=WGrid.data();

  const double* AData=Adjoint.WGrid.data();
  const size_t NData=WGrid.num_elements();
  const size_t ANData=WGrid.num_elements();
  if (NData!=ANData)
    throw ColErr::MisMatch<size_t>
      (NData,ANData,"Source/Adjoint grids do not match");

  if (NData!=gridPts.size()*static_cast<size_t>(WE))
    {
      throw ColErr::MisMatch<size_t>(NData,
				     gridPts.size()*static_cast<size_t>(WE),
				     "Source/Grids do not match");
    }


  const size_t EnergyStride(static_cast<size_t>(WE));

  std::vector<double> sumR(EnergyStride);
  std::vector<double> sumRA(EnergyStride);
  
  if (!zeroFlag && !Adjoint.zeroFlag)
    {
      const size_t tenthValue(gridPts.size()/10);
      ELog::EM<<"Source Point == "<<sourcePt<<ELog::endDiag;
      // STILL in log space
      for(size_t i=0;i<gridPts.size();i++)
	{
	  for(size_t j=0;j<EnergyStride;j++)
	    {
	      const double EVal=1e-6+EBand[j];
	      const double W=distTrack(System,sourcePt,EVal,
				       gridPts[i],1.0,1.0,2.0);
	      sumR[j]=(i) ? mathFunc::logAdd(sumR[j],SData[i*EnergyStride+j]+W) :
		SData[i*EnergyStride+j]+W;
	      
	      sumRA[j]=(i) ? mathFunc::logAdd(sumRA[j],AData[i*EnergyStride+j]+W) :
		AData[i*EnergyStride+j]+W;

	      
	      if (j==0 && !(i % tenthValue) )
		ELog::EM<<"CADIS norm["<<i<<"]:"<<SData[i*EnergyStride]<<" "
			<<AData[i*EnergyStride]<<" == "
			<<gridPts[i]<<ELog::endDiag;

	  //	  SData[i*EnergyStride]-=AData[i*EnergyStride];
	    }

	}

      for(size_t j=0;j<EnergyStride;j++)
	{
	  const double EVal=1e-6+EBand[j];
	  ELog::EM<<"sumR["<<EVal<<"]  == "<<sumR[j]<<" "
		  <<exp(sumR[j])<<ELog::endDiag;
	  ELog::EM<<"sumRA["<<EVal<<"]  == "<<sumRA[j]<<" "
		  <<exp(sumRA[j])<<ELog::endDiag;
	}
      
      // SETS THIS
      for(size_t i=0;i<gridPts.size();i++)
	for(size_t j=0;j<EnergyStride;j++)
	  SData[i*EnergyStride+j]=sumR[j]-AData[i*EnergyStride+j];
    }
  return;
}


void
WWGWeight::writeWWINP(std::ostream& OX) const
  /*!
    Write out the WWINP format
    \param OX :: Output stream
   */
{
  ELog::RegMethod RegA("WWGWeight","writeWWINP");
  
  for(long int EI=0;EI<WE;EI++)
    {
      size_t itemCnt=0;
      for(long int K=0;K<WZ;K++)
	for(long int J=0;J<WY;J++)
	  for(long int I=0;I<WX;I++)
	    StrFunc::writeLine(OX,std::exp(WGrid[I][J][K][EI]),itemCnt,6);
      // final terminator if needed:
      if (itemCnt)   OX<<std::endl;
    }
  return;
}

void
WWGWeight::writeVTK(std::ostream& OX,
		    const long int EIndex) const
  /*!
    Write out the VTK format [write log format]
    \param OX :: Output stream
    \param EIndex :: energy index
  */
{
  ELog::RegMethod RegA("WWGWeight","writeVTK");

  boost::format fFMT("%1$11.6g%|14t|");

  if (EIndex<0 || EIndex>=WE)
    throw ColErr::IndexError<long int>(EIndex,WE,"index in WMesh.ESize");
  
  for(long int K=0;K<WZ;K++)
    for(long int J=0;J<WY;J++)
      {
	for(long int I=0;I<WX;I++)
	  OX<<(fFMT % std::exp(WGrid[I][J][K][EIndex]));
	OX<<std::endl;
      }
  
  return;
}

void
WWGWeight::write(std::ostream& OX) const
  /*!
    Debug output
    \param OX :: Output stream
   */
{
  OX<<"# NPts == "<<WGrid.size()<<std::endl;
  for(long int i=0;i<WX;i++)
    for(long int  j=0;j<WY;j++)
      for(long int k=0;k<WZ;k++)
        {
          OX<<i<<" "<<j<<" "<<k;
          for(long int e=0;e<WE;e++)
            {
              OX<<" "<<WGrid[i][j][k][e];
            }
          OX<<std::endl;
        }

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
		       const std::vector<double>&,
		       const std::vector<Geometry::Vec3D>&,
		       const double,const double,
		       const double);

template
void WWGWeight::wTrack(const Simulation&,const Geometry::Plane&,
		       const std::vector<double>&,
		       const std::vector<Geometry::Vec3D>&,
		       const double,const double,
		       const double);

template 
void WWGWeight::CADISnorm(const Simulation&,const WWGWeight&,
                          const std::vector<double>&,
			  const std::vector<Geometry::Vec3D>&,
                          const Geometry::Vec3D&,const Geometry::Vec3D&);

template 
void WWGWeight::CADISnorm(const Simulation&,const WWGWeight&,
			  const std::vector<double>&,
                          const std::vector<Geometry::Vec3D>&,
                          const Geometry::Plane&,const Geometry::Plane&);

///\endcond TEMPLATE

} // Namespace WeightSystem
