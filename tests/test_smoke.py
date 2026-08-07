import subprocess, unittest
from pathlib import Path

REPO = Path("/home/sin/Projects/ringworld-redux")
BIN = REPO / "build/ringworld-redux-test"


class TestRingworldRedux(unittest.TestCase):
    def test_binary_exists(self):
        self.assertTrue(BIN.exists(), f"{BIN} missing")

    def test_build_succeeds(self):
        proc = subprocess.run(["cmake", "--build", str(REPO / "build")], capture_output=True, text=True)
        self.assertEqual(proc.returncode, 0, proc.stderr)

    def test_runtime_smoke(self):
        proc = subprocess.run(
            ["timeout", "15s", str(BIN)],
            capture_output=True, text=True,
        )
        out = (proc.stdout + proc.stderr).lower()
        self.assertIn("health", out)
        self.assertIn("shield", out)
        self.assertIn("ammo", out)
        self.assertIn("shutting down", out)

    def test_renderer_backend_present(self):
        code = (REPO / "src/renderer/te_renderer_backend.cpp").read_text()
        self.assertIn("TERendererBackend", code)
        self.assertIn("initialize", code)

    def test_controller_present(self):
        code = (REPO / "src/player/controller.cpp").read_text()
        self.assertIn("PlayerController", code)
        self.assertIn("set_key_state", code)

    def test_mission_objectives_present(self):
        code = (REPO / "src/core/game.cpp").read_text()
        self.assertIn("update_campaign", code)
        self.assertIn("completed()", code)


if __name__ == "__main__":
    unittest.main()
