// A Hello World! program in C#.
using System;
using System.Net;
using System.Net.Http;


namespace HelloWorld
{
    class Hello 
    {
//        private static readonly HttpClient client = new HttpClient();
//        
        static void Main() 
        {
        //private static readonly HttpClient client = new HttpClient(); 
        HttpClient client = new HttpClient(); 
            Console.WriteLine("Hello World!");


            string url = "http://localhost/mycheckin.php" ;

            var values = new Dictionary<string, string> 
              { { "thing1", "hello" }, { "thing2", "world" } };

            var content = new FormUrlEncodedContent(values);

            var response = await client.PostAsync(url, content);

            var responseString = await response.Content.ReadAsStringAsync();

            Console.WriteLine(responseString + "\n");
        }
    }
}
            //var responseString = await client.GetStringAsync("http://www.example.com/recepticle.aspx");

            /* 
             * string url = "http://localhost/mycheckin.php" ;
            ystring url2 = "http://localhost/mycheckin.php" ;
            WebClient webClient = new WebClient();
            webClient.QueryString.Add("image1", "value1");
//            webClient.QueryString.Add("param1", "value1");
//            webClient.QueryString.Add("param2", "value2");

            string s = webClient.UploadString(url2, "POST");

            //string s= webClient.Encoding.GetString(result);
            //string result = webClient.DownloadString(url2, "POST");
           */

            // Keep the console window open in debug mode.
  //            Console.WriteLine("Press any key to exit.");
   //         Console.ReadKey();
