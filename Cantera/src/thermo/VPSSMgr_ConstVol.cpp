/**
 *  @file VPSSMgr_ConstVol.cpp
 *  Definition file for a derived class that handles the calculation
 *  of standard state thermo properties for
 *  a set of species which have a constant molar volume pressure
 *  dependence (see \ref thermoprops and
 * class \link Cantera::VPSSMgr_ConstVol VPSSMgr_ConstVol\endlink).
 */
/*
 * Copywrite (2005) Sandia Corporation. Under the terms of 
 * Contract DE-AC04-94AL85000 with Sandia Corporation, the
 * U.S. Government retains certain rights in this software.
 */
/*
 *  $Author$
 *  $Date$
 *  $Revision$
 */

// turn off warnings under Windows
#ifdef WIN32
#pragma warning(disable:4786)
#pragma warning(disable:4503)
#endif

#include "VPSSMgr_ConstVol.h"
#include "xml.h"
#include "VPStandardStateTP.h"
#include "SpeciesThermoFactory.h"
#include "PDSS_ConstVol.h"

using namespace std;

namespace Cantera {

  VPSSMgr_ConstVol::VPSSMgr_ConstVol(VPStandardStateTP *vp_ptr, SpeciesThermo *spth) :
    VPSSMgr(vp_ptr, spth)
  {
    m_useTmpRefStateStorage      = true;
    m_useTmpStandardStateStorage = true;
  }


  VPSSMgr_ConstVol::~VPSSMgr_ConstVol()
  {
  }

  VPSSMgr_ConstVol::VPSSMgr_ConstVol(const VPSSMgr_ConstVol &right) :
    VPSSMgr(right.m_vptp_ptr, right.m_spthermo)
  {
    m_useTmpRefStateStorage = true;
    m_useTmpStandardStateStorage = true;
    *this = right;
  }


  VPSSMgr_ConstVol& VPSSMgr_ConstVol::operator=(const VPSSMgr_ConstVol &b) 
  {
    if (&b == this) return *this;
    VPSSMgr::operator=(b);
    return *this;
  }

  VPSSMgr *VPSSMgr_ConstVol::duplMyselfAsVPSSMgr() const {
    VPSSMgr_ConstVol *vpm = new VPSSMgr_ConstVol(*this);
    return (VPSSMgr *) vpm;
  }

  /*
   * Get the nondimensional Entropies for the species
   * standard states at the current T and P of the solution.
   *
   * Note, this is equal to the reference state entropies
   * due to the zero volume expansivity:
   * i.e., (dS/dp)_T = (dV/dT)_P = 0.0
   */
  void VPSSMgr_ConstVol::_updateStandardStateThermo() {

    doublereal del_pRT = (m_plast - m_p0) / (GasConstant * m_tlast);
 
    for (int k = 0; k < m_kk; k++) {
      m_hss_RT[k]  = m_h0_RT[k] + del_pRT * m_Vss[k];
      m_cpss_R[k]  = m_cp0_R[k];
      m_sss_R[k]   = m_s0_R[k];
      m_gss_RT[k]  = m_hss_RT[k] - m_sss_R[k];
      // m_Vss[k] constant
    }
  }

  /*
   *  Returns the vector of nondimensional
   *  Gibbs free energies of the reference state at the current temperature
   *  of the solution and the reference pressure for the species.
   *
   * @param grt Output vector contains the nondimensional Gibbs free energies
   *            of the reference state of the species
   *            length = m_kk, units = dimensionless.
   */
  void VPSSMgr_ConstVol::getGibbs_RT_ref(doublereal *grt) const {
    if (m_useTmpRefStateStorage) {
      std::copy(m_g0_RT.begin(), m_g0_RT.end(), grt);
      doublereal _rt = GasConstant * m_tlast;
      scale(grt, grt + m_kk, grt, _rt);
    } else {
      throw CanteraError("VPSSMgr_ConstVol::getGibbs_RT_ref",
			 "unimplemented without m_useTmpRefStateStorage");
    }
  }
  

