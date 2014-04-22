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
        http_get _httpget = new http_get();

		Queue<string> sendQueue=new Queue<string>();
		Thread _sendThread=null;
		object lockQueue=new object();

		//##########
		List<ec3k_data> _ec3kdata=new List<ec3k_data>();

		ec3k_data _ec3k_1=new ec3k_data();
		ec3k_data _ec3k_2=new ec3k_data();
		ec3k_data _ec3k_3=new ec3k_data();

		DateTime lastSend=DateTime.Now;
#if DEBUG
		TimeSpan timespanMin = new TimeSpan(0,1,0);
#else
		TimeSpan timespanMin = new TimeSpan(0,5,0);
#endif
		public tcpclientserver (string sHost, int iPort)
		{
			_sHost=sHost;
			_iPort=iPort;
			//open
			//_listener=new TcpListener(IPAddress.Any,_iPort);

			_client=new TcpClient();
            try
            {
                _client.Connect(_sHost, _iPort);
                _socketStream = _client.GetStream();
                _reader = new StreamReader(_socketStream, System.Text.Encoding.UTF8);
                _writer = new StreamWriter(_socketStream, System.Text.Encoding.UTF8);

                _readThread = new Thread(new ThreadStart(this._threadRead));
                _readThread.Start();

                _sendThread = new Thread(new ThreadStart(this._threadSend));
                _sendThread.Start();

                sendData("R2 A67A\n");
                Thread.Sleep(1000);
				sendData("R9 C4A4\n");
                Thread.Sleep(1000);
				sendData("EC\n");
            }
            catch (Exception ex)
            {
                log.addLog("Exception in tcpclientserver: " + ex.Message);
            }

		}

		void _threadRead(){
            addLog("_threadRead start");
			TimeSpan timeSpan;
			lastSend=DateTime.Now;
			while(bRunThread){
				try {

					//blocking read
					string sRead="";
					sRead = _reader.ReadLine();

					//##################################
					ec3k_data _ec3k=new ec3k_data(sRead);
					_ec3kdata.Add(_ec3k);

					addLog(sRead);
					if(_ec3k._bValid){
						if(_ec3k._sID.Equals("1B67"))
							_ec3k_1=_ec3k;
						else if(_ec3k._sID.Equals("22F0"))
							_ec3k_2=_ec3k;
						else if(_ec3k._sID.Equals("1E0E"))
							_ec3k_3=_ec3k;
						timeSpan=DateTime.Now-lastSend;
						if(timeSpan>=timespanMin){
							if(_ec3k_1._bValid){
								_httpget.add(_ec3k_1);
								_ec3k_1=new ec3k_data();
							}
							if(_ec3k_2._bValid){
								_httpget.add(_ec3k_2);
								_ec3k_2=new ec3k_data();
							}
							if(_ec3k_3._bValid){
								_httpget.add(_ec3k_3);
								_ec3k_3=new ec3k_data();
							}
							lastSend=DateTime.Now;
						}
					}
					//sleep some time
					Thread.Sleep(1000*10);//10 seconds
					addLog(_ec3k.dump());

				} catch (Exception ex) {
                    addLog("_threadRead: " + ex.Message);
				}
			};
            _reader.Dispose();
            addLog("_threadRead stopped");
		}

		AutoResetEvent sendWait=new AutoResetEvent(false);
		void _threadSend(){
			addLog("send thread start");
			do{
				sendWait.WaitOne(); //blocks until there is something to send, signaled by write function
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
				Thread.Sleep(1000);
			}while(bRunThread);
			addLog("send thread stopped");
		}

		public void sendData(string s){
			lock(lockQueue){
				sendQueue.Enqueue(s);
			}
			sendWait.Set();
		}

		public void Dispose(){
			bRunThread=false;

			if(_readThread!=null){
				_readThread.Abort();
			}
			if(_sendThread!=null){
				sendWait.Set();
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
            if(_httpget!=null)
                _httpget.Dispose();

		}
		void addLog(string s){
			System.Diagnostics.Debug.WriteLine(s);
			Console.WriteLine(s);
		}
	}
}
