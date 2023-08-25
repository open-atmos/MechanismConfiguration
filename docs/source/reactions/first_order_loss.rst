First-Order Loss
================

First-Order Loss reactions take the form:

.. math::

   \ce{X} \rightarrow

where :math:`\ce{X}` is the species being lost.

First-Order loss rate constants can be constant or set from an external module using the
``camp_rxn_first_order_loss::rxn_update_data_first_order_loss_t`` object.
External modules can use the
``camp_rxn_first_order_loss::rxn_first_order_loss_t::get_property_set()`` function during initialization to access any needed reaction parameters to identify certain first-order loss reactions.
An ``camp_rxn_first_order_loss::update_data_first_order_loss_t`` object should be initialized for each reaction. These objects can then be used during solving to update the first order loss rate from an external module.

Input data for first-order loss reactions have the following format:

.. tabs::

    .. tab:: YAML

        .. code-block:: yaml

            type: FIRST_ORDER_LOSS
            species: species_name
            scaling factor: 1.2
            ...

    .. tab:: JSON

        .. code-block:: json

            {
                "type": "FIRST_ORDER_LOSS",
                "species": "species_name",
                "scaling factor": 1.2,
            }

The key-value pair ``species`` is required, and its value must be the name of the species being removed by the reaction. The ``scaling factor`` is optional and can be used to set a constant scaling factor for the rate constant. When a ``scaling factor`` is not provided, it is assumed to be 1.0. All other data is optional and will be available to external modules during initialization. Rate constants are in units of :math:`(s^{-1})`, and must be set using a ``rxn_first_order_loss_update_data_t`` object.