  //  Get the molar volumes of the species reference states at the current
  //  <I>T</I> and <I>P_ref</I> of the solution.
  /*
   * units = m^3 / kmol
   *
   * @param vol     Output vector containing the standard state volumes.
   *                Length: m_kk.
   */
  void VPSSMgr_ConstVol::getStandardVolumes_ref(doublereal *vol) const {
  if (m_useTmpStandardStateStorage) {
      std::copy(m_Vss.begin(), m_Vss.end(), vol);
    } else {
      throw CanteraError("VPSSMgr_ConstVol::getStandardVolumes_ref",
			 "unimplemented without m_useTmpRefStateStorage");
    }
  }
  
  void VPSSMgr_ConstVol::initThermo() {
    VPSSMgr::initThermo();
  }

  void 
  VPSSMgr_ConstVol::initThermoXML(XML_Node& phaseNode, std::string id) {
    VPSSMgr::initThermoXML(phaseNode, id);
   
    XML_Node& speciesList = phaseNode.child("speciesArray");
    XML_Node* speciesDB = get_XML_NameID("speciesData", speciesList["datasrc"],
					 &phaseNode.root());
    const vector<string>&sss = m_vptp_ptr->speciesNames();

    for (int k = 0; k < m_kk; k++) {
      const XML_Node* s =  speciesDB->findByAttr("name", sss[k]);
      if (!s) {
	throw CanteraError("VPSSMgr_ConstVol::initThermoXML",
			   "no species Node for species " + sss[k]);
      }
      const XML_Node *ss = s->findByName("standardState");
      if (!ss) {
	throw CanteraError("VPSSMgr_ConstVol::initThermoXML",
			   "no standardState Node for species " + s->name());
      }
      std::string model = (*ss)["model"];
      if (model != "constant_incompressible" && model != "constantVolume") {
	throw CanteraError("VPSSMgr_ConstVol::initThermoXML",
			   "standardState model for species isn't constant_incompressible: " + s->name());
      }
      m_Vss[k] = ctml::getFloat(*ss, "molarVolume", "toSI");
    }   
  }

  //  void
  // VPSSMgr_ConstVol::installSpecies(int k, const XML_Node& speciesNode,  
  //				   const XML_Node *phaseNode_ptr) {
  //}

  PDSS *
  VPSSMgr_ConstVol::createInstallPDSS(int k, const XML_Node& speciesNode,  
				      const XML_Node * const phaseNode_ptr) {
    //VPSSMgr::installSpecies(k, speciesNode, phaseNode_ptr);
    const XML_Node *ss = speciesNode.findByName("standardState");
    if (!ss) {
      throw CanteraError("VPSSMgr_ConstVol::installSpecies",
			 "no standardState Node for species " + speciesNode.name());
    }
    std::string model = (*ss)["model"];
    if (model != "constant_incompressible" && model != "constantVolume") {
      throw CanteraError("VPSSMgr_ConstVol::initThermoXML",
			 "standardState model for species isn't "
			 "constant_incompressible: " + speciesNode.name());
    }
    if ((int) m_Vss.size() < k+1) {
      m_Vss.resize(k+1, 0.0);
    }
    m_Vss[k] = ctml::getFloat(*ss, "molarVolume", "toSI");

    installSTSpecies(k, speciesNode, phaseNode_ptr);
   

    PDSS *kPDSS = new PDSS_ConstVol(m_vptp_ptr, k, speciesNode,  
				    *phaseNode_ptr, true);
    return kPDSS;
  }

  PDSS_enumType VPSSMgr_ConstVol::reportPDSSType(int k) const {
    return cPDSS_CONSTVOL;
  }

  VPSSMgr_enumType VPSSMgr_ConstVol::reportVPSSMgrType() const {
    return  cVPSSMGR_CONSTVOL;
  }
}

