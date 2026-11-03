#!/usr/bin/env python3
"""Build and integration-test in a temporary directory; retain no executables."""
import concurrent.futures
import contextlib
import os
from pathlib import Path
import socket
import subprocess
import tempfile
import threading
import time
import unittest

ROOT = Path(__file__).resolve().parents[1]
BUILD = None


class Peer:
    def __init__(self, port):
        self.sock = socket.create_connection(("127.0.0.1", port), timeout=8)
        self.file = self.sock.makefile("rb")
        self.greeting = self.line()
        assert self.greeting.startswith("OK "), self.greeting

    def send(self, text):
        self.sock.sendall(text.encode() + b"\n")

    def line(self):
        raw = self.file.readline()
        if not raw:
            raise EOFError("Connection closed")
        assert raw.endswith(b"\n"), raw
        return raw.decode().rstrip("\r\n")

    def request(self, text):
        self.send(text)
        return self.line()

    def block(self, text):
        self.send(text)
        result = []
        while (line := self.line()) != "END":
            result.append(line)
        return result

    def close(self):
        self.file.close()
        self.sock.close()

    def __enter__(self):
        return self

    def __exit__(self, *_):
        self.close()


class Server:
    def __init__(self, service, cwd, *args):
        self.service, self.cwd, self.args = service, cwd, args
        self.process = None

    def start(self):
        with socket.socket() as probe:
            probe.bind(("127.0.0.1", 0))
            self.port = probe.getsockname()[1]
        self.output = open(self.cwd / f"{self.service}.process.log", "ab")
        self.process = subprocess.Popen(
            [str(BUILD / f"{self.service}_server"), str(self.port), *map(str, self.args)],
            cwd=self.cwd, stdout=self.output, stderr=self.output,
        )
        for _ in range(200):
            if self.process.poll() is not None:
                raise AssertionError((self.cwd / f"{self.service}.process.log").read_text())
            try:
                with socket.create_connection(("127.0.0.1", self.port), timeout=.1):
                    return self
            except OSError:
                time.sleep(.01)
        raise AssertionError("Server did not start")

    def stop(self):
        if self.process is not None:
            self.process.terminate()
            try:
                self.process.wait(timeout=10)
            except subprocess.TimeoutExpired:
                self.process.kill()
                self.process.wait()
            self.output.close()
            diagnostics = (self.cwd / f"{self.service}.process.log").read_text()
            assert "ERROR: AddressSanitizer" not in diagnostics, diagnostics
            assert "runtime error:" not in diagnostics, diagnostics
            self.process = None

    def __enter__(self):
        return self.start()

    def __exit__(self, *_):
        self.stop()


