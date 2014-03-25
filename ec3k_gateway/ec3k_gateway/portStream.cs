using System;
using System.IO;
using System.Threading;
using System.Collections.Generic;

namespace ec3k_gateway
{
	public class portStream:IDisposable
	{
		//string _sPortName = "/dev/ttyUSB0";// real port
//		string _sPortName = "/dev/pts/2";// virtual ser2net port
		//server:
		// _serialport=new SerialPort(_sPortName, baudRate 
		//client:
		// socat -d -d -d pty,link=$HOME/dev/ttyV0,waitslave tcp:atom2:3333
		// then minicom -D ~/dev/ttyV0 -b 57600
		string _sPort="/dev/pts/2";
		FileStream _fs=null;
		Thread _readThread=null;
		bool bRunThread=true;

		BinaryReader _br=null;
		BinaryWriter _wr=null;

		public portStream (string sPort)
		{
			_sPort=sPort;
			//open

			//_fs=new FileStream(_sPort,FileMode.Open ,FileAccess.ReadWrite,FileShare.None,1,false);
			_fs=new FileStream(_sPort,FileMode.Open);
			_br=new BinaryReader(_fs);
			_wr=new BinaryWriter(_fs);

			byte[] bSend;
			bSend = System.Text.Encoding.UTF8.GetBytes("?\r\n");
			_wr.Write("?\r\n");//   (bSend,0,bSend.Length);

			_readThread=new Thread(new ThreadStart(this._thread));
			_readThread.Start();

		}

		void _thread(){
			addLog("thread start");
			List<byte> bList=new List<byte>();
			do{
				try {
					//blocking read
					//byte b= (byte)_fs.ReadByte();

					byte b = _br.ReadByte();
					bList.Add(b);
					if(b==0x0d || b==0x0a){
						string s ="";
						byte[] bytes=bList.ToArray();
						s=System.Text.Encoding.UTF8.GetString(bytes);
						addLog(s);
						bList.Clear();
					}

				} catch (Exception ex) {
					addLog("_thread: " + ex.Message);
				}
			}while(bRunThread);
			addLog("thread stopped");
		}

		public void Dispose(){
			if(_readThread!=null){
				bRunThread=false;
				_readThread.Abort();
			}
			_br.Close();
			_wr.Close();
			_fs.Close();
		}
		void addLog(string s){
			System.Diagnostics.Debug.WriteLine(s);
			Console.Write(s);
		}
	}
}

