def word_count(text):
    """Return the number of words in a sentence."""
    return len(text.split())


def unique_word_count(text):
    """Return the number of different words, ignoring case."""
    words = {word.strip(".,!?;:").lower() for word in text.split()}
    return len(words - {""})


def longest_word(text):
    """Return the longest word without surrounding punctuation."""
    words = [word.strip(".,!?;:") for word in text.split()]
    words = [word for word in words if word]
    return max(words, key=len) if words else ""
