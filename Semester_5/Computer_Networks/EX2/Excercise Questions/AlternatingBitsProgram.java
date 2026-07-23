import java.util.Scanner;

class AlternatingBitsSolution
{
    public boolean hasAlternatingBits(int n)
    {
        int xor = n ^ (n >> 1);
        return (xor & (xor + 1)) == 0;
    }
}

public class AlternatingBitsProgram
{
    public static void main(String[] args)
    {
        Scanner scanner = new Scanner(System.in);
        System.out.print("Enter an integer: ");
        int n = scanner.nextInt();

        AlternatingBitsSolution solver = new AlternatingBitsSolution();
        boolean result = solver.hasAlternatingBits(n);
        System.out.println("Result: " + result);

        scanner.close();
    }
}
