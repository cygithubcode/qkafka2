/Generate tick data 
args:.Q.def[(enlist `kafserver)!(enlist `192.168.81.134:9092)].Q.opt .z.x;
//h:neg hopen`$":",string args[`tp]

kserv1:`$"",string args[`kafserver];
kserv:`192.168.81.134:9092;

\l kfk.q
kfk_cfg:(!) . flip(
  (`metadata.broker.list;kserv);
  (`statistics.interval.ms;`10000);
  (`queue.buffering.max.ms;`1);
  (`fetch.wait.max.ms;`10)
  );
producer:.kfk.Producer[kfk_cfg];
producer_meta:.kfk.Metadata[producer];
show producer_meta`topics;


/trade_feed_topic:.kfk.Topic[producer;`kx_tick;()!()]
trade_feed_topic:.kfk.Topic[producer;`kxfeed_trade;()!()]
/ quote_feed_topic:.kfk.Topic[producer;`kxfeed_quote;()!()]

//.z.ts:{show string x; .kfk.Pub[test_topic;.kfk.PARTITION_UA;string x;"from q session "]}
//.z.ts:{show string x; .kfk.Pub[test_topic;.kfk.PARTITION_UA;-8!x;"from q session "]}

/show "Publishing on topic:",string .kfk.TopicName test_topic;
//.kfk.Pub[test_topic;.kfk.PARTITION_UA;string .z.p;"start from q session "];
//.kfk.Pub[test_topic;.kfk.PARTITION_UA;string .z.p;"start from q session "];
/show "Published 1 message";


show "Set timer with \\t 1000 to publish message every second";






//////

sn:2 cut (
 `AMD;"ADVANCED MICRO DEVICES";
 `AIG;"AMERICAN INTL GROUP INC";
 `AAPL;"APPLE INC COM STK";
 `DELL;"DELL INC";
 `DOW;"DOW CHEMICAL CO";
 `GOOG;"GOOGLE INC CLASS A";
 `HPQ;"HEWLETT-PACKARD CO";
 `INTC;"INTEL CORP";
 `IBM;"INTL BUSINESS MACHINES CORP";
 `MSFT;"MICROSOFT CORP")

s:first each sn
n:last each sn
p:33 27 84 12 20 72 36 51 42 29 / price
m:" ABHILNORYZ" / mode
c:" 89ABCEGJKLNOPRTWZ" / cond
e:"NONNONONNN" / ex

/ init.q

cnt:count s
pi:acos -1
gen:{exp 0.001 * normalrand x}
normalrand:{(cos 2 * pi * x ? 1f) * sqrt neg 2 * log x ? 1f}
randomize:{value "\\S ",string "i"$0.8*.z.p%1000000000}
rnd:{0.01*floor 0.5+x*100}
vol:{10+`int$x?90}

/ randomize[]
\S 235721

/ =========================================================
/ generate a batch of prices
/ qx index, qb/qa margins, qp price, qn position
batch:{
 d:gen x;
 qx::x?cnt;
 qb::rnd x?1.0;
 qa::rnd x?1.0;
 n:where each qx=/:til cnt;
 s:p*prds each d n;
 qp::x#0.0;
 (qp raze n):rnd raze s;
 p::last each s;
 qn::0}
/ gen feed for ticker plant

len:60
batch len

maxn:8 / max trades per tick
qpt:3  / avg quotes per trade

/ =========================================================
t:{
 if[not (qn+x)<count qx;batch len];
 i:qx n:qn+til x;qn+:x;
 (s i;qp n;`int$x?99;1=x?20;x?c;e i)}

q:{
 if[not (qn+x)<count qx;batch len];
 i:qx n:qn+til x;p:qp n;qn+:x;
 (s i;p-qb n;p+qa n;vol x;vol x;x?m;e i)}

feed:{h$[rand 2;
 (".u.upd";`trade;ckck:t 1+rand maxn);
 (".u.upd";`trade;ckck:t 1+rand maxn)
 //(".u.upd";`quote;ckck:q 1+rand qpt*maxn)
 
	];
	show ckck;
	}



feedm:{h$[rand 2;
 (".u.upd";`trade;(enlist a#x),ckck:t a:1+rand maxn);
 (".u.upd";`quote;(enlist a#x),ckck:q a:1+rand qpt*maxn)];
 show ckck;
	}

init:{
 o:"t"$9e5*floor (.z.T-3600000)%9e5;
 d:.z.T-o;
 //len:floor d%113;
 len:30;
 
 feedm each .z.D+`timespan$o+asc len?d;}

/ h(".u.upd";`quote;q 15);
/ h(".u.upd";`trade;t 5);

// init 0
tckck:();
feed2kfk:{
	
	gckck::tckck:(enlist a#.z.P), t a: 1+rand maxn;
	show tckck;
	
	$[rand 2;  //fortest just pub trades..
	
	
	
	
 /(".u.upd";`trade;ckck:t 1+rand maxn);
 
	[
  .kfk.Pub[trade_feed_topic;.kfk.PARTITION_UA;-8!tckck;""]
	];
 
 //(".u.upd";`quote;ckck:q 1+rand qpt*maxn)
  .kfk.Pub[trade_feed_topic;.kfk.PARTITION_UA;-8!tckck;""]
 
 //.kfk.Pub[quote_feed_topic;.kfk.PARTITION_UA;-8!qckck:q 1+rand qpt*maxn;""]}
 
	];
	/ show ckck; 
	}


//.z.ts:feed
//.z.ts:feed2kfk
