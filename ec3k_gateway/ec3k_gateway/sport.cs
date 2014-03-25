using System;
using System.IO.Ports;
using System.Threading;
using System.Collections.Generic;
using System.Text;

namespace ec3k_gateway
{
	public class sport:IDisposable
	{
		//string _sPortName = "/dev/ttyUSB0";// real port
		string _sPortName = "/dev/pts/2";// virtual ser2net port
		//server:
		// _serialport=new SerialPort(_sPortName, baudRate 
		//client:
		// socat -d -d -d pty,link=$HOME/dev/ttyV0,waitslave tcp:atom2:3333
		// then minicom -D ~/dev/ttyV0 -b 57600

		static SerialPort _serialport=null; 
		Thread myThread=null;
		object myLock=new object();
		Queue<byte> byteQueue=new Queue<byte>();

		public sport ()
		{
			log.addLog("Serial ports:");
			foreach(string s in SerialPort.GetPortNames()){
				log.addLog(s);
			}

			if(openCOMM()){
				myThread=new Thread(readThread);
				myThread.Start();
			}
			else
				log.addLog("openCOMM() failed");
		}

		void readThread(object param){
			byte b;
			do{
				try {
					if(_serialport!=null && _serialport.IsOpen){
						b=(byte)_serialport.ReadByte(); //blocks
						lock(myLock){
							byteQueue.Enqueue(b);
							if(b==0x0A || b==0x0d){
								//fire event 
								byte[] buf=byteQueue.ToArray();
								string s=Encoding.UTF8.GetString(buf);
								log.addLog("read='"+s+"'");
							}
						}
					}
				}
				catch (ThreadAbortException){
					log.addLog("Thread aborting");
				}
				catch (Exception ex) {
					log.addLog("Exception in ReadThread: "+ex.Message);	
				}
			}while(_serialport!=null);
			log.addLog("Thread ended");
			return;
		}

		bool openCOMM(){
			closeCOMM();
			bool bRet=false;
			int baudRate=57600;
			try{
				//_serialport=new SerialPort(_sPortName, baudRate ,Parity.None,8,StopBits.One);
				_serialport=new SerialPort(_sPortName, baudRate);
				_serialport.Open();
				_serialport.ReadTimeout=System.IO.Ports.SerialPort.InfiniteTimeout;

				bRet=true;
			}catch(Exception ex){
				log.addLog("sport: Exception: "+ex.Message+" '"+_sPortName+"'");
			}
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
			if(myThread!=null)
				myThread.Abort();
		}


	}
}

