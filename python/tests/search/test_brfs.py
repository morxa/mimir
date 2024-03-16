from pymimir.formalism import PDDLParser
from pymimir.search import SSG, LiftedAAG, MinimalEventHandler, DebugEventHandler, BrFsAlgorithm, SearchStatus

from pathlib import Path

ROOT_DIR = (Path(__file__).parent.parent.parent.parent).absolute()


def test_brfs():
    """ Test parsing a PDDL domain and problem file.
    """
    domain_filepath = str(ROOT_DIR / "data" / "gripper" / "domain.pddl")
    problem_filepath = str(ROOT_DIR / "data" / "gripper" / "problem.pddl")
    parser = PDDLParser(domain_filepath, problem_filepath)
    ssg = SSG(parser.get_problem())
    aag = LiftedAAG(parser.get_problem(), parser.get_factories())
    event_handler = DebugEventHandler()
    brfs = BrFsAlgorithm(parser.get_problem(), parser.get_factories(), ssg, aag, event_handler)
    search_status, plan = brfs.find_solution()

    assert search_status == SearchStatus.SOLVED
    assert len(plan) == 5