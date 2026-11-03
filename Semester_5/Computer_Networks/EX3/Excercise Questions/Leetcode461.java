class Leetcode461 {
    public int hammingDistance(int x, int y) {
        int xor = x ^ y;
        int count = 0;

        while (xor != 0) {
            xor &= (xor - 1);
            count++;
        }

        return count;
    }

    public static void main(String[] args) {
        Leetcode461 solution = new Leetcode461();

        // Test case 1
        int x1 = 1;
        int y1 = 4;
        System.out.println("Input: x = 1, y = 4");
        System.out.println("Output: " + solution.hammingDistance(x1, y1));
        System.out.println("Expected: 2\n");

        // Test case 2
        int x2 = 3;
        int y2 = 1;
        System.out.println("Input: x = 3, y = 1");
        System.out.println("Output: " + solution.hammingDistance(x2, y2));
        System.out.println("Expected: 1");
    }
}
