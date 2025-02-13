#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <mechanism_configuration/v1/parser.hpp>
#include <mechanism_configuration/v1/types.hpp>

namespace py = pybind11;
using namespace mechanism_configuration::v1::types;

PYBIND11_MODULE(mechanism_configuration, m)
{
  py::class_<Species>(m, "Species")
      .def(py::init<>())
      .def_readwrite("name", &Species::name)
      .def_readwrite("optional_numerical_properties", &Species::optional_numerical_properties)
      .def_readwrite("unknown_properties", &Species::unknown_properties);

  py::class_<Phase>(m, "Phase")
      .def(py::init<>())
      .def_readwrite("name", &Phase::name)
      .def_readwrite("species", &Phase::species)
      .def_readwrite("unknown_properties", &Phase::unknown_properties);

  py::class_<ReactionComponent>(m, "ReactionComponent")
      .def(py::init<>())
      .def_readwrite("species_name", &ReactionComponent::species_name)
      .def_readwrite("coefficient", &ReactionComponent::coefficient)
      .def_readwrite("unknown_properties", &ReactionComponent::unknown_properties);

  py::class_<Arrhenius>(m, "Arrhenius")
      .def(py::init<>())
      .def_readwrite("A", &Arrhenius::A)
      .def_readwrite("B", &Arrhenius::B)
      .def_readwrite("C", &Arrhenius::C)
      .def_readwrite("D", &Arrhenius::D)
      .def_readwrite("E", &Arrhenius::E)
      .def_readwrite("reactants", &Arrhenius::reactants)
      .def_readwrite("products", &Arrhenius::products)
      .def_readwrite("name", &Arrhenius::name)
      .def_readwrite("gas_phase", &Arrhenius::gas_phase)
      .def_readwrite("unknown_properties", &Arrhenius::unknown_properties);

  py::class_<CondensedPhaseArrhenius>(m, "CondensedPhaseArrhenius")
      .def(py::init<>())
      .def_readwrite("A", &CondensedPhaseArrhenius::A)
      .def_readwrite("B", &CondensedPhaseArrhenius::B)
      .def_readwrite("C", &CondensedPhaseArrhenius::C)
      .def_readwrite("D", &CondensedPhaseArrhenius::D)
      .def_readwrite("E", &CondensedPhaseArrhenius::E)
      .def_readwrite("reactants", &CondensedPhaseArrhenius::reactants)
      .def_readwrite("products", &CondensedPhaseArrhenius::products)
      .def_readwrite("name", &CondensedPhaseArrhenius::name)
      .def_readwrite("aerosol_phase", &CondensedPhaseArrhenius::aerosol_phase)
      .def_readwrite("aerosol_phase_water", &CondensedPhaseArrhenius::aerosol_phase_water)
      .def_readwrite("unknown_properties", &CondensedPhaseArrhenius::unknown_properties);

  py::class_<Troe>(m, "Troe")
      .def(py::init<>())
      .def_readwrite("k0_A", &Troe::k0_A)
      .def_readwrite("k0_B", &Troe::k0_B)
      .def_readwrite("k0_C", &Troe::k0_C)
      .def_readwrite("kinf_A", &Troe::kinf_A)
      .def_readwrite("kinf_B", &Troe::kinf_B)
      .def_readwrite("kinf_C", &Troe::kinf_C)
      .def_readwrite("Fc", &Troe::Fc)
      .def_readwrite("N", &Troe::N)
      .def_readwrite("reactants", &Troe::reactants)
      .def_readwrite("products", &Troe::products)
      .def_readwrite("name", &Troe::name)
      .def_readwrite("gas_phase", &Troe::gas_phase)
      .def_readwrite("unknown_properties", &Troe::unknown_properties);

  py::class_<Branched>(m, "Branched")
      .def(py::init<>())
      .def_readwrite("X", &Branched::X)
      .def_readwrite("Y", &Branched::Y)
      .def_readwrite("a0", &Branched::a0)
      .def_readwrite("n", &Branched::n)
      .def_readwrite("reactants", &Branched::reactants)
      .def_readwrite("nitrate_products", &Branched::nitrate_products)
      .def_readwrite("alkoxy_products", &Branched::alkoxy_products)
      .def_readwrite("name", &Branched::name)
      .def_readwrite("gas_phase", &Branched::gas_phase)
      .def_readwrite("unknown_properties", &Branched::unknown_properties);

  py::class_<Tunneling>(m, "Tunneling")
      .def(py::init<>())
      .def_readwrite("A", &Tunneling::A)
      .def_readwrite("B", &Tunneling::B)
      .def_readwrite("C", &Tunneling::C)
      .def_readwrite("reactants", &Tunneling::reactants)
      .def_readwrite("products", &Tunneling::products)
      .def_readwrite("name", &Tunneling::name)
      .def_readwrite("gas_phase", &Tunneling::gas_phase)
      .def_readwrite("unknown_properties", &Tunneling::unknown_properties);

  py::class_<Surface>(m, "Surface")
      .def(py::init<>())
      .def_readwrite("reaction_probability", &Surface::reaction_probability)
      .def_readwrite("gas_phase_species", &Surface::gas_phase_species)
      .def_readwrite("gas_phase_products", &Surface::gas_phase_products)
      .def_readwrite("name", &Surface::name)
      .def_readwrite("gas_phase", &Surface::gas_phase)
      .def_readwrite("aerosol_phase", &Surface::aerosol_phase)
      .def_readwrite("unknown_properties", &Surface::unknown_properties);

  py::class_<Photolysis>(m, "Photolysis")
      .def(py::init<>())
      .def_readwrite("scaling_factor", &Photolysis::scaling_factor)
      .def_readwrite("reactants", &Photolysis::reactants)
      .def_readwrite("products", &Photolysis::products)
      .def_readwrite("name", &Photolysis::name)
      .def_readwrite("gas_phase", &Photolysis::gas_phase)
      .def_readwrite("unknown_properties", &Photolysis::unknown_properties);

  py::class_<CondensedPhasePhotolysis>(m, "CondensedPhasePhotolysis")
      .def(py::init<>())
      .def_readwrite("scaling_factor_", &CondensedPhasePhotolysis::scaling_factor_)
      .def_readwrite("reactants", &CondensedPhasePhotolysis::reactants)
      .def_readwrite("products", &CondensedPhasePhotolysis::products)
      .def_readwrite("name", &CondensedPhasePhotolysis::name)
      .def_readwrite("aerosol_phase", &CondensedPhasePhotolysis::aerosol_phase)
      .def_readwrite("aerosol_phase_water", &CondensedPhasePhotolysis::aerosol_phase_water)
      .def_readwrite("unknown_properties", &CondensedPhasePhotolysis::unknown_properties);

  py::class_<Emission>(m, "Emission")
      .def(py::init<>())
      .def_readwrite("scaling_factor", &Emission::scaling_factor)
      .def_readwrite("products", &Emission::products)
      .def_readwrite("name", &Emission::name)
      .def_readwrite("gas_phase", &Emission::gas_phase)
      .def_readwrite("unknown_properties", &Emission::unknown_properties);

  py::class_<FirstOrderLoss>(m, "FirstOrderLoss")
      .def(py::init<>())
      .def_readwrite("scaling_factor", &FirstOrderLoss::scaling_factor)
      .def_readwrite("reactants", &FirstOrderLoss::reactants)
      .def_readwrite("name", &FirstOrderLoss::name)
      .def_readwrite("gas_phase", &FirstOrderLoss::gas_phase)
      .def_readwrite("unknown_properties", &FirstOrderLoss::unknown_properties);

  py::class_<AqueousEquilibrium>(m, "AqueousEquilibrium")
      .def(py::init<>())
      .def_readwrite("name", &AqueousEquilibrium::name)
      .def_readwrite("gas_phase", &AqueousEquilibrium::gas_phase)
      .def_readwrite("aerosol_phase", &AqueousEquilibrium::aerosol_phase)
      .def_readwrite("aerosol_phase_water", &AqueousEquilibrium::aerosol_phase_water)
      .def_readwrite("reactants", &AqueousEquilibrium::reactants)
      .def_readwrite("products", &AqueousEquilibrium::products)
      .def_readwrite("A", &AqueousEquilibrium::A)
      .def_readwrite("C", &AqueousEquilibrium::C)
      .def_readwrite("k_reverse", &AqueousEquilibrium::k_reverse)
      .def_readwrite("unknown_properties", &AqueousEquilibrium::unknown_properties);

  py::class_<WetDeposition>(m, "WetDeposition")
      .def(py::init<>())
      .def_readwrite("scaling_factor", &WetDeposition::scaling_factor)
      .def_readwrite("name", &WetDeposition::name)
      .def_readwrite("aerosol_phase", &WetDeposition::aerosol_phase)
      .def_readwrite("unknown_properties", &WetDeposition::unknown_properties);

  py::class_<HenrysLaw>(m, "HenrysLaw")
      .def(py::init<>())
      .def_readwrite("name", &HenrysLaw::name)
      .def_readwrite("gas_phase", &HenrysLaw::gas_phase)
      .def_readwrite("gas_phase_species", &HenrysLaw::gas_phase_species)
      .def_readwrite("aerosol_phase", &HenrysLaw::aerosol_phase)
      .def_readwrite("aerosol_phase_water", &HenrysLaw::aerosol_phase_water)
      .def_readwrite("aerosol_phase_species", &HenrysLaw::aerosol_phase_species)
      .def_readwrite("unknown_properties", &HenrysLaw::unknown_properties);

  py::class_<SimpolPhaseTransfer>(m, "SimpolPhaseTransfer")
      .def(py::init<>())
      .def_readwrite("gas_phase", &SimpolPhaseTransfer::gas_phase)
      .def_readwrite("gas_phase_species", &SimpolPhaseTransfer::gas_phase_species)
      .def_readwrite("aerosol_phase", &SimpolPhaseTransfer::aerosol_phase)
      .def_readwrite("aerosol_phase_species", &SimpolPhaseTransfer::aerosol_phase_species)
      .def_readwrite("name", &SimpolPhaseTransfer::name)
      .def_readwrite("B", &SimpolPhaseTransfer::B)
      .def_readwrite("unknown_properties", &SimpolPhaseTransfer::unknown_properties);

  py::class_<Reactions>(m, "Reactions")
      .def(py::init<>())
      .def_readwrite("arrhenius", &Reactions::arrhenius)
      .def_readwrite("branched", &Reactions::branched)
      .def_readwrite("condensed_phase_arrhenius", &Reactions::condensed_phase_arrhenius)
      .def_readwrite("condensed_phase_photolysis", &Reactions::condensed_phase_photolysis)
      .def_readwrite("emission", &Reactions::emission)
      .def_readwrite("first_order_loss", &Reactions::first_order_loss)
      .def_readwrite("simpol_phase_transfer", &Reactions::simpol_phase_transfer)
      .def_readwrite("aqueous_equilibrium", &Reactions::aqueous_equilibrium)
      .def_readwrite("wet_deposition", &Reactions::wet_deposition)
      .def_readwrite("henrys_law", &Reactions::henrys_law)
      .def_readwrite("photolysis", &Reactions::photolysis)
      .def_readwrite("surface", &Reactions::surface)
      .def_readwrite("troe", &Reactions::troe)
      .def_readwrite("tunneling", &Reactions::tunneling);

  py::class_<Mechanism>(m, "Mechanism")
      .def(py::init<>())
      .def_readwrite("name", &Mechanism::name)
      .def_readwrite("species", &Mechanism::species)
      .def_readwrite("phases", &Mechanism::phases)
      .def_readwrite("reactions", &Mechanism::reactions);


  using V1Parser = mechanism_configuration::v1::Parser;

  py::class_<V1Parser>(m, "V1Parser")
      .def(py::init<>())
      .def("parse", &V1Parser::Parse);
}
