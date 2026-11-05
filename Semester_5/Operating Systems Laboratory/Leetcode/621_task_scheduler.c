int leastInterval(char *tasks, int tasksSize, int n) {
    int freq[26] = {0};

    for (int i = 0; i < tasksSize; i++) {
        freq[tasks[i] - 'A']++;
    }

    int maxFreq = 0, maxCount = 0;

    for (int i = 0; i < 26; i++) {
        if (freq[i] > maxFreq) {
            maxFreq = freq[i];
            maxCount = 1;
        } else if (freq[i] == maxFreq) {
            maxCount++;
        }
    }

    int minTime = (maxFreq - 1) * (n + 1) + maxCount;

    return tasksSize > minTime ? tasksSize : minTime;
}
