"""
wordkit.text_metrics
~~~~~~~~~~~~~~~~~~~~
Comprehensive text analysis and metric extraction module.
Author: Ragul Ramachandran (24BCS174)
Course: 19CSP01 - Free Open Source Software
"""

import re
from collections import Counter


def word_count(text: str) -> int:
    """Return the total number of words in the given text."""
    if not text or not text.strip():
        return 0
    return len(text.split())


def unique_word_count(text: str) -> int:
    """Return the number of unique words, ignoring case and punctuation."""
    if not text or not text.strip():
        return 0
    words = [re.sub(r'^[^\w]+|[^\w]+$', '', w).lower() for w in text.split()]
    unique_words = {w for w in words if w}
    return len(unique_words)


def longest_word(text: str) -> str:
    """Return the longest word without surrounding punctuation."""
    if not text or not text.strip():
        return ""
    words = [re.sub(r'^[^\w]+|[^\w]+$', '', w) for w in text.split()]
    words = [w for w in words if w]
    return max(words, key=len) if words else ""


def average_word_length(text: str) -> float:
    """Calculate the average length of words in characters."""
    if not text or not text.strip():
        return 0.0
    words = [re.sub(r'^[^\w]+|[^\w]+$', '', w) for w in text.split()]
    words = [w for w in words if w]
    if not words:
        return 0.0
    total_chars = sum(len(w) for w in words)
    return round(total_chars / len(words), 2)


def character_count(text: str, ignore_spaces: bool = True) -> int:
    """Return total character count with or without whitespace."""
    if not text:
        return 0
    if ignore_spaces:
        return len(re.sub(r'\s+', '', text))
    return len(text)


def sentence_count(text: str) -> int:
    """Count total sentences based on common terminators (.!?)."""
    if not text or not text.strip():
        return 0
    sentences = re.split(r'[.!?]+', text.strip())
    sentences = [s.strip() for s in sentences if s.strip()]
    return len(sentences)


def word_frequency(text: str, top_n: int = 5) -> dict:
    """Return frequency of the top N most frequent words."""
    if not text or not text.strip():
        return {}
    words = [re.sub(r'^[^\w]+|[^\w]+$', '', w).lower() for w in text.split()]
    words = [w for w in words if w]
    return dict(Counter(words).most_common(top_n))
