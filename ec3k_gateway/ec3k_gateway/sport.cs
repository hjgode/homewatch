//#define use_net
using System;
using System.IO.Ports;
using System.Threading;
using System.Collections.Generic;
using System.Text;

namespace ec3k_gateway
{
	public class sport:IDisposable
	{
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
        http_get _httpget = new http_get();

#if use_net
		string _sPortName = "/dev/pts/2";// virtual ser2net port
#else
		string _sPortName = "/dev/ttyUSB0";// real port
#endif
		//server:
		// _serialport=new SerialPort(_sPortName, baudRate 
		//client:
		// socat -d -d -d pty,link=$HOME/dev/ttyV0,waitslave tcp:atom2:3333
		// then minicom -D ~/dev/ttyV0 -b 57600

		static SerialPort _serialport=null; 
		Thread myThread=null;
		bool bRunThread=true;
		object myLock=new object();
		Queue<byte> byteQueue=new Queue<byte>();

		public sport ()
		{
			log.addLog("Serial ports:");
			foreach(string s in SerialPort.GetPortNames()){
				log.addLog(s);
			}

			if(openCOMM()){
				log.addLog("openCOMM() OK");
				myThread=new Thread(readThread);
				myThread.Start();
			}
			else
				log.addLog("openCOMM() failed");
		}

		public void writeCOMM(string s){
			try {
				if(_serialport!=null && _serialport.IsOpen){
					_serialport.WriteTimeout=500;
					byte[] buf=Encoding.UTF8.GetBytes(s);
					_serialport.Write(buf,0, buf.Length);
					log.addLog("write done");
				}
				else
					log.addLog("write: serialport not open");
			}
			catch(Exception ex)
			{
				log.addLog("Exception in write: "+ex.Message);
			}
		}

		void readThread(object param){
			log.addLog("read thread start...");
			byte b;
			TimeSpan timeSpan;
			lastSend=DateTime.Now;
			do{
				try {
					if(_serialport!=null && _serialport.IsOpen){
						string sRead="";
						do{
							sRead = _serialport.ReadLine();
							log.addLog(sRead);
						}while(_serialport.BytesToRead>0);
						//##################################
						ec3k_data _ec3k=new ec3k_data(sRead);
						_ec3kdata.Add(_ec3k);

						log.addLog(sRead);
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
							}//if timespan
						}//if bValid
					//sleep some time
					Thread.Sleep(1000*10);//10 seconds
					log.addLog(_ec3k.dump());
/*
						b=(byte)_serialport.ReadByte(); //blocks
						lock(myLock){
							byteQueue.Enqueue(b);
							if(b==0x0A || b==0x0d){
								//fire event 
								byte[] buf=byteQueue.ToArray();
								string s=Encoding.UTF8.GetString(buf);
								log.addLog("read='"+s+"'");
							}
						}//lock
*/						
					}//if
				}
				catch (ThreadAbortException){
					log.addLog("Thread aborting");
				}
				catch (Exception ex) {
					log.addLog("Exception in ReadThread: "+ex.Message);	
				}
			}while(_serialport!=null && bRunThread);
			log.addLog("Thread ended");
			return;
		}

		bool openCOMM(){
			log.addLog("openCOMM...");
			closeCOMM();
			bool bRet=false;
			int baudRate=57600;
			try{
				//_serialport=new SerialPort(_sPortName, baudRate ,Parity.None,8,StopBits.One);
				_serialport=new SerialPort(_sPortName, baudRate, Parity.None, 8, StopBits.None);
				_serialport.ReadTimeout=System.IO.Ports.SerialPort.InfiniteTimeout;
				_serialport.Open();
				log.addLog("SerialPort open");
				bRet=true;
			}catch(Exception ex){
				log.addLog("sport: Exception: "+ex.Message+" '"+_sPortName+"'");
			}
			log.addLog("OpenCOMM done");
			return bRet;
		}

		void closeCOMM(){
			if(_serialport!=null){
				if(_serialport.IsOpen){
					_serialport.Close();
					_serialport=null;
				}
			}
		}
		public void Dispose(){
			closeCOMM();
			if(myThread!=null){
				bRunThread=false;
				myThread.Abort();
			}
		}


	}
}

