\t 2000

\l kfk2.q

/ calculate ohlc vwap volume

args:.Q.def[(enlist `tp)!(enlist `:5001)].Q.opt .z.x;

/ h:hopen`$":",string args[`tp]

// \l ini_consumer.q

t:`trade;s:`;	

/t3sec:();

t3sec:([]time:`timestamp$();sym:`symbol$();price:`float$();size:`int$();stop:`boolean$();cond:`char$();ex:`char$());

//date:`date$();

//secbar:([`s#time:`second$();`g#sym:`symbol$()];open:`float$();high:`float$();low:`float$();close:`float$();vwap:`float$();volume:`long$());

secbar:([`s#time:`timestamp$();`g#sym:`symbol$()];open:`float$();high:`float$();low:`float$();close:`float$();vwap:`float$();volume:`long$());

 upd:{[t;x]
	ux::x;
	// show `upd;show t;show x; show type x;
	
	//pauseh; 
	
    if[(0<count x) & t~`trade;
    if[not 98=type x;x:flip x]; //handle the differrence between real feed and logfile.
	uux::x;
    t3sec,::x;

    //Can use rolling calculaiton for vwap, add up mv=size*price ,and add up size, then update vwap=mv%total size, and with calculation on lists instead of table could make it faster. Here we do a simple way for 3 second data calculation, then upsert to final table.
    `secbar upsert select open: first price, high: max price,low: min price,close:last price,vwap:size wavg price, volume:sum size by "p"$1e9 xbar time,sym from t3sec;
    //show t3sec;
    
    
    mt:(exec max time from t3sec)-00:00:03;
    delete from `t3sec where time < mt;
    
    //here or put in .z.ts to clean memory, limit table memory usage to 800MB for test sample
    //if[(838860800) < -22!secbar; delete from `secbar where i<0.1*count i; .Q.gc[]; ];
    
    ];    
    };

.z.ts:{if[(838860800) < -22!secbar; delete from `secbar where i<0.1*count i;];.Q.gc[];};

// create consumer process within group 0
/ client:.kfk.Consumer[`metadata.broker.list`group.id!`192.168.81.134:9092`0];
/ data:();
// setup meaningful consumer callback(do nothing by default)
//.kfk.consumecb:{[msg]
//    msg[`data]:"c"$msg[`data];
//    msg[`rcvtime]:.z.p;
//    data,::enlist msg;}
//// subscribe to the "test" topic with default partitioning
//.kfk.Sub[client;`test;enlist .kfk.PARTITION_UA];

 /////
 
 
kfk_cfg:(!) . flip(
    (`metadata.broker.list;`192.168.81.134:9092);
    (`group.id;`0);
	(`enable.auto.offset.store;`false);
	(`enable.auto.commit;`false)	
	/(`auto.offset.reset;`smallest)
	;(`auto.offset.reset;`beginning)
	;(`offset.store.method;`file)
	;(`offset.store.path;`$"/home/user1/kfkoffset/")
	
	
   / (`queue.buffering.max.ms;`1);
   / (`fetch.wait.max.ms;`10);
   / (`statistics.interval.ms;`10000)
    );
client:.kfk.Consumer[kfk_cfg];
data:();
rtdata:();
rd:();



.kfk.consumecb:{[msg]
    msg[`data]:msg[`data];
    msg[`rcvtime]:.z.p;
    data,::enlist msg;
	//show .z.P; show ckckmsg::msg[`data];
	if[0<count msg[`data]; rtdata,::enlist msg[`data]
	
	show -9!msg[`data];
	
	rd,::enlist -9!msg[`data];
	
	upd[`trade;-9!msg[`data]];
		];
		//bbk
	};
	 
	 
	 

 //timep2u:{`long$(("p"$x)- "p"$1970.01.01)%1e9};
 timep2u:{`long$(`long$(10957D+`timestamp$x))%1000000000};
 timep2u:{`long$(10957D+`timestamp$x)%1e6};
 
//timeu2p:{-10957D+`timestamp$(1e9*1519266277578j)};
timeu2p:{-10957D+`timestamp$(1e6*x)};

 //dd timep2u:{`long$(-10957D+`timestamp$x)%100000000};
//timep2u .z.P
	 
//but we don't want to sub, we just want to get all the old data
//.kfk.Sub2[client;`kxfeed_trade;enlist .kfk.PARTITION_UA];
//.kfk.Sub2[client;`kxfeed_trade;enlist 0i; 1519266278118j ];

// .kfk.Sub2[client;`kxfeed_trade;enlist 0i; 1519266277578j ];
// .kfk.Sub2[client;`kxfeed_trade;enlist 0i;timep2u 2018.02.22D02:22:37.577999872];
//.kfk.Sub2[client;`kxfeed_trade;enlist 0i; 1519927758695j ];
 // .kfk.Sub2[client;`kxfeed_trade;enlist 0i; 0j ]; 

.kfk.Sub2[client;`kxfeed_trade;enlist 0i;timep2u -00:02:00+.z.p];


 
//.kfk.SetTimeoffsetj(1519266277578j);


/.kfk.Sub[client;`kxfeed_trade;enlist 0i];

/.kfk.Sub[client;`kxfeed_trade;enlist 0i];
show "Metadata:";

client_meta:.kfk.Metadata[client];
show client_meta`topics;
show data



/ .u.rep:{(.[;();:;].) x;show y;if[null first y;:()];

	/ -11!y;

	/ system "cd ",1_-10_string first reverse y
	
	/ };
/ .u.rep . h "(.u.sub[`trade;`];`.u `i`L)";
/ .u.end:{@[`.;tables`.;0#];}
