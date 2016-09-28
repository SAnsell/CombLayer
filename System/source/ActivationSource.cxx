/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   source/ActivationSource.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include <boost/filesystem.hpp>

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
#include "MXcards.h"
#include "Zaid.h"
#include "Material.h"
#include "DBMaterial.h"
#include "ModeCard.h"
#include "Simulation.h"
#include "activeUnit.h"
#include "ActivationSource.h"

extern MTRand RNG;

namespace SDef
{

ActivationSource::ActivationSource() :
  timeStep(2),nPoints(0),nTotal(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
  */
{}


ActivationSource::~ActivationSource() 
  /*!
    Destructor
  */
{}


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
  cellID.clear();
  fluxPt.reserve(nPoints);
  cellID.reserve(nPoints);
  
  const Geometry::Vec3D BDiff(BBoxPt-ABoxPt);
  const double xStep(BDiff[0]);
  const double yStep(BDiff[1]);
  const double zStep(BDiff[2]);
  MonteCarlo::Object* cellPtr(0);
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
              fluxPt.push_back(testPt);
              cellID.push_back(cellN);
              index++;
            }
        }
      nTotal++;
      if (!(nTotal % (50*nPoints)))
	ELog::EM<<"Ntotal/nPoints == "<<nTotal<<":"<<nPoints<<ELog::endDiag;
    }
  ELog::EM<<"FINAL nPoints/Ntotal == "<<nPoints<<":"<<nTotal<<ELog::endDiag;

  // correct volumes by correct count
  const double boxVol=BDiff.volume()/static_cast<double>(nTotal);
  for(std::map<int,double>::value_type& MItem : volCorrection)
    MItem.second*=boxVol;

  // normalisze cellFlux
  for(std::map<int,activeUnit>::value_type& CA : cellFlux)
    {
      std::map<int,double>::const_iterator mc=
	volCorrection.find(CA.first);
      
      CA.second.normalize(mc->second);
    }
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


  boost::filesystem::directory_iterator curDIR(boost::filesystem::current_path());
  const boost::filesystem::directory_iterator endDIR;

  std::vector<std::string> fileNames;
  std::vector<int> cellNumList;
  while(curDIR != endDIR)
    {
      if (boost::filesystem::is_directory(*curDIR))
        {
          const std::string fileName(curDIR->path().filename().string());
	  if (fileName.length()>inputFileBase.length())
	    {
	      const std::string testBase(fileName.substr(0,inputFileBase.length()));
	      const std::string testNumber(fileName.substr(inputFileBase.length()));
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
  processFluxFiles(fileNames,cellNumList);
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
	  size_t timeIndex(0);
	  while(timeStep!=timeIndex && IX.good())
	    {                      
	      SLine=StrFunc::getLine(IX,512);
	      if (!StrFunc::section(SLine,G))  // line with words
		timeIndex++;
	    }
	  
	  // NOW Read Gamma flux
	  double totalFlux(0.0);
	  do
	    {
	      SLine=StrFunc::getLine(IX,512);
	      while(StrFunc::section(SLine,G))
		{
		  gamma.push_back(G);
		  totalFlux+=G;
		}
	    }
	  while(IX.good() && StrFunc::isEmpty(SLine));

	  cellFlux.emplace(cellNumbers[index],activeUnit(energy,gamma));
	}
      IX.close();
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

  OX<<"ActivationSource Write"<<std::endl;
  OX<<nPoints<<std::endl;
  for(size_t i=0;i<nPoints;i++)
    {
      const Geometry::Vec3D& Pt=fluxPt[i];
      const int& cellN=cellID[i];
      std::map<int,activeUnit>::const_iterator mc=
	cellFlux.find(cellN);

      mc->second.writePhoton(OX,Pt);
    }
  OX.close();
  return;
}
  
  
void
ActivationSource::createSource(Simulation& System,
                               const std::string& inputFileBase,
                               const std::string& outputName)
  /*!
    Create all the source
    \param System :: Simuation 
    \param souceCard :: Source Term
    \param inputFileBase :: input file key
    \param outputName :: Output file
   */
{
  ELog::RegMethod RegA("ActivationSource","createSource");

  // First loop is to generate all the points within the set
  // it allows volumes to be effectively calculated.
  //
  readFluxes(inputFileBase);
  createFluxVolumes(System);
  writePoints(outputName);

  
  return;
}


} // NAMESPACE SDef