class Systems(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix="ex7-test-data-")
        self.cwd = Path(self.temp.name)

    def tearDown(self):
        self.temp.cleanup()

    def test_exam_authentication_and_evaluation(self):
        with Server("examination", self.cwd) as server, Peer(server.port) as a, Peer(server.port) as b:
            self.assertEqual(a.request("QUESTIONS"), "ERR Authenticate first")
            self.assertTrue(a.request("LOGIN student1 wrong").startswith("ERR"))
            self.assertTrue(a.request("LOGINstudent1 exam1").startswith("ERR"))
            self.assertEqual(a.request("LOGIN student1 exam1"), "OK Authenticated")
            self.assertTrue(b.request("LOGIN student1 exam1").startswith("ERR"))
            self.assertEqual(len(a.block("QUESTIONS")), 4)
            self.assertTrue(a.request("SCORE").startswith("ERR"))
            for request in ("ANSWER 99999999999999999999999 A", "ANSWER 0 A", "ANSWER 1 Z", "ANSWER 1 A extra"):
                self.assertTrue(a.request(request).startswith("ERR"))
            for i, answer in enumerate("BCA", 1):
                self.assertEqual(a.request(f"ANSWER {i} {answer}"), "OK Answer saved")
            self.assertEqual(a.request("FINISH"), "OK SCORE 3/3")
            self.assertEqual(a.request("SCORE"), "OK SCORE 3/3")
            self.assertTrue(a.request("ANSWER 1 A").startswith("ERR"))
            self.assertEqual(a.request("FINISH"), "OK SCORE 3/3")
        log = (self.cwd / "examination.log").read_text()
        self.assertNotIn("exam1", log)
        self.assertNotIn("wrong", log)
        self.assertRegex(log, r"\d{4}-\d\d-\d\dT\d\d:\d\d:\d\d")
        self.assertIn("score=3/3", log)

    def test_exam_concurrent_sessions_and_disconnect(self):
        with Server("examination", self.cwd) as server:
            barrier = threading.Barrier(4)
            def student(i):
                with Peer(server.port) as peer:
                    self.assertEqual(peer.request(f"LOGIN student{i} exam{i}"), "OK Authenticated")
                    barrier.wait(timeout=5)
                    for j, answer in enumerate("BCA" if i % 2 else "AAA", 1):
                        peer.request(f"ANSWER {j} {answer}")
                    return peer.request("FINISH")
            with concurrent.futures.ThreadPoolExecutor(4) as pool:
                scores = list(pool.map(student, range(1, 5)))
            self.assertEqual(scores, ["OK SCORE 3/3", "OK SCORE 1/3"] * 2)
            with Peer(server.port) as peer:
                for _ in range(100):
                    result = peer.request("LOGIN student1 exam1")
                    if result == "OK Authenticated":
                        break
                    time.sleep(.01)
                self.assertEqual(result, "OK Authenticated")
                self.assertEqual(peer.request("FINISH"), "OK SCORE 0/3")

    def test_hotel_race_persistence_and_cancellation(self):
        journal = self.cwd / "bookings.db"
        server = Server("hotel", self.cwd, journal)
        with server:
            barrier = threading.Barrier(16)
            def book(i):
                with Peer(server.port) as peer:
                    barrier.wait(timeout=5)
                    return peer.request(f"BOOK 1 guest{i}")
            with concurrent.futures.ThreadPoolExecutor(16) as pool:
                results = list(pool.map(book, range(16)))
            winners = [r for r in results if r.startswith("OK BOOKED")]
            self.assertEqual(len(winners), 1)
            self.assertEqual(results.count("ERR Room already booked"), 15)
            token = winners[0].split()[-1]
            with Peer(server.port) as peer:
                self.assertIn("ROOM 1 BOOKED", peer.block("AVAILABLE"))
                self.assertTrue(peer.request("DETAILS " + token).startswith("OK BOOKING 1 guest"))
                for bad in ("BOOK 0 x", "BOOK 1 x extra", "BOOK 999999999999999999 x", "BOOK1 x", "CANCEL bogus", "DETAILS" + token):
                    self.assertTrue(peer.request(bad).startswith("ERR"), bad)
        with server:
            with Peer(server.port) as peer:
                self.assertTrue(peer.request("DETAILS " + token).startswith("OK BOOKING 1"))
                self.assertEqual(peer.request("CANCEL " + token), "OK Cancelled")
                self.assertEqual(peer.request("DETAILS " + token), "ERR Reservation not found")
        with server:
            with Peer(server.port) as peer:
                self.assertIn("ROOM 1 FREE", peer.block("AVAILABLE"))
        self.assertEqual(len(journal.read_text().splitlines()), 2)
        self.assertIn("request=BOOK", (self.cwd / "hotel.log").read_text())

    def test_hotel_exclusive_journal_and_corruption(self):
        journal = self.cwd / "bookings.db"
        with Server("hotel", self.cwd, journal):
            result = subprocess.run([str(BUILD / "hotel_server"), "54321", str(journal)], cwd=self.cwd, capture_output=True, timeout=5)
            self.assertNotEqual(result.returncode, 0)
        journal.write_text("BOOK 1 truncated")
        result = subprocess.run([str(BUILD / "hotel_server"), "54321", str(journal)], cwd=self.cwd, capture_output=True, timeout=5)
        self.assertNotEqual(result.returncode, 0)
        self.assertIn(b"Invalid or incomplete", result.stderr)

    def test_files_parallel_binary_empty_and_access(self):
        shared = self.cwd / "shared"
        shared.mkdir()
        payload = bytes(range(256)) * 4096
        (shared / "sample.bin").write_bytes(payload)
        (shared / "empty.txt").touch()
        (self.cwd / "private.txt").write_text("secret")
        (shared / "link.txt").symlink_to(self.cwd / "private.txt")
        os.mkfifo(shared / "pipe")
        with Server("files", self.cwd, shared) as server:
            def download(i):
                with Peer(server.port) as peer:
                    self.assertEqual(set(peer.block("LIST")), {"OK FILES", "FILE sample.bin", "FILE empty.txt"})
                    name, data = ("sample.bin", payload) if i % 2 else ("empty.txt", b"")
                    self.assertEqual(peer.request("GET " + name), f"DATA {len(data)}")
                    self.assertEqual(peer.file.read(len(data)), data)
                    self.assertEqual(peer.request("QUIT"), "OK Goodbye")
            with concurrent.futures.ThreadPoolExecutor(12) as pool:
                list(pool.map(download, range(12)))
            with Peer(server.port) as peer:
                for name in ("../private.txt", "/etc/passwd", "link.txt", "pipe", "missing", "sample.bin extra"):
                    self.assertTrue(peer.request("GET " + name).startswith("ERR"))
            destination = self.cwd / "download.bin"
            args = [str(BUILD / "files_client"), "127.0.0.1", str(server.port)]
            result = subprocess.run(args + ["GET", "sample.bin", str(destination)], capture_output=True, timeout=8)
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertEqual(destination.read_bytes(), payload)
            result = subprocess.run(args + ["GET", "empty.txt", str(destination)], capture_output=True, timeout=8)
            self.assertNotEqual(result.returncode, 0)
            self.assertEqual(destination.read_bytes(), payload)
            result = subprocess.run(args + ["LIST"], capture_output=True, timeout=8)
            self.assertEqual(set(result.stdout.splitlines()), {b"sample.bin", b"empty.txt"})
            # An aborted download must not stop subsequent clients.
            with Peer(server.port) as peer:
                peer.request("GET sample.bin")
            with Peer(server.port) as peer:
                self.assertEqual(peer.request("GET empty.txt"), "DATA 0")

    def test_file_client_removes_truncated_download(self):
        with socket.socket() as listener:
            listener.bind(("127.0.0.1", 0))
            listener.listen()
            def truncated_peer():
                conn, _ = listener.accept()
                with conn:
                    conn.sendall(b"OK File sharing\n")
                    conn.recv(1024)
                    conn.sendall(b"DATA 10\nshort")
            thread = threading.Thread(target=truncated_peer)
            thread.start()
            output = self.cwd / "partial.bin"
            result = subprocess.run([str(BUILD / "files_client"), "127.0.0.1", str(listener.getsockname()[1]), "GET", "file", str(output)], capture_output=True, timeout=8)
            thread.join(timeout=5)
            self.assertNotEqual(result.returncode, 0)
            self.assertFalse(output.exists())

    def test_chat_concurrent_broadcast_and_disconnect(self):
        with Server("chat", self.cwd) as server, contextlib.ExitStack() as stack:
            peers = []
            for i in range(8):
                peer = stack.enter_context(Peer(server.port))
                self.assertEqual(peer.request(f"JOIN user{i}"), f"OK Joined user{i}")
                for earlier in peers:
                    self.assertEqual(earlier.line(), f"NOTICE user{i} joined")
                peers.append(peer)
            with Peer(server.port) as duplicate:
                self.assertTrue(duplicate.request("JOIN user0").startswith("ERR"))
                self.assertTrue(duplicate.request("MSG no-login").startswith("ERR"))
            barrier = threading.Barrier(8)
            def message(i):
                barrier.wait(timeout=5)
                peers[i].send(f"MSG hello{i}")
                received = {peers[i].line() for _ in range(8)}
                expected = {f"MESSAGE user{j} hello{j}" for j in range(8) if j != i} | {"OK Sent"}
                self.assertEqual(received, expected)
            with concurrent.futures.ThreadPoolExecutor(8) as pool:
                list(pool.map(message, range(8)))
            peers[-1].close()
            for peer in peers[:-1]:
                self.assertEqual(peer.line(), "NOTICE user7 left")
            peers[0].send("MSG still here")
            self.assertEqual(peers[0].line(), "OK Sent")
            for peer in peers[1:-1]:
                self.assertEqual(peer.line(), "MESSAGE user0 still here")
            self.assertEqual(set(peers[0].block("WHO")), {"OK USERS"} | {f"USER user{i}" for i in range(7)})

    def test_malformed_lines_and_fragmented_requests(self):
        shared = self.cwd / "shared"
        shared.mkdir()
        for service, args in (("examination", ()), ("hotel", (self.cwd / "journal",)), ("files", (shared,)), ("chat", ())):
            with self.subTest(service=service), Server(service, self.cwd, *args) as server:
                for payload in (b"X" * 1100 + b"\n", b"BAD\x00DATA\n", b"BAD\rX\n", b"partial"):
                    with Peer(server.port) as peer:
                        peer.sock.sendall(payload)
                        peer.sock.shutdown(socket.SHUT_WR)
                        self.assertTrue(peer.line().startswith("ERR"))
                with Peer(server.port) as peer:
                    for byte in b"QUIT\r\n":
                        peer.sock.sendall(bytes([byte]))
                    self.assertEqual(peer.line(), "OK Goodbye")

    def test_terminal_clients(self):
        for service, args, script, expected in (
            ("examination", (), "LOGIN student1 exam1\nFINISH\nQUIT\n", b"OK SCORE 0/3"),
            ("hotel", (self.cwd / "journal",), "AVAILABLE\nQUIT\n", b"ROOM 10 FREE"),
            ("chat", (), "JOIN terminal\nMSG hello\nQUIT\n", b"OK Sent"),
        ):
            with self.subTest(service=service), Server(service, self.cwd, *args) as server:
                result = subprocess.run([str(BUILD / f"{service}_client"), "127.0.0.1", str(server.port)], input=script.encode(), capture_output=True, timeout=8)
                self.assertEqual(result.returncode, 0, result.stderr)
                self.assertIn(expected, result.stdout)


if __name__ == "__main__":
    with tempfile.TemporaryDirectory(prefix="ex7-build-") as build:
        BUILD = Path(build)
        flags = "-std=c11 -Wall -Wextra -Wpedantic -Werror -O1 -g"
        if os.environ.get("SANITIZE") == "1":
            flags += " -fsanitize=address,undefined -fno-omit-frame-pointer"
        subprocess.run(["make", "all", f"BUILD_DIR={BUILD}", f"CFLAGS={flags}"], cwd=ROOT, check=True)
        unittest.main(verbosity=2)
