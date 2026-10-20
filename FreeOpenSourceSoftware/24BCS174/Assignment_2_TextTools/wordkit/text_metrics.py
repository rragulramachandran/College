def word_count(text):
    """Return the number of words in a sentence."""
    return len(text.split())


def unique_word_count(text):
    """Return the number of different words, ignoring case."""
    words = {word.strip(".,!?;:").lower() for word in text.split()}
    return len(words - {""})
