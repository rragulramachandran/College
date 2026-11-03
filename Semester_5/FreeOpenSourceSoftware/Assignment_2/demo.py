from wordkit import (
    average_word_length,
    character_count,
    longest_word,
    sentence_count,
    unique_word_count,
    word_count,
    word_frequency,
)

sample_text = (
    "Open Source Software gives students the freedom to learn, inspect, "
    "modify, and improve code. FOSS principles foster collaboration across "
    "the global software engineering community."
)

print("=" * 70)
print("WORDKIT TEXT ANALYSIS PACKAGE DEMO (24BCS174 - Ragul Ramachandran)")
print("=" * 70)
print(f"Sample Input Text:\n'{sample_text}'\n")
print(f"[-] Total Word Count        : {word_count(sample_text)}")
print(f"[-] Unique Word Count       : {unique_word_count(sample_text)}")
print(f"[-] Longest Word            : '{longest_word(sample_text)}' ({len(longest_word(sample_text))} chars)")
print(f"[-] Average Word Length     : {average_word_length(sample_text)} chars")
print(f"[-] Character Count (no sp) : {character_count(sample_text, ignore_spaces=True)}")
print(f"[-] Total Character Count   : {character_count(sample_text, ignore_spaces=False)}")
print(f"[-] Total Sentences         : {sentence_count(sample_text)}")
print(f"[-] Top Word Frequencies    : {word_frequency(sample_text, top_n=4)}")
print("=" * 70)
print("Execution completed successfully.")
