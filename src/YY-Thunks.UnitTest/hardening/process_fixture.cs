using System;
using System.IO;
using System.Linq;
using System.Web.Script.Serialization;

class ProcessFixture
{
    static void Main()
    {
        string[] command = Environment.GetCommandLineArgs();
        string line = new JavaScriptSerializer().Serialize(new {
            kind = Path.GetFileNameWithoutExtension(command[0]).ToLowerInvariant(),
            args = command.Skip(1).ToArray()
        });
        File.AppendAllText(Environment.GetEnvironmentVariable("YY_HARDENING_LOG"), line + "\n");
    }
}
