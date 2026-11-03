public class HR_Maximizing_XOR {
    public static int maximizingXor(int l, int r) {
        int x = l ^ r;
        int ans = 1;

        while (x > 0) {
            ans <<= 1;
            x >>= 1;
        }

        return ans - 1;
    }

    public static void main(String[] args) {
        int l = 10;
        int r = 15;

        int result = maximizingXor(l, r);

        System.out.println("l = " + l);
        System.out.println("r = " + r);
        System.out.println("Maximum XOR = " + result);
    }
}