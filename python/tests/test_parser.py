import pytest
from mechanism_configuration import Parser, ReactionType


def test_parse_full_v1_configuration():
    parser = Parser()
    extensions = [".yaml", ".json"]
    for extension in extensions:
        path = f"examples/v1/full_configuration{extension}"
        mechanism = parser.parse(path)
        assert mechanism is not None
        assert mechanism.name == "Full Configuration"
        assert len(mechanism.species) == 11
        assert len(mechanism.phases) == 4
        assert len(mechanism.reactions.aqueous_equilibrium) == 1
        assert len(mechanism.reactions.arrhenius) == 2
        assert len(mechanism.reactions.branched) == 1
        assert len(mechanism.reactions.condensed_phase_arrhenius) == 2
        assert len(mechanism.reactions.condensed_phase_photolysis) == 1
        assert len(mechanism.reactions.emission) == 1
        assert len(mechanism.reactions.first_order_loss) == 1
        assert len(mechanism.reactions.henrys_law) == 1
        assert len(mechanism.reactions.photolysis) == 1
        assert len(mechanism.reactions.simpol_phase_transfer) == 1
        assert len(mechanism.reactions.surface) == 1
        assert len(mechanism.reactions.troe) == 1
        assert len(mechanism.reactions.tunneling) == 1
        assert mechanism.version.major == 1
        assert mechanism.version.minor == 0
        assert mechanism.version.patch == 0
        assert (len(mechanism.reactions) == 16)
        for reaction in mechanism.reactions:
            assert reaction is not None
            assert isinstance(reaction.type, ReactionType)
        

def test_parser_reports_bad_files():
    parser = Parser()
    extensions = [".yaml", ".json"]
    for extension in extensions:
        path = f"examples/_missing_configuration{extension}"
        with pytest.raises(Exception):
            parser.parse(path)

