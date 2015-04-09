//#define use_net

using System;

namespace ec3k_gateway
{
	class MainClass
	{
		public static void Main (string[] args)
		{
			Console.WriteLine ("Hello World!");
			//portStream ps=new portStream("/dev/pts/2");
			//socketPort sp=new socketPort("atom2",3333);
#if use_net
			tcpclientserver tcp=new tcpclientserver("atom2", 3333);
#else
			sport myPort=new sport();
#endif
			bool bExit=false;
			string sSend="";
			do{
				ConsoleKeyInfo ki = Console.ReadKey();
				if(ki.KeyChar.ToString().ToUpper()=="Q")
					bExit=true;
				else{
					if(ki.Key==ConsoleKey.Enter)
					{
						//sSend+="\n"; sendData uses writeline
#if use_net
						tcp.sendData(sSend);
#else
						myPort.writeCOMM(sSend+"\n");
#endif
						sSend="";
					}
					else
						sSend+=ki.KeyChar.ToString();
					//sp.sendData(ki.KeyChar.ToString());
				}
			}while(!bExit);
			//myPort.Dispose();
			//ps.Dispose();
			//sp.Dispose();
#if use_net
			tcp.Dispose();
#else
			myPort.Dispose();
#endif
		}
	}
}
