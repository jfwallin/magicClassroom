// A Hello World! program in C#.
using System;
using System.Net;
using System.IO;
using System.Text;

namespace HelloWorld
{
    class Hello 
    {
        static void Main() 
        {
            string url = "http://localhost/mycheckin.php" ;


            /*
            // Create a request using a URL that can receive a post.   
            //WebRequest request = WebRequest.Create("http://www.contoso.com/PostAccepter.aspx ");
            WebRequest request = WebRequest.Create(url);
            // Set the Method property of the request to POST.  
            request.Method = "POST";
            
            // Create POST data and convert it to a byte array.  
            string postData = "This is a test that posts this string to a Web server.";
            byte[] byteArray = Encoding.UTF8.GetBytes(postData);
            
            // Set the ContentType property of the WebRequest.  
            request.ContentType = "application/x-www-form-urlencoded";
            // Set the ContentLength property of the WebRequest.  
            request.ContentLength = byteArray.Length;
            */ 


            NameValueCollection formData = new NameValueCollection();
            formData["IP"] = "192.168.1.24";
            formData["Password"] = "myPassword";

            byte[] responseBytes = webClient.UploadValues(URLAuth, "POST", formData);
            
            Console.WriteLine(responseBytes);

            /*

            // Get the request stream.  
            Stream dataStream = request.GetRequestStream();
            // Write the data to the request stream.  
            dataStream.Write(byteArray, 0, byteArray.Length);
            // Close the Stream object.  
            dataStream.Close();
            

            // Get the response.  
            WebResponse response = request.GetResponse();
            // Display the status.  
            Console.WriteLine(((HttpWebResponse)response).StatusDescription);

            // Get the stream containing content returned by the server.  
            // The using block ensures the stream is automatically closed.
            using (dataStream = response.GetResponseStream())
            {
                // Open the stream using a StreamReader for easy access.  
                StreamReader reader = new StreamReader(dataStream);
                // Read the content.  
                string responseFromServer = reader.ReadToEnd();
                // Display the content.  
                Console.WriteLine(responseFromServer);
            }

            // Close the response.  
            response.Close();

            */



        }
    }
}
