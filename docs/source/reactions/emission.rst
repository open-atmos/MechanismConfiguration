Emission
========

Emission reactions take the form:

.. math::

   \rightarrow \ce{X}

where :math:`\ce{X}` is the species being emitted.

Emission rates can be constant or set from an external module using the
``camp_rxn_emission::rxn_update_data_emission_t`` object. External modules can use the
``camp_rxn_emission::rxn_emission_t::get_property_set()`` function during initialization to access any needed reaction parameters to identify certain emission reactions.
An ``camp_rxn_emission::update_data_emission_t`` object should be initialized for each emissions reaction. These objects can then be used during solving to update the emission rate from an external module.

Input data for emission reactions have the following format:

.. tabs::

    .. tab:: YAML

        .. code-block:: yaml

            type: EMISSION
            species: species_name
            scaling factor: 1.2
            ...

    .. tab:: JSON

        .. code-block:: json

            {
                "type": "EMISSION",
                "species": "species_name",
                "scaling factor": 1.2,
            }

The key-value pair ``species`` is required, and its value must be the name of the species being emitted. The ``scaling factor`` is optional and can be used to set a constant scaling factor for the rate. When a ``scaling factor`` is not provided, it is assumed to be 1.0. All other data is optional and will be available to external modules during initialization. Rates are in units of concentration units per second (\( \text{concentration units} \, \text{s}^{-1} \)) and must be set using a ``rxn_update_data_emission_t`` object.
