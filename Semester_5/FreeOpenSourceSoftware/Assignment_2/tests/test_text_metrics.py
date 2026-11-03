import unittest
from wordkit import (
    average_word_length,
    character_count,
    longest_word,
    sentence_count,
    unique_word_count,
    word_count,
    word_frequency,
)


class TestTextMetrics(unittest.TestCase):
    def setUp(self):
        self.text = "Open source software is free and open to all."

    def test_word_count(self):
        self.assertEqual(word_count(self.text), 9)

    def test_unique_word_count(self):
        self.assertEqual(unique_word_count(self.text), 8)

    def test_longest_word(self):
        self.assertEqual(longest_word(self.text), "software")

    def test_average_word_length(self):
        self.assertGreater(average_word_length(self.text), 3.0)

    def test_character_count(self):
        self.assertEqual(character_count("abc def", ignore_spaces=True), 6)
        self.assertEqual(character_count("abc def", ignore_spaces=False), 7)

    def test_sentence_count(self):
        self.assertEqual(sentence_count("Hello world. How are you? Fine!"), 3)

    def test_word_frequency(self):
        freq = word_frequency("apple banana apple orange apple")
        self.assertEqual(freq["apple"], 3)


if __name__ == "__main__":
    unittest.main()
