/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physicsInc/PhysicsCards.h
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
#ifndef PhysicsSystem_PhysicsCards_h
#define PhysicsSystem_PhysicsCards_h

namespace SDef
{
  class Source;
}
/*!
  \namespace PhysicsSystem
  \version 1.0
  \author S. Ansell
  \date April 2013
  \brief MCNP physics list cards
*/

namespace physicsSystem
{  
  class dbcnCard;
  class nameCard;
  class ExtControl;
  class PWTControl;
  class DXTControl;
  class PhysImp;
  
/*!
  \class PhysicsCards
  \version 1.0
  \date November 2005
  \author S.Ansell
  \brief Processes the physics cards in the MCNPX .i file

  Writes the physics cards. At the moment does
  not process all the special cards. 
  It holds all types of "all cell" cards (imp,vol,fcl etc.)
  in ImpCards.  
*/

class PhysicsCards 
{
 private:

  int mcnpVersion;                     ///< Version of MCNP(X) 
  size_t nps;                          ///< number of particles to run
  int histp;                           ///< Add a histp line
  std::set<int> histpCells;            ///< cells for the histp list

  std::unique_ptr<nameCard> RAND;      ///< RAND card [MCNP6]
  std::unique_ptr<nameCard> PTRAC;     ///< Particle Track card [MCNP6]
  std::unique_ptr<nameCard> dbCard;    ///< Reference to the dbcnCard
  
  std::vector<std::string> Basic;      ///< Basic cards (stripped of Variables)

  ModeCard mode;                          ///< Mode card
  bool voidCard;                          ///< Void card
  std::set<std::string> wImpOut;          ///< wImp flag [wwg | wcell]
  std::list<int> printNum;                ///< print numbers
  std::string prdmp;                      ///< prdmp string
  std::vector<PhysCard*> PCards;          ///< Physics cards
  std::vector<PhysImp*> PImpVec;          ///< Map of PhysImp cards
  LSwitchCard LEA;                        ///< LEA/LCA Card

  std::unique_ptr<ExtControl> ExtCard;    ///< Exponent control system
  std::unique_ptr<PWTControl> PWTCard;    ///< Photon Weight
  std::unique_ptr<DXTControl> DXTCard;    ///< Dxtran spheres [move to tally]
  
  void deletePCards();
  void deletePhysImp();
    
 public:
   
  PhysicsCards();
  PhysicsCards(const PhysicsCards&);
  PhysicsCards& operator=(const PhysicsCards&);
  virtual ~PhysicsCards();

  void clearAll();
  void setHist(const int H) { histp=H; }     ///< Set history flag
  void addHistpCells(const std::set<int>&);
  void clearHistpCells(); 

  // General [All particles] :
  double getValue(const std::string&,const std::string&,const int) const;

  /// Get Mode card
  ModeCard& getMode() { return mode; }
  /// Get LEA card
  LSwitchCard& getLEA() { return LEA; } 

  /// allows setting of flag
  void clearWImpFlag(const std::string&);
  void setWImpFlag(const std::string&);
  bool hasWImpFlag(const std::string&) const;
  bool hasImpFlag(const std::string&) const;
  
  template<typename T>
  T* addPhysCard(const std::string&,const std::string&);
  const PhysCard* getPhysCard(const std::string&,const std::string&) const;

  /// Access ExpControl card
  ExtControl& getExtCard() { return *ExtCard; }
  /// Access PWTControl card
  PWTControl& getPWTCard() { return *PWTCard; }
  /// Access DXTControl card
  DXTControl& getDXTCard() { return *DXTCard; }
  /// Access DXTControl card
  
  PhysImp& getPhysImp(const std::string&);
  PhysImp& getPhysImp(const std::string&,const std::string&);
  PhysImp& getPhysImp(const std::string&,const std::string&,const double);
  PhysImp& getPhysImp(const std::string&,const std::set<int>&);
  PhysImp& getPhysImp(const std::string&,const std::set<int>&,const double);
  const PhysImp& getPhysImp(const std::string&) const;
  const PhysImp& getPhysImp(const std::string&,const std::string&) const;
  const PhysImp& getPhysImp(const std::string&,const std::set<int>&) const;
  
  /// Access to NPS
  size_t getNPS() const { return nps; }     
  // ALL Particle/Type
  int processCard(const std::string&);
  void removeCell(const int);

  /// set MCNP version
  void setMCNPversion(const int V) { mcnpVersion=V;} 
  // Special for type: vol
  
  void setPWT(const std::vector<int>&,const double =1.0);
  void setPWT(const int,const double);

  void setNPS(const size_t N) { nps=N; }      ///< Set the Number of particles
  void setRND(const long int,const long int =0);
  
  template<typename T>
  void setPTRAC(const std::string&,const T&);
  template<typename T>
  void setDBCN(const std::string&,const T&);
  void setDBCNactive(const bool);
  void setPTRACactive(const bool);

  void setEnergyCut(const double);  
  void setMode(std::string);
  void setVoidCard(const bool V) { voidCard=V; }   ///< Set the void card
  void setPrintNum(std::string);
  /// Set the prdmp card
  void setPrdmp(const std::string& P) 
    { prdmp = P; } 

  long int getRNDseed() const;

  void rotateMaster();
  void substituteCell(const int,const int);
  //  void substituteSurface(const int,const int); 

  void writeHelp(const std::string&) const;
  
  void write(std::ostream&,const std::vector<int>&,
	     const std::set<int>&) const;   
};

}

#endif
