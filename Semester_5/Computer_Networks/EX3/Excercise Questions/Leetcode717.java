class Leetcode717 {
    public boolean isOneBitCharacter(int[] bits) {
        int i = 0;
        while (i < bits.length - 1) {
            if (bits[i] == 1) {
                i += 2;
            } else {
                i++;
            }
        }

        return i == bits.length - 1;

    }

    public static void main(String[] args) {
        Leetcode717 solution = new Leetcode717();

        // Test case 1
        int[] bits1 = {1, 0, 0};
        System.out.println("Input: [1, 0, 0]");
        System.out.println("Output: " + solution.isOneBitCharacter(bits1));
        System.out.println("Expected: true\n");

        // Test case 2
        int[] bits2 = {1, 1, 1, 0};
        System.out.println("Input: [1, 1, 1, 0]");
        System.out.println("Output: " + solution.isOneBitCharacter(bits2));
        System.out.println("Expected: false");
    }
}