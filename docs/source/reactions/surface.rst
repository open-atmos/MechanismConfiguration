Surface (Heterogeneous) Reaction
================================

Surface reactions transform gas-phase species into gas-phase products according to a rate that is calculated based on the total exposed surface area of a condensed phase.

For surface reactions of the gas-phase species X, the reaction rate is calculated assuming large particles (continuum regime) as:

.. math::

   r_{surface} = k_{surface}[X]

where [X] is the gas-phase concentration of species X [ppm], and the rate constant \(k_{surface}\) [1/s] is calculated as:

.. math::

   k_{surface} = \frac{4N_a \pi r_e^2}{\left(\frac{r_e}{D_g} + \frac{4}{v(T)\gamma}\right)}

where \(N_a\) is the number concentration of particles [particles/m^3], \(r_e\) is the effective particle radius [m], \(D_g\) is the gas-phase diffusion coefficient of the reactant [m^2/s], \(\gamma\) is the reaction probability [unitless], and \(v\) is the mean free speed of the gas-phase reactant:

.. math::

   v = \sqrt{\frac{8RT}{\pi MW}}

where \(R\) is the ideal gas constant [J K\(^{-1}\) mol\(^{-1}\)], \(T\) is temperature [K], and \(MW\) is the molecular weight of the gas-phase reactant [kg mol\(^{-1}\)].

Input data for surface reactions have the following format:

.. tabs::

   .. tab:: YAML

      .. code-block:: yaml

         type : SURFACE
         gas-phase reactant : my gas species
         reaction probability : 0.2
         gas-phase products :
            my other gas species : { }
            another gas species : { "yield" : 0.3 }
         aerosol phase : my aqueous phase

   .. tab:: JSON

      .. code-block:: json

         {
           "type" : "SURFACE",
           "gas-phase reactant" : "my gas species",
           "reaction probability" : 0.2,
           "gas-phase products" : {
              "my other gas species" : { },
              "another gas species" : { "yield" : 0.3 }
           },
           "aerosol phase" : "my aqueous phase"
         }

The key-value pairs `gas-phase reactant`, `reaction probability`, and `aerosol phase` are required. Only one gas-phase reactant is allowed, but multiple products can be present. The key-value pair `yield` for product species is optional and defaults to 1.0.

The gas-phase reactant species must include properties `diffusion coeff` [m^2 s\(^{-1}\)], which specifies the diffusion coefficient in m^2 s\(^{-1}\), and `molecular weight` [kg mol\(^{-1}\)], which specifies the molecular weight of the species in kg mol\(^{-1}\).