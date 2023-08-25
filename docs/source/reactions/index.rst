#########
Reactions
#########

These reactions are implemented in a few chemical solvers that read version |version| of the mechanism configuraiton.

====================================== ============================================= ======================================= ===============================================
Reaction Type                          `CAMP <https://github.com/open-atmos/camp>`_  `MICM <https://github.com/NCAR/micm>`_  `TCHEM <https://github.com/sandialabs/TChem>`_ 
====================================== ============================================= ======================================= ===============================================
arrhenius                              |:white_check_mark:|                          |:white_check_mark:|                     |:question:|                                   
aqueous_equilibrium                    |:white_check_mark:|                          |:x:|                                    |:question:|                                   
branched                               |:white_check_mark:|                          |:white_check_mark:|                     |:question:|                                   
cmaq_H2O2                              |:white_check_mark:|                          |:x:|                                    |:question:|                                   
cmaq_OH_HNO3                           |:white_check_mark:|                          |:x:|                                    |:question:|                                   
condensed_phase_arrhenius              |:white_check_mark:|                          |:x:|                                    |:question:|                                   
condensed_phase_photolysis             |:white_check_mark:|                          |:x:|                                    |:question:|                                   
emission                               |:white_check_mark:|                          |:x:|                                    |:question:|                                   
first_order_loss                       |:white_check_mark:|                          |:x:|                                    |:question:|                                   
hl_phase_transfer                      |:white_check_mark:|                          |:x:|                                    |:question:|                                   
photolysis                             |:white_check_mark:|                          |:white_check_mark:|                     |:question:|                                   
simpol_phase_transfer                  |:white_check_mark:|                          |:x:|                                    |:question:|                                   
surface                                |:white_check_mark:|                          |:white_check_mark:|                     |:question:|                                   
ternary_chemical_activation            |:white_check_mark:|                          |:x:|                                    |:question:|                                   
troe                                   |:white_check_mark:|                          |:white_check_mark:|                     |:question:|                                   
tunneling                              |:white_check_mark:|                          |:white_check_mark:|                     |:question:|                                   
wet_deposition                         |:white_check_mark:|                          |:x:|                                    |:question:|                                   
====================================== ============================================= ======================================= ===============================================



.. toctree::
   :maxdepth: 2
   :caption: Contents:

   aqueous_equilibrium
   arrhenius
   branched
   cmaq_H2O2
   cmaq_OH_HNO3
   condensed_phase_arrhenius
   condensed_phase_photolysis
   emission
   first_order_loss
   hl_phase_transfer
   photolysis
   simpol_phase_transfer
   surface
   ternary_chemical_activation
   troe
   tunneling
   wet_deposition
