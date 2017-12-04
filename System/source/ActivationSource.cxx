/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   source/ActivationSource.cxx
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
#include <sstream>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <memory>
#ifndef NO_REGEX
#include <boost/filesystem.hpp>
#endif

#include "Exception.h"
#include "MersenneTwister.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "doubleErr.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "Source.h"
#include "SrcItem.h"
#include "SrcData.h"
#include "surfRegister.h"
#include "ModelSupport.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "Object.h"
#include "WorkData.h"
#include "Zaid.h"
#include "MXcards.h"
#include "Material.h"
#include "DBMaterial.h"
#include "ModeCard.h"
#include "Simulation.h"
#include "localRotate.h"
#include "activeUnit.h"
#include "activeFluxPt.h"
#include "SourceBase.h"
#include "ActivationSource.h"

extern MTRand RNG;

namespace SDef
{

ActivationSource::ActivationSource() :
  SourceBase(),
  timeStep(2),nPoints(0),nTotal(0),PPtr(0),
  r2Power(2.0),weightDist(-1.0),externalScale(1.0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
  */
{}

ActivationSource::ActivationSource(const ActivationSource& A) :
  SourceBase(A),
  timeStep(A.timeStep),nPoints(A.nPoints),nTotal(A.nTotal),
  ABoxPt(A.ABoxPt),BBoxPt(A.BBoxPt),
  volCorrection(A.volCorrection),cellFlux(A.cellFlux),
  fluxPt(A.fluxPt),PPtr((A.PPtr) ? A.PPtr->clone() : 0),
  r2Power(A.r2Power),weightPt(A.weightPt),
  weightDist(A.weightDist),externalScale(A.externalScale)
  /*!
    Copy constructor
    \param A :: ActivationSource to copy
  */
{}

ActivationSource&
ActivationSource::operator=(const ActivationSource& A)
  /*!
    Assignment operator
    \param A :: ActivationSource to copy
    \return *this
  */
{
  if (this!=&A)
    {
      SourceBase::operator=(A);
      timeStep=A.timeStep;
      nPoints=A.nPoints;
      nTotal=A.nTotal;
      ABoxPt=A.ABoxPt;
      BBoxPt=A.BBoxPt;
      volCorrection=A.volCorrection;
      cellFlux=A.cellFlux;
      fluxPt=A.fluxPt;
      delete PPtr;
      PPtr=(A.PPtr) ? A.PPtr->clone() : 0;
      r2Power=A.r2Power;
      weightPt=A.weightPt;
      weightDist=A.weightDist;
      externalScale=A.externalScale;
    }
  return *this;
}

ActivationSource::~ActivationSource() 
  /*!
    Destructor
  */
{
  delete PPtr;
}

ActivationSource*
ActivationSource::clone() const
  /*!
    Clone operator
    \return new this
  */
{
  return new ActivationSource(*this);
}
  
void
ActivationSource::setPlane(const Geometry::Vec3D& APt,
			   const Geometry::Vec3D& AAxis,
			   const double rPower)
  /*!
    Build the plane if needed
    \param APt :: Point on plane
    \param AAxis :: Normal
   */
{
  ELog::RegMethod Rega("ActivationSource","setPlane");

  if (AAxis.abs()<Geometry::zeroTol)
    throw ColErr::NumericalAbort("AAxis has zero value");
  PPtr=new Geometry::Plane();
  PPtr->setPlane(APt,AAxis);
  r2Power=rPower;
  return;
}
  
  
void
ActivationSource::setBox(const Geometry::Vec3D& APt,
			 const Geometry::Vec3D& BPt)
  /*!
    Set the box coordinates
    \param APt :: Lower corner
    \param BPt :: Upper corner
   */
{
  ELog::RegMethod RegA("ActivationSource","setBox");

  for(size_t i=0;i<3;i++)
    if (BPt[i]-APt[i]<Geometry::zeroTol)
      ELog::EM<<"Error with box coordinate: [Low - High] required :"
	      <<APt<<":"<<BPt<<ELog::endErr;
      
  ABoxPt=APt;
  BBoxPt=BPt;
  return;
}

void
ActivationSource::setWeightPoint(const Geometry::Vec3D& Pt,
			   const double distScale)
  /*!
    Allow scaling realtive to a point base on R^2 distance
    \param Pt :: point
    \param distScale :: distance scale 
   */
{
  weightPt=Pt;
  weightDist=distScale;
  return;
}

void
ActivationSource::rotate(const localRotate& LR)
  /*!
    Rotate the source
    \param LR :: Rotation to apply
  */
{
  ELog::RegMethod Rega("ActivationSource","rotate");

  LR.applyFull(ABoxPt);
  LR.applyFull(BBoxPt);
  LR.applyFull(weightPt);
  return;
}

  
void
ActivationSource::createFluxVolumes(const Simulation& System)
 /*!
   Process a number of points to get the volume
   \param System :: Simulation to use
 */
{
  ELog::RegMethod RegA("ActivationSource","createFluxVolumes");

  nTotal=0;
  size_t index=0;
  fluxPt.clear();

  ELog::EM<<"Volume == "<<ABoxPt<<" : "<<BBoxPt<<ELog::endDiag;
  const Geometry::Vec3D BDiff(BBoxPt-ABoxPt);
  const double xStep(BDiff[0]);
  const double yStep(BDiff[1]);
  const double zStep(BDiff[2]);
  MonteCarlo::Object* cellPtr(0);
  size_t reportScore(nPoints*1);
  ELog::EM<<"Report == "<<reportScore<<ELog::endDiag;
  while(index<nPoints)
    {
      Geometry::Vec3D testPt(xStep*RNG.rand(),yStep*RNG.rand(),
			     zStep*RNG.rand());
      testPt+=ABoxPt;

      cellPtr=System.findCell(testPt,cellPtr);
      if (!cellPtr)
	throw ColErr::InContainerError<Geometry::Vec3D>
	  (testPt,"Point not in cell");
      const int matN=cellPtr->getMat();
      // test for Material / cellFlux / volume.
      if (matN!=0)
	{
	  const int cellN=cellPtr->getName();
          if (cellFlux.find(cellN)!=cellFlux.end())
            {
              std::map<int,double>::iterator mc=
                volCorrection.find(cellN);
              if (mc==volCorrection.end())
                volCorrection.emplace(cellN,1.0);
              else
                mc->second+=1.0;
              fluxPt.push_back(activeFluxPt(cellN,testPt));
              index++;
            }
        }
      nTotal++;
      if (!(nTotal % reportScore))
        {
          ELog::EM<<"Ntotal/nPoints == "<<nTotal<<":"<<nPoints<<" found="<<index<<ELog::endDiag;
          reportScore*=2;
        }
    }
  ELog::EM<<"FINAL nPoints/Ntotal == "<<nPoints<<":"<<nTotal<<ELog::endDiag;

  // correct volumes by correct count
  // volcorrection has good count : divide by total count and multiply by total volume
  const double boxVol=
    BDiff.volume()/static_cast<double>(nTotal);

  // normalisze cellFlux
  // The volume self cancels since flux was per volume and this is not:
  // BUT need to scale by fractional total:

  for(std::map<int,activeUnit>::value_type& CA : cellFlux)
    {
      std::map<int,double>::const_iterator mc=
	volCorrection.find(CA.first);
      if (mc->second>Geometry::zeroTol)
	CA.second.normalize(static_cast<double>(nPoints)/mc->second,
			    boxVol*mc->second);
    }

  for(std::map<int,double>::value_type& MItem : volCorrection)
    MItem.second*=boxVol;
  
  for(const std::map<int,double>::value_type& MItem : volCorrection)
    ELog::EM<<"Cell["<<MItem.first<<"] == "<<MItem.second<<ELog::endDiag;
  return;
}
  

void
ActivationSource::readFluxes(const std::string& inputFileBase)
   /*!
     Read the fluxes from the given file[s]
     \param inputFileBase :: FileBase e.g CellXXX were XXX is the cell number
   */
{
  ELog::RegMethod RegA("ActivationSource","readFluxes");

#ifndef NO_REGEX
  
  boost::filesystem::directory_iterator curDIR
    (boost::filesystem::current_path());
  const boost::filesystem::directory_iterator endDIR;

  std::vector<std::string> fileNames;
  std::vector<int> cellNumList;
  while(curDIR != endDIR)
    {
      if (boost::filesystem::is_directory(*curDIR))
        {
          const size_t iLen=inputFileBase.length();
          const std::string fileName(curDIR->path().filename().string());
	  if (fileName.length()>iLen)
	    {
	      const std::string testBase(fileName.substr(0,iLen));
	      const std::string testNumber(fileName.substr(iLen));
	      int cellNumber;
	      if (testBase==inputFileBase &&
		  StrFunc::convert(testNumber,cellNumber))
		{
		  const std::string fluxFile=fileName+"/spectra";
		  fileNames.push_back(fluxFile);
		  cellNumList.push_back(cellNumber);
		}
	    }
	}
      ++curDIR;
    }
  if (cellNumList.empty())
    throw ColErr::InContainerError<std::string>(inputFileBase,"cellNumList empty");
  
  processFluxFiles(fileNames,cellNumList);
#endif
  return;
}


void
ActivationSource::processFluxFiles(const std::vector<std::string>& fluxFiles,
				   const std::vector<int>& cellNumbers)
  /*!
    Read the files
    \param cellNumbers :: cell numbers to use
    \param fluxFiles :: files to read and procduce
   */
{
  ELog::RegMethod RegA("ActivationSource","processfluxFiles");

  for(size_t index=0;index<fluxFiles.size();index++)
    {
      std::ifstream IX;
      IX.open(fluxFiles[index].c_str());
      ELog::EM<<"Processing Spectra : "<<fluxFiles[index]<<ELog::endDiag;
      if (IX.good())
	{
	  // dump first line
	  std::string SLine=StrFunc::getLine(IX,512);
          std::string timeLine;
	  // read energy bins:
	  std::vector<double> energy;
	  std::vector<double> gamma;
	  gamma.push_back(0.0);
	  double E,G;
	  do
	    {
	      SLine=StrFunc::getLine(IX,512);
	      while(StrFunc::section(SLine,E))
		{
		  energy.push_back(E);
		}
	    }
	  while(IX.good() && StrFunc::isEmpty(SLine));
	  
	  // effective lose of SLine:= MUTLIGROUP ....
          double totalFlux(0.0);
	  size_t timeIndex(0);
	  
	  while(timeStep!=timeIndex && IX.good())
	    {                      
	      SLine=StrFunc::getLine(IX,512);
	      if (!StrFunc::sectionCINDER(SLine,G))  // line with words
		timeIndex++;
              // store time line for later:
              timeLine=SLine;
	    }
	  
	  // NOW Read Gamma flux

	  do
	    {
	      SLine=StrFunc::getLine(IX,512);
	      while(StrFunc::sectionCINDER(SLine,G))
                gamma.push_back(G);
	    }
	  while(IX.good() && StrFunc::isEmpty(SLine));

          ELog::EM<<"Time == "<<timeLine<<ELog::endDiag;

          std::string item;
          const size_t itemCnt((timeStep==1) ? 3 : 4);
          for(size_t i=0;i<itemCnt && StrFunc::section(timeLine,item);i++) ;
          if (!StrFunc::section(item,totalFlux))
            throw ColErr::FileError(static_cast<int>(index),fluxFiles[index],
                                    "Failed to get totalFlux");
              
          ELog::EM<<"Gamma total == "<<totalFlux<<ELog::endDiag;
          
	  cellFlux.emplace(cellNumbers[index],
                           activeUnit(totalFlux,energy,gamma));
	}
      IX.close();
    }
  return;

}

double
ActivationSource::calcWeight(const Geometry::Vec3D& Pt) const
  /*!
    Calculate the base weight
    \param Pt :: Point
    \return base weight
  */
{
  double D=1.0;
  if (PPtr)
    D=PPtr->distance(Pt);

  else if (weightDist>Geometry::zeroTol)
    D=(weightPt.Distance(Pt))/weightDist;

  return (D<0.1) ? 100.0 : 1/std::pow(D,r2Power);
}

void
ActivationSource::normalizeScale()
  /*!
    Normalizes the scale to integral of 1.0
  */
{
  ELog::RegMethod RegA("ActivationSource","normalizeScale");

  
  if (weightDist>Geometry::zeroTol)
    {
      for(std::map<int,activeUnit>::value_type& VT : cellFlux)
	VT.second.zeroScale();
    }
  
  for(const activeFluxPt& Pt : fluxPt)
    {
      std::map<int,activeUnit>::iterator mc=
	cellFlux.find(Pt.getCellID());
      if (mc==cellFlux.end())
	throw ColErr::InContainerError<int>(Pt.getCellID(),"fluxPoint CellN");
      const double sumWt(calcWeight(Pt.getPoint()));
      mc->second.addScaleSum(sumWt);
    }
  return;
}
  
void
ActivationSource::writePoints(const std::string& outputName) const
  /*!
    This writes out the points for MCNP to read them in.
    The method uses loadSave from RNG to needs to be paired
    with a createSave which is done in calcVolumes.

    \param outputName :: Output file name
   */
{
  ELog::RegMethod RegA("ActiationSource","writePoints");

  std::ofstream OX;
  OX.open(outputName.c_str());

  OX<<"ActivationSource TStep="<<StrFunc::makeString(timeStep)
    <<" Source == "<<ABoxPt<<" :: "<<BBoxPt<<std::endl;
  OX<<"-"<<nPoints<<std::endl;

  // nPoints can be changed from fluxPt size
  for(size_t i=0;i<nPoints;i++)
    {
      const size_t index(i % fluxPt.size());
      const Geometry::Vec3D& Pt=fluxPt[index].getPoint();
      const int cellN=fluxPt[index].getCellID();
      std::map<int,activeUnit>::const_iterator mc=
	cellFlux.find(cellN);
      if (mc==cellFlux.end())
	throw ColErr::InContainerError<int>(cellN,"cellN not in CellFlux");
      const double weight=externalScale*calcWeight(Pt)/
	mc->second.getScaleFlux();
      mc->second.writePhoton(OX,Pt,weight);
    }
  OX.close();
  return;
}
  
  
void
ActivationSource::createAll(const Simulation& System,
			    const std::string& inputFileBase,
			    const std::string& outputName)
  /*!
    Create all the source
    \param System :: Simuation 
    \param inputFileBase :: input file key
    \param outputName :: Output file
   */
{
  ELog::RegMethod RegA("ActivationSource","createSource");
  //
  // First loop is to generate all the points within the set
  // it allows volumes to be effectively calculated.
  //
  readFluxes(inputFileBase);
  createFluxVolumes(System);
  normalizeScale();
  writePoints(outputName);  
  return;
}


void
ActivationSource::createSource(SDef::Source&) const
  /*!
   */
{
  ELog::RegMethod RegA("ActivationSource","createSource");

  return;
}

void
ActivationSource::write(std::ostream&) const
  /*!
    Write out as a MCNP source system
    \param :: Output stream [no op : as sdefVoid]
  */
{
  ELog::RegMethod RegA("ActivationSource","write");
  return;
}

void
ActivationSource::writePHITS(std::ostream& OX) const
  /*!
    Write out as a PHITS source system
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("ActivationSource","write");

  ELog::EM<<"NOT YET WRITTEN "<<ELog::endCrit;
  return;
}




} // NAMESPACE SDef
