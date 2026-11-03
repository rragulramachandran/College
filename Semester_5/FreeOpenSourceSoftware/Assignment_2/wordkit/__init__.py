"""
wordkit package
~~~~~~~~~~~~~~~
A lightweight, modular Python package for text analysis and metrics extraction.
Developed for 19CSP01 - Free Open Source Software.
Student: Ragul Ramachandran (24BCS174)
"""

from .text_metrics import (
    average_word_length,
    character_count,
    longest_word,
    sentence_count,
    unique_word_count,
    word_count,
    word_frequency,
)

__version__ = "1.0.0"
__author__ = "Ragul Ramachandran"
__all__ = [
    "word_count",
    "unique_word_count",
    "longest_word",
    "average_word_length",
    "character_count",
    "sentence_count",
    "word_frequency",
]
