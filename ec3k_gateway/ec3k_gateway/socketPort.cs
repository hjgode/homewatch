using System;
using System.Net.Sockets;
using System.Threading;
using System.Collections.Generic;

namespace ec3k_gateway
{
	public class socketPort:IDisposable
	{
		//string _sPortName = "/dev/ttyUSB0";// real port
//		string _sPortName = "/dev/pts/2";// virtual ser2net port
		//server:
		// _serialport=new SerialPort(_sPortName, baudRate 
		//client:
		// socat -d -d -d pty,link=$HOME/dev/ttyV0,waitslave tcp:atom2:3333
		// then minicom -D ~/dev/ttyV0 -b 57600
		int _iPort=3333;
		string _sHost="atom2";
		//NetworkStream _ns=null;
		Socket _socket=null;

		Thread _readThread=null;
		bool bRunThread=true;

		Queue<byte> sendQueue=new Queue<byte>();
		Thread _sendThread=null;
		object lockQueue=new object();

		public socketPort (string sHost, int iPort)
		{
			_sHost=sHost;
			_iPort=iPort;
			//open
			_socket=new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

			_socket.Connect(_sHost,_iPort);

			_readThread=new Thread(new ThreadStart(this._thread));
			_readThread.Start();

			_sendThread=new Thread(new ThreadStart(this._threadSend));
			_sendThread.Start();

			sendData("R2 A67A\n");
			Thread.Sleep(1000);
			sendData("EC\n");

		}

		void _thread(){
			addLog("thread start");
			List<byte> bList=new List<byte>();
			do{
				try {
					//blocking read
					//byte b= (byte)_fs.ReadByte();
					byte[] buf=new byte[200];
					int count = _socket.Receive(buf);
					for(int i=0;i<count;i++)
						bList.Add(buf[i]);
					string sRead=System.Text.Encoding.UTF8.GetString(buf,0,count);
					addLog(sRead);
					//if(sRead.Contains(">"))
					//	sendData("EC\n");
					/*
					if(b==0x0d || b==0x0a){
						string s ="";
						byte[] bytes=bList.ToArray();
						s=System.Text.Encoding.UTF8.GetString(bytes);
						addLog(s);
						bList.Clear();
					}
					*/
				} catch (Exception ex) {
					addLog("_thread: " + ex.Message);
				}
			}while(bRunThread);
			addLog("thread stopped");
		}

		void _threadSend(){
			addLog("send thread start");
			do{
				try {
					//blocking read
					//byte b= (byte)_fs.ReadByte();
					lock(lockQueue){
						if(sendQueue.Count>0){
							byte[] buf= sendQueue.ToArray();
							int count = _socket.Send(buf);
							addLog("send :" + System.Text.Encoding.UTF8.GetString(buf,0,count));
							sendQueue.Clear();
						}
					}
				} catch (Exception ex) {
					addLog("_threadSend: " + ex.Message);
				}
			}while(bRunThread);
			addLog("send thread stopped");
		}

		public void sendData(string s){
			byte[] buf=System.Text.Encoding.UTF8.GetBytes(s);
			lock(lockQueue){
				foreach(byte b in buf)
					sendQueue.Enqueue(b);
			}
		}

		public void Dispose(){
			if(_readThread!=null){
				bRunThread=false;
				_readThread.Abort();
			}
			if(_sendThread!=null){
				_sendThread.Abort();
			}
			if(_socket!=null)
				_socket.Close();
		}
		void addLog(string s){
			System.Diagnostics.Debug.WriteLine(s);
			Console.Write(s);
		}
	}
}
