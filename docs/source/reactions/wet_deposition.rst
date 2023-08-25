Wet Deposition
==============

Wet Deposition reactions take the form:

.. math::

   \mbox{X} \rightarrow

where X is a set of species in an aerosol phase
undergoing wet deposition at a given rate.

Wet deposition rate constants can be constant or set from an external
module using the ``camp_rxn_wet_deposition::rxn_update_data_wet_deposition_t``
object. External modules should use the
``camp_rxn_wet_deposition::rxn_wet_deposition_t::get_property_set()`` function
during initialization to access any needed reaction parameters to identify
certain wet deposition reactions. An ``camp_rxn_wet_deposition::update_data_wet_deposition_t``
object should be initialized for each wet deposition reaction. These objects can
then be used during solving to update the wet deposition rate from an external module.

Input data for wet deposition reactions have the following format:

.. tabs::

    .. tab:: YAML

        .. code-block:: yaml

            {
                "type": "WET_DEPOSITION",
                "aerosol phase": "my aero phase",
                "scaling factor": 1.2,
                ...
            }

    .. tab:: JSON

        .. code-block:: json

            {
                "type": "WET_DEPOSITION",
                "aerosol phase": "my aero phase",
                "scaling factor": 1.2,
            }

The key-value pair "aerosol phase" is required and its value must be
the name of the aerosol phase undergoing wet deposition. All species within
the aerosol phase in all instances of the aerosol phase will be removed
according to the first-order loss rate constant. The "scaling factor" is
optional and can be used to set a constant scaling factor for the rate constant.
When a "scaling factor" is not provided, it is assumed to be 1.0.
All other data is optional and will be available to external modules
during initialization. Rate constants are in units of s^{-1},
and must be set using a ``rxn_wet_deposition_update_data_t`` object.
