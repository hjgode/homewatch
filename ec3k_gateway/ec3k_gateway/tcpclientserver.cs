using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Collections.Generic;
using System.IO;

namespace ec3k_gateway
{
	public class tcpclientserver:IDisposable
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

		TcpClient _client=null;

		NetworkStream _socketStream=null;
		StreamReader _reader=null;
		StreamWriter _writer = null;

		Thread _readThread=null;
		bool bRunThread=true;

		Queue<string> sendQueue=new Queue<string>();
		Thread _sendThread=null;
		object lockQueue=new object();

		//##########
		List<ec3k_data> _ec3kdata=new List<ec3k_data>();

		public tcpclientserver (string sHost, int iPort)
		{
			_sHost=sHost;
			_iPort=iPort;
			//open
			//_listener=new TcpListener(IPAddress.Any,_iPort);

			_client=new TcpClient();
			_client.Connect(_sHost,_iPort);
			_socketStream = _client.GetStream();
			_reader=new StreamReader(_socketStream, System.Text.Encoding.UTF8);
			_writer=new StreamWriter(_socketStream, System.Text.Encoding.UTF8);

			_readThread=new Thread(new ThreadStart(this._threadRead));
			_readThread.Start();

			_sendThread=new Thread(new ThreadStart(this._threadSend));
			_sendThread.Start();

			sendData("R2 A67A\n");
			Thread.Sleep(1000);
			sendData("EC\n");

		}

		void _threadRead(){
			addLog("thread start");
			http_get _httpget=new http_get();
			do{
				try {
					//blocking read
					string sRead="";
					sRead = _reader.ReadLine();

					//##################################
					ec3k_data _ec3k=new ec3k_data(sRead);
					_ec3kdata.Add(_ec3k);

					addLog(sRead);
					if(_ec3k._bValid)
						_httpget.add(_ec3k);
					addLog(_ec3k.dump());

				} catch (Exception ex) {
					addLog("_thread: " + ex.Message);
				}
			}while(bRunThread);
			_httpget.Dispose();
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
							string sSend = sendQueue.Dequeue();
							_writer.WriteLine(sSend);
							addLog("send :" + sSend + "\r\n");
						}
					}
				} catch (Exception ex) {
					addLog("_threadSend: " + ex.Message);
				}
			}while(bRunThread);
			addLog("send thread stopped");
		}

		public void sendData(string s){
			lock(lockQueue){
				sendQueue.Enqueue(s);
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
			if(_reader!=null)
				_reader.Close();
//			if(_writer!=null)
//				_writer.Close();
			if(_socketStream!=null)
				_socketStream.Close();
			if(_client!=null)
				_client.Close();
		}
		void addLog(string s){
			System.Diagnostics.Debug.WriteLine(s);
			Console.WriteLine(s);
		}
	}
}
