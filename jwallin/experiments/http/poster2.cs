using System;
using System.Collections.Generic;
using System.Text;
using System.Net;
using System.IO;
using System.Collections.Specialized;

namespace Webposter
{
class Program
{
static void Main(string[] args)
{
//string URLAuth = "https://technet.rapaport.com/HTTP/Authenticate.aspx";
//string url = "http://localhost/mycheckin.php" ;
string url = "http://localhost/mycheck4.php" ;
WebClient webClient = new WebClient();

NameValueCollection formData = new NameValueCollection();



//long milliseconds = DateTime.Now.Ticks / TimeSpan.TicksPerMillisecond; // local
System.DateTime dtDateTime = new DateTime(1970,1,1,0,0,0,0,System.DateTimeKind.Utc);
long msUnixStart = dtDateTime.Ticks / TimeSpan.TicksPerMillisecond;
long milliseconds = DateTime.UtcNow.Ticks / TimeSpan.TicksPerMillisecond;  //utc
long msUnix = milliseconds - msUnixStart ;


formData.Clear();
formData["dbtable"] = "Log";
formData["macid"] = "88:e9:fe:60:d2:f4";
formData["ipaddress"] = "192.168.1.24";
formData["devicetime"] = msUnix.ToString();
formData["image1"] = "i1.jpg";
formData["image1x"] = "2.43";
formData["image1y"] = "2.5";
formData["image1z"] = "1.3";

formData["image2"] = "i2.jpg";
formData["image2x"] = "-3.1";
formData["image2y"] = "2.3";
formData["image2z"] = "-0.3";
//formData["Password"] = "myPassword";

byte[] responseBytes = webClient.UploadValues(url, "POST", formData);
string resultAuthTicket = Encoding.UTF8.GetString(responseBytes);
webClient.Dispose();

Console.WriteLine(resultAuthTicket);

/*webRequest.Method = "POST";
webRequest.ContentType = "multipart/form-data; boundary=" + boundary;

string FilePath = "C:\\test.csv";
formData.Clear();
formData["ticket"] = resultAuthTicket;
formData["ReplaceAll"] = "false";

Stream postDataStream = GetPostStream(FilePath, formData, boundary);

webRequest.ContentLength = postDataStream.Length;
Stream reqStream = webRequest.GetRequestStream();

postDataStream.Position = 0;

byte[] buffer = new byte[1024];
int bytesRead = 0;

while ((bytesRead = postDataStream.Read(buffer, 0, buffer.Length)) != 0)
{
reqStream.Write(buffer, 0, bytesRead);
}

postDataStream.Close();
reqStream.Close();
*/

//StreamReader sr = new StreamReader(webRequest.GetResponse().GetResponseStream());
//string Result = sr.ReadToEnd();
}

private static Stream GetPostStream(string filePath, NameValueCollection formData, string boundary)
{
Stream postDataStream = new System.IO.MemoryStream();

//adding form data
string formDataHeaderTemplate = Environment.NewLine + "--" + boundary + Environment.NewLine +
"Content-Disposition: form-data; name=\"{0}\";" + Environment.NewLine + Environment .NewLine + "{1}";

foreach (string key in formData.Keys)
{
byte[] formItemBytes = System.Text.Encoding.UTF8.GetBytes(string.Format(formDataHeaderTemplate,
key, formData[key]));
postDataStream.Write(formItemBytes, 0, formItemBytes.Length);
}

//adding file data
FileInfo fileInfo = new FileInfo(filePath);

string fileHeaderTemplate = Environment.NewLine + "--" + boundary + Environment.NewLine +
"Content-Disposition: form-data; name=\"{0}\"; filename=\"{1}\"" +
Environment.NewLine + "Content-Type: application/vnd.ms-excel" + Environment.NewLine + Environment.NewLine;

byte[] fileHeaderBytes = System.Text.Encoding.UTF8.GetBytes(string.Format(fileHeaderTemplate,
"UploadCSVFile", fileInfo.FullName));

postDataStream.Write(fileHeaderBytes, 0, fileHeaderBytes.Length);

FileStream fileStream = fileInfo.OpenRead();

byte[] buffer = new byte[1024];

int bytesRead = 0;

while ((bytesRead = fileStream.Read(buffer, 0, buffer.Length)) != 0)
{
postDataStream.Write(buffer, 0, bytesRead);
}

fileStream.Close();

byte[] endBoundaryBytes = System.Text.Encoding.UTF8.GetBytes("--" + boundary + "--");
postDataStream.Write(endBoundaryBytes, 0, endBoundaryBytes.Length);

return postDataStream;
}
}
}
