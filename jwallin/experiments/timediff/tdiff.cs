
// A Hello World! program in C#.
//
using System;
using System.Threading;
using System.Net;
using System.IO;
//using DateTime;


namespace HelloWorld
{
    class Hello 
    {
        static void Main() 
        {
          HttpWebRequest http; 
          WebResponse response;

          Stream stream;
          StreamReader sr ;
          string content;

          long tserver;
          long timediff;
          
/*          HttpWebRequest http = (HttpWebRequest)WebRequest.Create("http://www.cs.mtsu.edu/~jwallin/ML/timestamp.php");
          WebResponse response = http.GetResponse();

          Stream stream = response.GetResponseStream();
          StreamReader sr = new StreamReader(stream);
          string content = sr.ReadToEnd();

          Console.WriteLine(content);
          tserver = Convert.ToInt64( content);

          response.Close();
          Console.WriteLine("---------");          
          
          Console.WriteLine("Hello World!");
*/

            long milliseconds = DateTime.UtcNow.Ticks / TimeSpan.TicksPerMillisecond;
            long msUnix, msUnixStart;
            //double ms = DateTime.Now.Ticks/ 1000.;
            long millisecondsLocal = DateTime.Now.Ticks / TimeSpan.TicksPerMillisecond;
            System.DateTime  dtDateTime ;
            int ttt ;

          
            // Creating a DateTime object and  
            // taking a particular date and time 

            dtDateTime = new DateTime(1970,1,1,0,0,0,0,System.DateTimeKind.Utc);
            msUnixStart = dtDateTime.Ticks / TimeSpan.TicksPerMillisecond;


                          for (int i = 0; i < 100; i++) {
              http = (HttpWebRequest)WebRequest.Create("http://www.cs.mtsu.edu/~jwallin/ML/timestamp.php");
              response = http.GetResponse();

              stream = response.GetResponseStream();
              sr = new StreamReader(stream);
              content = sr.ReadToEnd();

              Console.WriteLine(content);
              tserver = Convert.ToInt64( content);

              response.Close();
              
              //milliseconds = DateTime.Now.Ticks / TimeSpan.TicksPerMillisecond;
              //ttt = (int) (milliseconds / 1000);
              //msUnix = milliseconds - msUnixStart + (3600*5*1000);

              milliseconds = DateTime.UtcNow.Ticks / TimeSpan.TicksPerMillisecond;
              ttt = (int) (milliseconds / 1000);
              msUnix = milliseconds - msUnixStart;  // + (3600*5*1000);
              
              Console.WriteLine("server timne (ms)= {0}", tserver);
              Console.WriteLine("Unix time (ms) = {0}", msUnix);
              timediff = msUnix - tserver ;
              //timediff = msUnix - tserver + (3600*5*1000);

              Console.WriteLine("timediff = {0}", timediff);
              Thread.Sleep(1000);
            }

            
        }
    }


}
