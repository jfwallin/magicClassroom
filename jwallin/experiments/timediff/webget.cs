
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
          HttpWebRequest http = (HttpWebRequest)WebRequest.Create("http://www.cs.mtsu.edu/~jwallin/ML/timestamp.php");
          WebResponse response = http.GetResponse();

          Stream stream = response.GetResponseStream();
          StreamReader sr = new StreamReader(stream);
          string content = sr.ReadToEnd();

          Console.WriteLine(content);
          response.Close();
        }
    }
}
