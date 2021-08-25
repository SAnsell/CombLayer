/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   cosaxsInc/cosaxsExptLine.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell / Konstantin Batkov
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/
#ifndef xraySystem_cosaxsExptLine_h
#define xraySystem_cosaxsExptLine_h

namespace constructSystem
{
  class JawValveCylinder;
}

/*!
  \namespace xraySystem
  \brief General xray optics system
  \version 1.0
  \date January 2018
  \author S. Ansell
*/

namespace xraySystem
{
  /*!
    \class cosaxsExptLine
    \version 1.0
    \author S. Ansell / K. Batkov
    \date January-May 2018
    \brief Constructor for the CoSAXS experimental beam line
  */

  class FilterHolder;
  class cosaxsSampleArea;
  class cosaxsTube;
  class DiffPumpXIADP03;

class cosaxsExptLine :
  public attachSystem::CopiedComp,
  public attachSystem::ContainedComp,
  public attachSystem::FixedRotate,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap
{
 private:

  /// point to stop [normal none]
  std::string stopPoint;

  /// string for pre-insertion into mastercell:0
  std::shared_ptr<attachSystem::ContainedGroup> preInsert;
  /// construction space for main object
  attachSystem::BlockZone buildZone;
  int outerMat;                         ///< outermaterial if used
  
  /// Shared point to use for last component:
  std::shared_ptr<attachSystem::FixedComp> lastComp;

  /// Inital bellow
  std::shared_ptr<constructSystem::Bellows> bellowA;
  /// Vacuum gate valve A - round
  std::shared_ptr<constructSystem::GateValveCube> gateA;
  /// Double slits
  std::shared_ptr<constructSystem::JawValveCylinder>  doubleSlitA;
  /// Double slit
  std::shared_ptr<constructSystem::JawValveCylinder>  doubleSlitB;
  /// Diagnostic unit 
  std::shared_ptr<xraySystem::MonoBox>  diagUnit;
  size_t nFilterHolders; ///< Number of filter holders
  std::vector<std::shared_ptr<xraySystem::FilterHolder> > filterHolder;
  /// Vacuum gate valve B - flat
  std::shared_ptr<constructSystem::GateValveCube> gateB;
  /// Differential pumping
  std::shared_ptr<xraySystem::DiffPumpXIADP03> diffPump;
  /// Telescopic system - just a pipe with variable length
  /// (so the next component is not attached to this pipe)
  std::shared_ptr<constructSystem::VacuumPipe> telescopicSystem;
  std::shared_ptr<xraySystem::cosaxsSampleArea> sampleArea;
  std::shared_ptr<xraySystem::cosaxsTube> tube;

  double outerLength; ///< total length (used if the back cut surface is not set)
  double outerLeft;    ///< Left Width for cut rectangle
  double outerRight;   ///< Right width for cut rectangle
  double outerTop;     ///< Top lift for cut rectangle

  void buildFilters(Simulation&);
  
  void populate(const FuncDataBase&);
  void createSurfaces();
  void buildObjects(Simulation&);
  void createLinks();

 public:

  cosaxsExptLine(const std::string&);
  cosaxsExptLine(const cosaxsExptLine&);
  cosaxsExptLine& operator=(const cosaxsExptLine&);
  ~cosaxsExptLine();

  /// Assignment to outer void
  void setOuterMat(const int M) { outerMat=M; }
  /// Assignment to extra for first volume
  void setPreInsert
    (const std::shared_ptr<attachSystem::ContainedGroup>& A) { preInsert=A; }

  void insertSample(Simulation&,const int) const;
  
  /// set stop point
  void setStopPoint(const std::string& S) { stopPoint=S; }

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
