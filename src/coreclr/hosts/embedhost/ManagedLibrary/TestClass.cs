using System;
using System.Runtime.CompilerServices;

namespace ManagedLibrary
{
    public class TestClass
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern string GetExternalString();

        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern int GetExternalSum(int a, int b);

        static void TestMethod()
        {
            Console.WriteLine("Hello world!");

            try
            {
                Console.WriteLine("Give me some numbers:");

                Console.Write("A = ");
                int a = Convert.ToInt32(Console.ReadLine());

                Console.Write("B = ");
                int b = Convert.ToInt32(Console.ReadLine());

                int sum = GetExternalSum(a, b);

                Console.WriteLine("The sum is: " + sum.ToString());
            }
            catch
            {
                Console.WriteLine("I asked for numbers :(");
            }

            string externalString = GetExternalString();

            Console.WriteLine("External string: \"" + externalString + "\"");
        }
    }
}
