import java.util.Scanner;
import java.util.HashSet;
import java.util.Set;

class SubarrayOrsSolution
{
    public int subarrayBitwiseORs(int[] arr)
    {
        Set<Integer> result = new HashSet<>();
        Set<Integer> prev = new HashSet<>();

        for (int x : arr)
        {
            Set<Integer> curr = new HashSet<>();
            curr.add(x);

            for (int y : prev)
            {
                curr.add(y | x);
            }

            result.addAll(curr);
            prev = curr;
        }

        return result.size();
    }
}

public class SubarrayOrsProgram
{
    public static void main(String[] args)
    {
        Scanner scanner = new Scanner(System.in);
        System.out.print("Enter array size: ");
        int size = scanner.nextInt();
        int[] arr = new int[size];

        System.out.print("Enter array elements: ");
        for (int i = 0; i < size; i++)
        {
            arr[i] = scanner.nextInt();
        }

        SubarrayOrsSolution solver = new SubarrayOrsSolution();
        int result = solver.subarrayBitwiseORs(arr);
        System.out.println("Total unique subarray ORs: " + result);

        scanner.close();
    }
}
