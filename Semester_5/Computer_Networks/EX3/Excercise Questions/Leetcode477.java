class Leetcode477 {
    public int totalHammingDistance(int[] nums) {
        int ans = 0;
        int n = nums.length;

        for (int bit = 0; bit < 32; bit++) {
            int ones = 0;

            for (int num : nums) {
                if (((num >> bit) & 1) == 1)
                    ones++;
            }

            ans += ones * (n - ones);
        }

        return ans;
    }

    public static void main(String[] args) {
        Leetcode477 solution = new Leetcode477();

        // Test case 1
        int[] nums1 = { 4, 14, 2 };
        System.out.println("Input: [4, 14, 2]");
        System.out.println("Output: " + solution.totalHammingDistance(nums1));
        System.out.println("Expected: 6\n");

        // Test case 2
        int[] nums2 = { 1, 2, 3, 4, 5 };
        System.out.println("Input: [1, 2, 3, 4, 5]");
        System.out.println("Output: " + solution.totalHammingDistance(nums2));
        System.out.println("Expected: 10");
    }
}