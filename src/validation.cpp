#include <open_atmos/mechanism_configuration/validation.hpp>

namespace open_atmos
{
  namespace validation
  {
    struct Keys keys;
    struct Mechanism mechanism;
    struct Species species;
    struct Phase phase;
    struct ReactionComponent reaction_component;
    struct Arrhenius arrhenius;
    struct CondensedPhaseArrhenius condensed_phase_arrhenius;
    struct Troe troe;
    struct Branched branched;
    struct Tunneling tunneling;
    struct Surface surface;
    struct Photolysis photolysis;
    struct CondensedPhasePhotolysis condensed_phase_photolysis;
    struct Emission emission;
    struct FirstOrderLoss first_order_loss;
    struct SimpolPhaseTransfer simpol_phase_transfer;
    struct WetDeposition wet_deposition;
    struct HenrysLaw henrys_law;
    struct AqueousEquilibrium aqueous_equilibrium;
  }  // namespace validation
}  // namespace open_atmos
