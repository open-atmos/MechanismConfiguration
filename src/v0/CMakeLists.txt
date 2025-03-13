
target_sources(mechanism_configuration
  PRIVATE
    parser.cpp
    species_parser.cpp
    photolysis_parser.cpp
    emission_parser.cpp
    first_order_loss_parser.cpp
    arrhenius_parser.cpp
    troe_parser.cpp
    ternary_chemical_activation_parser.cpp
    branched_parser.cpp
    tunneling_parser.cpp
    surface_parser.cpp
    user_defined_reaction_parser.cpp
    export.cpp
)