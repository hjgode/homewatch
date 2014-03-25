using System;

namespace ec3k_gateway
{
	class MainClass
	{
		public static void Main (string[] args)
		{
			Console.WriteLine ("Hello World!");
			//portStream ps=new portStream("/dev/pts/2");
			socketPort sp=new socketPort("atom2",3333);

			//sport myPort=new sport();
			bool bExit=false;
			do{
				ConsoleKeyInfo ki = Console.ReadKey();
				if(ki.KeyChar.ToString().ToUpper()=="Q")
					bExit=true;
				else
					sp.sendData(ki.KeyChar.ToString());
			}while(!bExit);
			//myPort.Dispose();
			//ps.Dispose();
			sp.Dispose();
		}
	}
}
