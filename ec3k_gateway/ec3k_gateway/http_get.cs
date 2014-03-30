using System;
using System.Net;
using System.Web;

using System.Threading;
using System.Collections.Generic;

namespace ec3k_gateway
{
	public class http_get:IDisposable
	{
		string _sHost="atom2";
		Thread _sendThread=null;
		Thread _recvThread=null;
		bool bRunThread=true;

		Queue<ec3k_data> sendQueue=new Queue<ec3k_data>();
		object lockQueue=new object();

		public http_get ()
		{
			_sendThread=new Thread(sendThread);
			_sendThread.Start();
		}
		public void add(ec3k_data data){
			lock(lockQueue){
				sendQueue.Enqueue(data);
			}
		}
		void sendThread(){
			log.addLog("Send Thread started");
			Uri URI=new Uri( "http://" + _sHost + "/homewatch/index.php" + "?");
			System.Net.WebRequest req;
			System.Net.WebResponse resp;
			ec3k_data ec3k;
			while(bRunThread) {
				ec3k=new ec3k_data("");
				try {
					lock (lockQueue) {
						if (sendQueue.Count > 0) {
							ec3k = sendQueue.Dequeue ();
						}//queue count>0
					}//lock
					if (ec3k._bValid) {
						string sGet = ec3k.getPostString ();
						//make the request
						log.addLog ("WEB GET=" + URI + sGet);
						req = System.Net.WebRequest.Create (URI + sGet);
						//req.Proxy = new System.Net.WebProxy(ProxyString, true); //true means no proxy
						req.Timeout = 1000;
						resp = req.GetResponse ();
						System.IO.StreamReader sr = new System.IO.StreamReader (resp.GetResponseStream ());
						log.addLog ("RESP=" + sr.ReadToEnd ().Trim ());
					}//bValid
					Thread.Sleep (1000);
				} catch (Exception ex) {
					log.addLog ("web get exception: " + ex.Message);
				}
			};
			try{resp.Close();}catch(Exception){}
			try{req.Abort();}catch(Exception){}
			log.addLog("Send Thread ended");
		}
		void recvThread(){
		}
		public void Dispose(){
			bRunThread=false;
			Thread.Sleep(1000);
			if(_sendThread!=null){
				_sendThread.Abort();
				_sendThread=null;
			}
		}

	}
}

