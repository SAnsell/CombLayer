/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   bibBuildInc/BilReflector.h
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
#ifndef bibSystem_BilReflector_h
#define bibSystem_BilReflector_h

class Simulation;

namespace bibSystem
{

/*!
  \class BilReflector
  \version 1.0
  \author S. Ansell
  \date November 2012
  \brief Reflector Cylindrical for Bilbau
*/

class BilReflector : public attachSystem::ContainedComp,
    public attachSystem::FixedOffset
{
 private:
  
  const int refIndex;           ///< Index of surface offset 
  int cellIndex;                ///< Cell index
 
  double BeHeight;              ///< Be Height
  double BeDepth;               ///< Be Depth
  double BeRadius;              ///< Be radius
  int BeMat;                    ///< Be Material

  double InnerHeight;           ///< Inner Height
  double InnerDepth;            ///< Inner Depth
  double InnerRadius;           ///< Inner radius
  int InnerMat;                 ///< Inner Material


  double PbHeight;              ///< Pb Height
  double PbDepth;               ///< Pb Depth
  double PbRadius;              ///< Pb radius
  int PbMat;                    ///< Pb Material

  double MidHeight;              ///< Mid Height
  double MidDepth;               ///< Mid Depth
  double MidRadius;              ///< Mid radius
  int MidMat;                    ///< Mid Material

  double OuterHeight;              ///< Outer Height
  double OuterDepth;               ///< Outer Depth
  double OuterRadius;              ///< Outer radius
  int OuterMat;                    ///< Outer Material

  double OuterPbHeight;              ///< Final Pb Height
  double OuterPbDepth;               ///< Final Pb Depth
  double OuterPbRadius;              ///< Final Pb radius
  int OuterPbMat;                    ///< Final Pb Material

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);

  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&);

 public:

  BilReflector(const std::string&);
  BilReflector(const BilReflector&);
  BilReflector& operator=(const BilReflector&);
  virtual ~BilReflector();

  std::string getComposite(const std::string&) const;
  void addToInsertChain(attachSystem::ContainedComp&) const;
  void addToInsertControl(Simulation&,
			  const attachSystem::FixedComp&,
			  attachSystem::ContainedComp&) const;
  /// Main cell
  int getInnerCell() const { return refIndex+1; }
  std::vector<int> getCells() const;
  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
