using System;
using System.Net;
using System.Web;

using System.Threading;
using System.Collections.Generic;

namespace ec3k_gateway
{
	public class http_get:IDisposable
	{
		string _sHost="192.168.0.40";
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
            log.addLog("http_get: Send Thread started");
			Uri URI=new Uri( "http://" + _sHost + "/homewatch/power/index.php" + "?");
			System.Net.WebRequest req=null;
			System.Net.WebResponse resp=null;
			ec3k_data ec3k;
            try
            {
                while (bRunThread)
                {
                    ec3k = new ec3k_data("");
                    try
                    {
                        lock (lockQueue)
                        {
                            if (sendQueue.Count > 0)
                            {
                                ec3k = sendQueue.Dequeue();
                            }//queue count>0
                        }//lock
                        if (ec3k._bValid)
                        {
                            string sGet = ec3k.getPostString();
                            //make the request
                            log.addLog("http_get: WEB GET=" + URI + sGet);
                            req = System.Net.WebRequest.Create(URI + sGet);
                            //req.Proxy = new System.Net.WebProxy(ProxyString, true); //true means no proxy
                            req.Timeout = 5000;
                            resp = req.GetResponse();
                            System.IO.StreamReader sr = new System.IO.StreamReader(resp.GetResponseStream());
                            log.addLog("http_get: RESP=" + sr.ReadToEnd().Trim());
                        }//bValid
                        Thread.Sleep(1000);
                    }
                    catch (WebException ex)
                    {
                        log.addLog("http_get: WebException in sendThread(): " + ex.Message);
                    }
                    catch (Exception ex)
                    {
                        log.addLog("http_get: web get exception: " + ex.Message);
                    }
                };
            }
            catch (Exception ex)
            {
                log.addLog("http_get: Exception in sendThread(): " + ex.Message);
            }
			try{resp.Close();}catch(Exception){}
			try{req.Abort();}catch(Exception){}
            log.addLog("http_get: Send Thread ended");
		}
		void recvThread(){
		}
		public void Dispose(){
            log.addLog("http_get: Dispose() called");
			bRunThread=false;
			Thread.Sleep(1000);
			if(_sendThread!=null){
				_sendThread.Abort();
				_sendThread=null;
			}
		}

	}
}

