// A Hello World! program in C#.
using System;
using DateTime;

namespace HelloWorld
{
    class Hello 
    {
        static void Main() 
        {
            Console.WriteLine("Hello World!");

            long milliseconds = DateTime.Now.Ticks / TimeSpan.TicksPerMillisecond;
            Console.Writeline("time :{0}", milliseconds);

            // Keep the console window open in debug mode.
  //            Console.WriteLine("Press any key to exit.");
   //         Console.ReadKey();
        }
    }
}
