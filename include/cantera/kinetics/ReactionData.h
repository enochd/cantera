/**
 *  @file ReactionData.h
 */
// Copyright 2001  California Institute of Technology

#ifndef CT_REACTION_DATA_H
#define CT_REACTION_DATA_H

#include "cantera/kinetics/reaction_defs.h"
#include "cantera/base/utilities.h"

namespace Cantera
{

//! Intermediate class which stores data about a reaction and its rate
//! parameterization before adding the reaction to a Kinetics object.
/*!
 *  All data in this class is public.
 */
class ReactionData
{
public:
    ReactionData() :
        reactionType(ELEMENTARY_RXN),
        validate(false),
        number(0),
        rxn_number(0),
        filmResistivity(0.0),
        equilibriumConstantPower(1.0),
        affinityPower(1.0),
        reversible(true),
        duplicate(false),
        rateCoeffType(ARRHENIUS_REACTION_RATECOEFF_TYPE),
        falloffType(NONE),
        error(0),
        equation(""),
        default_3b_eff(1.0),
        global(false),
        isReversibleWithFrac(false),
        beta(0.0),
        chebTmin(-1.0),
        chebTmax(-1.0),
        chebPmin(-1.0),
        chebPmax(-1.0),
        chebDegreeT(0),
        chebDegreeP(0)
    {}

    virtual ~ReactionData() {}

    //! Type of the reaction. The valid types are listed in the file,
    //! reaction_defs.h, with constants ending in `RXN`.
    int reactionType;

    bool validate; //!< Perform validation of the rate coefficient data
    int number; //!< Index of this reaction within the mechanism
    int rxn_number; //!< @deprecated duplicate of #number
    std::vector<size_t> reactants; //!< Indices of reactant species
    std::vector<size_t> products; //!< Indices of product species

    //! Reaction order with respect to each reactant species, in the order
    //! given by #reactants. Usually the same as the stoichiometric coefficients.
    /*!
     *  Length is equal to the number of reactants defined in the reaction
     *  The order of species is given by the reactants vectors.
     */
    vector_fp rorder;

    //! Reaction order of the reverse reaction with respect to each product
    //! species, in the order given by #products. Usually the same as the stoichiometric coefficients.
    /*!
     *  Length is equal to the number of products defined in the reaction.
     *  The order of species is given by the products vectors.
     */
    vector_fp porder;

    //! Reaction order for the forward direction of the reaction
    /*!
     *  Length is equal to the number of kinetic species defined in the kinetics object
     *  The order of species is given by kinetics species vector.
     */
    vector_fp forwardFullOrder_;

    //! Reactant stoichiometric coefficients, in the order given by #reactants.
    /*!
     *  Length is equal to the number of products defined in the reaction.
     *  The order of species is given by the products vectors.
     */
    vector_fp rstoich;

    //! Product stoichiometric coefficients, in the order given by #products.
    /*!
     *  Length is equal to the number of products defined in the reaction.
     *  The order of species is given by the products vectors.
     */
    vector_fp pstoich;

    std::vector<grouplist_t> rgroups; //!< Optional data used in reaction path diagrams
    std::vector<grouplist_t> pgroups; //!< Optional data used in reaction path diagrams

    //! Map of species index to third body efficiency
    std::map<size_t, doublereal> thirdBodyEfficiencies;

    //! Net stoichiometric coefficients for participating species. Used for
    //! duplicate reaction detection. Key is `-1-k` for reactants, `1+k` for
    //! products.
    std::map<int, doublereal> net_stoich;

    //! Film Resistivity value
    /*!
     *  Only valid for Butler-Volmer formulations.
     *  Units are in ohms m2.
     *  default = 0.0 ohms m2
     */
    doublereal filmResistivity;

    //! Power of the equilibrium constant within the Affinity representation
    /*!   
     *  Only valid for Affinity representation.
     *  default = 1.0
     */
    doublereal equilibriumConstantPower;

    //! Power of the "One minus Affinity" term within the Affinity representation
    /*!
     *   Only valud for Affinity representation
     *   default = 1.0
     */
    doublereal affinityPower;

    //! True if the current reaction is reversible. False otherwise
    bool reversible;

    //! True if the current reaction is marked as duplicate
    bool duplicate;

    //! Type of the rate coefficient for the forward rate constant
    /*!
     *  The valid types are listed in the file, reaction_defs.h and they
     *  all end in RATECOEFF_TYPE
     */
    int rateCoeffType;

    //! Vector of rate coefficient parameters. For elementary reactions, these
    //! are the pre- exponential factor, temperature exponent, and activation
    //! energy in the Arrhenius expression.
    vector_fp rateCoeffParameters;

    //! Vector of auxiliary rate coefficient parameters. This is used for
    //! the alternate Arrhenius parameters used in falloff and chemically
    //! activated reactions.
    vector_fp auxRateCoeffParameters;

    //! Type of falloff parameterization to use. Values are defined in
    //! reaction_defs.h, with names ending in `FALLOFF`.
    int falloffType;

    //! Values used in the falloff parameterization. Meaning of each parameter
    //! depends on #falloffType.
    vector_fp falloffParameters;

    int error; //!< @deprecated unused. To be removed after Cantera 2.2.

    //! The reaction equation. Used only for display purposes.
    std::string equation;

    //! The reactants half of the reaction equation, used for display purposes.
    std::string reactantString;

    //! The products half of the reaction equation, used for display purposes.
    std::string productString;

    //! The default third body efficiency for species not listed in
    //! #thirdBodyEfficiencies.
    doublereal default_3b_eff;

    //! Adjustments to the Arrhenius rate expression dependent on surface
    //! species coverages. Contains 4 elements for each coverage dependency:
    //! the species index, and the three coverage parameters (a, E, m). See
    //! SurfaceArrhenius for details on the parameterization.
    vector_fp cov;

    //! True for "global" reactions which do not follow elementary mass action
    //! kinetics, i.e. reactions for which the reaction order is not given by
    //! the stoichiometric coefficients.
    bool global;

    //! Some reactions can be elementary reactions but have fractional
    //! stoichiometries with respect to some products and reactants. An
    //! example of these are solid reactions involving phase transformations.
    //! Species with fractional stoichiometries must be from single-species
    //! phases with unity activities.
    bool isReversibleWithFrac;

    //! Forward value of the apparent Electrochemical transfer coefficient
    doublereal beta; 

    //! Arrhenius parameters for P-log reactions.
    //! The keys are the pressures corresponding to each Arrhenius expression.
    //! Multiple sets of Arrhenius parameters may be specified at a given
    //! pressure.
    std::multimap<double, vector_fp> plogParameters;

    double chebTmin; //!< Minimum temperature for Chebyshev fit
    double chebTmax; //!< Maximum temperature for Chebyshev fit
    double chebPmin; //!< Minimum pressure for Chebyshev fit
    double chebPmax; //!< Maximum pressure for Chebyshev fit
    size_t chebDegreeT; //!< Degree of Chebyshev fit in T
    size_t chebDegreeP; //!< Degree of Chebyshev fit in P

    //! Chebyshev coefficients. length chebDegreeT * chebDegreeP
    vector_fp chebCoeffs;

    //! Get the actual third-body efficiency for species *k*
    double efficiency(size_t k) const {
        return getValue(thirdBodyEfficiencies, k, default_3b_eff);
    }
};
}

#endif
