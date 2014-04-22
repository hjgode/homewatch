using System;
using System.Text;

namespace ec3k_gateway
{
	public class ec3k_data
	{
		/*
		EC3K 3163760324 ID=1B67 37107326 sT 5581545 sON 0x0001ACF88D2F Ws 0.2 W 2217.6 Wmax 1 resets ON
		recv len=70 ovr=0 r=3165369256 01C0 b=3165369408 +152 81C0 e=3165397056 +27648 8040 crl=66 boffs=168 CRCerror
		EC3K 3167062188 ID=22F0 36326315 sT 36326093 sON 0x0001FCB19FDC Ws 2378.3 W 2615.1 Wmax 1 resets ON
		EC3K 3168719920 ID=1B67 37107331 sT 5581550 sON 0x0001ACF88D30 Ws 0.2 W 2217.6 Wmax 1 resets ON
		EC3K 3170353972 ID=1E0E 36861878 sT 36848059 sON 0x00022BD73E4A Ws 1.2 W 2080.8 Wmax 2 resets ON
		EC3K 3171945968 ID=22F0 36326320 sT 36326098 sON 0x0001FCB1CE3F Ws 2373.3 W 2615.1 Wmax 1 resets ON
		EC3K 3173830196 ID=1B67 37107336 sT 5581555 sON 0x0001ACF88D31 Ws 0.2 W 2217.6 Wmax 1 resets ON
		recv len=10 ovr=0 r=3174156524 01C0 b=3174156620 +96 81C0 e=3174160224 +3604 8000 crl=1 tooshort
		EC3K 3175463868 ID=1E0E 36861883 sT 36848064 sON 0x00022BD73E50 Ws 1.2 W 2080.8 Wmax 2 resets ON
		EC3K 3176904984 ID=22F0 36326325 sT 36326103 sON 0x0001FCB1FCA6 Ws 2377.0 W 2615.1 Wmax 1 resets ON
		EC3K 3178864728 ID=1B67 37107341 sT 5581560 sON 0x0001ACF88D32 Ws 0.2 W 2217.6 Wmax 1 resets ON
		*/
		public bool _bValid=false;
		public string _sID="";
		ulong _ticks=0;
		ulong _total=0;
		ulong _onTime=0;
		UInt64 _usedWs=0;
		decimal _currentWatt=0;
		decimal _maxWatt=0;
		uint _numResets=0;
		bool _statusON=false;

		public static uint _errorCount=0;
		public static uint _totalCount=0;

		public ec3k_data ()
		{
		}
		public ec3k_data (string sToParse)
		{
			_totalCount++;
			_bValid=decodeEC3K(sToParse);
			if(!_bValid)
				_errorCount++;
		}
		bool decodeEC3K(string s){
			bool bRet=false;
			string[] splitted=s.Split(new char[]{' '});
			if(splitted.Length>0){
				if(splitted[0]=="EC3K"){
					try{
					_ticks=Convert.ToUInt64(splitted[1]);
					_sID=splitted[2].Substring(3);
					_total=Convert.ToUInt32(splitted[3]);
					_onTime=Convert.ToUInt32(splitted[5]);
					_usedWs=Convert.ToUInt64(splitted[7].Substring(2),16); //from hex 0x...
					_currentWatt=Convert.ToDecimal(splitted[9]);
					_maxWatt=Convert.ToDecimal(splitted[11]);
					_numResets=Convert.ToUInt16(splitted[13]);

					if(splitted[15].Equals("ON"))
						_statusON=true;
					else
						_statusON=false;
					//_statusON=Convert.ToBoolean(splitted[15]);
					
					bRet=true;
					#region split
/* 
0: EC3K 
1: 3178864728 
2: ID=1B67 
3: 37107341
4: sT 
5: 5581560 
6: sON 
7: 0x0001ACF88D32 
8: Ws
9: 0.2
10: W 
11: 2217.6 
12: Wmax
13: 1 
14: resets
15: ON
*/
					#endregion
					}catch(Exception ex){
						log.addLog("\nException decoding '"+s+"' :"+ex.Message);
						//System.Diagnostics.Debugger.Break();
					}
				}//if EC3K
			}
			return bRet;
		}
		public string dump(){
			StringBuilder sb = new StringBuilder();
			sb.Append("VALID = "+this._bValid.ToString() +"\n");
			sb.Append("ID="+this._sID+"\n");
			sb.Append("ticks="+this._ticks.ToString()+"\n");
			sb.Append("total time="+this._total.ToString()+"\n");
			sb.Append("ON time="+this._onTime.ToString()+"\n");
			sb.Append("Used Ws="+this._usedWs.ToString()+"\n");
			sb.Append("current W="+this._currentWatt.ToString("00.0")+"\n");
			sb.Append("max Watt="+this._maxWatt.ToString("0.00")+"\n");
			sb.Append("num resets="+this._numResets.ToString()+"\n");
			sb.Append("status="+this._statusON.ToString()+"\n");
			sb.Append("total count="+_totalCount.ToString()+"\n");
			sb.Append("error count="+_errorCount.ToString()+"\n");

			return sb.ToString();
		}

/* GET /homewatch/power/index.php?
		id=1E0E
		&ticks=1056852848
		&total=37071698
		&ontime=37057879
		&usedws=9332520423
		&currentw=1.2
		&maxw=2080.8
		&numresets=2
		&status=True
		HTTP/1.1
*/
		public string getPostString(){
			StringBuilder sb=new StringBuilder();
			sb.Append("id="+this._sID+"&");
			sb.Append("ticks="+this._ticks.ToString()+"&");
			sb.Append("total="+this._total.ToString()+"&");
			sb.Append("ontime="+this._onTime.ToString()+"&");
			sb.Append("usedws="+this._usedWs.ToString()+"&");
			sb.Append("currentw="+this._currentWatt.ToString()+"&");
			sb.Append("maxw="+this._maxWatt.ToString()+"&");
			sb.Append("numresets="+this._numResets.ToString()+"&");
			sb.Append("status="+this._statusON.ToString());

			return sb.ToString();
		}
	}
}

