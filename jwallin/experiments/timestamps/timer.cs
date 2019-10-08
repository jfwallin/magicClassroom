// A Hello World! program in C#.
//
using System;
using System.Threading;
//using DateTime;

namespace HelloWorld
{
    class Hello 
    {
        static void Main() 
        {
            Console.WriteLine("Hello World!");

            long milliseconds = DateTime.UtcNow.Ticks / TimeSpan.TicksPerMillisecond;
            long msUnix, msUnixStart;
            //double ms = DateTime.Now.Ticks/ 1000.;
            long millisecondsLocal = DateTime.Now.Ticks / TimeSpan.TicksPerMillisecond;
            System.DateTime  dtDateTime, dd1;
            int ttt ;

            string dateFormat = "MM/dd/yyyy hh:mm:ss"; 
          
            // Creating a DateTime object and  
            // taking a particular date and time 
            DateTime d1 = new DateTime(2019, 8, 14, 7, 0, 0); 
          
            Console.WriteLine("@@@@@@@@@@@@@@@");
            Console.WriteLine("Original date: {0}", d1.ToString(dateFormat)); 

            dtDateTime = new DateTime(1970,1,1,0,0,0,0,System.DateTimeKind.Utc);
            Console.WriteLine("Starting date: {0}", dtDateTime.ToString(dateFormat)); 

            msUnixStart = dtDateTime.Ticks / TimeSpan.TicksPerMillisecond;
            msUnix = milliseconds - msUnixStart;
            Console.WriteLine("MS time Unix = {0}", msUnix);


            dtDateTime = dtDateTime.AddSeconds(100);
            Console.WriteLine("Incremented date: {0}", dtDateTime.ToString(dateFormat)); 


            milliseconds = DateTime.Now.Ticks / TimeSpan.TicksPerMillisecond;
            ttt = (int) (milliseconds / 1000);
            

            DateTime foo = DateTime.UtcNow;
            long unixTime = ((DateTimeOffset)foo).ToUnixTimeSeconds();
            Console.WriteLine("new unix time = {0}", unixTime.ToString());


            for (int i = 0; i < 100; i++) {
            milliseconds = DateTime.Now.Ticks / TimeSpan.TicksPerMillisecond;
            ttt = (int) (milliseconds / 1000);
            Console.WriteLine("time :{0}", milliseconds);

            Thread.Sleep(1000);
            }

            
        }
    }


}
